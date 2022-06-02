#ifndef DLGREADER_H
#define DLGREADER_H

#include <QDialog>
#include <QEvent>
#include <QFontDialog>
#include <QPlainTextEdit>
#include <QProcess>
#include <QQmlEngine>
#include <QQuickView>
#include <QQuickWidget>
#include <QTextBlock>
#include <QTextBrowser>

#include "documenthandler.h"
#include "file.h"
#include "smoothscrollbar.h"
namespace Ui {
class dlgReader;
}

class dlgReader : public QDialog {
  Q_OBJECT

 public:
  explicit dlgReader(QWidget *parent = nullptr);
  ~dlgReader();
  Ui::dlgReader *ui;

  int htmlIndex = 0;
  DocumentHandler *myDocHandler;
  bool isEpub = false;
  QStringList htmlFiles;
  QString fontname;
  qreal textPos;
  qreal textHeight;
  bool isOpen = false;
  int baseLines = 20;
  QStringList readTextList;
  int totallines;
  void saveReader();
  void initReader();
  QString fileName;
  void openFile(QString fileName);
  qulonglong vpos;
  int iPage;
  void drawB();

  QString getTextEditLineText(QTextEdit *txtEdit, int i);
  void getLines();
  QStringList readText(QString textFile);
  void goPostion();
  void setQML(QString);
  void setVPos();
  QFont get_Font();
  int deleteDirfile(QString dirName);
  void setQMLHtml();
  void setFontSize(int textFontSize);
  void TextEditToFile(QPlainTextEdit *txtEdit, QString fileName);
  void savePageVPos();
  void setPageVPos();
  void showInfo();
public slots:

  void on_btnPageNext_clicked();
  void on_btnPageUp_clicked();
  void on_btnOpen_clicked();
  void on_btnFont_clicked();
  void on_btnLines_clicked();
  void on_hSlider_sliderMoved(int position);

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
  bool isLines = false;
  int sPos;
  QString strSpace = "";
  QString strOpfPath;
};

#endif  // DLGREADER_H
