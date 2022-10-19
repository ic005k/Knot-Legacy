#ifndef REMARKSABOUT_H
#define REMARKSABOUT_H

#include <QDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

namespace Ui {
class dlgRemarks;
}

class dlgRemarks : public QDialog {
  Q_OBJECT

 public:
  explicit dlgRemarks(QWidget *parent = nullptr);
  ~dlgRemarks();
  Ui::dlgRemarks *ui;

  QNetworkAccessManager *manager;
  int parse_UpdateJSON(QString str);
  bool blAutoCheckUpdate;

  int sliderPos;
  void init_Notes();
  void CheckUpdate();
 public slots:
  bool eventFilter(QObject *obj, QEvent *evn) override;

 protected:
  void keyReleaseEvent(QKeyEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;
 private slots:
  void on_btnBack_clicked();

  void on_textEdit_textChanged();

  void on_btnDL_clicked();

  void on_btnHomePage_clicked();

  void on_btnMirrorDL_clicked();

  void on_btnPaste_clicked();

  void replyFinished(QNetworkReply *reply);

  void on_btnCheckUpdate_clicked();

 private:
  QString getUrl(QVariantList list);
};

#endif  // REMARKSABOUT_H
