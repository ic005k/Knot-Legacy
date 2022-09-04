#ifndef QTONEDRIVEWEBVIEW_H
#define QTONEDRIVEWEBVIEW_H

#include <QWebEngineView>

class QtOneDriveWebView : public QWebEngineView {
  Q_OBJECT
 public:
  QtOneDriveWebView(QWidget *parent = 0);
  void contextMenuEvent(QContextMenuEvent *event);
 signals:

 public slots:
};

#endif  // QTONEDRIVEWEBVIEW_H
