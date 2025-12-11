#include "syntaxhighlighter.hpp"

SyntaxHighlighter::SyntaxHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent) {
    
    HighlightingRule rule;

    // Ключевые слова (keywords)
    keywordFormat.setForeground(QColor(86, 156, 214)); // Синий
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns = {
        "\\bbpm\\b", "\\binstrument\\b", "\\bpattern\\b", 
        "\\beffect\\b", "\\bend\\b", "\\bresolution\\b"
    };
    for (const QString &pattern : keywordPatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    // Типы инструментов
    instrumentTypeFormat.setForeground(QColor(78, 201, 176)); // Бирюзовый
    instrumentTypeFormat.setFontWeight(QFont::Bold);
    QStringList instrumentTypes = {
        "\\bsampler\\b", "\\bsine\\b", "\\btriangle\\b", "\\bsquare\\b"
    };
    for (const QString &pattern : instrumentTypes) {
        rule.pattern = QRegularExpression(pattern);
        rule.format = instrumentTypeFormat;
        highlightingRules.append(rule);
    }

    // Эффекты
    effectFormat.setForeground(QColor(220, 220, 170)); // Желтоватый
    effectFormat.setFontWeight(QFont::Bold);
    QStringList effectPatterns = {
        "\\bgain\\b", "\\becho\\b", "\\btremolo\\b"
    };
    for (const QString &pattern : effectPatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.format = effectFormat;
        highlightingRules.append(rule);
    }

    // Ноты (C0-B9)
    noteFormat.setForeground(QColor(181, 206, 168)); // Зеленоватый
    rule.pattern = QRegularExpression("\\b[A-G][#b]?[0-9]\\b");
    rule.format = noteFormat;
    highlightingRules.append(rule);

    // Параметры (key=value)
    parameterFormat.setForeground(QColor(156, 220, 254)); // Голубой
    rule.pattern = QRegularExpression("\\b\\w+\\s*=");
    rule.format = parameterFormat;
    highlightingRules.append(rule);

    // Числа
    numberFormat.setForeground(QColor(181, 206, 168)); // Зеленоватый
    rule.pattern = QRegularExpression("\\b\\d+\\.?\\d*\\b");
    rule.format = numberFormat;
    highlightingRules.append(rule);

    // Комментарии (должны быть последними, чтобы иметь приоритет)
    commentFormat.setForeground(QColor(106, 153, 85)); // Темно-зеленый
    commentFormat.setFontItalic(true);
    rule.pattern = QRegularExpression("#[^\n]*");
    rule.format = commentFormat;
    highlightingRules.append(rule);
}

void SyntaxHighlighter::highlightBlock(const QString &text) {
    for (const HighlightingRule &rule : highlightingRules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}
