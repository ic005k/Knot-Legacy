#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAbstractButton>
#include <QDateTime>
#include <QDebug>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QDir>
#include <QElapsedTimer>
#include <QFileDialog>
#include <QHeaderView>
#include <QInputDialog>
#include <QMainWindow>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QScatterSeries>
#include <QScrollBar>
#include <QScroller>
#include <QSettings>
#include <QStringList>
#include <QTextEdit>
#include <QTimer>
#include <QToolTip>
#include <QTreeWidgetItem>

#include "chart.h"
#include "dlgnotes.h"
#include "dlgrename.h"
#include "dlgsettime.h"
#include "ui_dlgnotes.h"
#include "ui_dlgrename.h"
#include "ui_dlgsettime.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT
  Chart *chart;
  Chart *chartTimeLine;

 public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();
  Ui::MainWindow *ui;
  dlgNotes *mydlgNotes;
  dlgRename *mydlgRename;
  dlgSetTime *mydlgSetTime;
  QTreeWidgetItem *parentItem;
  bool isAdd = false;
  QTimer *tmer;
  QString strDate;
  void saveData(QTreeWidget *, int);
  void readData(QTreeWidget *);
  QString loadText(QString textFile);
  void TextEditToFile(QTextEdit *txtEdit, QString fileName);
  void initChart(QTreeWidget *);
  void saveNotes();
  bool isInit = false;
  int today = 0;
  void saveTab();
  bool isSlide = false;
  void init_Data();
  void set_Time();
  void add_Data(QTreeWidget *, QString, QString);
  void del_Data(QTreeWidget *);
  QTreeWidget *get_tw(int tabIndex);
  QString vsbarStyle =
      "QScrollBar:vertical{"  //垂直滑块整体
      "width:35px;"
      "background:#FFFFFF;"   //背景色
      "padding-top:32px;"     //上预留位置（放置向上箭头）
      "padding-bottom:32px;"  //下预留位置（放置向下箭头）
      "padding-left:3px;"     //左预留位置（美观）
      "padding-right:3px;"    //右预留位置（美观）
      "border-left:1px solid #d7d7d7;}"     //左分割线
      "QScrollBar::handle:vertical{"        //滑块样式
      "background:#dbdbdb;"                 //滑块颜色
      "border-radius:6px;"                  //边角圆润
      "min-height:60px;}"                   //滑块最小高度
      "QScrollBar::handle:vertical:hover{"  //鼠标触及滑块样式
      "background:#d0d0d0;}"                //滑块颜色
      "QScrollBar::add-line:vertical{"      //向下箭头样式
      "background:url(:/src/down.png) center no-repeat;}"
      "QScrollBar::sub-line:vertical{"  //向上箭头样式
      "background:url(:/src/up.png) center no-repeat;}";
  QString vsbarStyleSmall =
      "QScrollBar:vertical{"  //垂直滑块整体
      "width:10px;"
      "background:#FFFFFF;"  //背景色
      "padding-top:3px;"     //上预留位置（放置向上箭头）
      "padding-bottom:3px;"  //下预留位置（放置向下箭头）
      "padding-left:3px;"    //左预留位置（美观）
      "padding-right:3px;"   //右预留位置（美观）
      "border-left:1px solid #d7d7d7;}"     //左分割线
      "QScrollBar::handle:vertical{"        //滑块样式
      "background:#dbdbdb;"                 //滑块颜色
      "border-radius:6px;"                  //边角圆润
      "min-height:60px;}"                   //滑块最小高度
      "QScrollBar::handle:vertical:hover{"  //鼠标触及滑块样式
      "background:#d0d0d0;}"                //滑块颜色
      "QScrollBar::add-line:vertical{"      //向下箭头样式
      "background:url(:/src/down1.png) center no-repeat;}"
      "QScrollBar::sub-line:vertical{"  //向上箭头样式
      "background:url(:/src/up1.png) center no-repeat;}";
  void sort_childItem(QTreeWidgetItem *);
  QString getFileSize(const qint64 &size, int precision);
 public slots:
  void init_Stats(QTreeWidget *);

 protected:
  void closeEvent(QCloseEvent *event);
  bool eventFilter(QObject *watch, QEvent *evn);
 private slots:

  void timerUpdate();

  void on_btnPlus_clicked();

  void on_btnLess_clicked();

  void on_actionRename_triggered();

  void on_actionAdd_Tab_triggered();

  void on_actionDel_Tab_triggered();

  void on_tabWidget_currentChanged(int index);

  void on_btnLeft_clicked();

  void on_btnRight_clicked();

  void on_actionNotes_triggered();

  void on_btnNotes_clicked();

  void on_actionAbout_triggered();

  void on_actionExport_Data_triggered();

  void on_actionImport_Data_triggered();

  void on_twItemClicked();

  void on_twItemDoubleClicked();

  void on_tabWidget_tabBarClicked(int index);

  void on_tabCharts_tabBarClicked(int index);

  void on_actionView_App_Data_triggered();

 private:
  int x, y, w, h;
  void get_Today(QTreeWidget *);
  QTreeWidget *init_TreeWidget(QString);
  QObjectList getAllTreeWidget(QObjectList lstUIControls);
  QObjectList getAllUIControls(QObject *parent);
  QString init_Objname();
  void initChartTimeLine(QTreeWidget *tw);
  int get_Day(QString date);
  QString get_Year(QString date);
  QString get_Month(QString date);
};
#endif  // MAINWINDOW_H
