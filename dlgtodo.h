#ifndef DLGTODO_H
#define DLGTODO_H

#include <QDialog>
#include <QKeyEvent>
#include <QListWidgetItem>

namespace Ui {
class dlgTodo;
}

class dlgTodo : public QDialog {
  Q_OBJECT

 public:
  explicit dlgTodo(QWidget *parent = nullptr);
  Ui::dlgTodo *ui;
  ~dlgTodo();

  void init_Items();

 protected:
  void keyReleaseEvent(QKeyEvent *event) override;

 private slots:
  void on_btnBack_clicked();

  void on_btnAdd_clicked();

  void on_listWidget_itemClicked(QListWidgetItem *item);

 private:
};

#endif  // DLGTODO_H
