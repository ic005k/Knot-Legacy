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

  void showMsg(QString title, QString msgtxt, int btnCount);
 protected slots:
  bool eventFilter(QObject *watch, QEvent *evn) override;

 private slots:

 private:
  void init();
};

#endif  // SHOWMESSAGE_H
