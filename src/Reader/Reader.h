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

#include "src/Reader/DocumentHandler.h"
#include "src/Reader/File.h"

namespace Ui {
class Reader;
}

class Reader : public QDialog {
  Q_OBJECT

 public:
  explicit Reader(QWidget *parent = nullptr);
  ~Reader();
  Ui::Reader *ui;

  QDialog *frame;
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

  static QStringList readText(QString textFile);
  void goPostion();
  void setQMLText(QString);

  static int deleteDirfile(QString dirName);
  void setQMLHtml(QString htmlFile, QString skipID);
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

  void backDir();
  static QString get_idref(QString str0);
  void setVPos(qreal pos);

  static QString getCoverPicFile(QString htmlFile);
  qreal getVPos();
  void setReaderStyle();
  qreal getVHeight();
  qreal getNewVPos(qreal pos1, qreal h1, qreal h2);
  static void processHtml(QString htmlFile);

  void on_btnPageNext_clicked();
  void on_btnPageUp_clicked();
  void on_btnOpen_clicked();

  void setAni();
  void loadQMLText(QString str);
  void setPdfViewVisible(bool vv);
  int getPdfCurrentPage();
  void setPdfPage(int page);
  void setHideShowTopBar();
  bool copyDirectoryFiles(const QString &fromDir, const QString &toDir,
                          bool coverFileIfExist);
  qreal getScale();
  void setPdfScale(qreal scale);

  void clearAllReaderRecords();
  void on_hSlider_sliderReleased(int position);
  void setHtmlSkip(QString htmlFile, QString skipID);

  void StringToFile(QString buffers, QString fileName);
 public slots:
  void setEpubPagePosition(int index, QString htmlFile);
  void openBookListItem();
  void showCatalogue();
  void initLink(QString htmlFile);

  void openCataList(QString htmlFile);

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
  void getLines();
  static void ncx2html();
  QString getSkipText(QString htmlFile, QString skipID);
  QString strFind;
  void gotoCataList(QString htmlFile);
  int currentCataIndex = 0;
};

#endif  // READER_H
