#include "mainwindow.hpp"
#include "lib.hpp"
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QMenuBar>
#include <QToolBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QStatusBar>
#include <QGroupBox>
#include <QLabel>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <fstream>

namespace {
    constexpr size_t WAV_HEADER_SIZE = 44;  // Standard WAV header size in bytes
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      syntaxHighlighter(nullptr),
      mediaPlayer(nullptr),
      audioOutput(nullptr) {
    
    setupUi();
    setupMenuBar();
    applyDarkTheme();
    
    setAcceptDrops(true);
    
    showStatus("Готов к работе");
}

MainWindow::~MainWindow() {
    if (mediaPlayer) {
        mediaPlayer->stop();
    }
}

void MainWindow::setupUi() {
    setWindowTitle("ITMO Loops - Редактор музыкальных композиций");
    resize(1200, 800);
    
    // Центральный виджет
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    
    // Основной сплиттер (горизонтальный)
    QSplitter *mainSplitter = new QSplitter(Qt::Horizontal, this);
    
    // Левая панель - редактор кода
    QWidget *editorPanel = new QWidget();
    QVBoxLayout *editorLayout = new QVBoxLayout(editorPanel);
    editorLayout->setContentsMargins(0, 0, 0, 0);
    
    QLabel *editorLabel = new QLabel("Редактор кода:");
    editorLabel->setStyleSheet("font-weight: bold; padding: 5px;");
    editorLayout->addWidget(editorLabel);
    
    codeEditor = new QTextEdit();
    codeEditor->setFont(QFont("Courier New", 11));
    codeEditor->setAcceptRichText(false);
    syntaxHighlighter = new SyntaxHighlighter(codeEditor->document());
    connect(codeEditor, &QTextEdit::textChanged, this, &MainWindow::onEditorTextChanged);
    editorLayout->addWidget(codeEditor);
    
    // Панель инструментов
    QHBoxLayout *toolbarLayout = new QHBoxLayout();
    
    QPushButton *openBtn = new QPushButton("📂 Открыть");
    QPushButton *saveBtn = new QPushButton("💾 Сохранить");
    QPushButton *generateBtn = new QPushButton("🎵 Сгенерировать WAV");
    playButton = new QPushButton("▶️ Воспроизвести");
    stopButton = new QPushButton("⏹️ Стоп");
    
    playButton->setEnabled(false);
    stopButton->setEnabled(false);
    
    connect(openBtn, &QPushButton::clicked, this, &MainWindow::openFile);
    connect(saveBtn, &QPushButton::clicked, this, &MainWindow::saveFile);
    connect(generateBtn, &QPushButton::clicked, this, &MainWindow::generateWav);
    connect(playButton, &QPushButton::clicked, this, &MainWindow::playAudio);
    connect(stopButton, &QPushButton::clicked, this, &MainWindow::stopAudio);
    
    toolbarLayout->addWidget(openBtn);
    toolbarLayout->addWidget(saveBtn);
    toolbarLayout->addWidget(generateBtn);
    toolbarLayout->addWidget(playButton);
    toolbarLayout->addWidget(stopButton);
    toolbarLayout->addStretch();
    
    editorLayout->addLayout(toolbarLayout);
    
    mainSplitter->addWidget(editorPanel);
    
    // Правая панель - визуализация и информация
    QWidget *rightPanel = new QWidget();
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    
    // Визуализация волновой формы
    QGroupBox *waveformGroup = new QGroupBox("Визуализация волновой формы");
    QVBoxLayout *waveformLayout = new QVBoxLayout(waveformGroup);
    waveformWidget = new WaveformWidget();
    waveformLayout->addWidget(waveformWidget);
    rightLayout->addWidget(waveformGroup);
    
    // Информация о композиции
    QGroupBox *infoGroup = new QGroupBox("Информация о композиции");
    QVBoxLayout *infoLayout = new QVBoxLayout(infoGroup);
    
    bpmLabel = new QLabel("BPM: —");
    bpmLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    infoLayout->addWidget(bpmLabel);
    
    QLabel *instrumentsLabel = new QLabel("Инструменты:");
    instrumentsLabel->setStyleSheet("font-weight: bold; margin-top: 10px;");
    infoLayout->addWidget(instrumentsLabel);
    
    instrumentsList = new QListWidget();
    instrumentsList->setMaximumHeight(150);
    infoLayout->addWidget(instrumentsList);
    
    QLabel *patternsLabel = new QLabel("Паттерны:");
    patternsLabel->setStyleSheet("font-weight: bold; margin-top: 10px;");
    infoLayout->addWidget(patternsLabel);
    
    patternsList = new QListWidget();
    patternsList->setMaximumHeight(150);
    infoLayout->addWidget(patternsList);
    
    infoLayout->addStretch();
    rightLayout->addWidget(infoGroup);
    
    mainSplitter->addWidget(rightPanel);
    
    // Установить пропорции сплиттера
    mainSplitter->setStretchFactor(0, 2);
    mainSplitter->setStretchFactor(1, 1);
    
    mainLayout->addWidget(mainSplitter);
    
    // Строка состояния
    statusBar()->showMessage("Готов к работе");
    
    // Инициализация медиа-плеера
    audioOutput = new QAudioOutput(this);
    mediaPlayer = new QMediaPlayer(this);
    mediaPlayer->setAudioOutput(audioOutput);
    connect(mediaPlayer, &QMediaPlayer::playbackStateChanged, 
            this, &MainWindow::onPlayerStateChanged);
}

void MainWindow::setupMenuBar() {
    QMenuBar *menuBar = new QMenuBar(this);
    setMenuBar(menuBar);
    
    // Меню "Файл"
    QMenu *fileMenu = menuBar->addMenu("Файл");
    
    QAction *openAction = fileMenu->addAction("Открыть...");
    openAction->setShortcut(QKeySequence::Open);
    connect(openAction, &QAction::triggered, this, &MainWindow::openFile);
    
    QAction *saveAction = fileMenu->addAction("Сохранить");
    saveAction->setShortcut(QKeySequence::Save);
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveFile);
    
