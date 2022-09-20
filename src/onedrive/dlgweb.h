#ifndef DLGWEB_H
#define DLGWEB_H

#include <QDialog>

namespace Ui {
class dlgWeb;
}

class dlgWeb : public QDialog {
  Q_OBJECT

 public:
  explicit dlgWeb(QWidget *parent = nullptr);
  ~dlgWeb();
  Ui::dlgWeb *ui;

 protected:
  bool eventFilter(QObject *obj, QEvent *evn) override;
 private slots:
  void on_btnBack_clicked();

 private:
};

#endif  // DLGWEB_H
