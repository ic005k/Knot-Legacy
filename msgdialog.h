#ifndef MSGDIALOG_H
#define MSGDIALOG_H

#include <QDialog>

namespace Ui {
class msgDialog;
}

class msgDialog : public QDialog {
  Q_OBJECT

 public:
  explicit msgDialog(QWidget *parent = nullptr);
  ~msgDialog();
  Ui::msgDialog *ui;

  void initDlg();

 protected:
  bool eventFilter(QObject *obj, QEvent *evn) override;
 private slots:
  void on_btnOK_clicked();

 private:
};

#endif  // MSGDIALOG_H
