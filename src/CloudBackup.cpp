#include "CloudBackup.h"

#include <QFile>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QTimer>

#include "Comm/qaesencryption.h"
#include "src/MainWindow.h"
#include "src/onedrive/qtonedrive.h"
#include "src/onedrive/qtonedriveauthorizationdialog.h"
#include "ui_CloudBackup.h"
#include "ui_MainWindow.h"

extern MainWindow *mw_one;
extern Method *m_Method;
extern QString iniFile, iniDir, zipfile;
extern QtOneDriveAuthorizationDialog *dialog_;
extern bool isUpData;
extern bool isZipOK, isMenuImport, isTimeMachine, isDownData;

CloudBackup::CloudBackup(QWidget *parent)
    : QDialog(parent), ui(new Ui::CloudBackup) {
  ui->setupUi(this);

  this->installEventFilter(this);
  init();
  // initQuick();

  oneDrive = new QtOneDrive("a7d66295-9a9f-424b-8dd1-7396c3b4573c",
                            "nk28Q~yPYNqIQbPmTZTk_OIyseXcO0OY5LudcbPQ",
                            "My User Name", this);

  // oneDrive =
  //     new QtOneDrive("000000004012F592", "e7uiuaHcwcmuqyhaWKbqmQWN5o6enjgm",
  //                    "My User Name", this);

  connect(oneDrive, &QtOneDrive::error, [this](const QString error) {
    Q_UNUSED(this);
    // QMessageBox::critical(this, "OneDrive Error", error);

    ShowMessage *m_ShowMsg = new ShowMessage(this);
    m_ShowMsg->showMsg("OneDrive", error, 1);
  });

  connect(oneDrive, &QtOneDrive::successSignIn, [this]() {
    Q_UNUSED(this);
    // QMessageBox::information(this, "OneDrive", tr("Success Sign In"));

    ShowMessage *m_ShowMsg = new ShowMessage(this);
    m_ShowMsg->showMsg("OneDrive", tr("Success Sign In"), 1);
  });

  connect(oneDrive, &QtOneDrive::successSingOut, [this]() {
    Q_UNUSED(this);
    // QMessageBox::information(this, "OneDrive", tr("Success Sing Out"));

    ShowMessage *m_ShowMsg = new ShowMessage(this);
    m_ShowMsg->showMsg("OneDrive", tr("Success Sing Out"), 1);
  });

  connect(oneDrive, &QtOneDrive::successRefreshToken, [this]() {
    Q_UNUSED(this);
    // QMessageBox::information(this, "OneDrive", tr("Success Refresh Token"));

    ShowMessage *m_ShowMsg = new ShowMessage(this);
    m_ShowMsg->showMsg("OneDrive", tr("Success Refresh Token"), 1);
  });

  connect(oneDrive, &QtOneDrive::successDeleteItem, [this](const QString id) {
    QMessageBox::information(this, "OneDrive", tr("Success DeleteItem: ") + id);
  });

  connect(oneDrive, &QtOneDrive::successCreateFolder, [this](const QString id) {
    Q_UNUSED(this);
    QTextEdit *text = new QTextEdit(0);
    text->resize(QSize(500, 400));
    text->setWindowTitle("OneDrive");
    text->setText(QString("successCreateFolder:\nID: %1").arg(id));
    text->show();
  });

  connect(oneDrive, &QtOneDrive::successGetUserInfo,
          [this](const QJsonObject object) {
            Q_UNUSED(this);
            /*QMessageBox::information(
                this, "OneDrive",
                QString(tr("Success Get User Info") + "\n" +
                        QJsonDocument(object).toJson()));*/

            QString userInfo = initUserInfo(QJsonDocument(object).toJson());

            ShowMessage *m_ShowMsg = new ShowMessage(this);
            m_ShowMsg->showMsg(
                "OneDrive",
                QString(tr("Success Get User Info") + "\n" + userInfo), 1);
          });

  connect(oneDrive, &QtOneDrive::successGetStorageInfo,
          [this](const QJsonObject object) {
            QMessageBox::information(this, "OneDrive",
                                     QString("successGetStorageInfo") +
                                         QJsonDocument(object).toJson());
          });

  connect(oneDrive, &QtOneDrive::successUploadFile,
          [this](const QString filePath, const QString fileID) {
            Q_UNUSED(this);
            /*QMessageBox::information(
                this, "OneDrive",
                QString(tr("Success Upload File:") + "\n\nPATH: %1\n\nID: %2")
                    .arg(filePath, fileID));*/

            ShowMessage *m_ShowMsg = new ShowMessage(this);
            m_ShowMsg->showMsg(
                "OneDrive",
                QString(tr("Success Upload File:") + "\n\nPATH: %1\n\nID: %2" +
                        "\n\n" + QDateTime::currentDateTime().toString())
                    .arg(filePath, fileID),
                1);
            mw_one->ui->progBar->hide();
          });

  connect(oneDrive, &QtOneDrive::successDownloadFile,
          [this](const QString fileID) {
            Q_UNUSED(this);
            /*QMessageBox::information(
                 this, "OneDrive", QString(tr("Success Download File: ")) +
                 fileID);*/

            ShowMessage *m_ShowMsg = new ShowMessage(this);
            m_ShowMsg->showMsg(
                "OneDrive", QString(tr("Success Download File: ")) + fileID, 1);
          });

  connect(oneDrive, &QtOneDrive::progressUploadFile,
          [this](const QString, int percent) {
            Q_UNUSED(this);
            mw_one->ui->progressBar->setValue(percent);

            mw_one->ui->progBar->setValue(percent);
            if (percent == 100) mw_one->ui->progBar->hide();
          });

  connect(oneDrive, &QtOneDrive::progressDownloadFile,
          [this](const QString, int percent) {
            Q_UNUSED(this);
            mw_one->ui->progressBar->setValue(percent);
          });

  connect(oneDrive, &QtOneDrive::successTraverseFolder,
          [this](const QJsonObject array) {
            Q_UNUSED(this);
            QTextEdit *text = new QTextEdit(0);
            text->resize(QSize(500, 400));
            text->setWindowTitle("OneDrive");
            text->setText("successGetFoldersList:\n" +
                          QJsonDocument(array).toJson());
            text->show();
          });

  QTimer *timer = new QTimer(this);
  timer->start(15000);
  ui->label_info->setWordWrapMode(QTextOption::WrapAnywhere);
  connect(timer, &QTimer::timeout, [this]() {
    if (!mw_one->ui->frameOne->isHidden()) {
      loadText(oneDrive->debugInfo());
    }

    this->setEnabled(!oneDrive->isBusy());
  });
}

