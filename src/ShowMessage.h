#ifndef SHOWMESSAGE_H
#define SHOWMESSAGE_H

#include <QDialog>
#include <QEvent>
#include <QKeyEvent>

namespace Ui {
class ShowMessage;
}

class ShowMessage : public QDialog {
  Q_OBJECT

 public:
  explicit ShowMessage(QWidget *parent = nullptr);
  ~ShowMessage();
  Ui::ShowMessage *ui;

  bool isValue;
  bool showMsg(QString title, QString msgtxt, int btnCount);

 protected:
  void closeEvent(QCloseEvent *event) override;
  bool eventFilter(QObject *watch, QEvent *evn) override;

 protected slots:

 private slots:

  void on_btnCancel_clicked();

  void on_btnOk_clicked();

  void on_btnCopy_clicked();

 private:
  void init();
};

#endif  // SHOWMESSAGE_H
