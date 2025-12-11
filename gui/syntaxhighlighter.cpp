#include "syntaxhighlighter.hpp"

SyntaxHighlighter::SyntaxHighlighter(QTextDocument* parent)
    : QSyntaxHighlighter(parent) {
    
    // Ключевые слова
    keywordFormat.setForeground(QColor(86, 156, 214));  // Синий
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywords = {
        "bpm", "instrument", "pattern", "resolution", "end",
        "sampler", "sine", "triangle", "square",
        "sample", "root", "loop", "attack", "release", "duty",
        "effect", "gain", "delay", "decay", "freq", "depth"
    };
    for (const QString& word : keywords) {
        HighlightRule rule;
        rule.pattern = QRegularExpression("\\b" + word + "\\b");
        rule.format = keywordFormat;
        rules.append(rule);
    }

    // Числа
    numberFormat.setForeground(QColor(181, 206, 168));  // Зеленый
    HighlightRule numberRule;
    numberRule.pattern = QRegularExpression("\\b\\d+(\\.\\d+)?\\b");
    numberRule.format = numberFormat;
    rules.append(numberRule);

    // Комментарии
    commentFormat.setForeground(QColor(106, 153, 85));  // Темно-зеленый
    commentFormat.setFontItalic(true);
    HighlightRule commentRule;
    commentRule.pattern = QRegularExpression("#[^\n]*");
    commentRule.format = commentFormat;
    rules.append(commentRule);

    // Строки (пути к файлам)
    stringFormat.setForeground(QColor(206, 145, 120));  // Оранжевый
    HighlightRule stringRule;
    stringRule.pattern = QRegularExpression("=[^\\s]+");
    stringRule.format = stringFormat;
    rules.append(stringRule);
}

void SyntaxHighlighter::highlightBlock(const QString& text) {
    for (const HighlightRule& rule : rules) {
        QRegularExpressionMatchIterator it = rule.pattern.globalMatch(text);
        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}
