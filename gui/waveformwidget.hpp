#pragma once

#include <QWidget>
#include <vector>

class WaveformWidget : public QWidget {
    Q_OBJECT

public:
    explicit WaveformWidget(QWidget* parent = nullptr);
    void setWaveform(const std::vector<float>& samples);
    void clear();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    std::vector<float> waveform;
    int downsampleRate = 100;  // Показывать каждый N-ый семпл
};
