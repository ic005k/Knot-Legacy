#ifndef DLGREADERFUN_H
#define DLGREADERFUN_H

#include <QDialog>

namespace Ui {
class dlgReaderFun;
}

class dlgReaderFun : public QDialog {
  Q_OBJECT

 public:
  explicit dlgReaderFun(QWidget *parent = nullptr);
  ~dlgReaderFun();
  Ui::dlgReaderFun *ui;

  void init();

 protected:
  bool eventFilter(QObject *watch, QEvent *evn) override;
 private slots:
  void on_hSlider_sliderReleased();

  void on_btnFontPlus_clicked();

  void on_btnFontLess_clicked();

  void on_btnFont_clicked();

 private:
};

#endif  // DLGREADERFUN_H