    fileMenu->addSeparator();
    
    QAction *exitAction = fileMenu->addAction("Выход");
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    
    // Меню "Генерация"
    QMenu *generateMenu = menuBar->addMenu("Генерация");
    
    QAction *generateAction = generateMenu->addAction("Сгенерировать WAV");
    generateAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_G));
    connect(generateAction, &QAction::triggered, this, &MainWindow::generateWav);
    
    // Меню "Воспроизведение"
    QMenu *playbackMenu = menuBar->addMenu("Воспроизведение");
    
    QAction *playAction = playbackMenu->addAction("Воспроизвести");
    playAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_P));
    connect(playAction, &QAction::triggered, this, &MainWindow::playAudio);
    
    QAction *stopAction = playbackMenu->addAction("Остановить");
    stopAction->setShortcut(QKeySequence(Qt::Key_Space));
    connect(stopAction, &QAction::triggered, this, &MainWindow::stopAudio);
    
    // Меню "Справка"
    QMenu *helpMenu = menuBar->addMenu("Справка");
    
    QAction *aboutAction = helpMenu->addAction("О программе");
    connect(aboutAction, &QAction::triggered, [this]() {
        QMessageBox::about(this, "О программе ITMO Loops",
            "ITMO Loops - Редактор музыкальных композиций\n\n"
            "Версия 1.0\n\n"
            "Программа для создания и редактирования\n"
            "музыкальных композиций в формате ITMO Loops.");
    });
}

