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

  void init(int x, int y);

 protected:
  bool eventFilter(QObject *obj, QEvent *evn) override;

 private:
  Ui::dlgLeft *ui;
  bool isMouseRelease = false;
  bool isMousePress = false;
  bool iMouseMove = false;
};

#endif  // DLGLEFT_H
