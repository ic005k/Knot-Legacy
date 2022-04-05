#ifndef DLGLIST_H
#define DLGLIST_H

#include <QDialog>
#include <QListWidgetItem>

namespace Ui {
class dlgList;
}

class dlgList : public QDialog {
  Q_OBJECT

 public:
  explicit dlgList(QWidget *parent = nullptr);
  ~dlgList();
  Ui::dlgList *ui;

 protected:
  void keyReleaseEvent(QKeyEvent *event);
  bool eventFilter(QObject *watch, QEvent *evn);
private slots:
  void on_listWidget_itemClicked(QListWidgetItem *item);

  void on_btnClear_clicked();

  void on_btnBack_clicked();

 private:
};

#endif  // DLGLIST_H
