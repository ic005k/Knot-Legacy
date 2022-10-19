#ifndef AUTOUPDATEDIALOG_H
#define AUTOUPDATEDIALOG_H

#include <QDebug>
#include <QDialog>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProcess>
#include <QTextEdit>
#include <QTextStream>
#include <QtMath>

namespace Ui {
class AutoUpdateDialog;
}

class AutoUpdateDialog : public QDialog {
  Q_OBJECT

 public:
  explicit AutoUpdateDialog(QWidget* parent = nullptr);
  ~AutoUpdateDialog();
  Ui::AutoUpdateDialog* ui;

  QString tempDir;
  QString filename;
  QNetworkAccessManager* manager;
  QNetworkReply* reply;
  QFile* myfile;
  void Init();
  void doProcessReadyRead();
  void doProcessFinished();
  void doProcessDownloadProgress(qint64, qint64);

  void startUpdate();

  QString GetFileSize(qint64 size);

  void TextEditToFile(QTextEdit* txtEdit, QString fileName);

  QString GetFileSize(const qint64& size, int precision);

  void startDownload(QString strLink);

 protected:
  void closeEvent(QCloseEvent* event);
  void keyPressEvent(QKeyEvent* event);
  bool eventFilter(QObject* watch, QEvent* evn);
 private slots:

 private:
};

#endif  // AUTOUPDATEDIALOG_H
