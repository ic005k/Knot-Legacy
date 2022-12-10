#include "qtonedriveauthorizationdialog.h"

#include <QAction>
#include <QApplication>
#include <QBoxLayout>

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
#include <QDesktopWidget>
#endif

#include <QQmlContext>
#include <QTimer>
#include <QtWebView/QtWebView>

#ifdef Q_OS_ANDROID
#else
#include "qtonedrivewebview.h"
#endif

#include "src/mainwindow.h"
#include "ui_OneDrive.h"
#include "ui_mainwindow.h"

extern MainWindow *mw_one;

bool QtOneDriveAuthorizationDialog::isExists_ = false;

QtOneDriveAuthorizationDialog::QtOneDriveAuthorizationDialog(const QUrl &url,
                                                             QWidget *parent)
    : QDialog(parent), url_(url) {
  isExists_ = true;

#ifdef Q_OS_ANDROID
  mw_one->ui->frameOneFun->hide();
  mw_one->ui->frameFunWeb->show();

  mw_one->ui->qwOneDriver->setSource(
      QUrl(QStringLiteral("qrc:/src/onedrive/web.qml")));
  mw_one->ui->qwOneDriver->rootContext()->setContextProperty("initialUrl", url);

#else

  setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
  setAttribute(Qt::WA_DeleteOnClose);
  setWindowTitle(tr("OneDrive Authorization"));
  int x, y, w, h;
  x = mw_one->geometry().x();
  y = mw_one->geometry().y();
  w = mw_one->geometry().width();
  h = mw_one->geometry().height();
  setMinimumSize(QSize(w, h));
  resize(QSize(w, h));
  setGeometry(x, y, w, h);
  webView_ = new QtOneDriveWebView(this);
  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setMargin(5);
  this->setLayout(layout);
  layout->addWidget(webView_);
  webView_->load(url);
  connect(webView_, SIGNAL(loadFinished(bool)), this,
          SLOT(on_webView_loadFinished(bool)));
  show();
#endif

  timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(on_timer()));
  timer->setInterval(1000);
  timer->start(1000);

  qDebug() << "web url = " << url.toString();
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
#ifdef Q_OS_ANDROID
  QUrl url;
#else
  QUrl url = webView_->url();
#endif

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
