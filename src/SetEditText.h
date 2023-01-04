#ifndef SETEDITTEXT_H
#define SETEDITTEXT_H

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

public slots:
  void on_btnLeft1_clicked();

  void on_btnLeft0_clicked();

  void on_btnRight1_clicked();

  void on_btnRight0_clicked();

  void on_btnClose_clicked();

private slots:

  void on_btnCopy_clicked();

  void on_btnCut_clicked();

  void on_btnPaste_clicked();

  void on_btnSetAll_clicked();

  void on_btnBing_clicked();

  void on_btnDel_clicked();

private:
  bool isMouseRelease = false;
  bool isMousePress = false;
  bool isMouseMove = false;
};

#endif // SETEDITTEXT_H
