#include "qtonedriveauthorizationdialog.h"

#include <QAction>
#include <QApplication>
#include <QBoxLayout>

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
#include <QDesktopWidget>
#endif

#include <QQmlContext>
#include <QTimer>
#include <QtWebView>

#include "src/mainwindow.h"
#include "ui_dlgOneDrive.h"
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

  /*webView_ = new QQuickWidget;
  webView_->setGeometry(mw_one->geometry().x(), mw_one->geometry().y(),
                        mw_one->width(), mw_one->height() - 300);
  webView_->setWindowFlags(Qt::WindowStaysOnTopHint);
  webView_->show();
  webView_->setSource(QUrl(QStringLiteral("qrc:/src/onedrive/web.qml")));
  webView_->rootContext()->setContextProperty("initialUrl", url);*/

  /*QVBoxLayout *layout = new QVBoxLayout(this);
  this->setLayout(layout);
  layout->addWidget(webView_);
  webView_->load(url);*/

  isExists_ = true;

  timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(on_timer()));
  timer->setInterval(1000);
  timer->start(1000);

  mw_one->ui->frameOneFun->hide();
  mw_one->ui->frameFunWeb->show();
  mw_one->ui->quickWidgetOne->setSource(
      QUrl(QStringLiteral("qrc:/src/onedrive/web.qml")));
  mw_one->ui->quickWidgetOne->rootContext()->setContextProperty("initialUrl",
                                                                url);

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
    timer->stop();
    mw_one->ui->frameOneFun->show();
    mw_one->ui->frameFunWeb->hide();
    mw_one->mydlgOneDrive->loadLogQML();
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
