#ifndef DLGREADER_H
#define DLGREADER_H

#include <QDialog>
#include <QPlainTextEdit>
#include <QTextBlock>
#include <QTextBrowser>
namespace Ui {
class dlgReader;
}

class dlgReader : public QDialog {
  Q_OBJECT

 public:
  explicit dlgReader(QWidget *parent = nullptr);
  ~dlgReader();
  Ui::dlgReader *ui;

  QTextBrowser *myedit;
  void saveReader();
  void initReader();
  QString fileName;
  void openFile(QString fileName);
  qulonglong vpos;

  void drawB();

  QString getTextEditLineText(QTextBrowser *txtEdit, int i);
 public slots:
  void getPages();

 protected:
  bool eventFilter(QObject *obj, QEvent *evn) override;
  void keyReleaseEvent(QKeyEvent *event) override;
 private slots:
  void on_btnBack_clicked();

  void on_btnOpen_clicked();

  void on_btnFontPlus_clicked();

  void on_btnFontLess_clicked();

  void on_textBrowser_textChanged();

  void on_btnPage_clicked();

  void on_hSlider_sliderMoved(int position);

  void on_btnPageUp_clicked();

  void on_btnPageNext_clicked();

 private:
  int iPage;
};

#endif  // DLGREADER_H
