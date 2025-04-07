// MarkdownSyntax.h
#pragma once

#include <Qsci/qscilexercustom.h>
#include <Qsci/qsciscintilla.h>

#include <QDebug>
#include <QHash>
#include <QRegularExpression>
#include <QVector>

class MarkdownLexer : public QsciLexerCustom {
  Q_OBJECT
 public:
  enum TokenType {
    Default = 0,
    Header1,
    Header2,
    Header3,
    Header4,
    Header5,
    Header6,
    ListItem,
    CodeBlock,
    CodeLang,
    Blockquote,
    Link,
    Image,
    Emphasis,
    Strong,
    StrikeThrough,
    HorizontalRule,
    CodeLangIdentifier,
    Table
  };

  explicit MarkdownLexer(QObject *parent = nullptr);

  // QsciLexerCustom接口实现
  const char *language() const override;
  QString description(int style) const override;
  QColor defaultColor(int style) const override;

  void styleText(int start, int end) override;

  void setBaseFont(const QFont &font);

  void applyVividTheme(bool darkMode);

 private:
  enum ParserState { Normal, CodeBlockState };

  struct SyntaxRule {
    QRegularExpression pattern;
    TokenType type;
    int captureGroup;
  };

  // 语法规则
  QVector<SyntaxRule> m_rules;

  // 状态管理
  ParserState m_currentState = Normal;
  QString m_codeBlockDelimiter;
  QHash<int, int> m_blockStates;  // line number -> state

  void initSyntaxRules();
  void handleCodeBlock(int lineStart, int lineEnd, const QString &text);
  void handleNormalState(int lineStart, const QString &text);
  void applyStyle(int start, int length, TokenType type);

  struct StyleFormat {
    QColor color;
    QColor paper;
    QFont font;
  };

  QHash<int, StyleFormat> m_styleFormats;
  QFont m_baseFont;

  void updateAllStyles();
  void initDefaultTheme();
};
