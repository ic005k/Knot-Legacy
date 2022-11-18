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

  bool isToday = false;

  void init_Items();
  QString alarmStyle =
      "QLabel{background:rgb(112,128,105);color:rgb(255,255,255);border:2px;"
      "border-radius:4px;padding:2px 4px;}";
  QString alarmStyleToday =
      "QLabel{background:rgb(255,11,25);color:white;border:2px;"
      "border-radius:4px;padding:2px 4px;}";
  QString alarmStyleTomorrow =
      "QLabel{background:rgb(245,245,25);color:black;border:2px;"
      "border-radius:4px;padding:2px 4px;}";

  void saveTodo();
  QString styleDark =
      "#listWidget::item {background-color: #393d49;color: #ffffff;border: "
      "transparent;padding: 8px; height: 65;}"
      "#listWidget::item:hover {background-color: #4e5465;}"
      "#listWidget::item:selected {border-left: 5px solid #009688;}";

  void startTimerAlarm(QString text);
  void stopTimerAlarm();

  void sendMsgAlarm(QString text);

  qlonglong getSecond(QString strDateTime);
  void refreshAlarm();
  int getEditTextHeight(QTextEdit *edit);

  void insertItem(QString strTime, int type, QString strText, int curIndex);
  int getCurrentIndex();
  QString getItemTime(int index);
  QString getItemTodoText(int index);
  void delItem(int index);
  void setCurrentIndex(int index);
  void setHighPriority(bool isBool);

  int getCount();

  void modifyTime(int index, QString strTime);
  void modifyTodoText(int index, QString strTodoText);
  void clearAll();
  void addItem(QString strTime, int type, QString strText);
  int getItemType(int index);
  void modifyType(int index, int type);
  public slots:
  void on_btnOK_clicked();
  void on_btnCancel_clicked();
  void isAlarm(int index);

 protected:
  void keyReleaseEvent(QKeyEvent *event) override;
  void closeEvent(QCloseEvent *event) override;
  bool eventFilter(QObject *watch, QEvent *evn) override;
 public slots:

  void on_btnAdd_clicked();

  void on_btnHigh_clicked();

  void on_btnLow_clicked();

  void on_btnSetTime_clicked();

  void on_btnRecycle_clicked();

  void on_btnReturn_clicked();

  void on_btnClear_clicked();

  void on_btnRestore_clicked();

  void on_btnDel_clicked();

  void on_textEdit_textChanged();

  void reeditText();
  void addToRecycle();

 private:
  QListWidgetItem *editItem;

  QLabel *lblModi;
  QTextEdit *editModi;
  bool isModi = false;
  bool isWeekValid(QString lblDateTime, QString strDate);
  bool isTomorrow = false;
};
#endif  // DLGTODO_H
