#pragma once

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QTableWidget>
#include <QListWidget>
#include <QLabel>
#include "waveformwidget.hpp"
#include "syntaxhighlighter.hpp"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    void browseScoreFile();
    void browseSamplesDir();
    void browseOutputFile();
    void loadScore();
    void generateWav();
    void openInPlayer();

private:
    void setupUI();
    void applyDarkTheme();
    void parseAndDisplayScore(const QString& content);
    void updateSummary();
    std::vector<float> loadWavForPreview(const QString& path);

    // UI элементы
    QLineEdit* scorePathEdit;
    QLineEdit* samplesDirEdit;
    QLineEdit* outputPathEdit;
    QPushButton* browseScoreBtn;
    QPushButton* browseSamplesDirBtn;
    QPushButton* loadScoreBtn;
    QPushButton* browseOutputBtn;
    QPushButton* generateBtn;
    QPushButton* openPlayerBtn;
    
    QTextEdit* codeEditor;
    QTableWidget* instrumentsTable;
    QListWidget* patternsList;
    QLabel* summaryLabel;
    QLabel* statusLabel;
    
    WaveformWidget* waveformWidget;
    SyntaxHighlighter* highlighter;

    // Данные
    QString currentScorePath;
    QString currentSamplesDir;
    QString currentOutputPath;
    QString baseDirectory;
};
