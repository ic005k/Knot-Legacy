#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qmath.h>

#include <QAbstractButton>
#include <QAccelerometerReading>
#include <QClipboard>
#ifdef Q_OS_ANDROID
#include <QAndroidJniEnvironment>
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
#include <QMenu>
#include <QMessageBox>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWidget>
#include <QRandomGenerator>
#include <QScatterSeries>
#include <QScrollBar>
#include <QScroller>
#include <QSensorManager>
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

#include "dlgReaderFun.h"
#include "src/dlgMainNotes.h"
#include "src/dlgNotesList.h"
#include "src/dlgOneDrive.h"
#include "src/dlgRemarksAbout.h"
#include "src/dlgSetText.h"
#include "src/dlgfloatfun.h"
#include "src/dlglist.h"
#include "src/dlgloadpic.h"
#include "src/dlgpreferences.h"
#include "src/dlgreader.h"
#include "src/dlgrename.h"
#include "src/dlgreport.h"
#include "src/dlgsettime.h"
#include "src/dlgsteps.h"
#include "src/dlgtodo.h"
#include "src/file.h"
#include "src/onedrive/dlgweb.h"
#include "src/setTodoAlarm.h"
#include "src/specialaccelerometerpedometer.h"
#include "ui_dlgMainNotes.h"
#include "ui_dlgNotesList.h"
#include "ui_dlgReaderFun.h"
#include "ui_dlgRemarks.h"
#include "ui_dlgSetText.h"
#include "ui_dlgfloatfun.h"
#include "ui_dlglist.h"
#include "ui_dlgloadpic.h"
#include "ui_dlgpreferences.h"
#include "ui_dlgreader.h"
#include "ui_dlgrename.h"
#include "ui_dlgreport.h"
#include "ui_dlgsettime.h"
#include "ui_dlgsteps.h"
#include "ui_dlgtodo.h"
#include "ui_dlgweb.h"
#include "ui_msgdialog.h"

class SearchThread;
class ReadThread;
class ReadTWThread;
class ReadEBookThread;

