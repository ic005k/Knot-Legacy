#pragma once
#include <QDebug>
#include <QDirIterator>
#include <QObject>
#include <QPair>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVector>
#include <cppjieba/Jieba.hpp>

#include "Notes.h"

class DatabaseManager : public QObject {
  Q_OBJECT
 public:
  struct SearchResult {
    QString filePath;
    QString title;
    QString preview;
  };

  explicit DatabaseManager(QObject *parent = nullptr);
  bool initDatabase(const QString &path = "search.db");
  void updateFileIndex(const QString &directory);
  QVector<SearchResult> searchDocuments(const QString &query,
                                        NoteIndexManager *indexManager,
                                        int limit = 20);

 private:
  QSqlDatabase m_db;
  // cppjieba::Jieba m_jieba;

  QStringList tokenize(const QString &text);
  void processFile(const QString &filePath);
  QString extractPreview(const QString &content, const QStringList &keywords);
  void setupDatabaseSchema();
};
