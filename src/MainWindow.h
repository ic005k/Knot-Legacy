#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qmath.h>

#include <QAbstractButton>
#include <QAccelerometerReading>
#include <QClipboard>
#include <QInputMethod>

#ifdef Q_OS_ANDROID

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
#include <QAndroidJniEnvironment>
#include <QAndroidJniObject>
#include <QtAndroid>
#else
#include <QCoreApplication>
#include <QJniEnvironment>
#include <QJniObject>
#endif

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

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
#include <QDesktopWidget>
#endif

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
#include <QQmlEngine>
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

#include "src/AboutThis.h"
#include "src/CategoryList.h"
#include "src/CloudBackup.h"
#include "src/Comm/FileSystemWatcher.h"
#include "src/Comm/Method.h"
#include "src/Comm/ReceiveShare.h"
#include "src/Comm/ShowMessage.h"
#include "src/EditRecord.h"
#include "src/LoadPic.h"
#include "src/Notes/MarkdownHighlighter.h"
#include "src/Notes/Notes.h"
#include "src/Notes/NotesList.h"
#include "src/Preferences.h"
#include "src/Reader/File.h"
#include "src/Reader/PageIndicator.h"
#include "src/Reader/Reader.h"
#include "src/Reader/ReaderSet.h"
#include "src/Reader/SetReaderText.h"
#include "src/Report.h"
#include "src/Steps/SpecialAccelerometerPedometer.h"
#include "src/Steps/Steps.h"
#include "src/Steps/StepsOptions.h"
#include "src/SyncInfo.h"
#include "src/Todo/Todo.h"
#include "src/Todo/TodoAlarm.h"
#include "src/components/qtmaterialcircularprogress.h"
#include "src/components/qtmaterialdialog.h"
#include "ui_AboutThis.h"
#include "ui_CategoryList.h"
#include "ui_CloudBackup.h"
#include "ui_DateSelector.h"
#include "ui_Notes.h"
#include "ui_NotesList.h"
#include "ui_PageIndicator.h"
#include "ui_Preferences.h"
#include "ui_ReaderSet.h"
#include "ui_ReceiveShare.h"
#include "ui_SetReaderText.h"
#include "ui_ShowMessage.h"
#include "ui_StepsOptions.h"
#include "ui_SyncInfo.h"
#include "ui_TextSelector.h"
#include "ui_Todo.h"
#include "ui_TodoAlarm.h"

class SaveThread;
class ReadThread;
class ReadTWThread;
class ReadEBookThread;
class BakDataThread;
class ImportDataThread;
class SearchThread;

