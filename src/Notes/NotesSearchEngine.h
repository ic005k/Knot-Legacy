#pragma once
#include <QFutureWatcher>
#include <QHash>
#include <QObject>
#include <QSet>
#include <QtConcurrent>

#include "cppjieba/Jieba.hpp"

class NotesSearchEngine : public QObject {
  Q_OBJECT

 public:
  explicit NotesSearchEngine(QObject *parent = nullptr);

  // 添加/更新文档到索引
  void indexDocument(const QString &path, const QString &content);

  // 异步批量构建索引
  void buildIndexAsync(const QList<QString> &notePaths);

  // 执行搜索
  QList<QString> search(const QString &query);

  // 添加文档数量查询方法
  int documentCount() const;

 signals:
  void indexBuildProgress(int current, int total);
  void indexBuildFinished();

 private:
  // 预处理 Markdown
  QString preprocessMarkdown(const QString &content) const;

  // 中文分词
  QStringList tokenize(const QString &text) const;

  // 倒排索引结构
  QHash<QString, QSet<QString>> m_invertedIndex;
  QHash<QString, QString> m_documents;

  // 异步索引构建
  QFutureWatcher<void> m_indexWatcher;
  cppjieba::Jieba m_jieba;
};
