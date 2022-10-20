#include "autoupdatedialog.h"

#include "mainwindow.h"
#include "ui_autoupdatedialog.h"

extern MainWindow* mw_one;
extern QString iniDir;

AutoUpdateDialog::AutoUpdateDialog(QWidget* parent)
    : QDialog(parent), ui(new Ui::AutoUpdateDialog) {
  ui->setupUi(this);
  setModal(true);
  ui->lblTxt->adjustSize();
  ui->lblTxt->setText(tr("Download Progress") + " : \n" + "");

  myfile = new QFile(this);
  manager = new QNetworkAccessManager(this);
}

bool AutoUpdateDialog::eventFilter(QObject* watch, QEvent* evn) {
  if (evn->type() == QEvent::KeyPress) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      return false;
    }
  }

  return QWidget::eventFilter(watch, evn);
}

AutoUpdateDialog::~AutoUpdateDialog() { delete ui; }

void AutoUpdateDialog::doProcessReadyRead()  //读取并写入
{
  while (!reply->atEnd()) {
    QByteArray ba = reply->readAll();
    myfile->write(ba);
  }
}

void AutoUpdateDialog::doProcessFinished() {
  myfile->close();
  this->close();
  mw_one->closeGrayWindows();
  if (isCancel) return;

    // install apk
#ifdef Q_OS_ANDROID
  // "/storage/emulated/0/KnotBak/"
  QAndroidJniObject jo = QAndroidJniObject::fromString(tarFile);
  jo.callStaticMethod<void>("com.x/MyActivity", "setAPKFile",
                            "(Ljava/lang/String;)V", jo.object<jstring>());

  QAndroidJniObject m_activity = QtAndroid::androidActivity();
  m_activity.callMethod<void>("installApk");

#endif
}

void AutoUpdateDialog::doProcessDownloadProgress(qint64 recv_total,
                                                 qint64 all_total) {
  ui->progressBar->setMaximum(all_total);
  ui->progressBar->setValue(recv_total);
  ui->lblTxt->setText(tr("Download Progress") + " : \n" +
                      GetFileSize(recv_total, 2) + " -> " +
                      GetFileSize(all_total, 2));

  if (recv_total == all_total) {
    if (recv_total < 10000) {
      return;
    }

    this->repaint();
  }
}

void AutoUpdateDialog::startUpdate() {}

void AutoUpdateDialog::startDownload(QString strLink) {
  isCancel = false;

  QNetworkRequest request;
  request.setUrl(QUrl(strLink));

  // github redirects the request, so this attribute must be set to true,
  // otherwise returns nothing from qt5.6
  request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);

  reply = manager->get(request);  //发送请求
  connect(reply, &QNetworkReply::readyRead, this,
          &AutoUpdateDialog::doProcessReadyRead);  //可读
  connect(reply, &QNetworkReply::finished, this,
          &AutoUpdateDialog::doProcessFinished);  //结束
  connect(reply, &QNetworkReply::downloadProgress, this,
          &AutoUpdateDialog::doProcessDownloadProgress);  //大小

  filename = "kont.apk";

#ifdef Q_OS_MAC
  tarFile = iniDir + filename;
#endif

#ifdef Q_OS_ANDROID
  // "/storage/emulated/0/KnotBak/"
  tarFile = iniDir + filename;
#endif
  QFile apk(tarFile);
  apk.remove();

  myfile->setFileName(tarFile);
  bool ret =
      myfile->open(QIODevice::WriteOnly | QIODevice::Truncate);  //创建文件
  if (!ret) {
    QMessageBox::warning(this, "warning", "Failed to open.");
    return;
  }
  ui->progressBar->setValue(0);
  ui->progressBar->setMinimum(0);
}

void AutoUpdateDialog::closeEvent(QCloseEvent* event) {
  Q_UNUSED(event);
  reply->close();
  myfile->close();
}

QString AutoUpdateDialog::GetFileSize(qint64 size) {
  if (size < 0) return "0";
  if (!size) {
    return "0 Bytes";
  }
  static QStringList SizeNames;
  if (SizeNames.empty()) {
    SizeNames << " Bytes"
              << " KB"
              << " MB"
              << " GB"
              << " TB"
              << " PB"
              << " EB"
              << " ZB"
              << " YB";
  }
  int i = qFloor(qLn(size) / qLn(1024));
  return QString::number(size * 1.0 / qPow(1000, qFloor(i)), 'f',
                         (i > 1) ? 2 : 0) +
         SizeNames.at(i);
}

QString AutoUpdateDialog::GetFileSize(const qint64& size, int precision) {
  double sizeAsDouble = size;
  static QStringList measures;
  if (measures.isEmpty())
    measures << QCoreApplication::translate("QInstaller", "bytes")
             << QCoreApplication::translate("QInstaller", "KiB")
             << QCoreApplication::translate("QInstaller", "MiB")
             << QCoreApplication::translate("QInstaller", "GiB")
             << QCoreApplication::translate("QInstaller", "TiB")
             << QCoreApplication::translate("QInstaller", "PiB")
             << QCoreApplication::translate("QInstaller", "EiB")
             << QCoreApplication::translate("QInstaller", "ZiB")
             << QCoreApplication::translate("QInstaller", "YiB");
  QStringListIterator it(measures);
  QString measure(it.next());
  while (sizeAsDouble >= 1024.0 && it.hasNext()) {
    measure = it.next();
    sizeAsDouble /= 1024.0;
  }
  return QString::fromLatin1("%1 %2")
      .arg(sizeAsDouble, 0, 'f', precision)
      .arg(measure);
}

void AutoUpdateDialog::TextEditToFile(QTextEdit* txtEdit, QString fileName) {
  QFile* file;
  file = new QFile;
  file->setFileName(fileName);
  bool ok = file->open(QIODevice::WriteOnly);
  if (ok) {
    QTextStream out(file);
    out << txtEdit->toPlainText();
    file->close();
    delete file;
  }
}

void AutoUpdateDialog::keyPressEvent(QKeyEvent* event) {
  switch (event->key()) {
    case Qt::Key_Escape:
      // reply->close();
      // close();
      break;

    case Qt::Key_Return:

      break;

    case Qt::Key_Backspace:

      break;

    case Qt::Key_Space:

      break;

    case Qt::Key_F1:

      break;
  }

  if (event->modifiers() == Qt::ControlModifier) {
    if (event->key() == Qt::Key_M) {
      this->setWindowState(Qt::WindowMaximized);
    }
  }
}

void AutoUpdateDialog::on_btnCancel_clicked() {
  isCancel = true;
  close();
}
