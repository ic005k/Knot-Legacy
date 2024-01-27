#ifndef MOVETO_H
#define MOVETO_H

#include <QDialog>

namespace Ui {
class MoveTo;
}

class MoveTo : public QDialog {
  Q_OBJECT

 public:
  explicit MoveTo(QWidget *parent = nullptr);
  ~MoveTo();
  Ui::MoveTo *ui;
  QString strCurrentItem;

  QString getCurrentItem();

 protected:
  void closeEvent(QCloseEvent *event) override;
 private slots:
  void on_btnCancel_clicked();

  void on_btnOk_clicked();

 private:
  bool isOk = false;
};

#endif  // MOVETO_H
