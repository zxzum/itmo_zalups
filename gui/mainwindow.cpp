#include "mainwindow.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QSplitter>
#include <QHeaderView>
#include <QFile>
#include <QTextStream>
#include <QDesktopServices>
#include <QUrl>
#include <QApplication>
#include <fstream>
#include <sstream>
#include <filesystem>

#include "../lib/lib.hpp"

namespace fs = std::filesystem;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
    setupUI();
    applyDarkTheme();
    
    setWindowTitle("ITMO Loops - Генератор музыки");
    resize(1200, 800);
}

MainWindow::~MainWindow() {
}

void MainWindow::setupUI() {
    QWidget* central = new QWidget(this);
    setCentralWidget(central);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(central);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    // === 1. Панель ввода файлов ===
    QGroupBox* inputGroup = new QGroupBox("Входные файлы");
    QVBoxLayout* inputLayout = new QVBoxLayout(inputGroup);
    
    // Score файл
    QHBoxLayout* scoreLayout = new QHBoxLayout();
    QLabel* scoreLabel = new QLabel("Файл score.txt:");
    scoreLabel->setMinimumWidth(120);
    scorePathEdit = new QLineEdit();
    browseScoreBtn = new QPushButton("Обзор...");
    scoreLayout->addWidget(scoreLabel);
    scoreLayout->addWidget(scorePathEdit);
    scoreLayout->addWidget(browseScoreBtn);
    
    // Папка samples
    QHBoxLayout* samplesLayout = new QHBoxLayout();
    QLabel* samplesLabel = new QLabel("Папка samples:");
    samplesLabel->setMinimumWidth(120);
    samplesDirEdit = new QLineEdit();
    browseSamplesDirBtn = new QPushButton("Обзор...");
    samplesLayout->addWidget(samplesLabel);
    samplesLayout->addWidget(samplesDirEdit);
    samplesLayout->addWidget(browseSamplesDirBtn);
    
    // Кнопка загрузки
    loadScoreBtn = new QPushButton("Загрузить");
    loadScoreBtn->setStyleSheet("font-weight: bold; padding: 8px;");
    
    inputLayout->addLayout(scoreLayout);
    inputLayout->addLayout(samplesLayout);
    inputLayout->addWidget(loadScoreBtn);
    
    mainLayout->addWidget(inputGroup);

    // === 2. Центральная часть - редактор и таблицы ===
    QSplitter* centerSplitter = new QSplitter(Qt::Horizontal);
    
    // Левая часть - редактор кода
    QGroupBox* editorGroup = new QGroupBox("Редактор кода");
    QVBoxLayout* editorLayout = new QVBoxLayout(editorGroup);
    codeEditor = new QTextEdit();
    codeEditor->setReadOnly(false);
    codeEditor->setFontFamily("Courier New");
    codeEditor->setFontPointSize(10);
    highlighter = new SyntaxHighlighter(codeEditor->document());
    editorLayout->addWidget(codeEditor);
    centerSplitter->addWidget(editorGroup);
    
    // Правая часть - информация
    QWidget* rightWidget = new QWidget();
    QVBoxLayout* rightLayout = new QVBoxLayout(rightWidget);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    
    // Таблица инструментов
    QGroupBox* instGroup = new QGroupBox("Инструменты");
    QVBoxLayout* instLayout = new QVBoxLayout(instGroup);
    instrumentsTable = new QTableWidget(0, 5);
    instrumentsTable->setHorizontalHeaderLabels({"Имя", "Тип", "Sample/Параметры", "Attack", "Release"});
    instrumentsTable->horizontalHeader()->setStretchLastSection(true);
    instrumentsTable->setMaximumHeight(200);
    instLayout->addWidget(instrumentsTable);
    rightLayout->addWidget(instGroup);
    
    // Список паттернов и сводка
    QHBoxLayout* infoLayout = new QHBoxLayout();
    
    QGroupBox* patternsGroup = new QGroupBox("Паттерны");
    QVBoxLayout* patternsLayout = new QVBoxLayout(patternsGroup);
    patternsList = new QListWidget();
    patternsList->setMaximumHeight(150);
    patternsLayout->addWidget(patternsList);
    infoLayout->addWidget(patternsGroup);
    
    QGroupBox* summaryGroup = new QGroupBox("Сводка");
    QVBoxLayout* summaryLayout = new QVBoxLayout(summaryGroup);
    summaryLabel = new QLabel("BPM: -\nПродолжительность: -");
    summaryLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    summaryLayout->addWidget(summaryLabel);
    summaryLayout->addStretch();
    infoLayout->addWidget(summaryGroup);
    
    rightLayout->addLayout(infoLayout);
    rightLayout->addStretch();
    
    centerSplitter->addWidget(rightWidget);
    centerSplitter->setStretchFactor(0, 2);
    centerSplitter->setStretchFactor(1, 1);
    
    mainLayout->addWidget(centerSplitter, 1);

    // === 3. Панель рендера ===
    QGroupBox* renderGroup = new QGroupBox("Генерация");
    QVBoxLayout* renderLayout = new QVBoxLayout(renderGroup);
    
    QHBoxLayout* outputLayout = new QHBoxLayout();
    QLabel* outputLabel = new QLabel("Выходной файл:");
    outputLabel->setMinimumWidth(120);
    outputPathEdit = new QLineEdit();
    browseOutputBtn = new QPushButton("Обзор...");
    outputLayout->addWidget(outputLabel);
    outputLayout->addWidget(outputPathEdit);
    outputLayout->addWidget(browseOutputBtn);
    
    generateBtn = new QPushButton("Сгенерировать WAV");
    generateBtn->setStyleSheet("font-weight: bold; padding: 10px; font-size: 12pt;");
    
    statusLabel = new QLabel("Статус: Готово к генерации");
    statusLabel->setAlignment(Qt::AlignCenter);
    
    renderLayout->addLayout(outputLayout);
    renderLayout->addWidget(generateBtn);
    renderLayout->addWidget(statusLabel);
    
    mainLayout->addWidget(renderGroup);

    // === 4. Превью волновой формы ===
    QGroupBox* waveformGroup = new QGroupBox("Превью волновой формы");
    QVBoxLayout* waveformLayout = new QVBoxLayout(waveformGroup);
    waveformWidget = new WaveformWidget();
    waveformLayout->addWidget(waveformWidget);
    mainLayout->addWidget(waveformGroup);

    // === 5. Кнопка воспроизведения ===
    openPlayerBtn = new QPushButton("Открыть в плеере");
    openPlayerBtn->setEnabled(false);
    openPlayerBtn->setStyleSheet("padding: 8px;");
    mainLayout->addWidget(openPlayerBtn);

    // Подключение сигналов
    connect(browseScoreBtn, &QPushButton::clicked, this, &MainWindow::browseScoreFile);
    connect(browseSamplesDirBtn, &QPushButton::clicked, this, &MainWindow::browseSamplesDir);
    connect(browseOutputBtn, &QPushButton::clicked, this, &MainWindow::browseOutputFile);
    connect(loadScoreBtn, &QPushButton::clicked, this, &MainWindow::loadScore);
    connect(generateBtn, &QPushButton::clicked, this, &MainWindow::generateWav);
    connect(openPlayerBtn, &QPushButton::clicked, this, &MainWindow::openInPlayer);
}