#include <QMetaType>

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
QT_CHARTS_USE_NAMESPACE
#endif

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

  bool isEditItem = false;
  bool isDelItem = false;

  bool isNeedAutoBackup = false;
  QString strLatestModify = tr("None");

  QDialog *dlgProg;

  void setMini();
  bool isSelf = false;
  int days = 45;
  int newHeight;
  QInputMethod *pAndroidKeyboard = QApplication::inputMethod();
  QString androidIniDir, macIniDir;
  bool isMemoVisible;
  bool isReaderVisible;
  QString treeStyle = "QTreeWidget::item {height: 28;}";
  int mainHeight = 0;
  bool floatfun = true;
  int alertWindowsCount = 0;
  QString strText;
  QString strRefreshUrl;
  QString CurrentYear = "20xx";
  qreal curx = 0;
  bool isMousePress = false;
  bool isMouseMove = false;
  bool isTurnThePage = false;

  bool initMain = false;
  QObjectList getAllUIControls(QObject *parent);
  QString LatestTime = tr("Latest Time");
  QStringList timeLines;
  QString loginTime;

  int mwh;
  bool isOne = false;
  int isHardStepSensor = -1;
  qlonglong initTodaySteps, resetSteps, tc;
  QString listStyle =
      "QListWidget{item-background: "
      "rgb(244,23,24); item-color:rgb(255,255,255); "
      "border-radius:2px;border:2px "
      "solid red;}";
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
  SaveThread *mySaveThread;
  ReadThread *myReadThread;
  ReadTWThread *m_ReadTWThread;
  ReadEBookThread *myReadEBookThread;
  BakDataThread *myBakDataThread;
  ImportDataThread *myImportDataThread;
  SearchThread *mySearchThread;
  static void ReadChartData();
  static int get_Day(QString date);
  static QString get_Year(QString date);
  static QString get_Month(QString date);
  void init_Options();

  AboutThis *m_AboutThis;
  EditRecord *m_EditRecord;
  Todo *m_Todo;
  Report *m_Report;
  Preferences *m_Preferences;
  Notes *m_Notes;
  Steps *m_Steps;
  Reader *m_Reader;
  TodoAlarm *m_TodoAlarm;
  CloudBackup *m_CloudBackup;
  PageIndicator *m_PageIndicator;

  dlgSetText *mydlgSetText;
  NotesList *m_NotesList;
  SyncInfo *m_SyncInfo;
  StepsOptions *m_StepsOptions;
  ReceiveShare *m_ReceiveShare;

  File *myfile;

  QList<QTreeWidgetItem *> findItemList;
  bool isFindTextChange = false;
  int findPos = 0;
  bool isAdd = false;
  QTimer *timer;
  QTimer *timerStep;
  QTimer *timerMousePress;
  QTimer *timerSyncData;

  static void saveData(QTreeWidget *, int);
  static void readData(QTreeWidget *);

  void initChartMonth();
  void initChartDay();
  static void saveRemarks(int);

  static void saveTab();
  bool isSlide = false;
  void init_TotalData();
  void modify_Data();
  void add_Data(QTreeWidget *, QString, QString, QString);
  void del_Data(QTreeWidget *);
  static QTreeWidget *get_tw(int tabIndex);

  void sort_childItem(QTreeWidgetItem *);
  static QString getFileSize(const qint64 &size, int precision);

  static QStringList get_MonthList(QString strY, QString strM);
  static void drawMonthChart();

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

  QString secondsToTime(ulong ulSeconds);
  void stopJavaTimer();
  void pausePedometer();
  void sendMsg(int);
  void initTodayInitSteps();

  QString getYMD(QString date);
  QString bakData(QString fileName, bool msgbox);
  bool importBakData(QString fileName, bool msg, bool book, bool unre);

  QString getTabText();

  QObjectList getAllToolButton(QObjectList lstUIControls);

  void refreshMainUI();

  void clearSelectBox();

  QString getSelectedText();

  void bakIniData(QString unredoFile, bool unre);

  void set_ToolButtonStyle(QObject *parent);
  int calcStringPixelWidth(QString s_str, QFont font, int n_font_size);
  int calcStringPixelHeight(QFont font, int n_font_size);

  void delItem(int index);
  int getCount();
  void clearAll();
  void reloadMain();
  void setCurrentIndex(int index);
  void gotoEnd();
  void gotoIndex(int index);
  void setItemHeight(int h);

  QString getTop(int index);
  QString getText0(int index);
  int getItemType(int index);
  int getCurrentIndex();
  bool setTWCurrentItem();
  QString getText1(int index);
  QString getText2(int index);
  void gotoMainItem(QTreeWidgetItem *item);
  void startInitReport();

  void startSyncData();
  void removeFilesWatch();
  void addFilesWatch();

  int getMaxDay(QString sy, QString sm);
  void showProgress();

  bool copyFileToPath(QString sourceDir, QString toDir, bool coverFileIfExist);

  void setScrollBarPos(double pos);

  void init_Theme();

  void addItem(QString text0, QString text1, QString text2, QString text3,
               int itemH);

  void set_PushButtonStyle(QObject *parent);

  void execDeskShortcut();

  void on_DelayCloseProgressBar();

 protected:
  void closeEvent(QCloseEvent *event) override;
  bool eventFilter(QObject *watch, QEvent *evn) override;
  void paintEvent(QPaintEvent *event) override;
  void changeEvent(QEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;

 public slots:
  void on_hSlider_sliderMoved(int position);
  void clickMainTab();
  void on_SetReaderFunVisible();
  void updateSteps();
  void newDatas();
  void updateHardSensorSteps();
  void on_btnTodo_clicked();
  void readEBookDone();
  void on_btnPageUp_clicked();
  void on_btnPageNext_clicked();

  void on_actionOneDriveBackupData();
  void on_AddRecord();
  void on_actionReport_triggered();

  void on_btnCopy_clicked();

  void on_btnSearch_clicked();

  void on_btnCancelSel_clicked();

  void on_timerSyncData();
  void timerUpdate();

  void on_actionRename_triggered();

  void on_actionAdd_Tab_triggered();

  void on_actionDel_Tab_triggered();

  void on_tabWidget_currentChanged(int index);

  void on_btnRemarks_clicked();

  void on_actionExport_Data_triggered();

  void on_actionImport_Data_triggered();

  void on_twItemClicked();

  void on_twItemDoubleClicked();

  void on_actionAbout();

  void on_btnFind_clicked();

  void on_actionFind_triggered();

  void on_rbFreq_clicked();

  void on_rbAmount_clicked();

  void saveDone();

  void bakDataDone();

  void importDataDone();

  void readChartDone();

  void readTWDone();

  void searchDone();

  void on_actionPreferences_triggered();

  void on_tabCharts_currentChanged(int index);

  void on_btnSteps_clicked();

  void slotPointHoverd(const QPointF &point, bool state);

  void on_rbSteps_clicked();

  void on_btnSelTab_clicked();

  void on_btnMenu_clicked();

  void on_btnPause_clicked();

  void on_btnReader_clicked();

  void on_btnBackReader_clicked();

  void on_btnOpen_clicked();

  void on_btnPages_clicked();

  void on_btnReadList_clicked();

  void on_btnBackDir_clicked();

  void on_actionBakFileList();

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

  void on_btnBackNotes_clicked();

  void on_btnSetKey_clicked();

  void on_btnSetKeyOK_clicked();

  void on_btnEdit_clicked();

  void on_btnCode_clicked();

  void on_btnNotes_clicked();

  void on_btnZoomIn_clicked();

  void on_btnZoomOut_clicked();

  void on_openKnotBakDir();

  void reeditData();

  void clickData();

 public slots:
  void on_btnStartDate_clicked();
  void on_btnEndDate_clicked();
  void on_ExecShortcut();

  void on_btnShowBookmark_clicked();

  void on_btnBackBookList_clicked();

  void on_btnBackEditRecord_clicked();

  void on_btnBackNoteList_clicked();

  void on_btnBackReaderSet_clicked();

  void on_btnBackNoteRecycle_clicked();

  void on_textBrowser_selectionChanged();

  void on_timerMousePress();

  void on_btnNotesList_clicked();

  void on_btnBackImg_clicked();

  void on_btnReport_clicked();

  void on_btnPasteCode_clicked();

  void on_btnAdd_clicked();

  void on_btnDel_clicked();

  void on_KVChanged();

  void on_btnAddTodo_clicked();

  void on_btnBackTodo_clicked();

  void on_btnHigh_clicked();

  void on_btnLow_clicked();

  void on_btnSetTime_clicked();

  void on_btnRecycle_clicked();

  void on_btnReturnRecycle_clicked();

  void on_btnClearRecycle_clicked();

  void on_btnDelRecycle_clicked();

  void on_btnRestoreRecycle_clicked();

  void on_editTodo_textChanged();

  void on_btnBackSteps_clicked();

  void on_btnPauseSteps_clicked();

  void on_btnReset_clicked();

  void on_btnBack_Report_clicked();

  void on_btnYear_clicked();

  void on_btnMonth_clicked();

  void on_btnCategory_clicked();

  void closeProgress();

  void selTab();
  void stopTimerForPdf();

 private slots:
  void on_btnSync_clicked();

  void on_btnPDF_clicked();

  void on_btnPasteTodo_clicked();

  void on_actionTabRecycle();

  void on_btnBackSearch_clicked();

  void on_btnClearSearchText_clicked();

  void on_btnStartSearch_clicked();

  void on_btnBackBakList_clicked();

  void on_btnImportBakList_clicked();

  void on_btnOkViewCate_clicked();

  void on_btnBackTabRecycle_clicked();

  void on_btnDelTabRecycle_clicked();

  void on_btnRestoreTab_clicked();

  void on_btnDelBakFile_clicked();

  void on_btnNoteRecycle_clicked();

  void on_btnDelNoteRecycle_clicked();

  void on_btnRestoreNoteRecycle_clicked();

  void on_btnFindNotes_clicked();

  void on_editFindNote_textChanged(const QString &arg1);

  void on_btnFindPreviousNote_clicked();

  void on_btnFindNextNote_clicked();

  void on_btnClearNoteFindText_clicked();

  void on_btnShowFindNotes_clicked();

  void on_btnNoteBookMenu_clicked();

  void on_btnNoteMenu_clicked();

  void on_btnCancelType_clicked();

  void on_btnOkType_clicked();

  void on_btnDelType_clicked();

  void on_btnRenameType_clicked();

  void on_btnBackSetTab_clicked();

  void on_btnType_clicked();

  void on_btnOkEditRecord_clicked();

  void on_btnClearType_clicked();

  void on_btnClearDetails_clicked();

  void on_btnClearAmount_clicked();

  void on_editAmount_textChanged(const QString &arg1);

  void on_editCategory_textChanged(const QString &arg1);

  void on_editDetails_textChanged();

  void on_hsH_valueChanged(int value);

  void on_hsM_valueChanged(int value);

  void on_btn7_clicked();

  void on_btn8_clicked();

  void on_btn9_clicked();

  void on_btn4_clicked();

  void on_btn5_clicked();

  void on_btn6_clicked();

  void on_btn1_clicked();

  void on_btn2_clicked();

  void on_btn3_clicked();

  void on_btn0_clicked();

  void on_btnDot_clicked();

  void on_btnDel_Number_clicked();

  void on_btnOkBookList_clicked();

  void on_btnClearAllRecords_clicked();

  void on_btnAnd_clicked();

  void on_btnClear_clicked();

  void on_btnModify_clicked();

  void on_btnChartMonth_clicked();

  void on_btnChartDay_clicked();

  void on_btnTabMoveUp_clicked();

  void on_btnTabMoveDown_clicked();

  void on_btnChart_clicked();

  void on_btnManagement_clicked();

  void on_btnUpMove_clicked();

  void on_btnDownMove_clicked();

  void on_btnDelNote_NoteBook_clicked();

  void on_btnMoveTo_clicked();

  void on_btnBack_Tree_clicked();

  void on_btnRename_clicked();

  void on_btnHideFind_clicked();

  void on_btnStepsOptions_clicked();

  void on_btnRecentOpen_clicked();

  void on_btnMenuReport_clicked();

  void on_btnCatalogue_clicked();

  void on_btnRemoveBookList_clicked();

  void on_btnGoBack_clicked();

  void on_ReceiveShare();

  void on_btnShareImage_clicked();

  void on_actionShareFile();

  void on_btnHideKey_clicked();

  void on_btnDelImage_clicked();

  void on_btnSetBookmark_clicked();

  void on_btnFontLess_clicked();

  void on_btnFontPlus_clicked();

  void on_btnFont_clicked();

  void on_btnBackgroundColor_clicked();

  void on_btnForegroundColor_clicked();

  void on_editBackgroundColor_textChanged(const QString &arg1);

  void on_editForegroundColor_textChanged(const QString &arg1);

  void on_btnStyle1_clicked();

  void on_btnStyle2_clicked();

  void on_btnStyle3_clicked();

  void on_btnGoPage_clicked();

  void on_hSlider_sliderReleased();

  void on_CloseProgressBar();

  void on_btnShareBook_clicked();

  void slotSetBookmark();
  private:
  QString keyType;
  bool isShowDetails = false;
  QString strShowDetails;
  bool isTabChanged = false;
  bool isAndroidKeyShow = false;

  int frameChartHeight;
  int yScale = 3;
  int x, y, w, h;
  qreal aoldX, aoldY, aoldZ;
  int countOne = 0;
  int max_day = 31;
  QTreeWidget *init_TreeWidget(QString);
  QObjectList getAllTreeWidget(QObjectList lstUIControls);

  void init_ChartWidget();
  void init_Sensors();
  void init_UIWidget();
  void init_Menu(QMenu *);

  void updateRunTime();
  void showSensorValues();
  QString decMemos(QString strDec, QString file);
  void initHardStepSensor();
  void showNotes();
  void resetWinPos();

  void initQW();

  void init_Instance();
  void updateMainTab();
  void getMainTabs();
  QObjectList getAllPushButton(QObjectList lstUIControls);
  void init_ButtonStyle();
};

class SaveThread : public QThread {
  Q_OBJECT
 public:
  explicit SaveThread(QObject *parent = nullptr);

 protected:
  void run();
 signals:
  void isDone();

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

class BakDataThread : public QThread {
  Q_OBJECT
 public:
  explicit BakDataThread(QObject *parent = nullptr);

 protected:
  void run();
 signals:
  void isDone();

 signals:

 public slots:
};

class ImportDataThread : public QThread {
  Q_OBJECT
 public:
  explicit ImportDataThread(QObject *parent = nullptr);

 protected:
  void run();
 signals:
  void isDone();

 signals:

 public slots:
};

class SearchThread : public QThread {
  Q_OBJECT
 public:
  explicit SearchThread(QObject *parent = nullptr);

 protected:
  void run();
 signals:
  void isDone();

 signals:

 public slots:
};

#endif  // MAINWINDOW_H