#include <QMetaType>

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

  QString androidIniDir, macIniDir;
  bool isMemoVisible;
  bool isReaderVisible;
  int imgSize = 5000;
  QString treeStyle = "QTreeWidget::item {height: 28;}";
  QWidget *m_widget;
  int mainHeight = 0;
  bool floatfun = true;
  int alertWindowsCount = 0;
  QString strText;
  QString strRefreshUrl;
  qreal curx = 0;
  bool isMousePress = false;
  bool isMouseMove = false;
  bool isTurnThePage = false;
  bool isSelText = false;
  bool initMain = false;
  QObjectList getAllUIControls(QObject *parent);
  QString LatestTime = tr("Latest Time");
  QStringList timeLines;
  QString loginTime;
  QListWidget *listSelTab, *listSelFont, *listReadList, *listTimeMachine;
  int mwh;
  int textFontSize;
  bool isOne = false;
  int isHardStepSensor = -1;
  qlonglong initTodaySteps, resetSteps, tc;
  QString listStyle =
      "QListWidget{background: "
      "rgb(244,237,241);border-radius:0px;border:2px solid gray;}";
  int sRate = 0;
  qreal ax, ay, az, gx, gy, gz;
  int testCount1 = 0;
  int testCount = 0;
  int smallCount = 0;
  ulong timeTest = 0;
  ulong timeCount = 0;
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
  ReadEBookThread *myReadEBookThread;
  static void ReadChartData();
  static int get_Day(QString date);
  static QString get_Year(QString date);
  static QString get_Month(QString date);
  void init_Options();
  QStringList listMonth;
  dlgRemarks *m_Remarks;
  dlgRename *mydlgRename;
  dlgSetTime *mydlgSetTime;
  dlgTodo *mydlgTodo;
  dlgList *mydlgList;
  dlgReport *mydlgReport;
  dlgPreferences *mydlgPre;
  dlgMainNotes *mydlgMainNotes;
  dlgSteps *mydlgSteps;
  dlgReader *mydlgReader;
  msgDialog *mymsgDlg;
  TestDialog *mydlgOneDrive;
  dlgWeb *mydlgWeb;
  dlgFloatFun *mydlgFloatFun;
  dlgReaderFun *mydlgReaderFun;
  dlgSetText *mydlgSetText;
  dlgNotesList *m_NotesList;

  File *myfile;

  QList<QTreeWidgetItem *> findItemList;
  bool isFindTextChange = false;
  int findPos = 0;
  bool isAdd = false;
  QTimer *timer;
  QTimer *timerStep;
  QTimer *timerShowFloatFun;
  QTimer *timerMousePress;

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
  static QTreeWidget *get_tw(int tabIndex);
  QString listWidgetStyle =
      "QListWidget::indicator{width:25;height:25;right: 5px;}"
      "QListView {outline: none;}"
      "#listWidget::item {background-color: #ffffff;color: #000000;border: "
      "transparent;border-bottom: 1px solid #dbdbdb;padding: 8px;height: 85;}"
      "#listWidget::item:hover {background-color: #f5f5f5;}"
      "#listWidget::item:selected {border-left: 5px solid #777777;}";

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
      "width:6px;"
      "background:rgb(255,255,255,0);"  //背景色
      "padding-top:0px;"     //上预留位置（放置向上箭头）
      "padding-bottom:0px;"  //下预留位置（放置向下箭头）
      "padding-left:1px;"    //左预留位置（美观）
      "padding-right:1px;"   //右预留位置（美观）
      "border-left:0px solid #d7d7d7;}"     //左分割线
      "QScrollBar::handle:vertical{"        //滑块样式
      "background:#1296db;"                 //滑块颜色
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
  void stopJavaTimer();
  void pausePedometer();
  void sendMsg(int);
  void initTodayInitSteps();

  QString getYMD(QString date);
  void bakData(QString fileName, bool msgbox);
  void setSCrollPro(QObject *obj);

  bool importBakData(QString fileName, bool msg, bool book, bool unre);
  void addUndo(QString log);
  QString getTabText();
  void addRedo();

  QObjectList getAllToolButton(QObjectList lstUIControls);

  void refreshMainUI();
  void on_RunCategory();
  void clearSelectBox();
  bool showMsgBox(QString title, QString info, QString copyText,
                  int buttonCount);
  QString getSelectedText();

  void showGrayWindows();
  void closeGrayWindows();
  void bakIniData(QString unredoFile, bool unre);
 public slots:
  void on_SetReaderFunVisible();
  void updateSteps();
  void newDatas();
  void updateHardSensorSteps();
  void on_btnTodo_clicked();
  void readEBookDone();
  void on_btnPageUp_clicked();
  void on_btnPageNext_clicked();
  QString on_actionOneClickBakData(bool msg);
  void on_actionOneDriveBackupData();
  void on_AddRecord();
  void on_actionReport_triggered();
  void on_DelRecord();

 protected:
  void closeEvent(QCloseEvent *event) override;
  bool eventFilter(QObject *watch, QEvent *evn) override;
  void paintEvent(QPaintEvent *event) override;
  void changeEvent(QEvent *event) override;

 public slots:
  void on_btnCopy_clicked();

  void on_btnSearch_clicked();

  void on_btnCancelSel_clicked();

  void on_timerShowFloatFun();
  void timerUpdate();

  void on_actionRename_triggered();

  void on_actionAdd_Tab_triggered();

  void on_actionDel_Tab_triggered();

  void on_tabWidget_currentChanged(int index);

  void on_actionNotes_triggered();

  void on_btnNotes_clicked();

  void on_actionAbout_triggered();

  void on_actionExport_Data_triggered();

  void on_actionImport_Data_triggered();

  void on_twItemClicked();

  void on_twItemDoubleClicked();

  void on_about();

  void on_btnFind_clicked();

  void on_btnGo_clicked();

  void on_editFind_textChanged(const QString &arg1);

  void on_btnHide_clicked();

  void on_actionFind_triggered();

  void on_rbFreq_clicked();

  void on_rbAmount_clicked();

  void on_btnMax_clicked();

  void on_btnYear_clicked();

  void on_btnMonth_clicked();

  void on_btnDay_clicked();

  void dealDone();

  void readDone();

  void on_actionPreferences_triggered();

  void on_tabCharts_currentChanged(int index);

  void readTWDone();

  void on_btnSteps_clicked();

  void slotPointHoverd(const QPointF &point, bool state);

  void on_rbSteps_clicked();

  void on_actionMemos_triggered();

  void on_btnSelTab_clicked();

  void on_btnMenu_clicked();

  void on_btnPause_clicked();

  void on_btnReader_clicked();

  void on_btnBack_clicked();

  void on_btnOpen_clicked();

  void on_btnFont_clicked();

  void on_btnPages_clicked();

  void on_btnFontPlus_clicked();

  void on_btnFontLess_clicked();

  void on_hSlider_sliderMoved(int position);

  void on_btnReadList_clicked();

  void on_btnBackDir_clicked();

  void on_btnOneClickBak_clicked();

  void undo();
  void redo();

  void on_actionTimeMachine();

  void on_btnSelText_clicked();

  void on_btnSignIn_clicked();

  void on_btnSignOut_clicked();

  void on_btnUpload_clicked();

  void on_btnDownload_clicked();

  void on_btnBack_One_clicked();

  void on_btnRefreshToken_clicked();

  void on_btnStorageInfo_clicked();

  void on_btnRefreshWeb_clicked();

  void on_btnUserInfo_clicked();

  void on_btnBackMemo_clicked();

  void on_btnSetKey_clicked();

  void on_btnSetKeyOK_clicked();

  void on_btnEdit_clicked();

  void on_btnCode_clicked();

  void on_btnMemos_clicked();

  void on_btnZoomIn_clicked();

  void on_btnZoomOut_clicked();

 private slots:

  void on_textBrowser_selectionChanged();

  void on_timerMousePress();

  void on_btnNotesList_clicked();

  void on_btnBackImg_clicked();

  void on_btnReport_clicked();

 private:
  int memoHeight;
  int frameChartHeight = 220;
  int x, y, w, h;
  qreal aoldX, aoldY, aoldZ;
  int countOne = 0;
  QTreeWidget *init_TreeWidget(QString);
  QObjectList getAllTreeWidget(QObjectList lstUIControls);

  void init_ChartWidget();
  void init_Sensors();
  void init_UIWidget();
  void init_Menu(QMenu *);
  void on_btnZoom_clicked();
  void on_cboxYear_currentTextChanged(const QString &arg1);

  void updateRunTime();
  void showSensorValues();
  QString decMemos(QString strDec, QString file);
  void initHardStepSensor();
  void showMemos();
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

class ReadEBookThread : public QThread {
  Q_OBJECT
 public:
  explicit ReadEBookThread(QObject *parent = nullptr);

 protected:
  void run();
 signals:
  void isDone();

 signals:

 public slots:
};

#endif  // MAINWINDOW_H
