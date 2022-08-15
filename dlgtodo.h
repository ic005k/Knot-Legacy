#ifndef DLGTODO_H
#define DLGTODO_H

#include <QCheckBox>
#include <QDialog>
#include <QKeyEvent>
#include <QLabel>
#include <QListWidgetItem>
#include <QTextEdit>
#include <QToolButton>

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
  QString alarmStyle = "QLabel{background:rgb(66,114,255);color:white}";
  void add_Item(QString, QString, bool);
  static void saveTodo();
  QString styleDark =
      "#listWidget::item {background-color: #393d49;color: #ffffff;border: "
      "transparent;padding: 8px; height: 65;}"
      "#listWidget::item:hover {background-color: #4e5465;}"
      "#listWidget::item:selected {border-left: 5px solid #009688;}";

  QLabel *getTimeLabel(int);
  void on_Alarm();
  QLabel *getMainLabel(int);
  void startTimerAlarm(QString text);
  void stopTimerAlarm();

  void sendMsgAlarm(QString text);

  qlonglong getSecond(QString strDateTime);
  void refreshAlarm();
 public slots:
  void on_btnOK_clicked();
  void on_btnCancel_clicked();

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

  void on_btnModify_clicked();

  void on_btnHigh_clicked();

  void on_btnLow_clicked();

  void on_btnSetTime_clicked();

  void on_btnRecycle_clicked();

  void on_btnReturn_clicked();

  void on_btnClear_clicked();

  void on_btnRestore_clicked();

  void on_btnDel_clicked();

 private:
  QListWidgetItem *editItem;
  void add_ItemSn(int index);
  QLabel *lblModi;
  QTextEdit *editModi;
  bool isModi = false;
};
#endif  // DLGTODO_H
