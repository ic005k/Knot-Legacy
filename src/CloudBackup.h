#ifndef CLOUDBACKUP_H
#define CLOUDBACKUP_H

#include <QDialog>
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

  void loadLogQML();

  void initQuick();

  int getProg();

  void loadText(QString str);

  void uploadData();
 signals:

 protected:
  bool eventFilter(QObject *obj, QEvent *evn) override;
 public slots:
  void on_pushButton_SignIn_clicked();
  void on_pushButton_SingOut_clicked();
  void on_pushButton_downloadFile_clicked();
  void on_pushButton_upload2_clicked();

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

 private:
  QtOneDrive *oneDrive = nullptr;
  QString initUserInfo(QString info);
};

#endif  // CLOUDBACKUP_H
