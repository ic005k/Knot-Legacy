#ifndef DLGMAINNOTES_H
#define DLGMAINNOTES_H

#include <QApplication>
#include <QChar>
#include <QCharRef>
#include <QDialog>
#include <QInputMethod>
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

  void saveMainNotes();
  QString fileName;
  void init_MainNotes();
  qlonglong curPos;
  qlonglong sliderPos;

  void decode(QString filename);
  void encode(QString filename);

  void encryption(const QString &fileName);
  QString Deciphering(const QString &fileName);

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

  void on_KVChanged();

  void on_btnSetKey_clicked();

  void on_btnOK_clicked();

 private:
  int newHeight = 0;
  bool one = false;
  QInputMethod *pAndroidKeyboard = QApplication::inputMethod();
  SmoothScrollBar *vScrollBar;
  void wheelEvent(QWheelEvent *e) override;  //捕获鼠标滚轮事件
};

#endif  // DLGMAINNOTES_H
