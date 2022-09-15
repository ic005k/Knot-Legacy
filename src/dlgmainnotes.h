#ifndef DLGMAINNOTES_H
#define DLGMAINNOTES_H

#include <QApplication>
#include <QChar>
#include <QCharRef>
#include <QDialog>
#include <QImageReader>
#include <QInputMethod>
#include <QPlainTextEdit>
#include <QTextDocument>
#include <QTextDocumentFragment>

#include "src/smoothscrollbar.h"
namespace Ui {
class dlgMainNotes;
}

class dlgMainNotes : public QDialog {
  Q_OBJECT

 public:
  explicit dlgMainNotes(QWidget *parent = nullptr);
  ~dlgMainNotes();
  Ui::dlgMainNotes *ui;

  bool isShow = false;
  QString textMemo;
  void saveMainNotes();
  QString fileName;
  void init_MainNotes();
  qlonglong curPos;
  qreal sliderPos;

  void decode(QString filename);
  void encode(QString filename);

  void encryption(const QString &fileName);
  QString Deciphering(const QString &fileName);

  void setCursorPosition();

  void init();

  QStringList getImgFileFromHtml(QString htmlfile);
  void zipMemo();
  void unzipMemo();

  void loadMemoQML();

 protected:
  void keyReleaseEvent(QKeyEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;
  bool eventFilter(QObject *obj, QEvent *event) override;
 private slots:
  void on_btnBack_clicked();

  void on_btnCloseText_clicked();

  void on_KVChanged();

  void on_btnOK_clicked();

  void on_btnUndo_clicked();

  void on_btnRedo_clicked();

  void on_textEdit_textChanged();

  void on_textEdit_redoAvailable(bool b);

  void on_textEdit_undoAvailable(bool b);

  void on_btnPic_clicked();

 private:
  int newHeight = 0;
  bool one = false;
  QInputMethod *pAndroidKeyboard = QApplication::inputMethod();
  SmoothScrollBar *vScrollBar;
  void wheelEvent(QWheelEvent *e) override;  //捕获鼠标滚轮事件
};

#endif  // DLGMAINNOTES_H