void MainWindow::applyDarkTheme() {
    QString darkStyleSheet = R"(
        QMainWindow {
            background-color: #1e1e1e;
        }
        QWidget {
            background-color: #1e1e1e;
            color: #d4d4d4;
        }
        QTextEdit {
            background-color: #252526;
            color: #d4d4d4;
            border: 1px solid #3e3e42;
            border-radius: 3px;
            padding: 5px;
        }
        QListWidget {
            background-color: #252526;
            color: #d4d4d4;
            border: 1px solid #3e3e42;
            border-radius: 3px;
        }
        QGroupBox {
            border: 2px solid #3e3e42;
            border-radius: 5px;
            margin-top: 10px;
            font-weight: bold;
            color: #d4d4d4;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top left;
            padding: 5px 10px;
        }
        QPushButton {
            background-color: #0e639c;
            color: white;
            border: none;
            border-radius: 3px;
            padding: 8px 15px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #1177bb;
        }
        QPushButton:pressed {
            background-color: #0d5689;
        }
        QPushButton:disabled {
            background-color: #3e3e42;
            color: #6e6e6e;
        }
        QMenuBar {
            background-color: #2d2d30;
            color: #d4d4d4;
        }
        QMenuBar::item:selected {
            background-color: #3e3e42;
        }
        QMenu {
            background-color: #252526;
            color: #d4d4d4;
            border: 1px solid #3e3e42;
        }
        QMenu::item:selected {
            background-color: #0e639c;
        }
        QStatusBar {
            background-color: #007acc;
            color: white;
        }
        QLabel {
            color: #d4d4d4;
        }
    )";
    
    setStyleSheet(darkStyleSheet);
}

void MainWindow::openFile() {
    QString fileName = QFileDialog::getOpenFileName(this,
        "Открыть файл композиции", "",
        "ITMO Loops файлы (*.txt);;Все файлы (*)");
    
    if (fileName.isEmpty()) {
        return;
    }
    
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Ошибка", 
            "Не удалось открыть файл: " + fileName);
        return;
    }
    
    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);
    QString content = in.readAll();
    file.close();
    
    codeEditor->setPlainText(content);
    currentFilePath = fileName;
    
    showStatus("Файл загружен: " + QFileInfo(fileName).fileName());
    parseCurrentText();
}

void MainWindow::saveFile() {
    QString fileName = currentFilePath;
    
    if (fileName.isEmpty()) {
        fileName = QFileDialog::getSaveFileName(this,
            "Сохранить файл композиции", "",
            "ITMO Loops файлы (*.txt);;Все файлы (*)");
    }
    
    if (fileName.isEmpty()) {
        return;
    }
    
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Ошибка", 
            "Не удалось сохранить файл: " + fileName);
        return;
    }
    
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << codeEditor->toPlainText();
    file.close();
    
    currentFilePath = fileName;
    showStatus("Файл сохранен: " + QFileInfo(fileName).fileName());
}

void MainWindow::generateWav() {
    if (codeEditor->toPlainText().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Предупреждение", 
            "Редактор кода пуст. Загрузите или введите композицию.");
        return;
    }
    
    // Сохранить текст во временный файл
    QString tempInputPath = QDir::temp().filePath("itmoloops_temp_input.txt");
    QFile tempFile(tempInputPath);
    if (!tempFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось создать временный файл");
        return;
    }
    
    QTextStream out(&tempFile);
    out.setEncoding(QStringConverter::Utf8);
    out << codeEditor->toPlainText();
    tempFile.close();
    
    // Выбрать путь для сохранения WAV
    QString wavFileName = QFileDialog::getSaveFileName(this,
        "Сохранить WAV файл", "",
        "WAV файлы (*.wav);;Все файлы (*)");
    
    if (wavFileName.isEmpty()) {
        return;
    }
    
    showStatus("Генерация аудио...");
    QApplication::processEvents();
    
    // Вызвать функцию генерации из библиотеки
    int result = run_itmoloops(tempInputPath.toStdString(), 
                                wavFileName.toStdString());
    
    if (result != 0) {
        QMessageBox::critical(this, "Ошибка", 
            "Ошибка при генерации аудио. Проверьте код композиции.");
        showStatus("Ошибка генерации");
        return;
    }
    
    lastGeneratedWavPath = wavFileName;
    
    // Загрузить WAV для визуализации
    std::ifstream wavFile(wavFileName.toStdString(), std::ios::binary);
    if (wavFile.is_open()) {
        // Пропустить WAV заголовок
        wavFile.seekg(WAV_HEADER_SIZE);
        
        std::vector<float> samples;
        int16_t sample;
        while (wavFile.read(reinterpret_cast<char*>(&sample), sizeof(int16_t))) {
            samples.push_back(sample / 32768.0f);
        }
        wavFile.close();
        
        waveformWidget->setWaveformData(samples, 44100);
    }
    
    playButton->setEnabled(true);
    showStatus("Аудио файл сгенерирован: " + QFileInfo(wavFileName).fileName());
    
    QMessageBox::information(this, "Успех", 
        "WAV файл успешно сгенерирован!\n" + wavFileName);
}

