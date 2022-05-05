#ifndef QT_HUB_FILE_H
#define QT_HUB_FILE_H

#include <QObject>

class File : public QObject {
  Q_OBJECT
 public:
  File();
  QString m_text;

  Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
  Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)

  QString source() const;
  void setSource(const QString &source);

  QString text() const;
  void setText(const QString &text);

  void setStr(QString);
 signals:
  void sourceChanged();
  void textChanged();

 public slots:
  void readFile();

 private slots:

 private:
  QString m_source;
};

#endif  // FILE_H
