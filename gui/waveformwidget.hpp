#pragma once

#include <QWidget>
#include <vector>

class WaveformWidget : public QWidget {
    Q_OBJECT

public:
    explicit WaveformWidget(QWidget *parent = nullptr);
    void setWaveformData(const std::vector<float> &data, int sampleRate);
    void clear();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    std::vector<float> waveformData;
    int sampleRate;
};
