#ifndef DLGREMARKS_H
#define DLGREMARKS_H

#include <QDialog>

namespace Ui {
class dlgRemarks;
}

class dlgRemarks : public QDialog {
  Q_OBJECT

 public:
  explicit dlgRemarks(QWidget *parent = nullptr);
  ~dlgRemarks();
  Ui::dlgRemarks *ui;

  int sliderPos;
  void init_Notes();
 public slots:
  bool eventFilter(QObject *obj, QEvent *evn) override;

 protected:
  void keyReleaseEvent(QKeyEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;
 private slots:
  void on_btnBack_clicked();

  void on_textEdit_textChanged();

  void on_btnDL_clicked();

  void on_btnHomePage_clicked();

  void on_btnMirrorDL_clicked();

  void on_btnPaste_clicked();

 private:
};

#endif  // DLGREMARKS_H
