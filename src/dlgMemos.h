#ifndef DLGMEMOS_H
#define DLGMEMOS_H

#include <QApplication>
#include <QChar>
#include <QCharRef>
#include <QDialog>
#include <QImageReader>
#include <QInputMethod>
#include <QPlainTextEdit>
#include <QTextBlock>
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

  qreal textHeight;
  double maxSliderPosition, maxSliderMax, minSliderPosition, minSliderMax;
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

  void setVPos();
 public slots:
  void editVSBarValueChanged();

 protected:
  void keyReleaseEvent(QKeyEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;
  bool eventFilter(QObject *obj, QEvent *event) override;

 private slots:

  void highlightCurrentLine();

  void on_btnBack_clicked();

  void on_btnCloseText_clicked();

  void on_KVChanged();

  void on_btnUndo_clicked();

  void on_btnRedo_clicked();

  void on_textEdit_textChanged();

  void on_textEdit_redoAvailable(bool b);

  void on_textEdit_undoAvailable(bool b);

  void on_btnPic_clicked();

  void on_btnEditSource_clicked();

  void on_btnInsertTable_clicked();

  void on_editSource_redoAvailable(bool b);

  void on_editSource_undoAvailable(bool b);

  void on_btnSeparator_clicked();

  void on_btnWells_clicked();

  void on_btnVLine_clicked();

  void on_btnAsterisk_clicked();

  void on_btnS1_clicked();

  void on_btnS2_clicked();

  void on_btnS3_clicked();

  void on_btnS4_clicked();

  void on_btnColor_clicked();

  void on_btnS5_clicked();

  void on_btnLink_clicked();

  void on_btnS6_clicked();

  void on_btnS7_clicked();

  void on_btnS8_clicked();

  void on_btnS9_clicked();

  void onTextChange();

 private:
  QWidget *lineNumberArea;
  int lastLine;
  int newHeight = 0;
  bool one = false;
  QInputMethod *pAndroidKeyboard = QApplication::inputMethod();
  SmoothScrollBar *vScrollBar;
  void wheelEvent(QWheelEvent *e) override;  //捕获鼠标滚轮事件
};

#endif  // DLGMEMOS_H
