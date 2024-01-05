#ifndef REMARKSABOUT_H
#define REMARKSABOUT_H

#include <QDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

#include "src/AutoUpdate.h"

namespace Ui {
class dlgRemarks;
}

class dlgRemarks : public QDialog {
  Q_OBJECT

 public:
  explicit dlgRemarks(QWidget *parent = nullptr);
  ~dlgRemarks();
  Ui::dlgRemarks *ui;

  AutoUpdate *m_AutoUpdate;
  QNetworkAccessManager *manager;
  int parse_UpdateJSON(QString str);
  bool blAutoCheckUpdate;

  int sliderPos;
  void init_Remarks();
  void CheckUpdate();
  void show_download();
  int getAndroidVer();
 public slots:
  bool eventFilter(QObject *obj, QEvent *evn) override;

 protected:
  void keyReleaseEvent(QKeyEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;
 private slots:
  void on_btnBack_clicked();

  void on_btnHomePage_clicked();

  void on_btnPaste_clicked();

  void replyFinished(QNetworkReply *reply);

  void on_btnCheckUpdate_clicked();

  void on_btnDownloadUP_clicked();

 private:
  QString getUrl(QVariantList list);
  QString s_link;
};

#endif  // REMARKSABOUT_H
