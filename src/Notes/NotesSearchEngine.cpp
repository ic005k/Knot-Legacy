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

void NotesSearchEngine::indexDocument(const QString &path,
                                      const QString &content) {
  QString cleaned = preprocessMarkdown(content);
  m_documents.insert(path, cleaned);

  QStringList keywords = tokenize(cleaned);
  for (const QString &keyword : keywords) {
    m_invertedIndex[keyword].insert(path);
  }
}

void NotesSearchEngine::buildIndexAsync(const QList<QString> &notePaths) {
  // 创建可报告进度的 FutureInterface
  QFutureInterface<void> futureInterface;
  futureInterface.reportStarted();
  futureInterface.setProgressRange(0, notePaths.size());

  QFuture<void> future = QtConcurrent::run([=]() mutable {
    for (int i = 0; i < notePaths.size(); ++i) {
      QFile file(notePaths[i]);
      if (file.open(QIODevice::ReadOnly)) {
        QString content = QString::fromUtf8(file.readAll());
        indexDocument(notePaths[i], content);
      }
      futureInterface.setProgressValue(i + 1);  // 正确设置进度
    }
    futureInterface.reportFinished();
  });

  m_indexWatcher.setFuture(futureInterface.future());
}

QList<QString> NotesSearchEngine::search(const QString &query) {
  QString cleanedQuery = preprocessMarkdown(query);
  QStringList queryTokens = tokenize(cleanedQuery);

  QSet<QString> results;
  for (const QString &token : queryTokens) {
    results.unite(m_invertedIndex.value(token, QSet<QString>()));
  }

  return results.values();
}

int NotesSearchEngine::documentCount() const { return m_documents.size(); }