void MainWindow::playAudio() {
    if (lastGeneratedWavPath.isEmpty()) {
        QMessageBox::warning(this, "Предупреждение", 
            "Сначала сгенерируйте WAV файл.");
        return;
    }
    
    mediaPlayer->setSource(QUrl::fromLocalFile(lastGeneratedWavPath));
    mediaPlayer->play();
    
    playButton->setEnabled(false);
    stopButton->setEnabled(true);
    showStatus("Воспроизведение...");
}

void MainWindow::stopAudio() {
    mediaPlayer->stop();
    playButton->setEnabled(true);
    stopButton->setEnabled(false);
    showStatus("Воспроизведение остановлено");
}

void MainWindow::onEditorTextChanged() {
    // Отложенный парсинг при изменении текста
    parseCurrentText();
}

void MainWindow::onPlayerStateChanged(QMediaPlayer::PlaybackState state) {
    if (state == QMediaPlayer::StoppedState) {
        playButton->setEnabled(true);
        stopButton->setEnabled(false);
        showStatus("Воспроизведение завершено");
    }
}

void MainWindow::parseCurrentText() {
    // Сохранить текст во временный файл для парсинга
    QString tempPath = QDir::temp().filePath("itmoloops_parse_temp.txt");
    QFile tempFile(tempPath);
    if (!tempFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }
    
    QTextStream out(&tempFile);
    out.setEncoding(QStringConverter::Utf8);
    out << codeEditor->toPlainText();
    tempFile.close();
    
    // Парсить
    ScoreData score;
    if (parseScore(tempPath.toStdString(), score)) {
        currentScore = score;
        updateSidebar();
    }
}

void MainWindow::updateSidebar() {
    // Обновить BPM
    bpmLabel->setText(QString("BPM: %1").arg(currentScore.bpm));
    
    // Обновить список инструментов
    instrumentsList->clear();
    for (const auto& [name, inst] : currentScore.instruments) {
        QString instInfo = QString::fromStdString(name) + " (" + 
                          QString::fromStdString(inst.type) + ")";
        instrumentsList->addItem(instInfo);
    }
    
    // Обновить список паттернов
    patternsList->clear();
    for (const auto& [name, pattern] : currentScore.patterns) {
        QString patternInfo = QString::fromStdString(name) + 
                             " (res: " + QString::number(pattern.res) + 
                             ", notes: " + QString::number(pattern.acts.size()) + ")";
        patternsList->addItem(patternInfo);
    }
}

void MainWindow::showStatus(const QString &message, int timeout) {
    statusBar()->showMessage(message, timeout);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *event) {
    const QMimeData *mimeData = event->mimeData();
    
    if (mimeData->hasUrls()) {
        QList<QUrl> urlList = mimeData->urls();
        if (!urlList.isEmpty()) {
            QString fileName = urlList.first().toLocalFile();
            
            if (fileName.endsWith(".txt")) {
                QFile file(fileName);
                if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    QTextStream in(&file);
                    in.setEncoding(QStringConverter::Utf8);
                    QString content = in.readAll();
                    file.close();
                    
                    codeEditor->setPlainText(content);
                    currentFilePath = fileName;
                    showStatus("Файл загружен: " + QFileInfo(fileName).fileName());
                    parseCurrentText();
                }
            } else if (fileName.endsWith(".wav")) {
                lastGeneratedWavPath = fileName;
                playButton->setEnabled(true);
                showStatus("WAV файл загружен: " + QFileInfo(fileName).fileName());
            }
        }
    }
}
