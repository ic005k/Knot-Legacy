#include "NotesSearchEngine.h"

#include <QDebug>
#include <QFile>
#include <QRegularExpression>

// 需要配置 jieba 词典路径（需自行下载或修改路径）
const char *const DICT_PATH = "dict/jieba.dict.utf8";
const char *const HMM_PATH = "dict/hmm_model.utf8";
const char *const USER_DICT_PATH = "dict/user.dict.utf8";

NotesSearchEngine::NotesSearchEngine(QObject *parent)
    : QObject(parent), m_jieba(DICT_PATH, HMM_PATH, USER_DICT_PATH) {
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
  // 移除代码块
  static QRegularExpression codeBlockRegex(R"(```[\s\S]*?```)");
  QString cleaned = content;
  cleaned.remove(codeBlockRegex);

  // 移除行内格式和链接
  cleaned.remove(QRegularExpression(R"((\*\*|__|\*|_|~~|`|!?\[.*?\]\(.*?\)))"));

  // 合并空白字符
  return cleaned.simplified();
}

QStringList NotesSearchEngine::tokenize(const QString &text) const {
  QStringList tokens;

  // 判断是否为非中文（包含任意非CJK字符）
  bool isNonChinese = std::any_of(text.begin(), text.end(), [](QChar ch) {
    return ch.script() != QChar::Script_Han &&
           ch.script() != QChar::Script_Katakana &&
           ch.script() != QChar::Script_Hiragana;
  });

  if (isNonChinese) {
    // 英文/德文等：按非字母数字字符切分
    tokens = text.toLower().split(QRegularExpression("[^\\p{L}0-9_]+"),
                                  QString::SkipEmptyParts);
  } else {
    // 中文：使用分词库
    std::vector<std::string> words;
    m_jieba.Cut(text.toStdString(), words);
    for (const auto &word : words) {
      tokens.append(QString::fromStdString(word));
    }
  }

  return tokens;
}

// 在索引文档时，将内容分割为段落，并记录每个关键词的位置
void NotesSearchEngine::indexDocument(const QString &path,
                                      const QString &content) {
  QString cleaned = preprocessMarkdown(content);
  QStringList paragraphs =
      cleaned.split("\n", QString::SkipEmptyParts);  // 按换行符分割段落
  m_documentParagraphs.insert(path, paragraphs);
  m_documents.insert(path, cleaned);

  for (int paraIndex = 0; paraIndex < paragraphs.size(); ++paraIndex) {
    const QString paragraph = paragraphs[paraIndex];
    QStringList keywords = tokenize(paragraph);

    for (const QString &keyword : keywords) {
      // 查找关键词在段落中的所有出现位置
      int pos = 0;
      while ((pos = paragraph.indexOf(keyword, pos, Qt::CaseInsensitive)) !=
             -1) {
        KeywordPosition position;
        position.paragraphIndex = paraIndex;
        position.charStart = pos;
        position.charEnd = pos + keyword.length();
        position.context =
            paragraph.mid(qMax(0, pos - 20), 40);  // 取前后20字符作为上下文

        m_invertedIndex[keyword][path].append(position);
        pos += keyword.length();
      }
    }
  }
}

void NotesSearchEngine::buildIndexAsync(const QList<QString> &notePaths) {
  // 清空现有索引
  {
    QMutexLocker locker(&m_mutex);
    m_invertedIndex.clear();
    m_documents.clear();
    m_documentParagraphs.clear();
  }

  QFuture<void> future = QtConcurrent::run([this, notePaths]() {
    for (const QString &path : notePaths) {
      QFile file(path);
      if (file.open(QIODevice::ReadOnly)) {
        QString content = QString::fromUtf8(file.readAll());
        indexDocument(path, content);
      }
    }
  });
  m_indexWatcher.setFuture(future);
}

// NotesSearchEngine.cpp
QList<SearchResult> NotesSearchEngine::search(const QString &query) {
  QString cleanedQuery = preprocessMarkdown(query);
  QStringList queryKeywords = tokenize(cleanedQuery);

  QHash<QString, QList<KeywordPosition>> combinedResults;

  // 合并所有查询关键词的匹配位置
  for (const QString &keyword : queryKeywords) {
    auto it = m_invertedIndex.find(keyword);
    if (it != m_invertedIndex.end()) {
      for (auto docIt = it->begin(); docIt != it->end(); ++docIt) {
        combinedResults[docIt.key()].append(docIt.value());
      }
    }
  }

  // 转换为 SearchResult 列表
  QList<SearchResult> results;
  for (auto it = combinedResults.begin(); it != combinedResults.end(); ++it) {
    SearchResult result;
    result.filePath = it.key();
    result.positions = it.value();
    results.append(result);
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