void MainWindow::applyDarkTheme() {
    // Тёмная тема
    QString styleSheet = R"(
        QMainWindow, QWidget {
            background-color: #1e1e1e;
            color: #d4d4d4;
        }
        QGroupBox {
            border: 1px solid #3e3e3e;
            border-radius: 5px;
            margin-top: 10px;
            padding-top: 10px;
            font-weight: bold;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px;
        }
        QLineEdit, QTextEdit {
            background-color: #2d2d2d;
            border: 1px solid #3e3e3e;
            border-radius: 3px;
            padding: 5px;
            color: #d4d4d4;
        }
        QPushButton {
            background-color: #0e639c;
            border: none;
            border-radius: 3px;
            padding: 5px 15px;
            color: white;
        }
        QPushButton:hover {
            background-color: #1177bb;
        }
        QPushButton:pressed {
            background-color: #0d5689;
        }
        QPushButton:disabled {
            background-color: #3e3e3e;
            color: #888;
        }
        QTableWidget, QListWidget {
            background-color: #2d2d2d;
            border: 1px solid #3e3e3e;
            color: #d4d4d4;
        }
        QTableWidget::item, QListWidget::item {
            padding: 5px;
        }
        QHeaderView::section {
            background-color: #3e3e3e;
            color: #d4d4d4;
            padding: 5px;
            border: none;
        }
        QLabel {
            color: #d4d4d4;
        }
    )";
    setStyleSheet(styleSheet);
}

