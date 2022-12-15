#include "OneDrive.h"

#include <QFile>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QTimer>

#include "src/mainwindow.h"
#include "src/onedrive/qtonedrive.h"
#include "src/onedrive/qtonedriveauthorizationdialog.h"
#include "ui_OneDrive.h"
#include "ui_mainwindow.h"

extern MainWindow *mw_one;
extern QString iniFile, iniDir;
extern QtOneDriveAuthorizationDialog *dialog_;

TestDialog::TestDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::TestDialog) {
  ui->setupUi(this);

  this->installEventFilter(this);
  init();
  initQuick();

  oneDrive = new QtOneDrive("a7d66295-9a9f-424b-8dd1-7396c3b4573c",
                            "nk28Q~yPYNqIQbPmTZTk_OIyseXcO0OY5LudcbPQ",
                            "My User Name", this);

  // oneDrive =
  //     new QtOneDrive("000000004012F592", "e7uiuaHcwcmuqyhaWKbqmQWN5o6enjgm",
  //                    "My User Name", this);

  connect(oneDrive, &QtOneDrive::error, [this](const QString error) {
    Q_UNUSED(this);
    // QMessageBox::critical(this, "OneDrive Error", error);
    mw_one->showMsgBox("OneDrive", error, "", 1);
  });

  connect(oneDrive, &QtOneDrive::successSignIn, [this]() {
    Q_UNUSED(this);
    // QMessageBox::information(this, "OneDrive", tr("Success Sign In"));
    mw_one->showMsgBox("OneDrive", tr("Success Sign In"), "", 1);
  });

  connect(oneDrive, &QtOneDrive::successSingOut, [this]() {
    Q_UNUSED(this);
    // QMessageBox::information(this, "OneDrive", tr("Success Sing Out"));
    mw_one->showMsgBox("OneDrive", tr("Success Sing Out"), "", 1);
  });

  connect(oneDrive, &QtOneDrive::successRefreshToken, [this]() {
    Q_UNUSED(this);
    // QMessageBox::information(this, "OneDrive", tr("Success Refresh Token"));
    mw_one->showMsgBox("OneDrive", tr("Success Refresh Token"), "", 1);
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
            mw_one->showMsgBox("OneDrive",
                               QString(tr("Success Get User Info") + "\n" +
                                       QJsonDocument(object).toJson()),
                               "", 1);
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
            mw_one->showMsgBox(
                "OneDrive",
                QString(tr("Success Upload File:") + "\n\nPATH: %1\n\nID: %2")
                    .arg(filePath, fileID),
                "", 1);
          });

  connect(
      oneDrive, &QtOneDrive::successDownloadFile, [this](const QString fileID) {
        Q_UNUSED(this);
        /*QMessageBox::information(
             this, "OneDrive", QString(tr("Success Download File: ")) +
             fileID);*/
        mw_one->showMsgBox(
            "OneDrive", QString(tr("Success Download File: ")) + fileID, "", 1);
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

void TestDialog::init() {
  ui->frame->hide();
  ui->lineEdit_fileID->setFocus();
  this->setGeometry(mw_one->geometry().x(), mw_one->geometry().y(),
                    mw_one->width(), mw_one->height());
  ui->frameOne->setMaximumHeight(100);

  this->setModal(true);
}

TestDialog::~TestDialog() { delete ui; }

bool TestDialog::eventFilter(QObject *obj, QEvent *evn) {
  if (evn->type() == QEvent::KeyPress) {
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      on_btnBack_clicked();
      return true;
    }
  }

  return QWidget::eventFilter(obj, evn);
}

void TestDialog::on_pushButton_SignIn_clicked() { oneDrive->signIn(); }

void TestDialog::on_pushButton_SingOut_clicked() { oneDrive->signOut(); }

void TestDialog::on_pushButton_GetUserInfo_clicked() {
  oneDrive->getUserInfo();
}

void TestDialog::on_pushButton_clicked() { oneDrive->refreshToken(); }

void TestDialog::on_lineEdit_fileID_textChanged(const QString &arg1) {
  ui->pushButton_deleteFile->setEnabled(!arg1.isEmpty());
  // ui->pushButton_downloadFile->setEnabled(  !arg1.isEmpty() );
  ui->pushButton_traserveFolder->setEnabled(!arg1.isEmpty());
  // ui->pushButton_upload2->setEnabled(  !arg1.isEmpty() );
  ui->pushButton_createFolder->setEnabled(!arg1.isEmpty());
}

void TestDialog::on_pushButton_getFiles_clicked() {
  oneDrive->traverseFolder();
}

void TestDialog::on_pushButton_traserveFolder_clicked() {
  oneDrive->traverseFolder(ui->lineEdit_fileID->text().trimmed());
}

void TestDialog::on_pushButton_getFolders_clicked() {
  QFileDialog fdlg;
  QString filePath = fdlg.getOpenFileName(this, "Select File");
  if (filePath.isEmpty()) return;

  oneDrive->uploadFile(filePath, QFileInfo(filePath).fileName(), "");
  mw_one->ui->progressBar->setValue(0);
}

void TestDialog::on_pushButton_downloadFile_clicked() {
  QString filePath;  // = QFileDialog::getSaveFileName(this, "Select File");
  filePath = iniDir + "memo.zip";
  if (QFile(filePath).exists()) QFile(filePath).remove();
  if (filePath.isEmpty()) return;

  if (!mw_one->showMsgBox("OneDrive",
                          tr("Downloading data?") + "\n\n" +
                              tr("This operation will overwrite the local data "
                                 "with the data on OneDrive."),
                          "", 2))
    return;

  oneDrive->downloadFile(filePath, ui->lineEdit_fileID->text().trimmed());
  mw_one->ui->progressBar->setValue(0);
}

void TestDialog::on_pushButton_createFolder_clicked() {
  oneDrive->createFolder(ui->lineEdit_fileID->text().trimmed(), "");
}

void TestDialog::on_pushButton_deleteFile_clicked() {
  oneDrive->deleteItem(ui->lineEdit_fileID->text().trimmed());
}

void TestDialog::on_pushButton_upload2_clicked() {
  // QFileDialog fdlg;
  QString zipfile =
      iniDir + "memo.zip";  // = fdlg.getOpenFileName(this, "Select File");
  mw_one->bakData(zipfile, false, false);

  if (!mw_one->showMsgBox(
          "OneDrive",
          tr("Uploading data?") + "\n\n" +
              tr("This operation will overwrite the data on OneDrive.") +
              "\n\n" + mw_one->mydlgReader->getUriRealPath(zipfile) +
              "\n\nSIZE: " + mw_one->getFileSize(QFile(zipfile).size(), 2),
          "", 2))
    return;

  oneDrive->uploadFile(zipfile, "memo.zip",
                       ui->lineEdit_fileID->text().trimmed());
  mw_one->ui->progressBar->setValue(0);
}

void TestDialog::on_pushButton_storageInfo_clicked() {
  oneDrive->getStorageInfo();
}

void TestDialog::on_btnBack_clicked() {
  if (ui->frameOne->isHidden()) {
    ui->frameOne->show();

  } else {
    const QSize size(ui->frameQuick->width(), ui->frameQuick->height());
    quickWidget->resize(size);
    quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    close();
  }
}

void TestDialog::loadLogQML() {
  mw_one->ui->qwOneDriver->setSource(
      QUrl(QStringLiteral("qrc:/src/onedrive/log.qml")));
  loadText(oneDrive->debugInfo());
}

void TestDialog::loadText(QString str) {
  QQuickItem *root = mw_one->ui->qwOneDriver->rootObject();
  QMetaObject::invokeMethod((QObject *)root, "loadText", Q_ARG(QVariant, str));
}

void TestDialog::initQuick() {
  quickWidget = new QQuickWidget(ui->frameQuick);
  ui->gl->addWidget(quickWidget);
  const QSize size(400, 400);
  quickWidget->resize(size);
  quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
}

int TestDialog::getProg() {
  QQuickItem *root = mw_one->ui->qwOneDriver->rootObject();
  QVariant itemCount;
  QMetaObject::invokeMethod((QObject *)root, "getPorg",
                            Q_RETURN_ARG(QVariant, itemCount));
  return itemCount.toInt();
}
