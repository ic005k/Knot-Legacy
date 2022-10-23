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
  void keyReleaseEvent(QKeyEvent *event) override;
  bool eventFilter(QObject *watch, QEvent *evn) override;
  void closeEvent(QCloseEvent *event) override;
 private slots:
  void on_listWidget_itemClicked(QListWidgetItem *item);

  void on_btnClear_clicked();

  void on_btnBack_clicked();

  void on_btnChange_clicked();

  void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

 private:
  void setCategoryText();
};

#endif  // DLGLIST_H