QString CloudBackup::initUserInfo(QString info) {
  QTextEdit *edit = new QTextEdit;
  edit->setPlainText(info);
  int lineCount = edit->document()->lineCount();

  QString str1;
  for (int i = 0; i < lineCount; i++) {
    QString str = edit->document()->findBlockByLineNumber(i).text().trimmed();
    if (str.contains(":")) {
      str = str.replace(",", "");
      str = str.replace("[", "");
      str = str.replace("{", "");
      str = str.replace("\"", "");

      if (str.toLower().contains("remaining") || str.contains("total") ||
          str.contains("used")) {
        QStringList list = str.split(":");
        QString s_size = list.at(1);
        qint64 size = s_size.toLongLong();
        str = list.at(0) + ": " + mw_one->getFileSize(size, 2);
      }

      str1 = str1 + "\n" + str;
    }
  }

  return str1.trimmed();
}

void CloudBackup::init() {
  ui->frame->hide();
  ui->lineEdit_fileID->setFocus();
  this->setGeometry(mw_one->geometry().x(), mw_one->geometry().y(),
                    mw_one->width(), mw_one->height());
  ui->frameOne->setMaximumHeight(100);

  this->setModal(true);
}

CloudBackup::~CloudBackup() { delete ui; }

bool CloudBackup::eventFilter(QObject *obj, QEvent *evn) {
  if (evn->type() == QEvent::KeyRelease) {
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      on_btnBack_clicked();
      return true;
    }
  }

  return QWidget::eventFilter(obj, evn);
}

void CloudBackup::on_pushButton_SignIn_clicked() { oneDrive->signIn(); }

void CloudBackup::on_pushButton_SingOut_clicked() { oneDrive->signOut(); }

void CloudBackup::on_pushButton_GetUserInfo_clicked() {
  oneDrive->getUserInfo();
}

void CloudBackup::on_pushButton_clicked() { oneDrive->refreshToken(); }

void CloudBackup::on_lineEdit_fileID_textChanged(const QString &arg1) {
  ui->pushButton_deleteFile->setEnabled(!arg1.isEmpty());
  // ui->pushButton_downloadFile->setEnabled(  !arg1.isEmpty() );
  ui->pushButton_traserveFolder->setEnabled(!arg1.isEmpty());
  // ui->pushButton_upload2->setEnabled(  !arg1.isEmpty() );
  ui->pushButton_createFolder->setEnabled(!arg1.isEmpty());
}

