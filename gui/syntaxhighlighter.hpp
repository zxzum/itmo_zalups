#pragma once

#include <QSyntaxHighlighter>
#include <QTextDocument>
#include <QRegularExpression>

class SyntaxHighlighter : public QSyntaxHighlighter {
    Q_OBJECT

public:
    explicit SyntaxHighlighter(QTextDocument* parent = nullptr);

protected:
    void highlightBlock(const QString& text) override;

private:
    struct HighlightRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightRule> rules;

    QTextCharFormat keywordFormat;
    QTextCharFormat numberFormat;
    QTextCharFormat commentFormat;
    QTextCharFormat stringFormat;
};