void MainWindow::browseScoreFile() {
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Выберите файл score",
        "",
        "Text Files (*.txt);;All Files (*)"
    );
    
    if (!fileName.isEmpty()) {
        scorePathEdit->setText(fileName);
        
        // Автоматически установить папку samples
        QFileInfo fileInfo(fileName);
        QString dir = fileInfo.absolutePath();
        
        // Проверяем, есть ли папка samples рядом
        QString samplesPath = dir + "/samples";
        if (QDir(samplesPath).exists()) {
            samplesDirEdit->setText(samplesPath);
        } else {
            samplesDirEdit->setText(dir);
        }
        
        // Установить выходной файл по умолчанию
        QString baseName = fileInfo.completeBaseName();
        outputPathEdit->setText(dir + "/" + baseName + "_output.wav");
    }
}

void MainWindow::browseSamplesDir() {
    QString dirName = QFileDialog::getExistingDirectory(
        this,
        "Выберите папку с samples",
        ""
    );
    
    if (!dirName.isEmpty()) {
        samplesDirEdit->setText(dirName);
    }
}

void MainWindow::browseOutputFile() {
    QString fileName = QFileDialog::getSaveFileName(
        this,
        "Сохранить WAV файл",
        "",
        "WAV Files (*.wav);;All Files (*)"
    );
    
    if (!fileName.isEmpty()) {
        if (!fileName.endsWith(".wav", Qt::CaseInsensitive)) {
            fileName += ".wav";
        }
        outputPathEdit->setText(fileName);
    }
}

void MainWindow::loadScore() {
    currentScorePath = scorePathEdit->text();
    currentSamplesDir = samplesDirEdit->text();
    
    if (currentScorePath.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Выберите файл score.txt");
        return;
    }
    
    QFile file(currentScorePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть файл: " + currentScorePath);
        return;
    }
    
    QTextStream in(&file);
    QString content = in.readAll();
    file.close();
    
    codeEditor->setPlainText(content);
    parseAndDisplayScore(content);
    
    statusLabel->setText("Статус: Score загружен, готово к генерации");
    statusLabel->setStyleSheet("color: #4ec9b0;");
}

void MainWindow::parseAndDisplayScore(const QString& content) {
    // Простой парсинг для отображения
    QStringList lines = content.split('\n');
    
    instrumentsTable->setRowCount(0);
    patternsList->clear();
    
    double bpm = 120.0;
    int instCount = 0;
    int patternCount = 0;
    
    QString currentSection;
    QString instName, instType, instParams;
    double attack = 0, release = 0;
    
    for (const QString& line : lines) {
        QString trimmed = line.trimmed();
        if (trimmed.isEmpty() || trimmed.startsWith('#')) continue;
        
        QStringList tokens = trimmed.split(QRegularExpression("\\s+"));
        if (tokens.isEmpty()) continue;
        
        if (tokens[0] == "bpm" && tokens.size() >= 2) {
            bpm = tokens[1].toDouble();
        }
        else if (tokens[0] == "instrument" && tokens.size() >= 3) {
            currentSection = "instrument";
            instName = tokens[1];
            instType = tokens[2];
            instParams = "";
            attack = 0;
            release = 0;
        }
        else if (tokens[0] == "pattern" && tokens.size() >= 3) {
            currentSection = "pattern";
            QString patternName = tokens[1];
            QString resolution = tokens.size() >= 4 ? tokens[3] : tokens[2];
            patternsList->addItem(patternName + " (res=" + resolution + ")");
            patternCount++;
        }
        else if (tokens[0] == "end") {
            if (currentSection == "instrument") {
                instrumentsTable->insertRow(instCount);
                instrumentsTable->setItem(instCount, 0, new QTableWidgetItem(instName));
                instrumentsTable->setItem(instCount, 1, new QTableWidgetItem(instType));
                instrumentsTable->setItem(instCount, 2, new QTableWidgetItem(instParams));
                instrumentsTable->setItem(instCount, 3, new QTableWidgetItem(QString::number(attack)));
                instrumentsTable->setItem(instCount, 4, new QTableWidgetItem(QString::number(release)));
                instCount++;
            }
            currentSection = "";
        }
        else if (currentSection == "instrument") {
            for (const QString& token : tokens) {
                if (token.contains('=')) {
                    QStringList kv = token.split('=');
                    if (kv.size() == 2) {
                        QString key = kv[0];
                        QString value = kv[1];
                        
                        if (key == "sample" || key == "root" || key == "loop" || key == "duty") {
                            if (!instParams.isEmpty()) instParams += ", ";
                            instParams += key + "=" + value;
                        }
                        else if (key == "attack") {
                            attack = value.toDouble();
                        }
                        else if (key == "release") {
                            release = value.toDouble();
                        }
                    }
                }
            }
        }
    }
    
    updateSummary();
}

