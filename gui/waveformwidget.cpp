#include "waveformwidget.hpp"
#include <QPainter>
#include <QPaintEvent>
#include <algorithm>

WaveformWidget::WaveformWidget(QWidget* parent)
    : QWidget(parent) {
    setMinimumHeight(100);
    setMaximumHeight(150);
}

void WaveformWidget::setWaveform(const std::vector<float>& samples) {
    waveform = samples;
    update();
}

void WaveformWidget::clear() {
    waveform.clear();
    update();
}

void WaveformWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Фон
    painter.fillRect(rect(), QColor(30, 30, 30));

    if (waveform.empty()) {
        painter.setPen(QColor(128, 128, 128));
        painter.drawText(rect(), Qt::AlignCenter, "Превью волновой формы появится после генерации");
        return;
    }

    // Рисуем волновую форму
    int w = width();
    int h = height();
    int midY = h / 2;

    painter.setPen(QPen(QColor(86, 156, 214), 1));

    // Определяем шаг для отображения
    size_t step = std::max(size_t(1), waveform.size() / size_t(w));

    for (int x = 0; x < w - 1; x++) {
        size_t idx1 = x * step;
        size_t idx2 = (x + 1) * step;

        if (idx1 >= waveform.size()) break;
        if (idx2 >= waveform.size()) idx2 = waveform.size() - 1;

        // Находим максимальное значение в этом диапазоне
        float maxVal = 0;
        for (size_t i = idx1; i <= idx2 && i < waveform.size(); i++) {
            maxVal = std::max(maxVal, std::abs(waveform[i]));
        }

        int y1 = midY - static_cast<int>(maxVal * midY);
        int y2 = midY + static_cast<int>(maxVal * midY);

        painter.drawLine(x, y1, x, y2);
    }

    // Центральная линия
    painter.setPen(QPen(QColor(60, 60, 60), 1));
    painter.drawLine(0, midY, w, midY);
}
