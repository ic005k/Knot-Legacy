#ifndef QT_HUB_FILE_H
#define QT_HUB_FILE_H

#include <QObject>

class File : public QObject {
  Q_OBJECT
 public:
  File();
  QString m_text;
  qreal m_textPos;
  qreal m_curX;
  QString m_strUri;

  Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
  Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
  Q_PROPERTY(qreal textPos READ textPos WRITE setTextPos NOTIFY textPosChanged)
  Q_PROPERTY(qreal curX READ curX WRITE setCurX NOTIFY curXChanged)
  Q_PROPERTY(
      QString webEnd READ webEnd WRITE setWebEnd NOTIFY loadWebEndChanged)

  QString source() const;
  void setSource(const QString &source);

  QString text() const;
  void setText(const QString &text);

  qreal textPos();
  qreal curX();
  void setTextPos(qreal &textPos);
  void setCurX(qreal &curX);
  void setWebEnd(QString &strUri);
  QString webEnd();

  void setStr(QString);
 signals:
  void sourceChanged();
  void textChanged();
  void textPosChanged();
  void curXChanged();
  void loadWebEndChanged();

 public slots:
  void readFile();

 private slots:

 private:
  QString m_source;
};

#endif  // FILE_H
