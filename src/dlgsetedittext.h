#ifndef DLGSETEDITTEXT_H
#define DLGSETEDITTEXT_H

#include <QClipboard>
#include <QDialog>
#include <QKeyEvent>

namespace Ui {
class dlgSetEditText;
}

class dlgSetEditText : public QDialog {
  Q_OBJECT

 public:
  explicit dlgSetEditText(QWidget *parent = nullptr);
  ~dlgSetEditText();
  Ui::dlgSetEditText *ui;

  void init(int y);

 protected:
  bool eventFilter(QObject *watch, QEvent *evn) override;
 private slots:
  void on_btnClose_clicked();

  void on_btnCopy_clicked();

  void on_btnCut_clicked();

  void on_btnPaste_clicked();

  void on_btnSetAll_clicked();

 private:
};

#endif  // DLGSETEDITTEXT_H
