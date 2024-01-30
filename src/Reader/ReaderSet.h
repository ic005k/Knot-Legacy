#ifndef READERSET_H
#define READERSET_H

#include <QDialog>

namespace Ui {
class ReaderSet;
}

class ReaderSet : public QDialog {
  Q_OBJECT

 public:
  explicit ReaderSet(QWidget *parent = nullptr);
  ~ReaderSet();
  Ui::ReaderSet *ui;

  void init();

  void updateProgress();
  protected:
  bool eventFilter(QObject *watch, QEvent *evn) override;
 private slots:
  void on_hSlider_sliderReleased();

  void on_btnFontPlus_clicked();

  void on_btnFontLess_clicked();

  void on_hSlider_sliderMoved(int position);

  void on_btnStyle1_clicked();

  void on_btnStyle2_clicked();

  void on_btnStyle3_clicked();

  void on_btnFont_clicked();

  void on_hSlider_valueChanged(int value);

 private:
};

#endif  // READERSET_H