void CloudBackup::on_pushButton_getFiles_clicked() {
  oneDrive->traverseFolder();
}

void CloudBackup::on_pushButton_traserveFolder_clicked() {
  oneDrive->traverseFolder(ui->lineEdit_fileID->text().trimmed());
}

void CloudBackup::on_pushButton_getFolders_clicked() {
  QFileDialog fdlg;
  QString filePath = fdlg.getOpenFileName(this, "Select File");
  if (filePath.isEmpty()) return;

  oneDrive->uploadFile(filePath, QFileInfo(filePath).fileName(), "");
  mw_one->ui->progressBar->setValue(0);
}

void CloudBackup::on_pushButton_downloadFile_clicked() {
  QString filePath;
  filePath = iniDir + "memo.zip";
  if (QFile(filePath).exists()) QFile(filePath).remove();
  if (filePath.isEmpty()) return;

  ShowMessage *m_ShowMsg = new ShowMessage(this);
  if (!m_ShowMsg->showMsg("OneDrive",
                          tr("Downloading data?") + "\n\n" +
                              tr("This operation will overwrite the local data "
                                 "with the data on OneDrive."),
                          2))
    return;

  oneDrive->downloadFile(filePath, ui->lineEdit_fileID->text().trimmed());
  mw_one->ui->progressBar->setValue(0);
}

void CloudBackup::on_pushButton_createFolder_clicked() {
  oneDrive->createFolder(ui->lineEdit_fileID->text().trimmed(), "");
}

void CloudBackup::on_pushButton_deleteFile_clicked() {
  oneDrive->deleteItem(ui->lineEdit_fileID->text().trimmed());
}

void CloudBackup::uploadData() {
  QString strFlag;
  if (mw_one->ui->chkOneDrive->isChecked())
    strFlag = "OneDrive";
  else
    strFlag = "WebDAV";
  ShowMessage *m_ShowMsg = new ShowMessage(this);
  if (!m_ShowMsg->showMsg(
          strFlag,
          tr("Uploading data?") + "\n\n" +
              tr("This action updates the data in the cloud.") + "\n\n" +
              mw_one->m_Reader->getUriRealPath(zipfile) +
              "\n\nSIZE: " + mw_one->getFileSize(QFile(zipfile).size(), 2),
          2))
    return;

  if (mw_one->ui->chkOneDrive->isChecked()) {
    oneDrive->uploadFile(zipfile, "memo.zip",
                         ui->lineEdit_fileID->text().trimmed());
  }

  if (mw_one->ui->chkWebDAV->isChecked()) {
    QString url = mw_one->ui->editWebDAV->text().trimmed();
    USERNAME = mw_one->ui->editWebDAVUsername->text().trimmed();
    APP_PASSWORD = mw_one->ui->editWebDAVPassword->text().trimmed();
    QString file = iniDir + "memo.zip";
    createDirectory(url, "Knot/");
    uploadFileToWebDAV(url, file, "Knot/memo.zip");
  }
}

void CloudBackup::on_pushButton_storageInfo_clicked() {
  oneDrive->getStorageInfo();
}

void CloudBackup::on_btnBack_clicked() {
  if (ui->frameOne->isHidden()) {
    ui->frameOne->show();

  } else {
    const QSize size(ui->frameQuick->width(), ui->frameQuick->height());
    quickWidget->resize(size);
    quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    close();
  }
}

void CloudBackup::loadLogQML() {
  mw_one->ui->qwOneDriver->setSource(
      QUrl(QStringLiteral("qrc:/src/onedrive/log.qml")));
  loadText(oneDrive->debugInfo());
}

void CloudBackup::loadText(QString str) {
  QQuickItem *root = mw_one->ui->qwOneDriver->rootObject();
  QMetaObject::invokeMethod((QObject *)root, "loadText", Q_ARG(QVariant, str));
}

void CloudBackup::initQuick() {
  quickWidget = new QQuickWidget(ui->frameQuick);
  ui->gl->addWidget(quickWidget);
  const QSize size(400, 400);
  quickWidget->resize(size);
  quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
}

int CloudBackup::getProg() {
  QQuickItem *root = mw_one->ui->qwOneDriver->rootObject();
  QVariant itemCount;
  QMetaObject::invokeMethod((QObject *)root, "getPorg",
                            Q_RETURN_ARG(QVariant, itemCount));
  return itemCount.toInt();
}

