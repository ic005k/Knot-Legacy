#include "qtonedriveauthorizationdialog.h"

#include <QAction>
#include <QApplication>
#include <QBoxLayout>
#include <QDesktopWidget>
#include <QQmlContext>
#include <QTimer>
#include <QtWebView>

#include "src/mainwindow.h"
#include "ui_mainwindow.h"

extern MainWindow *mw_one;

bool QtOneDriveAuthorizationDialog::isExists_ = false;

QtOneDriveAuthorizationDialog::QtOneDriveAuthorizationDialog(const QUrl &url,
                                                             QWidget *parent)
    : QDialog(parent), url_(url) {
  setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
  setAttribute(Qt::WA_DeleteOnClose);
  setWindowTitle(tr("OneDrive Authorization"));
  setMinimumSize(QSize(550, 650));
  resize(QSize(550, 650));

  isExists_ = true;

  webView_ = new QQuickWidget(this);
  QVBoxLayout *layout = new QVBoxLayout(this);
  this->setLayout(layout);
  layout->addWidget(webView_);

  QTimer *timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(on_timer()));
  timer->setInterval(1000);
  timer->start(1000);

  // webView_->load(url);

  QUrl url1("https://www.qq.com/");
  mw_one->ui->frameMain->hide();
  // mw_one->ui->frameReader->hide();
  mw_one->ui->frameQML->show();
  mw_one->ui->quickWidget->setSource(
      QUrl(QStringLiteral("qrc:/src/onedrive/main.qml")));
  mw_one->ui->quickWidget->rootContext()->setContextProperty("initialUrl", url);

  qDebug() << "web url = " << url.toString();

  // emit success(url);

  // QDesktopServices::openUrl(url);

  connect(webView_, SIGNAL(loadFinished(bool)), this,
          SLOT(on_webView_loadFinished(bool)));
}

QtOneDriveAuthorizationDialog::~QtOneDriveAuthorizationDialog() {
  isExists_ = false;
}

void QtOneDriveAuthorizationDialog::on_timer() {
  if (isNeedToClose_) close();
}

void QtOneDriveAuthorizationDialog::on_webView_loadFinished(bool arg1) {
  QUrl url;  // = webView_->url();

  if (arg1)
    emit success(url);
  else
    emit error(url);
}

void QtOneDriveAuthorizationDialog::sendMsg(QString strUri) {
  QUrl url(strUri);
  emit success(url);
  qDebug() << "emit success...";
}
