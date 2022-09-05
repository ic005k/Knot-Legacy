#include "qtonedriveauthorizationdialog.h"

#include <QAction>
#include <QApplication>
#include <QBoxLayout>
#include <QDesktopWidget>
#include <QQmlContext>
#include <QTimer>
#include <QtWebView>

#include "src/mainwindow.h"
#include "src/onedrive/dlgweb.h"
#include "ui_dlgweb.h"
#include "ui_mainwindow.h"

extern MainWindow *mw_one;

bool QtOneDriveAuthorizationDialog::isExists_ = false;

QtOneDriveAuthorizationDialog::QtOneDriveAuthorizationDialog(const QUrl &url,
                                                             QWidget *parent)
    : QDialog(parent), url_(url) {
  /*setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
  setAttribute(Qt::WA_DeleteOnClose);
  setWindowTitle(tr("OneDrive Authorization"));
  setMinimumSize(QSize(550, 650));
  resize(QSize(550, 650));*/

  /*webView_ = new QQuickWidget(this);
  QVBoxLayout *layout = new QVBoxLayout(this);
  this->setLayout(layout);
  layout->addWidget(webView_);
  webView_->load(url);*/

  isExists_ = true;

  QTimer *timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(on_timer()));
  timer->setInterval(1000);
  timer->start(1000);

  QUrl url1("https://www.qq.com/");

  mw_one->mydlgWeb->setGeometry(mw_one->geometry().x(), mw_one->geometry().y(),
                                mw_one->width(), mw_one->height());
  mw_one->mydlgWeb->setFixedHeight(mw_one->height());
  mw_one->mydlgWeb->setModal(true);
  mw_one->mydlgWeb->show();
  mw_one->mydlgWeb->ui->quickWidget->setSource(
      QUrl(QStringLiteral("qrc:/src/onedrive/main.qml")));
  mw_one->mydlgWeb->ui->quickWidget->rootContext()->setContextProperty(
      "initialUrl", url);

  qDebug() << "web url = " << url.toString();

  // QDesktopServices::openUrl(url);

  // connect(webView_, SIGNAL(loadFinished(bool)), this,
  //         SLOT(on_webView_loadFinished(bool)));
}

QtOneDriveAuthorizationDialog::~QtOneDriveAuthorizationDialog() {
  isExists_ = false;
}

void QtOneDriveAuthorizationDialog::on_timer() {
  if (isNeedToClose_) {
    close();
    mw_one->mydlgWeb->close();
    mw_one->refreshMainUI();
  }
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
