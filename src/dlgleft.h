#ifndef DLGLEFT_H
#define DLGLEFT_H

#include <QDialog>
#include <QMouseEvent>

namespace Ui {
class dlgLeft;
}

class dlgLeft : public QDialog {
  Q_OBJECT

 public:
  explicit dlgLeft(QWidget *parent = nullptr);
  ~dlgLeft();

  Ui::dlgLeft *ui;
  void init();

 protected:
  bool eventFilter(QObject *obj, QEvent *evn) override;

 private slots:
  void on_btnClose_clicked();

 private:
  bool isMouseRelease = false;
  bool isMousePress = false;
  bool iMouseMove = false;
};

#endif  // DLGLEFT_H
