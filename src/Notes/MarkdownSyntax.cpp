// MarkdownSyntax.cpp
#include "MarkdownSyntax.h"

#include <QColor>

MarkdownLexer::MarkdownLexer(QObject *parent)
    : QsciLexerCustom(parent)  // 调用基类构造函数
{
  initSyntaxRules();
  initDefaultTheme();
}

void MarkdownLexer::initDefaultTheme() {
  // 初始化所有样式状态
  setColor(Qt::black, Default);
  setPaper(Qt::white, Default);
  setFont(QFont("Arial", 12), Default);

  setColor(QColor("#4EC9B0"), CodeLangIdentifier);
  setPaper(QColor("#282C34"), CodeLangIdentifier);
  setFont(QFont("Consolas", 10, QFont::Bold), CodeLangIdentifier);
}

void MarkdownLexer::initSyntaxRules() {
  QRegularExpression::PatternOptions options =
      QRegularExpression::OptimizeOnFirstUsageOption |
      QRegularExpression::DontCaptureOption;

  m_rules = {// 代码块规则（优先级最高）
             {QRegularExpression(R"(^```(\w*)\s*$)"), CodeBlock, 1},
             // 标题（仅匹配#符号）
             {QRegularExpression(R"(^(#{1,6})(?=\s|$|\S))"), Header1, 1},
             // 列表项
             {QRegularExpression(R"(^\s*([-*+])\s)"), ListItem, 1},
             // 强调和加粗（严格边界）
             {QRegularExpression(R"(\*\*(.+?)\*\*)"), Strong, 2},
             {QRegularExpression(R"(\*(.+?)\*)"), Emphasis, 2}};
}

QColor MarkdownLexer::defaultColor(int style) const {
  switch (style) {
    case Header1:
      return QColor("#FF0000");
    case Header2:
      return QColor("#FFA500");
    case Header3:
      return QColor("#008000");
    case ListItem:
      return QColor("#800080");
    case CodeBlock:
      return QColor("#444444");
    case CodeLangIdentifier:
      return QColor("#4EC9B0");
    // 补充其他样式...
    default:
      return QColor("#000000");
  }
}

QString MarkdownLexer::description(int style) const {
  static const QStringList desc = {
      "Default",         "Header1",       "Header2",    "Header3",
      "Header4",         "Header5",       "Header6",    "List Item",
      "Code Block",      "Code Language", "Blockquote", "Link",
      "Image",           "Emphasis",      "Strong",     "Strikethrough",
      "Horizontal Rule", "Table"};
  return (style >= 0 && style < desc.size()) ? desc[style] : "";
}

const char *MarkdownLexer::language() const { return "Markdown"; }

void MarkdownLexer::handleCodeBlock(int lineStart, int lineEnd,
                                    const QString &text) {
  QsciScintilla *sci = editor();
  static QRegularExpression startRe(R"(^(```|~~~)\s*(\w*)\s*$)");
  static QRegularExpression endRe(R"(^(```|~~~)\s*$)");

  auto match = startRe.match(text);
  if (match.hasMatch() && m_currentState != CodeBlockState) {
    // 代码块开始
    m_codeBlockDelimiter = match.captured(1);
    m_currentState = CodeBlockState;
    // 高亮语言标识符
    if (match.capturedLength(2) > 0) {
      sci->SendScintilla(QsciScintilla::SCI_SETSTYLING, match.capturedStart(2),
                         CodeBlock);
      sci->SendScintilla(QsciScintilla::SCI_SETSTYLING, match.capturedLength(2),
                         CodeLangIdentifier);
    }
  } else if (endRe.match(text).hasMatch() &&
             text.trimmed() == m_codeBlockDelimiter) {
    // 代码块结束
    m_currentState = Normal;
    m_codeBlockDelimiter.clear();
  }

  // 始终应用代码块样式
  sci->SendScintilla(QsciScintilla::SCI_SETSTYLING, text.length(), CodeBlock);
}

// 在MarkdownSyntax.cpp文件顶部添加头文件
#include <Qsci/qsciscintilla.h>

// 修改handleNormalState中的消息调用
void MarkdownLexer::handleNormalState(int lineStart, const QString &text) {
  QsciScintilla *sci = editor();
  if (!sci) return;

  int styledPos = 0;

  for (const SyntaxRule &rule : m_rules) {
    auto match = rule.pattern.match(text);
    if (match.hasMatch()) {
      int pos = match.capturedStart(rule.captureGroup);
      int len = match.capturedLength(rule.captureGroup);

      // 计算全局位置并限制范围
      int globalStart = qBound(lineStart, lineStart + pos, sci->length());
      int safeLen = qMin(len, sci->length() - globalStart);

      // 使用QsciScintilla枚举
      sci->SendScintilla(QsciScintilla::SCI_STARTSTYLING, globalStart, 0x1f);
      sci->SendScintilla(QsciScintilla::SCI_SETSTYLING, safeLen, rule.type);

      styledPos = pos + len;
    }
  }

  // 处理剩余文本
  if (styledPos < text.length()) {
    int remaining = text.length() - styledPos;
    int globalRemainingStart = qMin(lineStart + styledPos, sci->length());
    int safeRemaining = qMin(remaining, sci->length() - globalRemainingStart);

    sci->SendScintilla(QsciScintilla::SCI_STARTSTYLING, globalRemainingStart,
                       0x1f);
    sci->SendScintilla(QsciScintilla::SCI_SETSTYLING, safeRemaining, Default);
  }
}

