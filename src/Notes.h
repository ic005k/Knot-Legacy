#ifndef NOTES_H
#define NOTES_H

#include <QApplication>
#include <QChar>

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
#include <QCharRef>
#endif

#include <QDialog>
#include <QImageReader>
#include <QInputMethod>
#include <QPainter>
#include <QPlainTextEdit>
#include <QSettings>
#include <QTextBlock>
#include <QTextDocument>
#include <QTextDocumentFragment>

#include "src/dlgleft.h"
#include "src/dlgright.h"
#include "src/dlgsetedittext.h"
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

  bool isSave = true;
  QString htmlBuffer;
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
  void loadMemoQML();
  void decode(QString filename);
  void encode(QString filename);

  void encryption(const QString &fileName);
  QString Deciphering(const QString &fileName);

  void init();

  QStringList getImgFileFromHtml(QString htmlfile);
  void zipMemo();

  void selectText(int start, int end);

  void getEditPanel(QTextEdit *textEdit, QEvent *evn);

  qreal getVHeight();

  QString getDateTimeStr();
  void MD2Html(QString mdFile);
  void saveQMLVPos();
  bool androidCopyFile(QString src, QString des);

  qreal getVPos();
  void unzip(QString zipfile);

  void show_findText();

  void findText();
  protected:
  void keyReleaseEvent(QKeyEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;
  bool eventFilter(QObject *obj, QEvent *event) override;
  void paintEvent(QPaintEvent *pEvent) override;
  void closeEvent(QCloseEvent *event) override;

 public slots:
  void editVSBarValueChanged();
  void timerSlot();
  void setVPos();
  void highlightCurrentLine();

  void on_btnDone_clicked();

  void on_KVChanged();

  void on_btnUndo_clicked();

  void on_btnRedo_clicked();

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

  void on_btnLeft_clicked();

  void on_btnRight_clicked();

  void on_btnS10_clicked();

 signals:
  void sendUpdate();

 private slots:
  void on_editSource_textChanged();

  void on_editSource_cursorPositionChanged();

  void on_btnReference_clicked();

  void on_btnShowFind_clicked();

  void on_btnFind_clicked();

 private:
  int x_left, x_right, y_left, y_right;
  int y1;

  QTimer *timerCur;
  QPainter *pPainter;
  bool bCursorVisible;

  bool isFunShow;
  QTimer *timer;
  bool isMouseRelease = false;
  bool isMousePress = false;
  bool iMouseMove = false;
  QWidget *lineNumberArea;
  int lastLine;
  int newHeight = 0;

  QInputMethod *pAndroidKeyboard = QApplication::inputMethod();
  void wheelEvent(QWheelEvent *e) override;
};

#endif  // NOTES_H
