#ifndef DLGMAINNOTES_H
#define DLGMAINNOTES_H

#include <QDialog>
#include <QPlainTextEdit>
namespace Ui {
class dlgMainNotes;
}

class dlgMainNotes : public QDialog {
  Q_OBJECT

 public:
  explicit dlgMainNotes(QWidget *parent = nullptr);
  ~dlgMainNotes();
  Ui::dlgMainNotes *ui;

  void saveMainNotes();

  void init_MainNotes();
  qlonglong curPos;
  qlonglong sliderPos;
  bool isOpenText = false;

 protected:
  void keyReleaseEvent(QKeyEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;
  bool eventFilter(QObject *obj, QEvent *event) override;
 private slots:
  void on_btnBack_clicked();

  void on_btnOpenText_clicked();

  void on_btnCloseText_clicked();

 private:
};

#endif  // DLGMAINNOTES_H