void MarkdownLexer::updateAllStyles() {
  for (auto it = m_styleFormats.constBegin(); it != m_styleFormats.constEnd();
       ++it) {
    const StyleFormat &fmt = it.value();
    setColor(fmt.color, it.key());
    setPaper(fmt.paper, it.key());
    setFont(fmt.font, it.key());
  }
}

void MarkdownLexer::setBaseFont(const QFont &font) {
  m_baseFont = font;
  // 更新所有相关样式
  for (auto &fmt : m_styleFormats) {
    if (fmt.font.family().isEmpty()) {
      fmt.font = m_baseFont;
    }
  }
  updateAllStyles();
}

void MarkdownLexer::applyVividTheme(bool darkMode) {
  if (darkMode) {
    // 暗色主题
    setPaper(QColor("#282C34"));           // 背景-深灰蓝
    setColor(QColor("#ABB2BF"), Default);  // 默认文字颜色

    setColor(QColor("#E06C75"), Header1);    // 标题1-珊瑚红
    setColor(QColor("#98C379"), Header2);    // 标题2-草木绿
    setColor(QColor("#61AFEF"), Header3);    // 标题3-天空蓝
    setColor(QColor("#C678DD"), CodeBlock);  // 代码块-紫罗兰
    setColor(QColor("#E5C07B"), Link);       // 链接-琥珀黄
    setPaper(QColor("#282C34"));             // 背景-深灰蓝
    // setFont(QFont("Consolas", 12));          // 等宽字体
  } else {
    // 亮色主题
    setPaper(QColor("#F5F5F5"));           // 背景-浅灰白
    setColor(QColor("#000000"), Default);  // 默认文字颜色

    setColor(QColor("#2F4F4F"), Header1);    // 标题1-石板灰
    setColor(QColor("#8B4513"), Header2);    // 标题2-鞍棕
    setColor(QColor("#006400"), Header3);    // 标题3-森林绿
    setColor(QColor("#483D8B"), CodeBlock);  // 代码块-暗蓝
    setColor(QColor("#0000FF"), Link);       // 链接-标准蓝
    setPaper(QColor("#F5F5F5"));             // 背景-浅灰白
    // setFont(QFont("Segoe UI", 11));          // 无衬线字体
  }

  // 修改链接字体
  QFont linkFont = font(Link);  // 使用Link样式的字体
  linkFont.setUnderline(true);
  setFont(linkFont, Link);  // 必须指定样式编号

  // 必须显式设置默认样式背景
  setPaper(paper(Default), Default);
  setColor(color(Default), Default);
}

void MarkdownLexer::styleText(int start, int end) {
  QsciScintilla *sci = editor();
  if (!sci || start >= end) return;

  // 使用QsciScintilla提供的消息枚举
  const int SCI_STARTSTYLING = QsciScintilla::SCI_STARTSTYLING;
  const int SCI_SETSTYLING = QsciScintilla::SCI_SETSTYLING;

  // 初始化全局样式（安全范围检查）
  start = qBound(0, start, sci->length());
  end = qBound(start, end, sci->length());

  sci->SendScintilla(SCI_STARTSTYLING, start, 0x1f);
  sci->SendScintilla(SCI_SETSTYLING, end - start, Default);

  // 获取行范围（安全处理空行）
  int startLine =
      sci->SendScintilla(QsciScintilla::SCI_LINEFROMPOSITION, start);
  int endLine = sci->SendScintilla(QsciScintilla::SCI_LINEFROMPOSITION, end);
  if (startLine < 0) startLine = 0;
  if (endLine >= sci->lines()) endLine = sci->lines() - 1;

  for (int line = startLine; line <= endLine; ++line) {
    // 安全获取行位置
    int lineStart =
        sci->SendScintilla(QsciScintilla::SCI_POSITIONFROMLINE, line);
    lineStart = qMax(lineStart, 0);
    int lineEnd =
        sci->SendScintilla(QsciScintilla::SCI_GETLINEENDPOSITION, line);
    lineEnd = qMin(lineEnd, sci->length());

    // 跳过无效行
    if (lineStart >= lineEnd || lineStart >= end) continue;

    // 初始化行样式（带边界检查）
    sci->SendScintilla(SCI_STARTSTYLING, lineStart, 0x1f);
    sci->SendScintilla(SCI_SETSTYLING, lineEnd - lineStart, Default);

    // 获取行文本（带缓冲区溢出保护）
    QByteArray bytes(lineEnd - lineStart + 2, 0);
    bytes[bytes.size() - 1] = '\0';  // 确保终止符
    sci->SendScintilla(QsciScintilla::SCI_GETTEXTRANGE, lineStart, lineEnd,
                       bytes.data());
    QString text = QString::fromUtf8(bytes.constData()).trimmed();

    // 状态处理（带递归深度限制）
    static int recursionDepth = 0;
    if (recursionDepth > 10) {
      qWarning() << "Style recursion too deep";
      return;
    }
    recursionDepth++;

    ParserState prevState = m_currentState;
    m_currentState =
        static_cast<ParserState>(m_blockStates.value(line - 1, Normal));

    if (m_currentState == CodeBlockState) {
      handleCodeBlock(lineStart, lineEnd, text);
    } else {
      handleNormalState(lineStart, text);
    }

    if (m_currentState != prevState) {
      m_blockStates[line] = m_currentState;
    } else {
      m_blockStates.remove(line);
    }

    recursionDepth--;
  }

  // 安全设置换行模式
  int wrapMode = (m_currentState == CodeBlockState)
                     ? QsciScintilla::SC_WRAP_NONE
                     : QsciScintilla::SC_WRAP_WORD;
  sci->SendScintilla(QsciScintilla::SCI_SETWRAPMODE, wrapMode);
}
