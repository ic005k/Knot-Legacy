#ifndef READER_H
#define READER_H

#include <QDialog>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNamedNodeMap>
#include <QEvent>
#include <QFontDialog>
#include <QPlainTextEdit>
#include <QProcess>
#include <QQmlEngine>
#include <QQuickView>
#include <QQuickWidget>
#include <QTextBlock>
#include <QTextBrowser>
#include <QTextCodec>
#include <QVBoxLayout>
#include <QXmlStreamReader>

#include "src/documenthandler.h"
#include "src/file.h"

namespace Ui {
class dlgReader;
}

class dlgReader : public QDialog {
  Q_OBJECT

 public:
  explicit dlgReader(QWidget *parent = nullptr);
  ~dlgReader();
  Ui::dlgReader *ui;

  QString openfile;
  QString readerStyle;
  QString currentTxt;
  QString currentHtmlFile;
  bool isPageNext = false;
  int mainDirIndex = -1;
  DocumentHandler *myDocHandler;
  QStringList bookList;

  qreal textPos;
  qreal textHeight;

  void saveReader();
  void initReader();

  static void openFile(QString fileName);
  qulonglong vpos;

  static QString getTextEditLineText(QTextEdit *txtEdit, int i);
  void getLines();
  static QStringList readText(QString textFile);
  void goPostion();
  void setQMLText(QString);
  QFont get_Font();
  static int deleteDirfile(QString dirName);
  void setQMLHtml();
  void setFontSize(int textFontSize);
  static void TextEditToFile(QPlainTextEdit *txtEdit, QString fileName);
  void savePageVPos();
  void setPageVPos();
  void showInfo();
  void startOpenFile(QString openfile);
  static QString getUriRealPath(QString uripath);
  static QString getNCX_File(QString path);

  static QString GetCorrectUnicode(const QByteArray &text);
  void getReadList();
  void getBookList();
  void setEpubPagePosition(int index);
  void backDir();
  static QString get_idref(QString str0);
  void setVPos(qreal pos);
  QDialog *getProgBar();
  void refreshEpubPage();
  static QString getCoverPicFile(QString htmlFile);
  qreal getVPos();
  void setReaderStyle();
  qreal getVHeight();
  qreal getNewVPos(qreal pos1, qreal h1, qreal h2);
  static void processHtml(int index);

  void on_btnPageNext_clicked();
  void on_btnPageUp_clicked();
  void on_btnOpen_clicked();
  void selectFont();

  void on_hSlider_sliderReleased(int position);

  void setAni();
  void loadQMLText(QString str);
  void setPdfViewVisible(bool vv);
  int getPdfCurrentPage();
  void setPdfPage(int page);
  void setHideShowTopBar();
  public slots:

 protected:
  bool eventFilter(QObject *obj, QEvent *evn) override;
  void keyReleaseEvent(QKeyEvent *event) override;
  void closeEvent(QCloseEvent *event) override;
  void paintEvent(QPaintEvent *event) override;
 private slots:

 private:
  int x, y, w, h;

  QString strSpace = "";

  static void SplitFile(QString qfile);
  static QString get_href(QString idref, QStringList opfList);
  static void proceImg();
};

#endif  // READER_H
