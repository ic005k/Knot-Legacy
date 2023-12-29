#ifndef CATEGORYLIST_H
#define CATEGORYLIST_H

#include <QDialog>
#include <QListWidgetItem>

#include "ShowMessage.h"

namespace Ui {
class dlgList;
}

class dlgList : public QDialog {
  Q_OBJECT

 public:
  explicit dlgList(QWidget *parent = nullptr);
  ~dlgList();
  Ui::dlgList *ui;

  QFrame *frame;

 protected:
  void keyReleaseEvent(QKeyEvent *event) override;
  bool eventFilter(QObject *watch, QEvent *evn) override;
  void closeEvent(QCloseEvent *event) override;
 public slots:
  void on_listWidget_itemClicked(QListWidgetItem *item);

  void on_btnDel_clicked();

  void on_btnOk_clicked();

  void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

  void on_btnRename_clicked();

  void on_btnCancel_clicked();

 private slots:

 private:
  void setCategoryText();
};

#endif  // CATEGORYLIST_H
