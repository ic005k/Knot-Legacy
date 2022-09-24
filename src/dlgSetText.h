#ifndef DLGSETTEXT_H
#define DLGSETTEXT_H

#include <QDialog>
#include <QKeyEvent>

namespace Ui {
class dlgSetText;
}

class dlgSetText : public QDialog {
  Q_OBJECT

 public:
  explicit dlgSetText(QWidget *parent = nullptr);
  ~dlgSetText();
  Ui::dlgSetText *ui;

  void init(int x, int y, int w, int h);

 protected:
  bool eventFilter(QObject *watch, QEvent *evn) override;

 private slots:
  void on_btnBack_clicked();

  void on_btnCopy_clicked();

  void on_btnSearch_clicked();

 private:
};

#endif  // DLGSETTEXT_H
