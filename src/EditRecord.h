#ifndef EDITRECORD_H
#define EDITRECORD_H

#include <QCompleter>
#include <QDialog>
#include <QListWidget>
#include <QSet>
#include <QWidgetAction>

#include "src/CategoryList.h"

namespace Ui {
class EditRecord;
}

class EditRecord : public QDialog {
  Q_OBJECT

 public:
  explicit EditRecord(QWidget *parent = nullptr);
  ~EditRecord();
  Ui::EditRecord *ui;

  static void saveCustomDesc();
  QString lblStyleHighLight =
      "QLabel{background: rgb(45,182,116); color:white;}";
  static int removeDuplicates(QStringList *that);

  void init_Desc();

  void getTime(int h, int m);

  void init();
  void showGrayWindows();

  static void saveOne();

 protected:
  void keyReleaseEvent(QKeyEvent *event) override;
  bool eventFilter(QObject *watch, QEvent *evn) override;
 private slots:
  void on_btnBack_clicked();

  void on_btnOk_clicked();

  void on_btn7_clicked();
  void on_btn8_clicked();
  void on_btn9_clicked();
  void on_btn4_clicked();
  void on_btn5_clicked();
  void on_btn6_clicked();
  void on_btn1_clicked();
  void on_btn2_clicked();
  void on_btn3_clicked();
  void on_btn0_clicked();
  void on_btnDot_clicked();
  void on_btnDel_clicked();

  void on_btnCustom_clicked();

  void on_btnClearAmount_clicked();

  void on_btnClearDesc_clicked();

  void on_editAmount_textChanged(const QString &arg1);

  void on_hsH_valueChanged(int value);

  void on_hsM_valueChanged(int value);

  void on_btnClearDetails_clicked();

  void on_editCategory_textChanged(const QString &arg1);

  void on_editDetails_textChanged();

 private:
  void set_Amount(QString Number);

  QString lblStyle;
};

#endif  // EDITRECORD_H
