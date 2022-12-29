#ifndef LOADPIC_H
#define LOADPIC_H

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

#endif  // LOADPIC_H
