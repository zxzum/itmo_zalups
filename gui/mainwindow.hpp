#pragma once

#include <QMainWindow>
#include <QTextEdit>
#include <QLabel>
#include <QListWidget>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QPushButton>
#include "syntaxhighlighter.hpp"
#include "waveformwidget.hpp"
#include "parser.hpp"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private slots:
    void openFile();
    void saveFile();
    void generateWav();
    void playAudio();
    void stopAudio();
    void onEditorTextChanged();
    void onPlayerStateChanged(QMediaPlayer::PlaybackState state);

private:
    void setupUi();
    void setupMenuBar();
    void applyDarkTheme();
    void updateSidebar();
    void parseCurrentText();
    void showStatus(const QString &message, int timeout = 3000);
    
    // UI элементы
    QTextEdit *codeEditor;
    WaveformWidget *waveformWidget;
    QListWidget *instrumentsList;
    QListWidget *patternsList;
    QLabel *bpmLabel;
    QPushButton *playButton;
    QPushButton *stopButton;
    
    // Подсветка синтаксиса
    SyntaxHighlighter *syntaxHighlighter;
    
    // Медиа-плеер
    QMediaPlayer *mediaPlayer;
    QAudioOutput *audioOutput;
    
    // Данные
    QString currentFilePath;
    QString lastGeneratedWavPath;
    ScoreData currentScore;
};
