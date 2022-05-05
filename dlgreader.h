#ifndef DLGREADER_H
#define DLGREADER_H

#include <QDialog>
#include <QEvent>
#include <QPlainTextEdit>
#include <QQmlEngine>
#include <QQuickView>
#include <QTextBlock>
#include <QTextBrowser>

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

  bool isOpen = false;
  int baseLines = 15;
  QStringList readTextList;
  int totallines;
  void saveReader();
  void initReader();
  QString fileName;
  void openFile(QString fileName);
  qulonglong vpos;
  int iPage;
  void drawB();

  QString getTextEditLineText(QPlainTextEdit *txtEdit, int i);
  void getLines();
  QStringList readText(QString textFile);
  void goPostion();
 public slots:
  void getPages();
  void on_btnPageNext_clicked();

 protected:
  bool eventFilter(QObject *obj, QEvent *evn) override;
  void keyReleaseEvent(QKeyEvent *event) override;
  void closeEvent(QCloseEvent *event) override;
  void paintEvent(QPaintEvent *event) override;
 private slots:
  void on_btnBack_clicked();

  void on_btnOpen_clicked();

  void on_btnFontPlus_clicked();

  void on_btnFontLess_clicked();

  void on_textBrowser_textChanged();

  void on_btnPage_clicked();

  void on_hSlider_sliderMoved(int position);

  void on_btnPageUp_clicked();

  void on_btnLines_clicked();

 private:
  int x, y, w, h;
  bool isLines = false;
  bool isPages = true;
  int sPos;
  QString strSpace = "";
};

#endif  // DLGREADER_H
