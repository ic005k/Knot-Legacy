#include "dlgOneDrive.h"

#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QTimer>

#include "src/mainwindow.h"
#include "src/onedrive/qtonedrive.h"
#include "ui_dlgOneDrive.h"
#include "ui_mainwindow.h"

extern MainWindow *mw_one;

TestDialog::TestDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::TestDialog) {
  ui->setupUi(this);
  this->installEventFilter(this);
  init();

  // oneDrive = new QtOneDrive("144c427c-78c7-409a-a3d1-86a53209bb17",
  //                           "kPn8Q~ydQ~IVO9bgD5yJbSL0GQMczTYWD.mbZbi2",
  //                           "My User Name", this);

  oneDrive =
      new QtOneDrive("000000004012F592", "e7uiuaHcwcmuqyhaWKbqmQWN5o6enjgm",
                     "My User Name", this);

  connect(oneDrive, &QtOneDrive::error, [this](const QString error) {
    QMessageBox::critical(this, "OneDrive Error", error);
  });

  connect(oneDrive, &QtOneDrive::successSignIn, [this]() {
    QMessageBox::information(this, "OneDrive", "successSignIn");
  });

  connect(oneDrive, &QtOneDrive::successSingOut, [this]() {
    QMessageBox::information(this, "OneDrive", "successSingOut");
  });

  connect(oneDrive, &QtOneDrive::successRefreshToken, [this]() {
    QMessageBox::information(this, "OneDrive", "successRefreshToken");
  });

  connect(oneDrive, &QtOneDrive::successDeleteItem, [this](const QString id) {
    QMessageBox::information(this, "OneDrive", "successDeleteItem: " + id);
  });

  connect(oneDrive, &QtOneDrive::successCreateFolder, [this](const QString id) {
    QTextEdit *text = new QTextEdit(0);
    text->resize(QSize(500, 400));
    text->setWindowTitle("OneDrive");
    text->setText(QString("successCreateFolder:\nID: %1").arg(id));
    text->show();
  });

  connect(oneDrive, &QtOneDrive::successGetUserInfo,
          [this](const QJsonObject object) {
            QMessageBox::information(this, "OneDrive",
                                     QString("successGetUserInfo\n") +
                                         QJsonDocument(object).toJson());
          });

  connect(oneDrive, &QtOneDrive::successGetStorageInfo,
          [this](const QJsonObject object) {
            QMessageBox::information(this, "OneDrive",
                                     QString("successGetStorageInfo") +
                                         QJsonDocument(object).toJson());
          });

  connect(oneDrive, &QtOneDrive::successUploadFile,
          [this](const QString filePath, const QString fileID) {
            /*QTextEdit *text = new QTextEdit(0);
            text->resize(QSize(500, 400));
            text->setWindowTitle("OneDrive");
            text->setText(QString("successUploadFile:\nPATH: %1\nID: %2")
                              .arg(filePath, fileID));
            text->show();*/

            QMessageBox::information(
                this, "OneDrive",
                QString("successUploadFile:\n\nPATH: %1\n\nID: %2")
                    .arg(filePath, fileID));
          });

  connect(oneDrive, &QtOneDrive::successDownloadFile,
          [this](const QString fileID) {
            QMessageBox::information(this, "OneDrive",
                                     QString("successDownloadFile: ") + fileID);
          });

  connect(oneDrive, &QtOneDrive::progressUploadFile,
          [this](const QString, int percent) {
            ui->progressBar->setValue(percent);
          });

  connect(oneDrive, &QtOneDrive::progressDownloadFile,
          [this](const QString, int percent) {
            ui->progressBar->setValue(percent);
          });

  connect(oneDrive, &QtOneDrive::successTraverseFolder,
          [this](const QJsonObject array) {
            QTextEdit *text = new QTextEdit(0);
            text->resize(QSize(500, 400));
            text->setWindowTitle("OneDrive");
            text->setText("successGetFoldersList:\n" +
                          QJsonDocument(array).toJson());
            text->show();
          });

  QTimer *timer = new QTimer(this);
  timer->setInterval(500);
  timer->start(10000);
  ui->label_info->setWordWrapMode(QTextOption::WrapAnywhere);
  connect(timer, &QTimer::timeout, [this]() {
    // ui->label_info->setText(oneDrive->debugInfo());

    if (!this->isHidden()) {
      ui->quickWidget->rootContext()->setContextProperty("strText",
                                                         oneDrive->debugInfo());
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
  ui->frameOne->layout()->setMargin(1);
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
  ui->progressBar->setValue(0);
}

void TestDialog::on_pushButton_downloadFile_clicked() {
  QString filePath = QFileDialog::getSaveFileName(this, "Select File");
  if (filePath.isEmpty()) return;

  oneDrive->downloadFile(filePath, ui->lineEdit_fileID->text().trimmed());
  ui->progressBar->setValue(0);
}

void TestDialog::on_pushButton_createFolder_clicked() {
  oneDrive->createFolder(ui->lineEdit_fileID->text().trimmed(), "");
}

void TestDialog::on_pushButton_deleteFile_clicked() {
  oneDrive->deleteItem(ui->lineEdit_fileID->text().trimmed());
}

void TestDialog::on_pushButton_upload2_clicked() {
  QFileDialog fdlg;
  QString filePath = fdlg.getOpenFileName(this, "Select File");
  if (filePath.isEmpty()) return;

  oneDrive->uploadFile(filePath, QFileInfo(filePath).fileName(),
                       ui->lineEdit_fileID->text().trimmed());
  ui->progressBar->setValue(0);
}

void TestDialog::on_pushButton_storageInfo_clicked() {
  oneDrive->getStorageInfo();
}

void TestDialog::on_btnBack_clicked() {
  if (ui->frameOne->isHidden()) {
    ui->frameOne->show();

  } else {
    close();
    this->update();
    this->repaint();
    mw_one->refreshMainUI();
  }
}

void TestDialog::loadLogQML() {
  ui->quickWidget->setSource(QUrl(QStringLiteral("qrc:/src/onedrive/log.qml")));
  ui->quickWidget->rootContext()->setContextProperty("strText",
                                                     oneDrive->debugInfo());
}
