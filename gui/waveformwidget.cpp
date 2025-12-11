#include "waveformwidget.hpp"
#include <QPainter>
#include <QPaintEvent>
#include <cmath>
#include <algorithm>

WaveformWidget::WaveformWidget(QWidget *parent)
    : QWidget(parent), sampleRate(44100) {
    setMinimumHeight(100);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

void WaveformWidget::setWaveformData(const std::vector<float> &data, int sr) {
    waveformData = data;
    sampleRate = sr;
    update();
}

void WaveformWidget::clear() {
    waveformData.clear();
    update();
}

void WaveformWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Фон
    painter.fillRect(rect(), QColor(30, 30, 30));
    
    if (waveformData.empty()) {
        painter.setPen(QColor(100, 100, 100));
        painter.drawText(rect(), Qt::AlignCenter, "Нет данных для визуализации");
        return;
    }
    
    // Рисуем центральную линию
    int midY = height() / 2;
    painter.setPen(QPen(QColor(60, 60, 60), 1));
    painter.drawLine(0, midY, width(), midY);
    
    // Рисуем волновую форму
    painter.setPen(QPen(QColor(78, 201, 176), 1));
    
    int w = width();
    int h = height();
    size_t dataSize = waveformData.size();
    
    if (dataSize < 2) return;
    
    // Вычисляем количество сэмплов на пиксель
    double samplesPerPixel = static_cast<double>(dataSize) / w;
    
    for (int x = 0; x < w - 1; ++x) {
        size_t startIdx = static_cast<size_t>(x * samplesPerPixel);
        size_t endIdx = static_cast<size_t>((x + 1) * samplesPerPixel);
        endIdx = std::min(endIdx, dataSize);
        
        // Найти минимальное и максимальное значение в этом диапазоне
        float minVal = 0.0f, maxVal = 0.0f;
        for (size_t i = startIdx; i < endIdx; ++i) {
            minVal = std::min(minVal, waveformData[i]);
            maxVal = std::max(maxVal, waveformData[i]);
        }
        
        // Масштабируем к размеру виджета
        int y1 = midY - static_cast<int>(maxVal * (h / 2 - 5));
        int y2 = midY - static_cast<int>(minVal * (h / 2 - 5));
        
        painter.drawLine(x, y1, x, y2);
    }
}