void CloudBackup::startBakData() {
  zipfile = iniDir + "memo.zip";

  isUpData = true;
  mw_one->showProgress();

  mw_one->ui->progressBar->setValue(0);
  mw_one->ui->progBar->show();
  mw_one->ui->progBar->setMaximum(100);
  mw_one->ui->progBar->setMinimum(0);
  mw_one->ui->progBar->setValue(0);

  mw_one->myBakDataThread->start();
}

// 上传文件到WebDAV,默认坚果云
void CloudBackup::uploadFileToWebDAV(QString webdavUrl, QString localFilePath,
                                     QString remoteFileName) {
  QNetworkAccessManager *manager = new QNetworkAccessManager();
  QUrl url(webdavUrl + remoteFileName);
  QNetworkRequest request(url);

  // 认证头
  QString auth = USERNAME + ":" + APP_PASSWORD;
  request.setRawHeader("Authorization", "Basic " + auth.toUtf8().toBase64());

  // 调试输出
  qDebug() << "上传URL：" << url.toString();
  qDebug() << "认证头：" << request.rawHeader("Authorization");

  QFile *file = new QFile(localFilePath);
  if (!file->open(QIODevice::ReadOnly)) {
    qDebug() << "无法打开本地文件：" << localFilePath;
    delete manager;
    delete file;
    return;
  }

  mw_one->ui->progBar->show();
  mw_one->ui->progBar->setValue(0);
  mw_one->ui->progressBar->setValue(0);

  QNetworkReply *reply = manager->put(request, file);

  connect(reply, &QNetworkReply::uploadProgress, this,
          &CloudBackup::updateUploadProgress);

  QObject::connect(reply, &QNetworkReply::finished, [=]() {
    const int statusCode =
        reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    qDebug() << "HTTP状态码：" << statusCode;
    if (reply->error() == QNetworkReply::NoError) {
      qDebug() << "上传成功！";

      ShowMessage *m_ShowMsg = new ShowMessage(this);
      m_ShowMsg->showMsg(
          "WebDAV",
          QString(tr("Success Upload File:") + "\n\nPath: %1\n\nID: %2" +
                  "\n\n" + QDateTime::currentDateTime().toString())
              .arg(localFilePath, webdavUrl + remoteFileName),
          1);
      mw_one->ui->progBar->hide();

    } else {
      qDebug() << "上传失败：" << reply->errorString();
      qDebug() << "服务器响应：" << reply->readAll();

      mw_one->ui->progBar->hide();

      if (statusCode == 401) {
        ShowMessage *m_ShowMsg = new ShowMessage(mw_one);
        m_ShowMsg->showMsg("WebDAV", tr("Authentication failed."), 1);
      } else {
        ShowMessage *m_ShowMsg = new ShowMessage(mw_one);
        m_ShowMsg->showMsg(
            "WebDAV", tr("Download error") + " : " + reply->errorString(), 1);
      }
    }
    file->close();
    reply->deleteLater();
    manager->deleteLater();
  });
}

void CloudBackup::updateUploadProgress(qint64 bytesSent, qint64 bytesTotal) {
  if (bytesTotal > 0) {
    int percent = static_cast<int>((bytesSent * 100) / bytesTotal);
    mw_one->ui->progBar->setValue(percent);
    mw_one->ui->progressBar->setValue(percent);
  }
}

void CloudBackup::createDirectory(QString webdavUrl, QString remoteDirPath) {
  QNetworkAccessManager manager;
  QEventLoop loop;
  QUrl url(webdavUrl + remoteDirPath);
  QNetworkRequest request(url);

  // 认证头
  QString auth = USERNAME + ":" + APP_PASSWORD;
  request.setRawHeader("Authorization", "Basic " + auth.toUtf8().toBase64());

  QNetworkReply *reply = manager.sendCustomRequest(request, "MKCOL");

  QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
  loop.exec();  // 阻塞直到请求完成

  int statusCode =
      reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
  if (statusCode == 201 || statusCode == 405) {  // 405表示目录已存在
    qDebug() << "目录已就绪:" << remoteDirPath;
  } else {
    qDebug() << "目录创建失败，状态码:" << statusCode;
  }

  delete reply;
}

