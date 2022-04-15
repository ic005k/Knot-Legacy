#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qmath.h>

#include <QAbstractButton>
#include <QAccelerometerReading>
#ifdef Q_OS_ANDROID
#include <QAndroidJniObject>
#include <QtAndroid>
#endif
#include <QBarCategoryAxis>
#include <QBarSeries>
#include <QBarSet>
#include <QChart>
#include <QChartView>
#include <QComboBox>
#include <QDateTime>
#include <QDebug>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QDir>
#include <QElapsedTimer>
#include <QFileDialog>
#include <QGyroscope>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QInputDialog>
#include <QLineEdit>
#include <QMainWindow>
#include <QMessageBox>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QRandomGenerator>
#include <QScatterSeries>
#include <QScrollBar>
#include <QScroller>
#include <QSettings>
#include <QSplineSeries>
#include <QSplitter>
#include <QStringList>
#include <QTextEdit>
#include <QThread>
#include <QTimer>
#include <QToolTip>
#include <QTreeWidgetItem>
#include <QValueAxis>

#include "dlglist.h"
#include "dlgmainnotes.h"
#include "dlgnotes.h"
#include "dlgpreferences.h"
#include "dlgrename.h"
#include "dlgreport.h"
#include "dlgsettime.h"
#include "dlgsteps.h"
#include "dlgtodo.h"
#include "specialaccelerometerpedometer.h"
#include "ui_dlglist.h"
#include "ui_dlgmainnotes.h"
#include "ui_dlgnotes.h"
#include "ui_dlgpreferences.h"
#include "ui_dlgrename.h"
#include "ui_dlgreport.h"
#include "ui_dlgsettime.h"
#include "ui_dlgsteps.h"
#include "ui_dlgtodo.h"
class SearchThread;
class ReadThread;
class ReadTWThread;

