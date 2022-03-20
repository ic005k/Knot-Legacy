#ifndef DLGTODO_H
#define DLGTODO_H

#include <QCheckBox>
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

  void add_Item(QString, bool);
  void saveTodo();

 protected:
  void keyReleaseEvent(QKeyEvent *event) override;

  void closeEvent(QCloseEvent *event) override;
  bool eventFilter(QObject *watch, QEvent *evn) override;
 private slots:
  void on_btnBack_clicked();

  void on_btnAdd_clicked();

  void on_listWidget_itemClicked(QListWidgetItem *item);

  void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

  void on_listWidget_currentRowChanged(int currentRow);

  void on_btnModi_clicked();

 private:
  QListWidgetItem *editItem;
};

#endif  // DLGTODO_H