void CloudBackup::downloadFile(QString remoteFileName, QString localSavePath) {
  m_manager = new QNetworkAccessManager();

  QUrl url(WEBDAV_URL + remoteFileName);
  QNetworkRequest request(url);
  request.setTransferTimeout(30000);

  // 设置认证头
  QString auth = QString("%1:%2").arg(USERNAME).arg(APP_PASSWORD);
  request.setRawHeader("Authorization", "Basic " + auth.toUtf8().toBase64());

  QFile *localFile = new QFile(localSavePath);
  if (!localFile->open(QIODevice::WriteOnly)) {
    qDebug() << "无法创建本地文件：" << localSavePath;
    delete localFile;
    // ShowMessage::showError("WebDAV", tr("Failed to create local file"));
    return;
  }

  QNetworkReply *reply = m_manager->get(request);
  m_activeDownloads.insert(reply, localFile);

  // 进度更新
  QObject::connect(
      reply, &QNetworkReply::downloadProgress,
      [this](qint64 bytesReceived, qint64 bytesTotal) {  // 显式捕获this
        QMetaObject::invokeMethod(this, [this, bytesReceived, bytesTotal]() {
          int percent =
              (bytesTotal > 0)
                  ? static_cast<int>((bytesReceived * 100) / bytesTotal)
                  : 0;
          mw_one->ui->progressBar->setValue(percent);
        });
      });

  // 数据写入
  QObject::connect(reply, &QNetworkReply::readyRead,
                   [this, reply]() {  // 显式捕获this
                     if (QFile *file = m_activeDownloads.value(reply)) {
                       file->write(reply->readAll());
                     }
                   });

  // 完成处理
  QObject::connect(
      reply, &QNetworkReply::finished,
      [this, reply, localSavePath]() {  // 显式捕获this
        QFile *file = m_activeDownloads.take(reply);
        const int statusCode =
            reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        if (file) {
          if (reply->error() == QNetworkReply::NoError && statusCode >= 200 &&
              statusCode < 300) {
            file->write(reply->readAll());
            file->close();

            zipfile = localSavePath;
            ShowMessage *showbox = new ShowMessage(this);
            showbox->showMsg(
                "WebDAV",
                tr("Successfully downloaded file,File saved to") + " : " +
                    localSavePath + "\n\nSize: " +
                    mw_one->getFileSize(QFile(localSavePath).size(), 2),
                1);

            if (QFile(localSavePath).exists()) {
              if (!localSavePath.isNull()) {
                ShowMessage *m_ShowMsg = new ShowMessage(mw_one);
                if (!m_ShowMsg->showMsg(
                        "Kont",
                        tr("Import this data?") + "\n" +
                            mw_one->m_Reader->getUriRealPath(localSavePath),
                        2)) {
                  isZipOK = false;
                  return;
                }
              }
              isZipOK = true;

              mw_one->showProgress();

              isMenuImport = false;
              isTimeMachine = false;
              isDownData = true;
              mw_one->myImportDataThread->start();

              if (isZipOK) mw_one->on_btnBack_One_clicked();
            }
          } else {
            file->remove();
            if (statusCode == 401) {
              ShowMessage *m_ShowMsg = new ShowMessage(mw_one);
              m_ShowMsg->showMsg("WebDAV", tr("Authentication failed."), 1);
            } else {
              ShowMessage *m_ShowMsg = new ShowMessage(mw_one);
              m_ShowMsg->showMsg(
                  "WebDAV", tr("Download error") + " : " + reply->errorString(),
                  1);
            }
          }
          delete file;
        }

        reply->deleteLater();
      });
}

// 加密函数（返回Base64编码字符串）
QString CloudBackup::aesEncrypt(QString plainText, QByteArray key,
                                QByteArray iv) {
  QAESEncryption encryption(QAESEncryption::AES_256, QAESEncryption::CBC);

  // 处理密钥和IV长度（AES-256需要32字节，CBC模式需要16字节IV）
  QByteArray adjustedKey =
      key.leftJustified(32, '\0', true);  // 截断或填充到32字节
  QByteArray adjustedIv = iv.leftJustified(16, '\0', true);  // 调整IV到16字节

  // 加密
  QByteArray encrypted =
      encryption.encode(plainText.toUtf8(), adjustedKey, adjustedIv);

  // 转换为Base64方便传输
  return encrypted.toBase64();
}

// 解密函数
QString CloudBackup::aesDecrypt(QString cipherText, QByteArray key,
                                QByteArray iv) {
  QAESEncryption encryption(QAESEncryption::AES_256, QAESEncryption::CBC);

  QByteArray adjustedKey = key.leftJustified(32, '\0', true);
  QByteArray adjustedIv = iv.leftJustified(16, '\0', true);

  // 解码Base64并解密
  QByteArray decoded = QByteArray::fromBase64(cipherText.toUtf8());
  QByteArray decrypted = encryption.decode(decoded, adjustedKey, adjustedIv);

  // 移除PKCS#7填充
  decrypted = encryption.removePadding(decrypted);

  return QString::fromUtf8(decrypted);
}
