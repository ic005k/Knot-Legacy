#include "NotesSearchEngine.h"

#include <QDebug>
#include <QFile>
#include <QRegularExpression>

NotesSearchEngine::NotesSearchEngine(QObject *parent) : QObject(parent) {
  // 注册 KeywordPosition 以便与 Qt 信号槽兼容
  qRegisterMetaType<KeywordPosition>("KeywordPosition");
  qRegisterMetaTypeStreamOperators<KeywordPosition>("KeywordPosition");

  connect(&m_indexWatcher, &QFutureWatcher<void>::progressValueChanged, this,
          [this](int progress) {
            // 计算当前进度和总任务数
            int total = m_indexWatcher.progressMaximum();
            emit indexBuildProgress(progress, total);
          });

  connect(&m_indexWatcher, &QFutureWatcher<void>::finished, this,
          &NotesSearchEngine::indexBuildFinished);
}

QString NotesSearchEngine::preprocessMarkdown(const QString &content) const {
  // 1. 移除代码块
  static QRegularExpression codeBlockRegex(R"(```[\s\S]*?```)");
  QString cleaned = content;
  cleaned.remove(codeBlockRegex);

  // 2. 移除行内格式符号（但保留文本）
  cleaned.replace(QRegularExpression(R"((\*\*|__|\*|_|~~|`))"), "");

  // 3. 移除链接（保留链接文字）
  cleaned.remove(QRegularExpression(R"(!?\[(.*?)\]\(.*?\))"));
  cleaned.replace(QRegularExpression(R"(\[(.*?)\]\(.*?\))"),
                  "\\1");  // 保留链接文字

  return cleaned.simplified();
}

QStringList NotesSearchEngine::tokenize(const QString &text) const {
  QStringList tokens;

  // 判断是否为非中文（包含任意非CJK字符）
  bool hasNonChinese = std::any_of(text.begin(), text.end(), [](QChar ch) {
    return ch.script() != QChar::Script_Han &&
           ch.script() != QChar::Script_Katakana &&
           ch.script() != QChar::Script_Hiragana;
  });

  if (hasNonChinese) {
    // 英文/德文等：按非字母数字字符切分，并转为小写
    tokens = text.toLower().split(QRegularExpression("[^\\p{L}0-9_]+"),
                                  QString::SkipEmptyParts);
  } else {
    // 中文：按单字切分（过滤标点和空格）
    for (QChar ch : text) {
      if (ch.isLetter() && (ch.script() == QChar::Script_Han ||
                            ch.script() == QChar::Script_Katakana ||
                            ch.script() == QChar::Script_Hiragana)) {
        tokens.append(ch);
      }
    }
  }

  return tokens;
}

// 在索引文档时，将内容分割为段落，并记录每个关键词的位置
void NotesSearchEngine::indexDocument(const QString &path,
                                      const QString &content) {
  QMutexLocker locker(&m_mutex);
  QString cleaned = preprocessMarkdown(content);

  // 1. 彻底清理旧索引
  if (m_documents.contains(path)) {
    // 获取旧内容并分词
    QString oldContent = m_documents[path];
    QStringList oldKeywords = tokenize(oldContent);

    // 遍历所有旧关键词，完全移除路径关联
    for (const QString &keyword : oldKeywords) {
      if (m_invertedIndex.contains(keyword)) {
        QHash<QString, QList<KeywordPosition>> &docMap =
            m_invertedIndex[keyword];
        docMap.remove(path);  // 直接删除路径条目
        if (docMap.isEmpty()) {
          m_invertedIndex.remove(keyword);  // 清理空关键词
        }
      }
    }
    m_documents.remove(path);
    m_documentParagraphs.remove(path);
  }

  // 2. 预处理并插入新内容
  m_documents[path] = cleaned;
  QStringList paragraphs = cleaned.split("\n", QString::SkipEmptyParts);
  m_documentParagraphs[path] = paragraphs;

  // 3. 构建新索引
  for (int paraIndex = 0; paraIndex < paragraphs.size(); ++paraIndex) {
    const QString paragraph = paragraphs[paraIndex];
    QStringList keywords = tokenize(paragraph);
    for (const QString &keyword : keywords) {
      // 查找关键词在段落中的所有位置
      int pos = 0;
      while ((pos = paragraph.indexOf(keyword, pos, Qt::CaseInsensitive)) !=
             -1) {
        KeywordPosition position;
        position.paragraphIndex = paraIndex;
        position.charStart = pos;
        position.charEnd = pos + keyword.length();
        position.context = paragraph.mid(qMax(0, pos - 20), 40);  // 上下文截取
        m_invertedIndex[keyword][path].append(position);
        pos += keyword.length();
      }
    }
  }
}