QT_CHARTS_USE_NAMESPACE
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  bool isTesting = false;
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();
  Ui::MainWindow *ui;

  qreal ax, ay, az, gx, gy, gz;
  int testCount1 = 0;
  int testCount = 0;
  ulong timeTest = 0;
  ulong timeCount = 0;
  QVector<int8_t> a3list;
  QChartView *chartview;
  QChartView *chartview1;
  QLabel *m_valueLabel;
  QBarCategoryAxis *axisX;
  QValueAxis *axisY;
  QValueAxis *axisX2;
  QValueAxis *axisY2;
  qlonglong CurrentSteps = 0;
  qlonglong CurTableCount = 0;
  SpecialAccelerometerPedometer *accel_pedometer;
  QGyroscope *gyroscope;
  QChart *chartMonth;
  QChart *chartDay;
  QBarSeries *barSeries;
  QSplineSeries *series;
  QScatterSeries *m_scatterSeries;
  QSplineSeries *series2;
  QScatterSeries *m_scatterSeries2;
  QScatterSeries *m_scatterSeries2_1;
  static void get_Today(QTreeWidget *);
  SearchThread *mySearchThread;
  ReadThread *myReadThread;
  ReadTWThread *myReadTWThread;
  static void ReadChartData();
  static int get_Day(QString date);
  static QString get_Year(QString date);
  static QString get_Month(QString date);
  void init_Options();
  QStringList listMonth;
  dlgNotes *mydlgNotes;
  dlgRename *mydlgRename;
  dlgSetTime *mydlgSetTime;
  dlgTodo *mydlgTodo;
  dlgList *mydlgList;
  dlgReport *mydlgReport;
  dlgPreferences *mydlgPre;
  dlgMainNotes *mydlgMainNotes;
  dlgSteps *mydlgSteps;

  QList<QTreeWidgetItem *> findItemList;
  bool isFindTextChange = false;
  int findPos = 0;
  bool isAdd = false;
  QTimer *timer;
  QTimer *timerStep;

  static void saveData(QTreeWidget *, int);
  static void readData(QTreeWidget *);
  static QString loadText(QString textFile);
  static void TextEditToFile(QTextEdit *txtEdit, QString fileName);
  void initChartMonth(QString, QString);
  void initChartDay();
  static void saveNotes(int);
  bool isInit = false;

  static void saveTab();
  bool isSlide = false;
  void init_TabData();
  void set_Time();
  void add_Data(QTreeWidget *, QString, QString, QString);
  void del_Data(QTreeWidget *);
  QTreeWidget *get_tw(int tabIndex);
  QString vsbarStyle =
      "QScrollBar:vertical{"  //垂直滑块整体
      "width:30px;"
      "background:#FFFFFF;"   //背景色
      "padding-top:25px;"     //上预留位置（放置向上箭头）
      "padding-bottom:25px;"  //下预留位置（放置向下箭头）
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
      "background:url(:/src/down.png) bottom no-repeat;}"
      "QScrollBar::sub-line:vertical{"  //向上箭头样式
      "background:url(:/src/up.png) top no-repeat;}";
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
      "background:url() center no-repeat;}"
      "QScrollBar::sub-line:vertical{"  //向上箭头样式
      "background:url() center no-repeat;}";
  void sort_childItem(QTreeWidgetItem *);
  static QString getFileSize(const qint64 &size, int precision);

  static QStringList get_MonthList(QString strY, QString strM);
  static void drawMonthChart();
  QList<QTreeWidgetItem *> findDisc();
  QString setLineEditQss(QLineEdit *txt, int radius, int borderWidth,
                         const QString &normalColor, const QString &focusColor);
  QString setComboBoxQss(QComboBox *txt, int radius, int borderWidth,
                         const QString &normalColor, const QString &focusColor);
  static void SaveFile(QString);

  static void init_Stats(QTreeWidget *);

  void startSave(QString);
  void startRead(QString);
  static void drawDayChart();
  static void readDataInThread(int ExceptIndex);

  void Sleep(int msec);
  void getSteps2();
  QString secondsToTime(ulong ulSeconds);
 public slots:
  void updateSteps();
  void newDatas();

 protected:
  void closeEvent(QCloseEvent *event) override;
  bool eventFilter(QObject *watch, QEvent *evn) override;
  void paintEvent(QPaintEvent *event) override;
  void changeEvent(QEvent *event) override;

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

  void on_actionView_App_Data_triggered();

  void on_btnFind_clicked();

  void on_cboxYear_currentTextChanged(const QString &arg1);

  void on_btnGo_clicked();

  void on_editFind_textChanged(const QString &arg1);

  void on_btnHide_clicked();

  void on_actionFind_triggered();

  void on_btnTodo_clicked();

  void on_rbFreq_clicked();

  void on_rbAmount_clicked();

  void on_btnZoom_clicked();

  void on_btnMax_clicked();

  void on_btnYear_clicked();

  void on_btnMonth_clicked();

  void on_btnDay_clicked();

  void on_actionReport_triggered();

  void on_btnReport_clicked();

  void dealDone();

  void readDone();

  void on_actionPreferences_triggered();

  void on_tabCharts_currentChanged(int index);

  void readTWDone();

  void on_btnMainNotes_clicked();

  void slotPointHoverd(const QPointF &point, bool state);

  void on_rbSteps_clicked();

  void timerUpdateStep();

  void on_actionMemos_triggered();

  void on_btnSelTab_clicked();

 private:
  int x, y, w, h;

  qreal aoldX, aoldY, aoldZ;
  int countOne = 0;
  QTreeWidget *init_TreeWidget(QString);
  QObjectList getAllTreeWidget(QObjectList lstUIControls);
  QObjectList getAllUIControls(QObject *parent);

  QList<QToolButton *> listNBtn;
  void init_TabNavigate();
  void init_NavigateBtnColor();

  QString treeStyle = "QTreeWidget::item {height: 28;}";
  //"QTreeWidget::item {background-color: #ffffff;color: #000000;border: "
  //"transparent;border-bottom: 1px solid #dbdbdb; padding: 2px;height: 20;}"
  //"QTreeWidget::item:hover {background-color: #f5f5f5;}"
  //"QTreeWidget::item:selected {border-left: 0px solid #777777;}"
  void init_ChartWidget();
  void init_Sensors();
  void init_UIWidget();
};

class SearchThread : public QThread {
  Q_OBJECT
 public:
  explicit SearchThread(QObject *parent = nullptr);

 protected:
  void run();
 signals:
  void isDone();  //处理完成信号

 signals:

 public slots:
};

class ReadThread : public QThread {
  Q_OBJECT
 public:
  explicit ReadThread(QObject *parent = nullptr);

 protected:
  void run();
 signals:
  void isDone();

 signals:

 public slots:
};

class ReadTWThread : public QThread {
  Q_OBJECT
 public:
  explicit ReadTWThread(QObject *parent = nullptr);

 protected:
  void run();
 signals:
  void isDone();

 signals:

 public slots:
};

#endif  // MAINWINDOW_H
