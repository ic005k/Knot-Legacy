#include "CloudBackup.h"

#include <QFile>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QTimer>

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

    m_Method->m_widget = new QWidget(mw_one);
    ShowMessage *m_ShowMsg = new ShowMessage(this);
    m_ShowMsg->showMsg("OneDrive", error, 1);
  });

  connect(oneDrive, &QtOneDrive::successSignIn, [this]() {
    Q_UNUSED(this);
    // QMessageBox::information(this, "OneDrive", tr("Success Sign In"));

    m_Method->m_widget = new QWidget(mw_one);
    ShowMessage *m_ShowMsg = new ShowMessage(this);
    m_ShowMsg->showMsg("OneDrive", tr("Success Sign In"), 1);
  });

  connect(oneDrive, &QtOneDrive::successSingOut, [this]() {
    Q_UNUSED(this);
    // QMessageBox::information(this, "OneDrive", tr("Success Sing Out"));

    m_Method->m_widget = new QWidget(mw_one);
    ShowMessage *m_ShowMsg = new ShowMessage(this);
    m_ShowMsg->showMsg("OneDrive", tr("Success Sing Out"), 1);
  });

  connect(oneDrive, &QtOneDrive::successRefreshToken, [this]() {
    Q_UNUSED(this);
    // QMessageBox::information(this, "OneDrive", tr("Success Refresh Token"));

    m_Method->m_widget = new QWidget(mw_one);
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

            m_Method->m_widget = new QWidget(mw_one);
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

            m_Method->m_widget = new QWidget(mw_one);
            ShowMessage *m_ShowMsg = new ShowMessage(this);
            m_ShowMsg->showMsg(
                "OneDrive",
                QString(tr("Success Upload File:") + "\n\nPATH: %1\n\nID: %2")
                    .arg(filePath, fileID),
                1);
          });

  connect(oneDrive, &QtOneDrive::successDownloadFile,
          [this](const QString fileID) {
            Q_UNUSED(this);
            /*QMessageBox::information(
                 this, "OneDrive", QString(tr("Success Download File: ")) +
                 fileID);*/

            m_Method->m_widget = new QWidget(mw_one);
            ShowMessage *m_ShowMsg = new ShowMessage(this);
            m_ShowMsg->showMsg(
                "OneDrive", QString(tr("Success Download File: ")) + fileID, 1);
          });

  connect(oneDrive, &QtOneDrive::progressUploadFile,
          [this](const QString, int percent) {
            Q_UNUSED(this);
            mw_one->ui->progressBar->setValue(percent);
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
  QString filePath;  // = QFileDialog::getSaveFileName(this, "Select File");
  filePath = iniDir + "memo.zip";
  if (QFile(filePath).exists()) QFile(filePath).remove();
  if (filePath.isEmpty()) return;

  m_Method->m_widget = new QWidget(mw_one);
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

void CloudBackup::on_pushButton_upload2_clicked() {
  // QFileDialog fdlg;
  zipfile =
      iniDir + "memo.zip";  // = fdlg.getOpenFileName(this, "Select File");

  isUpData = true;
  mw_one->showProgress();
  mw_one->myBakDataThread->start();
}

void CloudBackup::uploadData() {
  m_Method->m_widget = new QWidget(mw_one);
  ShowMessage *m_ShowMsg = new ShowMessage(this);
  if (!m_ShowMsg->showMsg(
          "OneDrive",
          tr("Uploading data?") + "\n\n" +
              tr("This action will update the data on OneDrive.") + "\n\n" +
              mw_one->m_Reader->getUriRealPath(zipfile) +
              "\n\nSIZE: " + mw_one->getFileSize(QFile(zipfile).size(), 2),
          2))
    return;

  oneDrive->uploadFile(zipfile, "memo.zip",
                       ui->lineEdit_fileID->text().trimmed());
  mw_one->ui->progressBar->setValue(0);
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