void NotesSearchEngine::buildIndexAsync(const QList<QString> &notePaths,
                                        bool fullRebuild) {
  // 仅在 fullRebuild 为 true 时清空索引
  if (fullRebuild) {
    QMutexLocker locker(&m_mutex);
    m_invertedIndex.clear();
    m_documents.clear();
    m_documentParagraphs.clear();
    qDebug() << "已清空旧索引（全量构建）";
  }

  QFuture<void> future = QtConcurrent::run([this, notePaths]() {
    for (const QString &path : notePaths) {
      QFile file(path);
      if (file.open(QIODevice::ReadOnly)) {
        QString content = QString::fromUtf8(file.readAll());
        indexDocument(path, content);  // 新增或更新文档
      }
    }
  });
  m_indexWatcher.setFuture(future);
}

QList<SearchResult> NotesSearchEngine::search(const QString &query) {
  QString cleanedQuery = preprocessMarkdown(query);
  QStringList queryKeywords = tokenize(cleanedQuery);

  QHash<QString, QList<KeywordPosition>> combinedResults;

  // 合并所有查询关键词的匹配位置（使用 += 展开列表）
  for (const QString &keyword : queryKeywords) {
    auto it = m_invertedIndex.find(keyword);
    if (it != m_invertedIndex.end()) {
      for (auto docIt = it->begin(); docIt != it->end(); ++docIt) {
        combinedResults[docIt.key()] += docIt.value();  // 合并列表而非嵌套
      }
    }
  }

  // 去重：对每个文档的位置列表按段落和起始位置去重
  QList<SearchResult> results;
  QSet<QString> seenPaths;
  for (auto it = combinedResults.begin(); it != combinedResults.end(); ++it) {
    if (!seenPaths.contains(it.key())) {
      SearchResult result;
      result.filePath = it.key();

      // 去重位置列表
      QSet<QPair<int, int>> uniquePositions;
      QList<KeywordPosition> filtered;
      for (const KeywordPosition &pos : it.value()) {
        QPair<int, int> key(pos.paragraphIndex, pos.charStart);
        if (!uniquePositions.contains(key)) {
          uniquePositions.insert(key);
          filtered.append(pos);
        }
      }
      result.positions = filtered;
      results.append(result);
      seenPaths.insert(it.key());
    }
  }
  return results;
}

int NotesSearchEngine::documentCount() const { return m_documents.size(); }

void NotesSearchEngine::saveIndex(const QString &path) {
  QFile file(path);
  if (file.open(QIODevice::WriteOnly)) {
    QDataStream stream(&file);
    stream << m_invertedIndex << m_documentParagraphs;
  }
}

void NotesSearchEngine::loadIndex(const QString &path) {
  QFile file(path);
  if (file.open(QIODevice::ReadOnly)) {
    QDataStream stream(&file);
    stream >> m_invertedIndex >> m_documentParagraphs;
  }
}

bool NotesSearchEngine::hasDocument(const QString &path) const {
  return m_documents.contains(path);
}

// NotesSearchEngine.cpp
QDataStream &operator<<(QDataStream &out, const KeywordPosition &pos) {
  out << pos.paragraphIndex << pos.charStart << pos.charEnd << pos.context;
  return out;
}

QDataStream &operator>>(QDataStream &in, KeywordPosition &pos) {
  in >> pos.paragraphIndex >> pos.charStart >> pos.charEnd >> pos.context;
  return in;
}
