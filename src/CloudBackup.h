#ifndef CLOUDBACKUP_H
#define CLOUDBACKUP_H

#include <QDialog>
#include <QNetworkAccessManager>
#include <QQuickWidget>

namespace Ui {
class CloudBackup;
}

class QtOneDrive;
class CloudBackup : public QDialog {
  Q_OBJECT
 public:
  explicit CloudBackup(QWidget *parent = 0);
  ~CloudBackup();
  Ui::CloudBackup *ui;

  QQuickWidget *quickWidget;
  void init();

  QString WEBDAV_URL = "";
  QString USERNAME = "";
  QString APP_PASSWORD = "";

  void loadLogQML();

  void initQuick();

  int getProg();

  void loadText(QString str);

  void uploadData();

  void uploadFileToWebDAV(QString webdavUrl, QString localFilePath,
                          QString remoteFileName);
  void createDirectory(QString webdavUrl, QString remoteDirPath);
  void startBakData();
  void downloadFile(QString remoteFileName, QString localSavePath);

  QString aesEncrypt(QString plainText, QByteArray key, QByteArray iv);
  QString aesDecrypt(QString cipherText, QByteArray key, QByteArray iv);

  QString getWebDAVArgument();
  signals:

 protected:
  bool eventFilter(QObject *obj, QEvent *evn) override;
 public slots:
  void on_pushButton_SignIn_clicked();
  void on_pushButton_SingOut_clicked();
  void on_pushButton_downloadFile_clicked();

  void on_pushButton_GetUserInfo_clicked();

  void on_pushButton_clicked();

  void on_lineEdit_fileID_textChanged(const QString &arg1);

  void on_pushButton_getFiles_clicked();

  void on_pushButton_traserveFolder_clicked();

  void on_pushButton_getFolders_clicked();

  void on_pushButton_createFolder_clicked();

  void on_pushButton_deleteFile_clicked();

  void on_pushButton_storageInfo_clicked();

  void on_btnBack_clicked();

 private slots:
  void updateUploadProgress(qint64 bytesSent, qint64 bytesTotal);

 private:
  QtOneDrive *oneDrive = nullptr;
  QString initUserInfo(QString info);

  QNetworkAccessManager *m_manager = nullptr;
  QHash<QNetworkReply *, QFile *> m_activeDownloads;  // 必须声明为类成员
};

#endif  // CLOUDBACKUP_H
