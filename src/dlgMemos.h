#ifndef DLGMEMOS_H
#define DLGMEMOS_H

#include <QApplication>
#include <QChar>
#include <QCharRef>
#include <QDialog>
#include <QImageReader>
#include <QInputMethod>
#include <QPlainTextEdit>
#include <QSettings>
#include <QTextBlock>
#include <QTextDocument>
#include <QTextDocumentFragment>

#include "src/dlgleft.h"
#include "src/dlgright.h"
#include "src/dlgsetedittext.h"
#include "src/smoothscrollbar.h"
#include "ui_dlgsetedittext.h"

namespace Ui {
class dlgMainNotes;
}

class dlgMainNotes : public QDialog {
  Q_OBJECT

 public:
  explicit dlgMainNotes(QWidget *parent = nullptr);
  ~dlgMainNotes();
  Ui::dlgMainNotes *ui;

  QTextEdit *byTextEdit;
  QLineEdit *byLineEdit;
  int androidKeyH;
  int start;
  int end;
  dlgSetEditText *m_SetEditText;
  dlgLeft *m_Left;
  dlgRight *m_Right;
  qreal textHeight;
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

  void init();

  QStringList getImgFileFromHtml(QString htmlfile);
  void zipMemo();
  void unzipMemo();

  void loadMemoQML();

  void openMD(QString mdFileName);
  void saveMD(QString mdFileName);
  void selectText(int start, int end);

  void getEditPanel(QTextEdit *textEdit, QEvent *evn);

  qreal getVHeight();
  void setVPos(qreal sliderPos);
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

  void on_btnPaste_clicked();

  void showFunPanel();

  void on_btnTest_clicked();

 private:
  int x_left, x_right, y_left, y_right;
  int y1;

  bool isFunShow;
  QTimer *timer;
  bool isMouseRelease = false;
  bool isMousePress = false;
  bool iMouseMove = false;
  QWidget *lineNumberArea;
  int lastLine;
  int newHeight = 0;

  QInputMethod *pAndroidKeyboard = QApplication::inputMethod();
  SmoothScrollBar *vScrollBar;
  void wheelEvent(QWheelEvent *e) override;  //捕获鼠标滚轮事件
};

#endif  // DLGMEMOS_H