void MainWindow::updateSummary() {
    // Простое чтение BPM и примерная оценка длительности
    QString content = codeEditor->toPlainText();
    QStringList lines = content.split('\n');
    
    double bpm = 120.0;
    for (const QString& line : lines) {
        QString trimmed = line.trimmed();
        if (trimmed.startsWith("bpm ")) {
            QStringList tokens = trimmed.split(QRegularExpression("\\s+"));
            if (tokens.size() >= 2) {
                bpm = tokens[1].toDouble();
                break;
            }
        }
    }
    
    summaryLabel->setText(QString("BPM: %1\nПродолжительность: (после генерации)").arg(bpm));
}

void MainWindow::generateWav() {
    QString scorePath = scorePathEdit->text();
    QString samplesDir = samplesDirEdit->text();
    QString outputPath = outputPathEdit->text();
    
    if (scorePath.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Выберите файл score.txt");
        return;
    }
    
    if (outputPath.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Укажите путь для выходного файла");
        return;
    }
    
    if (samplesDir.isEmpty()) {
        QFileInfo fileInfo(scorePath);
        samplesDir = fileInfo.absolutePath();
    }
    
    // Определяем базовую директорию
    QFileInfo scoreInfo(scorePath);
    baseDirectory = scoreInfo.absolutePath();
    
    statusLabel->setText("Статус: Генерация...");
    statusLabel->setStyleSheet("color: #dcdcaa;");
    QApplication::processEvents();
    
    // Вызываем функцию генерации из библиотеки
    std::string scorePathStd = scorePath.toStdString();
    std::string outputPathStd = outputPath.toStdString();
    std::string baseDirStd = baseDirectory.toStdString();
    
    int result = run_itmoloops(scorePathStd, outputPathStd, baseDirStd);
    
    if (result == 0) {
        statusLabel->setText("Статус: Успешно сгенерировано: " + outputPath);
        statusLabel->setStyleSheet("color: #4ec9b0;");
        
        // Загружаем waveform для превью
        std::vector<float> waveform = loadWavForPreview(outputPath);
        waveformWidget->setWaveform(waveform);
        
        openPlayerBtn->setEnabled(true);
        currentOutputPath = outputPath;
        
        QMessageBox::information(this, "Успех", "WAV файл успешно сгенерирован:\n" + outputPath);
    } else {
        statusLabel->setText("Статус: Ошибка генерации");
        statusLabel->setStyleSheet("color: #f48771;");
        QMessageBox::critical(this, "Ошибка", "Не удалось сгенерировать WAV файл. Проверьте консоль для подробностей.");
    }
}

void MainWindow::openInPlayer() {
    if (currentOutputPath.isEmpty() || !QFile::exists(currentOutputPath)) {
        QMessageBox::warning(this, "Ошибка", "WAV файл не найден");
        return;
    }
    
    QUrl url = QUrl::fromLocalFile(currentOutputPath);
    if (!QDesktopServices::openUrl(url)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл в системном плеере");
    }
}

std::vector<float> MainWindow::loadWavForPreview(const QString& path) {
    std::vector<float> samples;
    
    std::ifstream file(path.toStdString(), std::ios::binary);
    if (!file) return samples;
    
    // Читаем заголовок WAV
    char header[44];
    file.read(header, 44);
    
    if (std::string(header, 4) != "RIFF") return samples;
    if (std::string(header + 8, 4) != "WAVE") return samples;
    
    // Читаем данные
    std::vector<char> data((std::istreambuf_iterator<char>(file)), {});
    
    // Преобразуем в float (упрощенно)
    const int16_t* ptr = reinterpret_cast<const int16_t*>(data.data());
    size_t count = data.size() / 2;
    
    samples.reserve(count);
    for (size_t i = 0; i < count; i++) {
        samples.push_back(ptr[i] / 32768.0f);
    }
    
    return samples;
}
