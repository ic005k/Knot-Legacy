#ifndef DLGNOTES_H
#define DLGNOTES_H

#include <QDialog>

namespace Ui {
class dlgNotes;
}

class dlgNotes : public QDialog {
  Q_OBJECT

 public:
  explicit dlgNotes(QWidget *parent = nullptr);
  ~dlgNotes();
  Ui::dlgNotes *ui;

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

 private:
};

#endif  // DLGNOTES_H
