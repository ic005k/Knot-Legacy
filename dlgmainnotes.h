#ifndef DLGMAINNOTES_H
#define DLGMAINNOTES_H

#include <QDialog>
#include <QPlainTextEdit>

#include "smoothscrollbar.h"
namespace Ui {
class dlgMainNotes;
}

class dlgMainNotes : public QDialog {
  Q_OBJECT

 public:
  explicit dlgMainNotes(QWidget *parent = nullptr);
  ~dlgMainNotes();
  Ui::dlgMainNotes *ui;

  void saveMainNotes(bool);
  QString fileName;
  void init_MainNotes(bool);
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

  void on_btnLastBrowse_clicked();

  void on_textBrowser_cursorPositionChanged();

 private:
  SmoothScrollBar *vScrollBar;
  void wheelEvent(QWheelEvent *e) override;  //捕获鼠标滚轮事件
};

#endif  // DLGMAINNOTES_H
