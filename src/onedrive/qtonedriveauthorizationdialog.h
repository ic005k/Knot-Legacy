#ifndef ONEDRIVEAUTHORIZATIONDIALOG_H
#define ONEDRIVEAUTHORIZATIONDIALOG_H

#include <QDialog>
#include <QQuickWidget>
#include <QUrl>

class QWebEngineView;

class QtOneDriveAuthorizationDialog : public QDialog {
  Q_OBJECT

 public:
  explicit QtOneDriveAuthorizationDialog(const QUrl &url, QWidget *parent = 0);
  ~QtOneDriveAuthorizationDialog();

  static bool isExists() { return isExists_; }
  void setNeedToClose(bool close) { isNeedToClose_ = close; }

 private slots:
  void on_timer();
  void on_webView_loadFinished(bool arg1);

 signals:
  void success(const QUrl &url);
  void error(const QUrl &url);

 private:
  static bool isExists_;
  bool isNeedToClose_ = false;
  QUrl url_;
  QQuickWidget *webView_ = nullptr;
};

#endif  // ONEDRIVEAUTHORIZATIONDIALOG_H
