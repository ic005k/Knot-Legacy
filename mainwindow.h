#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QHeaderView>
#include <QInputDialog>
#include <QMainWindow>
#include <QSettings>
#include <QTextEdit>
#include <QTimer>
#include <QTreeWidgetItem>

#include "chart.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT
  Chart *chart;

 public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();
  QTimer *tmer;
  QString strDate;
  void saveData(QTreeWidget *);
  void readData(QTreeWidget *);
  QString loadText(QString textFile);
  void TextEditToFile(QTextEdit *txtEdit, QString fileName);
  void initChart(QTreeWidget *);

  bool isInit = false;
  int today = 0;
  bool isIOS = false;
 public slots:
  void init_Stats(QTreeWidget *);

 protected:
  void closeEvent(QCloseEvent *event);
 private slots:

  void timerUpdate();

  void on_btnPlus_clicked();

  void on_btnLess_clicked();

  void on_actionRename_triggered();

  void on_actionAdd_Tab_triggered();

  void on_actionDel_Tab_triggered();

  void on_tabWidget_currentChanged(int index);

  void on_textEdit_textChanged();

 private:
  bool loading = false;
  Ui::MainWindow *ui;
  void get_Today(QTreeWidget *);
  void add_Data(QTreeWidget *);
  void del_Data(QTreeWidget *);
  void init_TreeWidget(QTreeWidget *);
  QObjectList getAllTreeWidget(QObjectList lstUIControls);
  QObjectList getAllUIControls(QObject *parent);
  void saveTab();
};
#endif  // MAINWINDOW_H
