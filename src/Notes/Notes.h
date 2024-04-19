#ifndef NOTES_H
#define NOTES_H

#include <QApplication>
#include <QChar>
#include <QClipboard>
#include <QMimeData>

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
#include <QCharRef>
#else

#ifdef Q_OS_ANDROID
#include <QtCore/private/qandroidextras_p.h>
#endif

#endif

#include <QColorDialog>
#include <QDialog>
#include <QFileDialog>
#include <QImageReader>
#include <QInputMethod>
#include <QPainter>
#include <QPlainTextEdit>
#include <QPrinter>
#include <QSettings>
#include <QTextBlock>
#include <QTextDocument>
#include <QTextDocumentFragment>

#include "src/Comm/ShowMessage.h"
#include "src/Notes/ColorDialog.h"
#include "src/Notes/LineNumberArea.h"
#include "src/Notes/PrintPDF.h"
#include "src/Notes/QTextEditHighlighter.h"
#include "src/Notes/TextSelector.h"
#include "ui_PrintPDF.h"
#include "ui_TextSelector.h"

namespace Ui {
class Notes;
}

class Notes : public QDialog {
  Q_OBJECT

 public:
  explicit Notes(QWidget *parent = nullptr);
  ~Notes();
  Ui::Notes *ui;

  QTextEditHighlighter *m_EditSource;
  QTimer *timerEditNote;
  int px, py, mx, my;

  bool isTextChange;
  bool isNeedSave;
  bool isDone;
  QTimer *timerEditPanel;
  QString htmlBuffer;
  QTextEdit *byTextEdit;
  QLineEdit *byLineEdit;
  int androidKeyH;
  int start;
  int end;
  TextSelector *m_TextSelector;
  qreal textHeight;

  QString textMemo;
  void saveMainNotes();
  QString fileName;
  void init_MainNotes();
  qlonglong curPos;
  qreal sliderPos;
  void loadNoteToQML();
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
  void show_findTextBack();

  bool selectPDFFormat(QPrinter *printer);
  void on_btnPDF_clicked();

  bool eventFilterEditTodo(QObject *watch, QEvent *evn);

  bool eventFilterEditRecord(QObject *watch, QEvent *evn);

  QString insertImage(QString fileName, bool isToAndroidView);

  bool eventFilterQwNote(QObject *watch, QEvent *event);

  void setVPos(qreal pos);
  void setEditorVPos();
  qreal getEditorVPos();

  QString getEditorText();

  void openNoteEditor();

  void appendNote(QString str);
  void insertNote(QString str);
  auto getAndroidNoteConfig(QString key);
  void setAndroidNoteConfig(QString key, QString value);

  void delImage();

  void javaNoteToQMLNote();
  QString formatMDText(QString text);

  void init_all_notes();
  void loadEmptyNote();

 protected:
  void keyReleaseEvent(QKeyEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;
  bool eventFilter(QObject *obj, QEvent *event) override;
  void paintEvent(QPaintEvent *pEvent) override;
  void closeEvent(QCloseEvent *event) override;

 public slots:
  void on_btnHideKey_clicked();

  void on_btnShowTools_clicked();

  void editVSBarValueChanged();

  void timerSlot();

  void on_showEditPanel();

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

  void on_btnLeft_clicked();

  void on_btnRight_clicked();

  void on_btnS10_clicked();

  void editNote();
  void showNoteList();
  void on_editNote();

  void showTextSelector();
  void delLink(QString link);
 signals:
  void sendUpdate();

 private slots:

  void on_editSource_textChanged();

  void on_editSource_cursorPositionChanged();

  void on_btnReference_clicked();

  void on_btnFind_clicked();

  void on_btnPrev_clicked();

  void on_btnNext_clicked();

  void on_editFind_returnPressed();

  void on_editFind_textChanged(const QString &arg1);

  void on_btnGetShare_clicked();

  void on_btnDate_clicked();

  void on_btnTime_clicked();

  void on_btnS11_clicked();

 private:
  int x_left, x_right, y_left, y_right;

  int y1;

  bool isMouseRelease = false;
  bool isMousePress = false;
  bool isMouseMove = false;

  QTimer *timerCur;
  QPainter *pPainter;
  bool bCursorVisible;

  bool isFunShow;

  int newHeight = 0;

  QInputMethod *pAndroidKeyboard = QApplication::inputMethod();
  void wheelEvent(QWheelEvent *e) override;
  QString imgDir = "==Image==";
  QColor StringToColor(QString mRgbStr);
};

#endif  // NOTES_H
