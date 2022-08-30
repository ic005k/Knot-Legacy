#ifndef DLGREADER_H
#define DLGREADER_H

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
#include "src/smoothscrollbar.h"
namespace Ui {
class dlgReader;
}

class dlgReader : public QDialog {
  Q_OBJECT

 public:
  explicit dlgReader(QWidget *parent = nullptr);
  ~dlgReader();
  Ui::dlgReader *ui;

  bool isPageNext = false;
  int mainDirIndex = -1;
  DocumentHandler *myDocHandler;
  QStringList bookList;
  QString fontname;
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
  void setQML(QString);
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
 public slots:

  void on_btnPageNext_clicked();
  void on_btnPageUp_clicked();
  void on_btnOpen_clicked();
  void on_btnFont_clicked();
  void on_btnPages_clicked();
  void on_hSlider_sliderReleased(int position);

 protected:
  bool eventFilter(QObject *obj, QEvent *evn) override;
  void keyReleaseEvent(QKeyEvent *event) override;
  void closeEvent(QCloseEvent *event) override;
  void paintEvent(QPaintEvent *event) override;
 private slots:
  void on_btnBack_clicked();

  void on_btnFontPlus_clicked();

  void on_btnFontLess_clicked();

  void on_textBrowser_textChanged();

 private:
  int x, y, w, h;

  QString strSpace = "";

  static void SplitFile(QString qfile);
  static QString get_href(QString idref, QStringList opfList);
  static void proceImg();
};

#endif  // DLGREADER_H
