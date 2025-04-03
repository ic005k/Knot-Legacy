#include "database_manager.h"
#include <QDir>
#include <QFileInfo>
#include <QDateTime>
#include <QTextStream>
#include <QRegularExpression>

extern std::unique_ptr<cppjieba::Jieba> jieba;

// 词典资源路径
//const char* const DICT_PATH = ":/cppjieba/dict/jieba.dict.utf8";
//const char* const HMM_PATH = ":/cppjieba/dict/hmm_model.utf8";
//const char* const USER_DICT_PATH = ":/cppjieba/dict/user.dict.utf8";
//const char* const IDF_PATH = ":/cppjieba/dict/idf.utf8";
//const char* const STOP_WORD_PATH = ":/cppjieba/dict/stop_words.utf8";

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent)/*,
    m_jieba(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH)*/ {}

bool DatabaseManager::initDatabase(const QString &path) {
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(path);

    if (!m_db.open()) return false;
    setupDatabaseSchema();
    return true;
}

void DatabaseManager::setupDatabaseSchema() {
    m_db.exec("CREATE TABLE IF NOT EXISTS documents ("
              "id INTEGER PRIMARY KEY,"
              "path TEXT UNIQUE,"
              "modified INTEGER,"
              "content TEXT,"
              "terms TEXT)");

    m_db.exec("CREATE VIRTUAL TABLE IF NOT EXISTS fts_documents USING fts5(path, content, terms)");
}

void DatabaseManager::updateFileIndex(const QString &directory) {
    QDir dir(directory);
    auto files = dir.entryList({"*.md"}, QDir::Files | QDir::NoDotAndDotDot);

    // 开始事务
    m_db.transaction();

    // 清空旧数据
    m_db.exec("DELETE FROM documents");
    m_db.exec("DELETE FROM fts_documents");

    foreach (const QString &file, files) {
        processFile(dir.filePath(file));
    }

    // 提交事务
    if (!m_db.commit()) {
        qDebug() << "Commit error:" << m_db.lastError().text();
        m_db.rollback();
    }
}

void DatabaseManager::processFile(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    QTextStream in(&file);
    in.setCodec("UTF-8");
    QString content = in.readAll();
    file.close();

    QStringList terms = tokenize(content);
    qint64 modified = QFileInfo(file).lastModified().toSecsSinceEpoch();

    QSqlQuery q(m_db);
    q.prepare("INSERT INTO documents (path, modified, content, terms) "
              "VALUES (?, ?, ?, ?)");
    q.addBindValue(filePath);
    q.addBindValue(modified);
    q.addBindValue(content);
    q.addBindValue(terms.join(' '));
    q.exec();

    q.prepare("INSERT INTO fts_documents (path, content, terms) VALUES (?, ?, ?)");
    q.addBindValue(filePath);
    q.addBindValue(content);
    q.addBindValue(terms.join(' '));
    q.exec();
}

QStringList DatabaseManager::tokenize(const QString &text) {
    std::vector<std::string> words;
    QStringList result;

    // 分离中英文处理
    QString chinesePart;
    QString westernPart;

    for (const QChar &c : text) {
        if (c.unicode() >= 0x4E00 && c.unicode() <= 0x9FA5) {
            chinesePart.append(c);
        } else {
            westernPart.append(c);
        }
    }

    // 中文分词
    if (!chinesePart.isEmpty()) {
        std::string s = chinesePart.toStdString();
        jieba->CutForSearch(s, words);
    }

    // 西文分词
    QRegularExpression re("[^\\w]");
    QStringList westernWords = westernPart.split(re, Qt::SkipEmptyParts);

    // 合并结果
    for (const auto &w : words) {
        result << QString::fromStdString(w);
    }
    result.append(westernWords);

    return result.toSet().toList(); // 去重
}

QVector<DatabaseManager::SearchResult> DatabaseManager::searchDocuments(const QString &query,NoteIndexManager* indexManager,  int limit) {
    QStringList keywords = tokenize(query);

    QStringList conditions;
    for (const QString &kw : keywords) {
        QString escaped = kw;
        escaped.replace("'", "''");
        conditions << QString("(content MATCH '\"%1\"' OR terms MATCH '\"%1\"')").arg(escaped);
    }

    QString sql = QString("SELECT path, content FROM fts_documents WHERE %1 LIMIT %2")
                      .arg(conditions.join(" AND ")).arg(limit);

    QVector<SearchResult> results;
    QSqlQuery q(m_db);
    if (q.exec(sql)) {
        while (q.next()) {
            QString path = q.value(0).toString();
            QString content = q.value(1).toString();

            // results.append({path, extractPreview(content, keywords)});

            SearchResult item;
            item.filePath = path;
            item.preview = extractPreview(content, keywords);

            // 获取标题逻辑
            QString title = indexManager->getNoteTitle(path);
            item.title = !title.isEmpty() ? title : QFileInfo(path).baseName();

            results.append(item);
        }
    }
    return results;
}

QString DatabaseManager::extractPreview(const QString &content, const QStringList &keywords) {
    const int CONTEXT_LEN = 80;
    QString simplified = content.simplified();

    int firstPos = -1;
    for (const QString &kw : keywords) {
        int pos = simplified.indexOf(kw, 0, Qt::CaseInsensitive);
        if (pos != -1 && (firstPos == -1 || pos < firstPos)) {
            firstPos = pos;
        }
    }

    if (firstPos == -1) return simplified.left(CONTEXT_LEN) + "...";

    int start = qMax(0, firstPos - CONTEXT_LEN/2);
    int end = qMin(simplified.length(), start + CONTEXT_LEN);
    QString preview = simplified.mid(start, end - start);

    for (const QString &kw : keywords) {
        QRegularExpression re(QString("(%1)").arg(QRegularExpression::escape(kw)),
                              QRegularExpression::CaseInsensitiveOption);

        //以粗体显示
        //preview.replace(re, "<b>\\1</b>");

        preview.replace(re,
                        "<span style='background-color:#fff9c4; color:#c62828; padding:2px; border-radius:2px;'>\\1</span>");
    }

    return (start > 0 ? "..." : "") + preview + (end < simplified.length() ? "..." : "");
}

