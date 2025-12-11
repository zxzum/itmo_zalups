#pragma once

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>

class SyntaxHighlighter : public QSyntaxHighlighter {
    Q_OBJECT

public:
    explicit SyntaxHighlighter(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightingRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QTextCharFormat keywordFormat;
    QTextCharFormat instrumentTypeFormat;
    QTextCharFormat effectFormat;
    QTextCharFormat noteFormat;
    QTextCharFormat commentFormat;
    QTextCharFormat numberFormat;
    QTextCharFormat parameterFormat;
};
