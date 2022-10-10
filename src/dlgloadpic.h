#ifndef DLGLOADPIC_H
#define DLGLOADPIC_H

#include <QDialog>
#include <QKeyEvent>

namespace Ui {
class dlgLoadPic;
}

class dlgLoadPic : public QDialog {
  Q_OBJECT

 public:
  explicit dlgLoadPic(QWidget *parent = nullptr);
  ~dlgLoadPic();
  Ui::dlgLoadPic *ui;

  void initMain(QString imgFile);

 protected:
  bool eventFilter(QObject *watch, QEvent *evn) override;

 private slots:

 private:
};

#endif  // DLGLOADPIC_H
