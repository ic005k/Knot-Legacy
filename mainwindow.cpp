#include "mainwindow.h"

#include "count_steps.h"
#include "math.h"  //using this for converting the CSV data from float to int
#include "stdint.h"
#include "stdio.h"  //using this for printing debug outputs
#include "ui_mainwindow.h"

QList<QPointF> PointList;
QList<double> doubleList;

QString ver = "1.0.08";
QGridLayout* gl1;
QTreeWidgetItem* parentItem;
bool isrbFreq = true;
bool isImport;
QString appName = "Knot";
QString iniFile, iniDir, strDate, readDate, noteText, strStats, SaveType, strY,
    strM, btnYText, btnMText, btnDText, CurrentYearMonth;
QStringList listM;

int curPos, sliderPos, today, fontSize, red, currentTabIndex;
double yMaxMonth, yMaxDay;
MainWindow* mw_one;
QTabWidget *tabData, *tabChart;
bool loading, isReadEnd, isReadTWEnd;
bool isSaveEnd = true;
bool isBreak = false;
extern bool isAndroid, isIOS, zh_cn;
extern QString btnYearText, btnMonthText, strPage;
QRegularExpression regxNumber("^-?\[0-9.]*$");

void RegJni();
void RegJniMyActivity();
static void JavaNotify_1();
static void JavaNotify_2();
static void JavaNotify_3();

/* Low pass filter for smoothening sensor input data */
static filter_t lp_filter_x, lp_filter_y, lp_filter_z;
static filter_t ll_filter_x, ll_filter_y, ll_filter_z;

/* Algo output data */
static algo_out_t step_algo_output;

/* Sensor input data buffer for algo processing */
static float AccBuff[NUM_DIM + 1][SAMP_BUFF_LEN];

/* Number of steps during Walk, Run, and Hop motions */
unsigned int num_steps_walk, num_steps_run, num_steps_hop;

QList<float> rlistX, rlistY, rlistZ, glistX, glistY, glistZ;

ReadTWThread::ReadTWThread(QObject* parent) : QThread{parent} {}
void ReadTWThread::run() {
  isReadTWEnd = false;
  MainWindow::readDataInThread(currentTabIndex);
  emit isDone();
}
void MainWindow::readTWDone() {
  for (int i = 0; i < tabData->tabBar()->count(); i++) {
    QTreeWidget* tw = (QTreeWidget*)tabData->widget(i);
    tw->setCurrentItem(tw->topLevelItem(tw->topLevelItemCount() - 1));
  }
  ui->btnPlus->setEnabled(true);
  ui->btnLess->setEnabled(true);
  ui->actionImport_Data->setEnabled(true);
  ui->actionExport_Data->setEnabled(true);
  ui->actionDel_Tab->setEnabled(true);
  ui->actionAdd_Tab->setEnabled(true);
  ui->actionView_App_Data->setEnabled(true);
  isReadTWEnd = true;

  ui->progBar->setMaximum(100);
}

ReadThread::ReadThread(QObject* parent) : QThread{parent} {}
void ReadThread::run() {
  if (isBreak) {
    emit isDone();
    return;
  }
  isReadEnd = false;
  MainWindow::ReadChartData();
  emit isDone();
}

void MainWindow::ReadChartData() {
  int index = tabData->currentIndex();
  QTreeWidget* tw = (QTreeWidget*)tabData->widget(index);

  strY = get_Year(readDate);
  strM = get_Month(readDate);

  if (tabChart->currentIndex() == 0) {
    drawMonthChart();
  }
  if (tabChart->currentIndex() == 1) {
    drawDayChart();
  }
  get_Today(tw);
  init_Stats(tw);
}

void MainWindow::readDone() {
  if (tabChart->currentIndex() == 0) {
    initChartMonth(strY, strM);
  }
  if (tabChart->currentIndex() == 1) {
    initChartDay();
  }

  ui->lblStats->setText(strStats);

  isReadEnd = true;
}

SearchThread::SearchThread(QObject* parent) : QThread{parent} {}
void SearchThread::run() {
  isSaveEnd = false;
  MainWindow::SaveFile(SaveType);
  emit isDone();
}
void MainWindow::dealDone() {
  if (isBreak) {
    isSaveEnd = true;
    return;
  }
  isSaveEnd = true;
  isImport = false;

  ui->progBar->setMaximum(100);

  if (SaveType == "tab" || SaveType == "alltab") startRead(strDate);
}
void MainWindow::SaveFile(QString SaveType) {
  if (SaveType == "tab") {
    QTreeWidget* tw = (QTreeWidget*)tabData->currentWidget();
    int index = tabData->currentIndex();
    saveData(tw, index);
    saveNotes(index);
    saveTab();
    dlgSetTime::saveCustomDesc();
  }

  if (SaveType == "alltab") {
    for (int i = 0; i < tabData->tabBar()->count(); i++) {
      if (isBreak) break;
      QString name = "tab" + QString::number(i + 1);
      QString ini_file = iniDir + name + ".ini";
      if (QFile(ini_file).exists()) QFile(ini_file).remove();

      QTreeWidget* tw = (QTreeWidget*)tabData->widget(i);
      tw->setObjectName(name);
      saveData(tw, i);
      saveNotes(i);
    }

    saveTab();
    dlgSetTime::saveCustomDesc();
    dlgTodo::saveTodo();
  }

  if (SaveType == "todo") {
    dlgTodo::saveTodo();
  }

  if (SaveType == "notes") {
    saveNotes(tabData->currentIndex());
  }

  if (SaveType == "ymd") {
    dlgReport::saveYMD();
  }
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  ui->frameQML->hide();
  ui->quickWidget->installEventFilter(this);

  QDesktopWidget* desktop = QApplication::desktop();
  QRect screen = desktop->screenGeometry();
  int screenWidth = screen.width();
  int screenHeight = screen.height();
  if (!isAndroid)
    this->setGeometry((screenWidth - this->width()) / 2, 0, this->width(),
                      screenHeight);

  qRegisterMetaType<QVector<int>>("QVector<int>");
  loading = true;
  init_UIWidget();
  init_ChartWidget();
  init_Menu();
  init_Options();
  init_Sensors();
  init_TabData();
  loading = false;

  QTreeWidget* tw = (QTreeWidget*)tabData->currentWidget();
  startRead(strDate);
  get_Today(tw);
  init_Stats(tw);
  isInit = true;

#ifdef Q_OS_ANDROID

  QAndroidJniObject jo = QAndroidJniObject::fromString("CheckSensorWin");
  isHardStepSensor =
      jo.callStaticMethod<int>("com.x/MyActivity", "getHardStepCounter", "()I");

  if (isHardStepSensor == 0) {
    if (mydlgSteps->ui->rbAlg1->isChecked()) mydlgSteps->on_rbAlg1_clicked();
    if (mydlgSteps->ui->rbAlg2->isChecked()) mydlgSteps->on_rbAlg2_clicked();
  }
  if (isHardStepSensor == 1) {
    mydlgSteps->ui->btnPause->click();
    mydlgSteps->ui->gboxAlg->hide();
    mydlgSteps->ui->lblSteps->hide();
    mydlgSteps->ui->btnPause->hide();
    mydlgSteps->ui->lblTotalRunTime->hide();
    mydlgPre->ui->chkDebug->setChecked(false);
    mydlgPre->on_chkDebug_clicked();
    mydlgPre->ui->chkDebug->hide();
    ui->btnPause->hide();
    initTodayInitSteps();
    resetSteps = tc;
    // timerStep->start(5000);

    // QAndroidJniObject jo = QAndroidJniObject::fromString("Sleep3Win");
    // jo.callStaticMethod<int>("com.x/MyService", "setSleep3", "()I");
  }
#endif
}

void MainWindow::initTodayInitSteps() {
  qlonglong a, b;
#ifdef Q_OS_ANDROID
  QAndroidJniObject jo = QAndroidJniObject::fromString("getSteps");
  a = jo.callStaticMethod<float>("com.x/MyActivity", "getSteps", "()F");
#endif
  tc = a;

  QSettings Reg(iniDir + "initsteps.ini", QSettings::IniFormat);
  Reg.setIniCodec("utf-8");
  QString str = QDate::currentDate().toString("ddd MM dd yyyy");

  if (!Reg.allKeys().contains(str)) {
    Reg.setValue(str, a);
    initTodaySteps = a;
  } else {
    b = Reg.value(str).toLongLong();
    if (a < b) {
      initTodaySteps = 0 - Reg.value("TodaySteps", 0).toLongLong();
      Reg.setValue(str, initTodaySteps);
    } else
      initTodaySteps = b;
  }

  // mydlgSteps->ui->lblStepLength->setText(QString::number(initTodaySteps));
}

void MainWindow::newDatas() {
  ax = ay = az = gx = gy = gz = 0;
  az = accel_pedometer->reading()->z();

  updateRunTime();
  writeLogs();

  countOne++;
  if (mydlgSteps->ui->rbAlg1->isChecked()) {
    if (countOne >= 2) {
      aoldZ = az;
      countOne = 0;
    }
  }
  if (mydlgSteps->ui->rbAlg2->isChecked()) {
    if (countOne >= 300) {
      aoldZ = az;
      countOne = 0;
    }
  }

  if (qAbs(qAbs(az) - qAbs(aoldZ)) < 0.5) {
    return;
  }

  if (mydlgSteps->ui->rbAlg1->isChecked()) {
    accel_pedometer->runStepCountAlgorithm();
    timeCount++;
    mydlgSteps->ui->lblSteps->setText(tr("Number of Operations") + " : " +
                                      QString::number(timeCount));
  }

  if (mydlgSteps->ui->rbAlg2->isChecked()) {
    ax = accel_pedometer->reading()->x();
    ay = accel_pedometer->reading()->y();
    gx = gyroscope->reading()->x();
    gy = gyroscope->reading()->y();
    gz = gyroscope->reading()->z();
    rlistX.append(ax);
    rlistY.append(ay);
    rlistZ.append(az);
    glistX.append(gx);
    glistY.append(gy);
    glistZ.append(gz);
    if (rlistX.count() == 120) getSteps2();
  }

  showSensorValues();
}

void MainWindow::showSensorValues() {
  if (mydlgPre->ui->chkDebug->isChecked()) {
    mydlgSteps->ui->lblX->setText("AX:" + QString::number(ax) + "\n" +
                                  "GX:" + QString::number(gx));
    mydlgSteps->ui->lblY->setText("AY:" + QString::number(ay) + "\n" +
                                  "GY:" + QString::number(gy));
    mydlgSteps->ui->lblZ->setText("AZ:" + QString::number(az) + "\n" +
                                  "GZ:" + QString::number(gz));

    if (mydlgSteps->ui->lblX->isHidden()) {
      mydlgSteps->ui->lblX->show();
      mydlgSteps->ui->lblY->show();
      mydlgSteps->ui->lblZ->show();
    }
  } else {
    if (!mydlgSteps->ui->lblX->isHidden()) {
      mydlgSteps->ui->lblX->hide();
      mydlgSteps->ui->lblY->hide();
      mydlgSteps->ui->lblZ->hide();
    }
  }
}

void MainWindow::updateRunTime() {
  smallCount++;
  if (mydlgSteps->ui->rbAlg1->isChecked()) {
    if (smallCount >= 5) {
      timeTest++;
      smallCount = 0;
      pausePedometer();
    }
  }
  if (mydlgSteps->ui->rbAlg2->isChecked()) {
    if (smallCount >= 100) {
      timeTest++;
      smallCount = 0;
      pausePedometer();
    }
  }
  mydlgSteps->ui->lblTotalRunTime->setText(tr("Total Working Hours") + " : " +
                                           secondsToTime(timeTest));
}

void MainWindow::pausePedometer() {
  if (QTime::currentTime().toString("hh-mm-ss") == "22-00-00") {
    if (mydlgSteps->ui->btnPause->text() == tr("Pause"))
      mydlgSteps->ui->btnPause->click();
  }
}

void MainWindow::writeLogs() {
  if (mydlgPre->ui->chkDebug->isChecked()) {
    testCount1++;
    if (testCount1 >= 8000) {
      testCount1 = 0;
      testCount++;
      QString s0, s1, s2;
      s1 = "ax:" + QString::number(ax) + "  " + "ay:" + QString::number(ay) +
           "  " + "az:" + QString::number(az);
      s2 = "gx:" + QString::number(gx) + "  " + "gy:" + QString::number(gy) +
           "  " + "gz:" + QString::number(gz);
      s0 = QString::number(testCount) + " . " + QTime::currentTime().toString();
      mydlgMainNotes->ui->textBrowser->append(s0 + " : " + s1 + "  " + s2 +
                                              "\n");

      if (testCount >= 720) {
        mydlgMainNotes->ui->textBrowser->clear();
        testCount = 0;
      }
    }
  }
}

void MainWindow::updateSteps() {
  // CurrentSteps = accel_pedometer->stepCount();
  if (mydlgSteps->ui->rbAlg1->isChecked()) {
    CurrentSteps++;
    CurTableCount = mydlgSteps->getCurrentSteps();
    CurTableCount++;
    mydlgSteps->toDayInitSteps++;
  }

  if (mydlgSteps->ui->rbAlg2->isChecked()) {
    CurrentSteps = num_steps_walk + num_steps_run + num_steps_hop;
    CurTableCount = mydlgSteps->toDayInitSteps + CurrentSteps;
  }

  mydlgSteps->ui->lcdNumber->display(QString::number(CurTableCount));
  mydlgSteps->ui->lblSingle->setText(QString::number(CurrentSteps));
  mydlgSteps->setTableSteps(CurTableCount);

  if (CurrentSteps == 0) return;
  sendMsg(CurTableCount);
}

void MainWindow::sendMsg(int CurTableCount) {
#ifdef Q_OS_ANDROID
  double sl = mydlgSteps->ui->editStepLength->text().toDouble();
  double d0 = sl / 100;
  double x = CurTableCount * d0;
  double gl = x / 1000;
  QString strNotify = tr("Today") + " : " + QString::number(CurTableCount) +
                      "  ( " + QString::number(gl) + " " + tr("km") + " )";
  QAndroidJniObject javaNotification = QAndroidJniObject::fromString(strNotify);
  QAndroidJniObject::callStaticMethod<void>(
      "com/x/MyService", "notify",
      "(Landroid/content/Context;Ljava/lang/String;)V",
      QtAndroid::androidContext().object(), javaNotification.object<jstring>());
#endif
}

void MainWindow::init_Options() {
  // Font Size
  QSettings Reg(iniDir + "options.ini", QSettings::IniFormat);
  Reg.setIniCodec("utf-8");
  QFont font(this->font());
  QFontInfo fInfo(font);
  fontSize = fInfo.pointSize();
  qDebug() << "fontSize:" << fontSize;

  QFont font1;
  font1.setPointSize(fontSize - 1);
  chartMonth->setTitleFont(font1);
  chartDay->setTitleFont(font1);
  axisX->setLabelsFont(font1);
  axisY->setLabelsFont(font1);
  axisX2->setLabelsFont(font1);
  axisY2->setLabelsFont(font1);

  mydlgPre->ui->rb0->setChecked(Reg.value("/Options/rb0", 1).toBool());
  mydlgPre->ui->rb1->setChecked(Reg.value("/Options/rb1", 0).toBool());
  mydlgPre->ui->rb2->setChecked(Reg.value("/Options/rb2", 0).toBool());
  if (mydlgPre->ui->rb1->isChecked()) fontSize = fontSize + 3;
  if (mydlgPre->ui->rb2->isChecked()) fontSize = fontSize + 5;
  QFont userFont;
  userFont.setPointSize(fontSize);
  mydlgReport->ui->tableReport->setFont(userFont);
  mydlgReport->ui->tableDetails->setFont(userFont);
  mydlgReport->ui->tableCategory->setFont(userFont);
  mydlgNotes->ui->textEdit->setFont(userFont);
  tabData->setFont(userFont);
  tabChart->setFont(userFont);
  mydlgMainNotes->ui->textBrowser->setFont(userFont);
  mydlgMainNotes->ui->textEdit->setFont(userFont);
  mydlgSteps->ui->tableWidget->setFont(userFont);
  mydlgTodo->ui->listRecycle->setFont(userFont);

  userFont.setBold(true);
  ui->lblStats->setFont(userFont);
  mydlgReport->ui->tableReport->horizontalHeader()->setFont(userFont);
  mydlgReport->ui->tableDetails->horizontalHeader()->setFont(userFont);
  mydlgSteps->ui->tableWidget->horizontalHeader()->setFont(userFont);
  ui->btnSteps->setFont(userFont);
  mydlgList->ui->listWidget->setFont(userFont);

  mydlgPre->ui->chkClose->setChecked(
      Reg.value("/Options/Close", false).toBool());
  mydlgPre->ui->chkAutoTime->setChecked(
      Reg.value("/Options/AutoTimeY", true).toBool());

  bool debugmode = Reg.value("/Options/Debug", false).toBool();
  mydlgPre->ui->chkDebug->setChecked(debugmode);
  mydlgPre->on_chkDebug_clicked();

  mydlgPre->ui->rbSM1->setChecked(Reg.value("/Options/SM1", false).toBool());
  mydlgPre->ui->rbSM2->setChecked(Reg.value("/Options/SM2", true).toBool());

  // MainUI Find YMD
  listMonth = QStringList() << "01"
                            << "02"
                            << "03"
                            << "04"
                            << "05"
                            << "06"
                            << "07"
                            << "08"
                            << "09"
                            << "10"
                            << "11"
                            << "12";

  QSettings Reg2(iniDir + "ymd.ini", QSettings::IniFormat);
  Reg2.setIniCodec("utf-8");
  btnYText = Reg2.value("/YMD/btnYText", 2022).toString();
  ui->btnYear->setText(btnYText);
  btnMText = Reg2.value("/YMD/btnMText", tr("Month")).toString();
  ui->btnMonth->setText(btnMText);
  btnDText = Reg2.value("/YMD/btnDText", 1).toString();
  ui->btnDay->setText(btnDText);
  btnYearText = Reg2.value("/YMD/btnYearText", "2022").toString();
  mydlgReport->ui->btnYear->setText(btnYearText);
  btnMonthText = Reg2.value("/YMD/btnMonthText", tr("Month")).toString();
  mydlgReport->ui->btnMonth->setText(btnMonthText);
}

void MainWindow::init_ChartWidget() {
  ui->tabCharts->setCornerWidget(ui->frame_cw);
  ui->glMonth->layout()->setContentsMargins(0, 0, 0, 0);
  ui->glMonth->layout()->setMargin(0);
  ui->glMonth->layout()->setSpacing(0);
  ui->glDay->layout()->setContentsMargins(0, 0, 0, 0);
  ui->glDay->layout()->setMargin(0);
  ui->glDay->layout()->setSpacing(0);

  chartMonth = new QChart();
  chartview = new QChartView(chartMonth);
  chartview->installEventFilter(this);
  ui->glMonth->addWidget(chartview);
  chartview->setRenderHint(QPainter::Antialiasing);
  chartMonth->legend()->hide();
  chartMonth->setMargins(QMargins(0, 0, 0, 0));
  chartMonth->setContentsMargins(0, 0, 0, 0);
  chartMonth->setAnimationOptions(QChart::SeriesAnimations);

  barSeries = new QBarSeries();
  series = new QSplineSeries();
  series->setPen(QPen(Qt::blue, 3, Qt::SolidLine));
  m_scatterSeries = new QScatterSeries();
  m_scatterSeries->setMarkerShape(QScatterSeries::MarkerShapeCircle);
  m_scatterSeries->setMarkerSize(10);
  chartMonth->addSeries(barSeries);
  chartMonth->addSeries(series);
  chartMonth->addSeries(m_scatterSeries);

  // Day
  chartDay = new QChart();
  chartview1 = new QChartView(chartDay);
  chartview1->installEventFilter(this);
  ui->glDay->addWidget(chartview1);
  chartview1->setRenderHint(QPainter::Antialiasing);
  chartDay->legend()->hide();
  chartDay->setMargins(QMargins(0, 0, 0, 0));
  chartDay->setContentsMargins(0, 0, 0, 0);
  chartDay->setAnimationOptions(QChart::SeriesAnimations);

  series2 = new QSplineSeries(chartDay);
  series2->setPen(QPen(Qt::blue, 3, Qt::SolidLine));
  m_scatterSeries2 = new QScatterSeries();
  m_scatterSeries2_1 = new QScatterSeries();

  //散点图(用于边框)
  m_scatterSeries2->setMarkerShape(
      QScatterSeries::MarkerShapeCircle);                    //圆形的点
  m_scatterSeries2->setBorderColor(QColor(21, 100, 255));    //边框颜色
  m_scatterSeries2->setBrush(QBrush(QColor(21, 100, 255)));  //背景颜色
  m_scatterSeries2->setMarkerSize(12);                       //点大小

  //散点图(用于中心)
  m_scatterSeries2_1->setMarkerShape(
      QScatterSeries::MarkerShapeCircle);           //圆形的点
  m_scatterSeries2_1->setBorderColor(Qt::white);    //边框颜色
  m_scatterSeries2_1->setBrush(QBrush(Qt::white));  //背景颜色
  m_scatterSeries2_1->setMarkerSize(6);             //点大小
  connect(m_scatterSeries2_1, &QScatterSeries::hovered, this,
          &MainWindow::slotPointHoverd);  //用于鼠标移动到点上显示数值
  m_valueLabel = new QLabel(this);
  m_valueLabel->adjustSize();
  m_valueLabel->setHidden(true);

  chartDay->addSeries(series2);
  chartDay->addSeries(m_scatterSeries2);
  chartDay->addSeries(m_scatterSeries2_1);

  // chartMonth->createDefaultAxes();
  axisX = new QBarCategoryAxis();
  chartMonth->addAxis(axisX, Qt::AlignBottom);
  barSeries->attachAxis(axisX);
  axisY = new QValueAxis();
  chartMonth->addAxis(axisY, Qt::AlignLeft);
  barSeries->attachAxis(axisY);

  // chartDay->createDefaultAxes();
  axisX2 = new QValueAxis();
  chartDay->addAxis(axisX2, Qt::AlignBottom);
  axisY2 = new QValueAxis();
  chartDay->addAxis(axisY2, Qt::AlignLeft);
  series2->attachAxis(axisX2);
  series2->attachAxis(axisY2);
  m_scatterSeries2->attachAxis(axisX2);
  m_scatterSeries2->attachAxis(axisY2);
  m_scatterSeries2_1->attachAxis(axisX2);
  m_scatterSeries2_1->attachAxis(axisY2);

  chartMonth->setTitle(tr("Freq"));
  chartDay->setTitle(tr("Freq"));

  // 获取背景色
  QPalette pal = this->palette();
  QBrush brush = pal.window();
  red = brush.color().red();
  if (red < 55) {
    mydlgTodo->ui->listWidget->setStyleSheet(mydlgTodo->styleDark);
    chartMonth->setTheme(QChart::ChartThemeDark);
    chartDay->setTheme(QChart::ChartThemeDark);

  } else {
    mydlgTodo->ui->listWidget->setStyleSheet(
        mydlgTodo->ui->listWidget->styleSheet());
    chartMonth->setTheme(QChart::ChartThemeLight);
    chartDay->setTheme(QChart::ChartThemeLight);
  }
}

void MainWindow::slotPointHoverd(const QPointF& point, bool state) {
  if (state) {
    m_valueLabel->setText(QString::asprintf("%1.0f", point.y()));

    QPoint curPos = mapFromGlobal(QCursor::pos());
    m_valueLabel->move(curPos.x() - m_valueLabel->width() / 2,
                       curPos.y() - m_valueLabel->height() * 1.5);  //移动数值

    m_valueLabel->show();
  } else
    m_valueLabel->hide();
}

void MainWindow::init_TabData() {
  ui->progBar->setHidden(false);
  ui->progBar->setMaximum(0);

  int count = ui->tabWidget->tabBar()->count();
  for (int i = 0; i < count; i++) {
    ui->tabWidget->removeTab(0);
  }
  QString ini_file, ini_file1;
  if (isImport)
    ini_file = iniFile;
  else
    ini_file = iniDir + "tab.ini";
  QSettings RegTab(ini_file, QSettings::IniFormat);
  RegTab.setIniCodec("utf-8");

  if (isImport)
    ini_file1 = iniFile;
  else
    ini_file1 = iniDir + "notes.ini";
  QSettings RegNotes(ini_file1, QSettings::IniFormat);
  RegNotes.setIniCodec("utf-8");
  int TabCount = RegTab.value("TabCount", 0).toInt();

  for (int i = 0; i < TabCount; i++) {
    QString name = "tab" + QString::number(i + 1);
    QTreeWidget* tw = init_TreeWidget(name);
    ui->tabWidget->addTab(tw, RegTab
                                  .value("TabName" + QString::number(i),
                                         tr("Tab") + QString::number(i + 1))
                                  .toString());
    QString strNotes = RegNotes.value("/" + name + "/Note").toString();
    ui->tabWidget->setTabToolTip(i, strNotes);
  }

  if (TabCount == 0) {
    ui->tabWidget->addTab(init_TreeWidget("tab1"),
                          tr("Tab") + " " + QString::number(1));
    ui->tabWidget->setTabToolTip(0, "");
  }

  mydlgMainNotes->init_MainNotes();
  mydlgTodo->init_Items();
  mydlgSetTime->init_Desc();
  mydlgSteps->init_Steps();
  mydlgReader->initReader();

  currentTabIndex = RegTab.value("CurrentIndex").toInt();
  ui->tabWidget->setCurrentIndex(currentTabIndex);
  QTreeWidget* twCur = (QTreeWidget*)tabData->currentWidget();
  readData(twCur);
  ui->btnPlus->setEnabled(false);
  ui->btnLess->setEnabled(false);
  ui->actionImport_Data->setEnabled(false);
  ui->actionExport_Data->setEnabled(false);
  ui->actionDel_Tab->setEnabled(false);
  ui->actionAdd_Tab->setEnabled(false);
  ui->actionView_App_Data->setEnabled(false);
  myReadTWThread->start();
}

void MainWindow::readDataInThread(int ExceptIndex) {
  int count = tabData->tabBar()->count();
  for (int i = 0; i < count; i++) {
    if (i != ExceptIndex) {
      QTreeWidget* tw = (QTreeWidget*)tabData->widget(i);
      readData(tw);
    }
  }
}

void MainWindow::timerUpdate() {
  mydlgSteps->ui->lblTotalRunTime->setText(tr("Total Working Hours") + " : " +
                                           secondsToTime(timeTest++));
  if (QTime::currentTime().toString("hh-mm-ss") == "00-30-00") {
    mydlgPre->isFontChange = true;
    this->close();
  }
}

MainWindow::~MainWindow() {
  delete ui;
  mySearchThread->quit();
  mySearchThread->wait();

  myReadThread->quit();
  myReadThread->wait();

  myReadTWThread->quit();
  myReadTWThread->wait();
}

void MainWindow::startSave(QString str_type) {
  if (!isSaveEnd) {
    isBreak = true;
    mySearchThread->quit();
    mySearchThread->wait();

    while (!isSaveEnd)
      QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
  }
  if (isSaveEnd) {
    isBreak = false;
    SaveType = str_type;

    ui->progBar->setHidden(false);
    ui->progBar->setMaximum(0);

    mySearchThread->start();
  }
}

void MainWindow::startRead(QString Date) {
  if (!isSaveEnd || loading) return;

  readDate = Date;
  if (!isReadEnd) {
    isBreak = true;
    myReadThread->quit();
    myReadThread->wait();

    while (!isReadEnd)
      QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
  }

  if (isReadEnd) {
    isBreak = false;
    myReadThread->start();
    if (ui->rbSteps->isChecked()) ui->rbFreq->click();
  }
}

void MainWindow::add_Data(QTreeWidget* tw, QString strTime, QString strAmount,
                          QString strDesc) {
  bool isYes = false;

  strDate = QDate::currentDate().toString("ddd MM dd yyyy");
  if (isTesting) {
    // strDate = strDate.replace("3", "3");
  }
  for (int i = 0; i < tw->topLevelItemCount(); i++) {
    QString str = tw->topLevelItem(i)->text(0);
    if (getYMD(str) == getYMD(strDate)) {
      isYes = true;

      QTreeWidgetItem* topItem = tw->topLevelItem(i);
      QTreeWidgetItem* item11 = new QTreeWidgetItem(topItem);
      item11->setText(0, strTime);
      if (strAmount == "")
        item11->setText(1, "");
      else
        item11->setText(1, QString("%1").arg(strAmount.toDouble(), 0, 'f', 2));

      item11->setText(2, strDesc);

      int childCount = topItem->childCount();

      topItem->setTextAlignment(1, Qt::AlignHCenter | Qt::AlignVCenter);

      // Amount
      double amount = 0;
      for (int m = 0; m < childCount; m++) {
        QString str = topItem->child(m)->text(1);
        amount = amount + str.toDouble();
      }
      QString strAmount = QString("%1").arg(amount, 0, 'f', 2);
      topItem->setText(1, QString::number(childCount));
      if (strAmount == "0.00")
        topItem->setText(2, "");
      else
        topItem->setText(2, strAmount);

      break;
    }
  }
  // if (isTesting) isYes = false;
  if (!isYes) {
    QTreeWidgetItem* topItem = new QTreeWidgetItem;
    topItem->setText(0, strDate);
    tw->addTopLevelItem(topItem);
    QTreeWidgetItem* item11 = new QTreeWidgetItem(topItem);
    item11->setText(0, strTime);
    if (strAmount == "")
      item11->setText(1, "");
    else
      item11->setText(1, QString("%1").arg(strAmount.toDouble(), 0, 'f', 2));
    item11->setText(2, strDesc);
    int child = topItem->childCount();
    topItem->setTextAlignment(1, Qt::AlignHCenter | Qt::AlignVCenter);
    //  Amount
    double amount = 0;
    for (int m = 0; m < child; m++) {
      QString str = topItem->child(m)->text(1);
      amount = amount + str.toDouble();
    }
    QString strAmount = QString("%1").arg(amount, 0, 'f', 2);
    topItem->setText(1, QString::number(child));
    if (strAmount == "0.00")
      topItem->setText(2, "");
    else
      topItem->setText(2, strAmount);
  }

  if (!isTesting) {
    int topCount = tw->topLevelItemCount();
    QTreeWidgetItem* topItem = tw->topLevelItem(topCount - 1);
    tw->setCurrentItem(topItem);
    sort_childItem(topItem->child(0));
    tw->setCurrentItem(topItem->child(topItem->childCount() - 1));

    startSave("tab");
  }
}

void MainWindow::del_Data(QTreeWidget* tw) {
  bool isNo = true;
  strDate = QDate::currentDate().toString("ddd MM dd yyyy");
  for (int i = 0; i < tw->topLevelItemCount(); i++) {
    QString str = tw->topLevelItem(i)->text(0);
    if (getYMD(str) == getYMD(strDate)) {
      isNo = false;
      QTreeWidgetItem* topItem = tw->topLevelItem(i);
      int childCount = topItem->childCount();
      if (childCount > 0) {
        QString str = ui->tabWidget->tabText(ui->tabWidget->currentIndex());
        QString str1 = topItem->child(childCount - 1)->text(0) + "  " +
                       topItem->child(childCount - 1)->text(1);
        QMessageBox msgBox;
        msgBox.setText(str);
        msgBox.setInformativeText(tr("Less") + "\n" + str1);
        QPushButton* btnCancel =
            msgBox.addButton(tr("Cancel"), QMessageBox::RejectRole);
        QPushButton* btnOk =
            msgBox.addButton(tr("Ok"), QMessageBox::AcceptRole);
        btnOk->setFocus();
        msgBox.exec();
        if (msgBox.clickedButton() == btnCancel) {
          return;
        }
        topItem->removeChild(topItem->child(childCount - 1));
        topItem->setTextAlignment(1, Qt::AlignHCenter | Qt::AlignVCenter);

        // Amount
        double amount = 0;
        for (int m = 0; m < childCount - 1; m++) {
          QString str = topItem->child(m)->text(1);
          amount = amount + str.toDouble();
        }
        QString strAmount = QString("%1").arg(amount, 0, 'f', 2);
        if (strAmount == "0.00") strAmount = "";
        topItem->setText(1, QString::number(childCount - 1));
        topItem->setText(2, strAmount);
        break;
      }
    }
  }

  if (isNo) {
    QMessageBox msgBox;
    QString str = ui->tabWidget->tabText(ui->tabWidget->currentIndex());
    msgBox.setText(str);
    msgBox.setInformativeText(
        tr("Only the reduction of the day's records is allowed."));

    QPushButton* btnOk = msgBox.addButton(tr("Ok"), QMessageBox::AcceptRole);
    btnOk->setFocus();
    msgBox.exec();
    return;
  }

  int topCount = tw->topLevelItemCount();
  if (topCount > 0) {
    QTreeWidgetItem* topItem = tw->topLevelItem(topCount - 1);
    tw->setCurrentItem(topItem);

  } else
    return;

  startSave("tab");
}

void MainWindow::on_btnPlus_clicked() {
  isAdd = true;
  mydlgSetTime->setFixedHeight(this->height());
  mydlgSetTime->setFixedWidth(this->width());
  mydlgSetTime->setModal(true);
  mydlgSetTime->ui->lblTitle->setText(
      tr("Add") + "  : " + tabData->tabText(tabData->currentIndex()));
  mydlgSetTime->ui->dialH->setValue(QTime::currentTime().hour());
  mydlgSetTime->ui->dialM->setValue(QTime::currentTime().minute());
  mydlgSetTime->getTime(mydlgSetTime->ui->dialH->value(),
                        mydlgSetTime->ui->dialM->value());
  mydlgSetTime->ui->editDesc->setText("");
  mydlgSetTime->ui->editAmount->setText("");
  mydlgSetTime->show();
}

void MainWindow::on_btnLess_clicked() {
  del_Data((QTreeWidget*)ui->tabWidget->currentWidget());
}

QObjectList MainWindow::getAllTreeWidget(QObjectList lstUIControls) {
  QObjectList lst;
  foreach (QObject* obj, lstUIControls) {
    if (obj->metaObject()->className() == QStringLiteral("QTreeWidget")) {
      lst.append(obj);
    }
  }
  return lst;
}

QObjectList MainWindow::getAllUIControls(QObject* parent) {
  QObjectList lstOfChildren, lstTemp;
  if (parent) {
    lstOfChildren = parent->children();
  }
  if (lstOfChildren.isEmpty()) {
    return lstOfChildren;
  }

  lstTemp = lstOfChildren;

  foreach (QObject* obj, lstTemp) {
    QObjectList lst = getAllUIControls(obj);
    if (!lst.isEmpty()) {
      lstOfChildren.append(lst);
    }
  }
  return lstOfChildren;
}

void MainWindow::saveTab() {
  // Tab
  QSettings Reg(iniDir + "tab.ini", QSettings::IniFormat);
  Reg.setIniCodec("utf-8");
  int TabCount = tabData->tabBar()->count();
  Reg.setValue("TabCount", TabCount);
  int CurrentIndex = tabData->currentIndex();
  Reg.setValue("CurrentIndex", CurrentIndex);
  for (int i = 0; i < TabCount; i++) {
    if (isBreak) break;
    Reg.setValue("TabName" + QString::number(i), tabData->tabText(i));
  }
}

QString MainWindow::getFileSize(const qint64& size, int precision) {
  double sizeAsDouble = size;
  static QStringList measures;
  if (measures.isEmpty())
    measures << QCoreApplication::translate("QInstaller", "bytes")
             << QCoreApplication::translate("QInstaller", "KiB")
             << QCoreApplication::translate("QInstaller", "MiB")
             << QCoreApplication::translate("QInstaller", "GiB")
             << QCoreApplication::translate("QInstaller", "TiB")
             << QCoreApplication::translate("QInstaller", "PiB")
             << QCoreApplication::translate("QInstaller", "EiB")
             << QCoreApplication::translate("QInstaller", "ZiB")
             << QCoreApplication::translate("QInstaller", "YiB");
  QStringListIterator it(measures);
  QString measure(it.next());
  while (sizeAsDouble >= 1024.0 && it.hasNext()) {
    measure = it.next();
    sizeAsDouble /= 1024.0;
  }
  return QString::fromLatin1("%1 %2")
      .arg(sizeAsDouble, 0, 'f', precision)
      .arg(measure);
}

void MainWindow::saveData(QTreeWidget* tw, int tabIndex) {
  QString name = "tab" + QString::number(tabIndex + 1);
  QString ini_file = iniDir + name + ".ini";
  QSettings Reg(ini_file, QSettings::IniFormat);
  Reg.setIniCodec("utf-8");
  int count = tw->topLevelItemCount();
  tw->setObjectName(name);
  Reg.setValue("/" + name + "/TopCount", count);
  for (int i = 0; i < count; i++) {
    if (isBreak) break;
    Reg.setValue("/" + name + "/" + QString::number(i + 1) + "-topDate",
                 tw->topLevelItem(i)->text(0));
    Reg.setValue("/" + name + "/" + QString::number(i + 1) + "-topFreq",
                 tw->topLevelItem(i)->text(1));
    Reg.setValue("/" + name + "/" + QString::number(i + 1) + "-topAmount",
                 tw->topLevelItem(i)->text(2));
    int childCount = tw->topLevelItem(i)->childCount();
    Reg.setValue("/" + name + "/" + QString::number(i + 1) + "-childCount",
                 childCount);
    for (int j = 0; j < childCount; j++) {
      if (isBreak) return;
      Reg.setValue("/" + name + "/" + QString::number(i + 1) + "-childTime" +
                       QString::number(j),
                   tw->topLevelItem(i)->child(j)->text(0));
      Reg.setValue("/" + name + "/" + QString::number(i + 1) + "-childAmount" +
                       QString::number(j),
                   tw->topLevelItem(i)->child(j)->text(1));
      Reg.setValue("/" + name + "/" + QString::number(i + 1) + "-childDesc" +
                       QString::number(j),
                   tw->topLevelItem(i)->child(j)->text(2));
    }
  }
}

void MainWindow::drawMonthChart() {
  listM.clear();
  listM = get_MonthList(strY, strM);
  CurrentYearMonth = strY + strM;
  // qDebug() << "Month List Count: " << listM.count();
}

void MainWindow::drawDayChart() {
  QTreeWidget* tw = (QTreeWidget*)tabData->currentWidget();
  if (loading) return;
  PointList.clear();

  int topCount = tw->topLevelItemCount();
  if (topCount == 0) {
    return;
  }

  if (topCount > 0) {
    if (!tw->currentIndex().isValid()) {
      QTreeWidgetItem* topItem = tw->topLevelItem(topCount - 1);
      tw->setCurrentItem(topItem);
    }
  }

  QTreeWidgetItem* item = tw->currentItem();
  bool child;
  int childCount;
  if (item->parent() == NULL)
    child = false;
  else
    child = true;

  QString month;
  int day;
  if (child) {
    childCount = item->parent()->childCount();
    parentItem = item->parent();
    day = get_Day(item->parent()->text(0));
    month = get_Month(item->parent()->text(0));
  } else {
    childCount = item->childCount();
    parentItem = item;
    day = get_Day(item->text(0));
    month = get_Month(item->text(0));
  }

  QString strMonthDay = month + QString("%1").arg(day, 2, 10, QLatin1Char('0'));
  tabChart->setTabText(1, strMonthDay);

  QList<double> dList;
  double x, y;
  QString str, str1;
  int step = 1;
  if (childCount > 500) step = 100;

  for (int i = 0; i < childCount; i = i + step) {
    if (isBreak) break;

    if (child) {
      str = item->parent()->child(i)->text(0);
      str1 = item->parent()->child(i)->text(1);
    } else {
      str = item->child(i)->text(0);
      str1 = item->child(i)->text(1);
    }
    QStringList l0 = str.split(".");
    if (l0.count() == 2) str = l0.at(1);
    QStringList list = str.split(":");
    int t = 0;
    if (list.count() == 3) {
      QString a, b, c;
      a = list.at(0);
      b = list.at(1);
      c = list.at(2);
      int a1, b1;
      a1 = a.toInt();
      b1 = b.toInt();
      t = a1 * 3600 + b1 * 60 + c.toInt();
    }
    x = (double)t / 3600;
    if (isrbFreq)
      y = i + 1;
    else {
      y = str1.toDouble();
      dList.append(y);
    }

    PointList.append(QPointF(x, y));
  }

  if (isrbFreq) {
    int a = 20;
    if (childCount > a)
      yMaxDay = childCount;
    else
      yMaxDay = a;
  } else {
    yMaxDay = *std::max_element(dList.begin(), dList.end());
  }
}

void MainWindow::readData(QTreeWidget* tw) {
  tw->clear();
  QString name = tw->objectName();
  QString ini_file;
  if (isImport)
    ini_file = iniFile;
  else
    ini_file = iniDir + name + ".ini";
  QSettings Reg(ini_file, QSettings::IniFormat);
  Reg.setIniCodec("utf-8");
  int rowCount = Reg.value("/" + name + "/TopCount").toInt();
  for (int i = 0; i < rowCount; i++) {
    int childCount =
        Reg.value("/" + name + "/" + QString::number(i + 1) + "-childCount")
            .toInt();
    // 不显示子项为0的数据
    if (childCount > 0) {
      QTreeWidgetItem* topItem = new QTreeWidgetItem;
      QString strD0 =
          Reg.value("/" + name + "/" + QString::number(i + 1) + "-topDate")
              .toString();
      int m = strD0.split(" ").at(1).toInt();
      if (m == 0) {
        QString strD1 = QDate::fromString(strD0, "ddd MMM d yyyy")
                            .toString("ddd MM dd yyyy");
        topItem->setText(0, strD1);
      } else
        topItem->setText(0, strD0);
      tw->addTopLevelItem(topItem);

      topItem->setTextAlignment(1, Qt::AlignHCenter | Qt::AlignVCenter);
      topItem->setText(
          1, Reg.value("/" + name + "/" + QString::number(i + 1) + "-topFreq")
                 .toString());
      topItem->setText(
          2, Reg.value("/" + name + "/" + QString::number(i + 1) + "-topAmount")
                 .toString());

      for (int j = 0; j < childCount; j++) {
        QTreeWidgetItem* item11 = new QTreeWidgetItem(topItem);
        item11->setText(0, Reg.value("/" + name + "/" + QString::number(i + 1) +
                                     "-childTime" + QString::number(j))
                               .toString());
        item11->setText(1, Reg.value("/" + name + "/" + QString::number(i + 1) +
                                     "-childAmount" + QString::number(j))
                               .toString());
        item11->setText(2, Reg.value("/" + name + "/" + QString::number(i + 1) +
                                     "-childDesc" + QString::number(j))
                               .toString());
      }
    }
  }
}

void MainWindow::get_Today(QTreeWidget* tw) {
  int count = tw->topLevelItemCount();
  if (count <= 0) {
    today = 0;
    return;
  }
  QString str0 = tw->topLevelItem(count - 1)->text(0);
  QString str1 = tw->topLevelItem(count - 1)->text(1);
  if (strDate == str0) {
    today = str1.toInt();

  } else
    today = 0;
}

QString MainWindow::loadText(QString textFile) {
  if (QFile(textFile).exists()) {
    QFile file(textFile);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
      qDebug() << tr("Cannot read file %1:\n%2.")
                      .arg(QDir::toNativeSeparators(textFile),
                           file.errorString());

    } else {
      QTextStream in(&file);
      in.setCodec("UTF-8");
      QString text = in.readAll();
      return text;
    }
    file.close();
  }

  return "";
}

void MainWindow::TextEditToFile(QTextEdit* txtEdit, QString fileName) {
  QFile* file;
  QString txtFile;
  file = new QFile;
  file->setFileName(fileName);
  file->setPermissions(txtFile, QFile::WriteOwner | QFile::ReadOwner);
  bool ok = file->open(QIODevice::WriteOnly | QIODevice::Text);
  if (ok) {
    QTextStream out(file);
    out << txtEdit->toPlainText();
    file->close();
    delete file;
  } else
    qDebug() << "Write failure!" << fileName;
}

void MainWindow::closeEvent(QCloseEvent* event) {
  mydlgSteps->saveSteps();
  if (mydlgPre->ui->chkClose->isChecked()) {
    stopJavaTimer();
    event->accept();
  } else {
    if (mydlgPre->isFontChange) {
      stopJavaTimer();
      event->accept();
      return;
    }

#ifdef Q_OS_ANDROID
    QAndroidJniObject jo = QAndroidJniObject::fromString("MiniWin");
    jo.callStaticMethod<int>("com.x/MyActivity", "mini", "()I");
#endif

    event->ignore();
  }
}

void MainWindow::init_Stats(QTreeWidget* tw) {
  int count = tw->topLevelItemCount();
  int tatol = 0;
  double amount = 0;
  for (int i = 0; i < count; i++) {
    if (isBreak) break;
    QString str1 = tw->topLevelItem(i)->text(1);
    QString str2 = tw->topLevelItem(i)->text(2);
    tatol = tatol + str1.toInt();
    amount = amount + str2.toDouble();
  }

  QString strAmount = QString("%1").arg(amount, 0, 'f', 2);
  strStats = tr("Total") + " : " + QString::number(tatol) + "    $" + strAmount;
}

void MainWindow::initChartMonth(QString strY, QString strM) {
  if (loading) return;
  tabChart->setTabText(0, strM);
  strY = "";

  int count = PointList.count();
  if (count == 0) {
    return;
  }

  barSeries->clear();
  series->clear();
  m_scatterSeries->clear();
  bool isOne = true;

  QBarSet* setY = new QBarSet("Y");
  QStringList categories;

  for (int i = 0; i < count; i++) {
    // series->append(PointList.at(i));
    // m_scatterSeries->append(PointList.at(i));
    if (PointList.at(i).y() != 1) isOne = false;
  }

  if (isOne && mydlgPre->ui->chkAutoTime->isChecked()) {
    series->clear();
    m_scatterSeries->clear();
    QList<QPointF> tempPointList;
    for (int i = 0; i < count; i++) {
      double y0 = 0.0;
      QString str = listM.at(i);
      QStringList list1 = str.split(".");
      if (list1.count() == 2) {
        QStringList list = list1.at(1).split(":");
        int t = 0;
        if (list.count() == 3) {
          QString a, b, c;
          a = list.at(0);
          b = list.at(1);
          c = list.at(2);
          int a1, b1;
          a1 = a.toInt();
          b1 = b.toInt();
          t = a1 * 3600 + b1 * 60 + c.toInt();
        }
        y0 = (double)t / 3600;
      }
      // series->append(PointList.at(i).x(), y0);
      // m_scatterSeries->append(PointList.at(i).x(), y0);

      tempPointList.append(QPointF(PointList.at(i).x(), y0));
    }
    PointList.clear();
    PointList = tempPointList;
  }

  double maxValue = *std::max_element(doubleList.begin(), doubleList.end());
  double max;
  if (isrbFreq) {
    max = 20;
    if (maxValue >= max) {
      max = maxValue;
    }

  } else {
    max = 50.00;
    if (maxValue >= max) max = maxValue;
  }

  yMaxMonth = max;
  // chartMonth->axes(Qt::Horizontal).first()->setRange(0, 31);
  // chartMonth->axes(Qt::Vertical).first()->setRange(0, yMaxMonth);

  QList<double> dList, tempDList;
  for (int i = 0; i < PointList.count(); i++) {
    tempDList.append(PointList.at(i).y());
    categories.append(QString::number(PointList.at(i).x()));
  }
  for (int i = 0; i < 31; i++) {
    dList.append(0);
  }
  for (int i = 0; i < categories.count(); i++) {
    for (int n = 0; n < 31; n++) {
      if (categories.at(i) == QString::number(n + 1)) {
        dList.removeAt(n);
        dList.insert(n, PointList.at(i).y());
      }
    }
  }

  for (int i = 0; i < 31; i++) setY->append(dList.at(i));
  categories.clear();
  for (int i = 0; i < 31; i++) categories.append(QString::number(i + 1));
  barSeries->append(setY);
  axisX->append(categories);
  axisY->setRange(0, yMaxMonth);

  if (isOne && mydlgPre->ui->chkAutoTime->isChecked()) {
    // chartMonth->axes(Qt::Horizontal).first()->setRange(0, 31);
    // chartMonth->axes(Qt::Vertical).first()->setRange(0, 24);

    axisY->setRange(0, 24);
  }
}

void MainWindow::initChartDay() {
  if (loading) return;
  series2->clear();
  m_scatterSeries2->clear();
  m_scatterSeries2_1->clear();

  int count = PointList.count();
  if (count == 0) return;
  for (int i = 0; i < count; i++) {
    series2->append(PointList.at(i));
    m_scatterSeries2->append(PointList.at(i));
    m_scatterSeries2_1->append(PointList.at(i));
  }

  // chartDay->axes(Qt::Horizontal).first()->setRange(0, 24);
  // chartDay->axes(Qt::Vertical).first()->setRange(0, yMaxDay);

  axisX2->setRange(0, 24);
  axisY2->setRange(0, yMaxDay);
}

void MainWindow::on_actionRename_triggered() {
  int index = ui->tabWidget->currentIndex();
  /*bool ok;
  QString text = QInputDialog::getText(this, tr("Rename tab name"),
                                       tr("Tab name:"), QLineEdit::Normal,
                                       ui->tabWidget->tabText(index), &ok);
  if (ok && !text.isEmpty()) {
    ui->tabWidget->setTabText(index, text);
    saveTab();
  }*/
  mydlgRename->setFixedHeight(this->height());
  mydlgRename->setFixedWidth(this->width());
  mydlgRename->setModal(true);
  mydlgRename->ui->editName->setText(ui->tabWidget->tabText(index));
  mydlgRename->ui->editName->setFocus();
  mydlgRename->ui->editName->selectAll();
  mydlgRename->show();
}

void MainWindow::on_actionAdd_Tab_triggered() {
  int count = ui->tabWidget->tabBar()->count();
  QString ini_file = iniDir + "tab" + QString::number(count + 1) + ".ini";
  if (QFile(ini_file).exists()) QFile(ini_file).remove();

  ui->tabWidget->addTab(init_TreeWidget("tab" + QString::number(count + 1)),
                        tr("Tab") + " " + QString::number(count + 1));
  ui->tabWidget->setCurrentIndex(count);

  on_actionRename_triggered();
}

void MainWindow::on_actionDel_Tab_triggered() {
  int index = ui->tabWidget->currentIndex();
  if (index < 0) return;

  QString str1 = ui->tabWidget->tabText(index);
  QMessageBox msgBox;
  msgBox.setText("Knot");
  msgBox.setInformativeText(tr("Whether to remove") + "  " + str1 + " ? ");
  QPushButton* btnCancel =
      msgBox.addButton(tr("Cancel"), QMessageBox::RejectRole);
  QPushButton* btnOk = msgBox.addButton(tr("Ok"), QMessageBox::AcceptRole);
  btnOk->setFocus();
  msgBox.exec();
  if (msgBox.clickedButton() == btnCancel) {
    return;
  }

  ui->tabWidget->removeTab(index);

  int TabCount = ui->tabWidget->tabBar()->count();
  if (TabCount == 0) {
    ui->tabWidget->addTab(init_TreeWidget("tab1"),
                          tr("Tab") + " " + QString::number(1));
    ui->tabWidget->setTabToolTip(0, "");
  }

  // Save all
  startSave("alltab");
}

QTreeWidget* MainWindow::init_TreeWidget(QString name) {
  QTreeWidget* tw = new QTreeWidget(this);
  tw->setObjectName(name);

  // QString style = "QTreeWidget::item:selected{background:  #567dbc);}";
  // tw->setStyleSheet(style);
  QFont font;
  font.setPointSize(fontSize);
  tw->setFont(font);
  font.setBold(true);
  tw->header()->setFont(font);
  tw->setColumnCount(3);
  tw->headerItem()->setText(0, "  " + tr("Date") + "  ");
  tw->headerItem()->setText(1, "  " + tr("Freq") + "  ");
  tw->headerItem()->setText(2, tr("Amount"));
  tw->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
  tw->header()->setDefaultAlignment(Qt::AlignCenter);
  tw->setAlternatingRowColors(true);
  tw->setFrameShape(QTreeWidget::NoFrame);
  tw->installEventFilter(this);
  tw->setUniformRowHeights(true);  //加快展开速度
  connect(tw, &QTreeWidget::itemClicked, this, &MainWindow::on_twItemClicked);
  connect(tw, &QTreeWidget::itemDoubleClicked, this,
          &MainWindow::on_twItemDoubleClicked);
  connect(tw, &QTreeWidget::itemPressed, [=]() {});
  // tw->setUniformRowHeights(false); //对速度可能有影响，数据量大时
  QScrollBar* SB = tw->verticalScrollBar();
  SB->setStyleSheet(vsbarStyleSmall);
  tw->setStyleSheet(treeStyle);
  tw->setVerticalScrollMode(QTreeWidget::ScrollPerPixel);
  QScroller::grabGesture(tw, QScroller::LeftMouseButtonGesture);
  setSCrollPro(tw);
  return tw;
}

void MainWindow::on_twItemClicked() {
  QTreeWidget* tw = (QTreeWidget*)ui->tabWidget->currentWidget();
  QTreeWidgetItem* item = tw->currentItem();
  if (item->parent() == NULL && item->childCount() == 0) return;
  QTreeWidgetItem* pItem;

  if (item->childCount() > 0) {
    pItem = item;
    if (tabChart->currentIndex() == 0) {
      QString str = item->text(0);
      QString strYearMonth = get_Year(str) + get_Month(str);
      if (strYearMonth == CurrentYearMonth) return;
      startRead(str);
    }
  }
  if (item->childCount() == 0 && item->parent()->childCount() > 0)
    pItem = item->parent();
  if (item->parent() != NULL) {
    QString str = item->text(2);
    if (str.length() > 0)
      ui->lblStats->setText(str);
    else {
      init_Stats(tw);
      ui->lblStats->setText(strStats);
    }

    if (tabChart->currentIndex() == 0) {
      QString str = item->parent()->text(0);
      QString strYearMonth = get_Year(str) + get_Month(str);
      if (strYearMonth == CurrentYearMonth) return;
      startRead(str);
    }
  } else
    init_Stats(tw);
  if (parentItem == pItem) {
    return;
  }

  if (tabChart->currentIndex() == 1) startRead(strDate);
}

void MainWindow::set_Time() {
  QTreeWidget* tw = (QTreeWidget*)ui->tabWidget->currentWidget();
  QTreeWidgetItem* item = tw->currentItem();
  if (item->childCount() == 0 && item->parent()->childCount() > 0) {
    item->setText(0, mydlgSetTime->ui->lblTime->text().trimmed());
    QString sa = mydlgSetTime->ui->editAmount->text().trimmed();
    if (sa == "")
      item->setText(1, "");
    else
      item->setText(1, QString("%1").arg(sa.toFloat(), 0, 'f', 2));
    item->setText(2, mydlgSetTime->ui->editDesc->text().trimmed());
    // Amount
    int child = item->parent()->childCount();
    double amount = 0;
    for (int m = 0; m < child; m++) {
      QString str = item->parent()->child(m)->text(1);
      amount = amount + str.toDouble();
    }
    QString strAmount = QString("%1").arg(amount, 0, 'f', 2);
    item->parent()->setTextAlignment(1, Qt::AlignHCenter | Qt::AlignVCenter);
    item->parent()->setText(1, QString::number(child));
    if (strAmount == "0.00")
      item->parent()->setText(2, "");
    else
      item->parent()->setText(2, strAmount);

    sort_childItem(item);

    startSave("tab");
  }
}

void MainWindow::sort_childItem(QTreeWidgetItem* item) {
  QStringList keys, list, keyTime, keysNew;
  int childCount = item->parent()->childCount();

  for (int i = 0; i < childCount; i++) {
    QString txt = item->parent()->child(i)->text(0);
    QStringList list0 = txt.split(".");
    if (list0.count() == 2) {
      txt = list0.at(1);
      txt = txt.trimmed();
    }
    QString txt1 = item->parent()->child(i)->text(1);
    QString txt2 = item->parent()->child(i)->text(2);
    keys.append(txt + "|" + txt1 + "|" + txt2);
    keyTime.append(txt);
  }

  std::sort(keyTime.begin(), keyTime.end(),
            [](const QString& s1, const QString& s2) { return s1 < s2; });

  for (int i = 0; i < keyTime.count(); i++) {
    QString time = keyTime.at(i);
    for (int n = 0; n < keys.count(); n++) {
      QString str1 = keys.at(n);
      QStringList l0 = str1.split("|");
      if (time == l0.at(0)) {
        keysNew.append(str1);
        break;
      }
    }
  }

  for (int i = 0; i < childCount; i++) {
    QTreeWidgetItem* childItem = item->parent()->child(i);
    QString str = keysNew.at(i);
    list.clear();
    list = str.split("|");
    if (list.count() == 3) {
      int number = i + 1;
      QString strChildCount = QString::number(childCount);
      QString strNum;
      strNum = QString("%1").arg(number, strChildCount.length(), 10,
                                 QLatin1Char('0'));
      childItem->setText(0, strNum + ". " + list.at(0).trimmed());
      childItem->setText(1, list.at(1).trimmed());
      childItem->setText(2, list.at(2).trimmed());
    }
  }
}

void MainWindow::on_twItemDoubleClicked() {
  QTreeWidget* tw = (QTreeWidget*)ui->tabWidget->currentWidget();
  QTreeWidgetItem* item = tw->currentItem();
  if (item->childCount() == 0 && item->parent()->childCount() > 0) {
    QString t = item->text(0);
    QStringList l0 = t.split(".");
    if (l0.count() == 2) t = l0.at(1);
    QStringList list = t.split(":");
    QString sh, sm, ss;
    if (list.count() == 3) {
      sh = list.at(0);
      sm = list.at(1);
      ss = list.at(2);
    }
    mydlgSetTime->ui->lblTitle->setText(
        tr("Modify") + "  : " + tabData->tabText(tabData->currentIndex()));
    mydlgSetTime->ui->dialH->setValue(sh.toInt());
    mydlgSetTime->ui->dialM->setValue(sm.toInt());
    mydlgSetTime->ui->lblTime->setText(t.trimmed());

    QString str = item->text(1);
    if (str == "0.00")
      mydlgSetTime->ui->editAmount->setText("");
    else
      mydlgSetTime->ui->editAmount->setText(str);
    mydlgSetTime->ui->editDesc->setText(item->text(2));

    mydlgSetTime->setFixedHeight(this->height());
    mydlgSetTime->setFixedWidth(this->width());
    mydlgSetTime->setModal(true);
    isAdd = false;
    mydlgSetTime->show();
  }

  if (item == tw->topLevelItem(tw->topLevelItemCount() - 1)) {
    if (item->childCount() > 0) {
    }
  }
}

void MainWindow::on_tabWidget_currentChanged(int index) {
  int count = ui->tabWidget->tabBar()->count();
  if (index >= 0) {
    for (int i = 0; i < count; i++) {
      if (i == index)
        tabData->tabBar()->setTabTextColor(i, Qt::blue);
      else
        tabData->tabBar()->setTabTextColor(i, Qt::black);
    }
  }

  if (isSlide || loading || count <= 0) {
    return;
  }

  QTreeWidget* tw = (QTreeWidget*)tabData->widget(index);
  tw->setFocus();
  if (!loading) {
    QSettings Reg(iniDir + "tab.ini", QSettings::IniFormat);
    Reg.setIniCodec("utf-8");
    Reg.setValue("CurrentIndex", index);
  }

  series->clear();
  m_scatterSeries->clear();
  barSeries->clear();
  startRead(strDate);
}

void MainWindow::saveNotes(int tabIndex) {
  if (loading) return;
  QSettings Reg(iniDir + "notes.ini", QSettings::IniFormat);
  Reg.setIniCodec("utf-8");
  QTreeWidget* tw = (QTreeWidget*)tabData->widget(tabIndex);
  QString name = tw->objectName();
  Reg.setValue("/" + name + "/Note", tabData->tabToolTip(tabIndex));
}

void MainWindow::on_actionNotes_triggered() {
  mydlgNotes->setGeometry(0, 0, this->width(), this->height() / 2);

  mydlgNotes->ui->textEdit->clear();

  mydlgNotes->ui->frameAbout->hide();
  mydlgNotes->ui->textBrowser->setHidden(true);
  mydlgNotes->ui->textEdit->setHidden(false);
  mydlgNotes->setModal(true);
  mydlgNotes->ui->lblTitle->setText(tr("Notes") + " : " +
                                    tabData->tabText(tabData->currentIndex()));
  mydlgNotes->show();
  mydlgNotes->init_Notes();
}

void MainWindow::on_btnNotes_clicked() { on_actionNotes_triggered(); }

bool MainWindow::eventFilter(QObject* watch, QEvent* evn) {
  if (loading) return QWidget::eventFilter(watch, evn);
  QMouseEvent* event = static_cast<QMouseEvent*>(evn);  //将之转换为鼠标事件
  QTreeWidget* tw = (QTreeWidget*)ui->tabWidget->currentWidget();

  if (evn->type() == QEvent::ToolTip) {
    QToolTip::hideText();
    evn->ignore();
    return true;
  }

  if (watch == tw) {
  }

  if (watch == chartview || watch == chartview1) {
    if (event->type() == QEvent::MouseButtonDblClick) {
      on_btnZoom_clicked();
    }
  }

  if (watch == ui->tabWidget->tabBar()) {
    if (!isReadTWEnd) return QWidget::eventFilter(watch, evn);
    if (event->type() == QEvent::MouseButtonPress) {
    }
    if (event->type() == QEvent::MouseButtonRelease) {
      bool move = false;
      for (int i = 0; i < ui->tabWidget->tabBar()->count(); i++) {
        QTreeWidget* tw = (QTreeWidget*)ui->tabWidget->widget(i);
        QString name = tw->objectName();
        if (name != "tab" + QString::number(i + 1)) {
          move = true;
        }
      }
      if (move) {
        qDebug() << "ok save";
        startSave("alltab");
      }
    }
  }

  if (watch != ui->tabWidget->tabBar() && watch != tw) {
    static int press_x;
    static int press_y;
    static int relea_x;
    static int relea_y;
    int index = ui->tabWidget->currentIndex();
    int count = ui->tabWidget->tabBar()->count();

    if (event->type() == QEvent::MouseButtonPress) {
      press_x = event->globalX();
      press_y = event->globalY();
      x = 0;
      y = 0;
      w = tw->width();
      h = tw->height();
      // qDebug() << "Press:" << press_x << press_y;
    }

    if (event->type() == QEvent::MouseButtonRelease) {
      relea_x = event->globalX();
      relea_y = event->globalY();
      // qDebug() << "Release:" << relea_x << relea_y;
    }

    //判断滑动方向（右滑）
    if ((relea_x - press_x) > 20 &&
        event->type() == QEvent::MouseButtonRelease &&
        qAbs(relea_y - press_y) < 50) {
      int current_page = ui->tabWidget->currentIndex();
      if (current_page < count - 1) {
        isSlide = true;
        // ui->lblStats->setPixmap(ui->tabWidget->currentWidget()
        //                             ->grab());
        //                             //捕获当前界面并绘制到label上
        QPropertyAnimation* animation1 =
            // new QPropertyAnimation(ui->lblStats, "geometry");
            new QPropertyAnimation(ui->tabWidget->currentWidget(), "geometry");
        animation1->setDuration(500);  //设置动画时间为0.5秒
        animation1->setStartValue(QRect(x, y, w, h));
        animation1->setEndValue(QRect(w * 2, y, w, h));

        ui->tabWidget->setCurrentIndex(current_page + 1);  //切换界面

        QPropertyAnimation* animation2 =
            new QPropertyAnimation(ui->tabWidget->currentWidget(), "geometry");
        animation2->setDuration(500);
        animation2->setStartValue(QRect(-w * 2, y, w, h));
        animation2->setEndValue(QRect(x, y, w, h));

        QParallelAnimationGroup* group =
            new QParallelAnimationGroup;  //动画容器
        group->addAnimation(animation1);
        group->addAnimation(animation2);
        group->start();
        QElapsedTimer t;
        t.start();
        while (t.elapsed() < 600) {
          QCoreApplication::processEvents();
        }
        series->clear();
        m_scatterSeries->clear();
        startRead(strDate);

        isSlide = false;
      }
    }

    //判断滑动方向（左滑）
    if ((press_x - relea_x) > 20 &&
        event->type() == QEvent::MouseButtonRelease &&
        qAbs(relea_y - press_y) < 50 && index > 0) {
      int current_page = ui->tabWidget->currentIndex();
      if (current_page >= 0) {
        isSlide = true;
        // ui->lblStats->setPixmap(ui->tabWidget->currentWidget()->grab());

        QPropertyAnimation* animation1 =
            // new QPropertyAnimation(ui->lblStats, "geometry");
            new QPropertyAnimation(ui->tabWidget->currentWidget(), "geometry");
        animation1->setDuration(500);
        animation1->setStartValue(QRect(x, y, w, h));
        animation1->setEndValue(QRect(-w, y, w, h));

        ui->tabWidget->setCurrentIndex(current_page - 1);

        QPropertyAnimation* animation2 =
            new QPropertyAnimation(ui->tabWidget->currentWidget(), "geometry");
        animation2->setDuration(500);
        animation2->setStartValue(QRect(w * 2, y, w, h));
        animation2->setEndValue(QRect(x, y, w, h));

        QParallelAnimationGroup* group = new QParallelAnimationGroup;
        group->addAnimation(animation1);
        group->addAnimation(animation2);
        group->start();
        QElapsedTimer t;
        t.start();
        while (t.elapsed() < 600) {
          QCoreApplication::processEvents();
        }
        series->clear();
        m_scatterSeries->clear();
        startRead(strDate);

        isSlide = false;
      }
    }
  }

  if (evn->type() == QEvent::KeyPress) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      if (!ui->frameQML->isHidden()) {
        on_btnBack_clicked();
        return true;
      }
    }
  }

  if (watch == ui->quickWidget) {
    static int press_x;
    static int press_y;
    static int relea_x;
    static int relea_y;

    if (event->type() == QEvent::MouseButtonPress) {
      press_x = event->globalX();
      press_y = event->globalY();
      x = 0;
      y = 0;
      w = ui->quickWidget->width();
      h = ui->quickWidget->height();
    }

    if (event->type() == QEvent::MouseButtonRelease) {
      relea_x = event->globalX();
      relea_y = event->globalY();
    }

    int abc = 500;
    //判断滑动方向（右滑）
    if ((relea_x - press_x) > 50 &&
        event->type() == QEvent::MouseButtonRelease &&
        qAbs(relea_y - press_y) < 50) {
      if (mydlgReader->iPage - mydlgReader->baseLines <= 0)
        return QWidget::eventFilter(watch, evn);

      ui->lblTitle->setPixmap(ui->quickWidget->grab());
      QPropertyAnimation* animation1 =
          new QPropertyAnimation(ui->lblTitle, "geometry");

      animation1->setDuration(abc);
      animation1->setStartValue(QRect(x, y, w, h));
      animation1->setEndValue(QRect(w * 1, y, w, h));

      on_btnPageUp_clicked();

      QPropertyAnimation* animation2 =
          new QPropertyAnimation(ui->quickWidget, "geometry");
      animation2->setDuration(abc);
      animation2->setStartValue(QRect(-w * 1, y, w, h));
      animation2->setEndValue(QRect(x, y, w, h));

      QParallelAnimationGroup* group = new QParallelAnimationGroup;
      group->addAnimation(animation1);
      group->addAnimation(animation2);
      group->start();
      ui->lblTitle->show();
    }

    //判断滑动方向（左滑）
    if ((press_x - relea_x) > 50 &&
        event->type() == QEvent::MouseButtonRelease &&
        qAbs(relea_y - press_y) < 50) {
      if (mydlgReader->iPage + mydlgReader->baseLines > mydlgReader->totallines)
        return QWidget::eventFilter(watch, evn);

      ui->lblTitle->setPixmap(ui->quickWidget->grab());
      QPropertyAnimation* animation1 =
          new QPropertyAnimation(ui->lblTitle, "geometry");

      animation1->setDuration(abc);
      animation1->setStartValue(QRect(x, y, w, h));
      animation1->setEndValue(QRect(-w, y, w, h));

      on_btnPageNext_clicked();

      QPropertyAnimation* animation2 =
          new QPropertyAnimation(ui->quickWidget, "geometry");
      animation2->setDuration(abc);
      animation2->setStartValue(QRect(w * 1, y, w, h));
      animation2->setEndValue(QRect(x, y, w, h));

      QParallelAnimationGroup* group = new QParallelAnimationGroup;
      group->addAnimation(animation1);
      group->addAnimation(animation2);
      group->start();
      ui->lblTitle->show();
    }
  }

  return QWidget::eventFilter(watch, evn);
}

void MainWindow::on_actionAbout_triggered() {
  QString str;
  if (zh_cn)
    str = "https://gitee.com/ic005k/knot/releases";
  else
    str = "https://github.com/ic005k/Knot/releases/latest";
  QUrl url(str);
  QDesktopServices::openUrl(url);
}

void MainWindow::on_actionExport_Data_triggered() {
  if (!isSaveEnd) return;

  QString fileName;
  QFileDialog fd;
  fileName =
      fd.getSaveFileName(this, tr("KnotBak"), "", tr("Data Files(*.ini)"));

  bakData(fileName, true);

  QSettings Reg(iniDir + "mainnotes.ini", QSettings::IniFormat);
  Reg.setIniCodec("utf-8");
  Reg.setValue("/MainNotes/FileName", fileName);
}

void MainWindow::on_actionOneClickBakData() {
  QSettings Reg(iniDir + "mainnotes.ini", QSettings::IniFormat);
  Reg.setIniCodec("utf-8");
  QString fileName = Reg.value("/MainNotes/FileName").toString();
  if (QFile(fileName).exists()) {
    bakData(fileName, true);
  } else {
    QMessageBox msgBox;
    msgBox.setText(appName);
    msgBox.setInformativeText(
        tr("The previous file does not exist, please export the data first to "
           "generate a default file name."));
    QPushButton* btnOk = msgBox.addButton(tr("Ok"), QMessageBox::AcceptRole);
    btnOk->setFocus();
    msgBox.exec();
  }
}

void MainWindow::bakData(QString fileName, bool msgbox) {
  if (!fileName.isNull()) {
    ui->progBar->setHidden(false);
    ui->progBar->setMaximum(0);

    QTextEdit* edit = new QTextEdit;
    edit->append("[" + appName + "]");
    edit->append("Ver: " + ver);
    edit->append(loadText(iniDir + "tab.ini"));
    edit->append(loadText(iniDir + "font.ini"));
    edit->append(loadText(iniDir + "desc.ini"));
    edit->append(loadText(iniDir + "todo.ini"));
    edit->append(loadText(iniDir + "ymd.ini"));
    edit->append(loadText(iniDir + "notes.ini"));
    edit->append(loadText(iniDir + "mainnotes.ini"));
    edit->append(loadText(iniDir + "steps.ini"));
    edit->append(loadText(iniDir + "reader.ini"));

    for (int i = 0; i < tabData->tabBar()->count(); i++) {
      QString tabIniFile = iniDir + "tab" + QString::number(i + 1) + ".ini";
      if (QFile(tabIniFile).exists()) edit->append(loadText(tabIniFile));
    }

    TextEditToFile(edit, fileName);

    ui->progBar->setMaximum(100);

    if (QFile(fileName).exists() && msgbox) {
      QMessageBox msgBox;
      msgBox.setText(appName);
      msgBox.setInformativeText(tr("The data was exported successfully.") +
                                +"\n\n" + fileName);
      QPushButton* btnOk = msgBox.addButton(tr("Ok"), QMessageBox::AcceptRole);
      btnOk->setFocus();
      msgBox.exec();
    }
  }
}

void MainWindow::on_actionImport_Data_triggered() {
  if (!isSaveEnd) return;
  QString fileName;
  fileName = QFileDialog::getOpenFileName(this, tr("KnotBak"), "",
                                          tr("Data Files (*.ini)"));
  if (!fileName.isNull()) {
    QMessageBox msgBox;
    msgBox.setText(appName);
    msgBox.setInformativeText(tr("Import this data?") + "\n" + fileName);
    QPushButton* btnCancel =
        msgBox.addButton(tr("Cancel"), QMessageBox::RejectRole);
    QPushButton* btnOk = msgBox.addButton(tr("Ok"), QMessageBox::AcceptRole);
    btnOk->setFocus();
    msgBox.exec();
    if (msgBox.clickedButton() == btnCancel) {
      return;
    }

    QString txt = loadText(fileName);
    if (!txt.contains(appName) && !txt.contains("Xcounter")) {
      QMessageBox msgBox;
      msgBox.setText(appName);
      msgBox.setInformativeText(tr("Invalid data file."));

      QPushButton* btnOk = msgBox.addButton(tr("Ok"), QMessageBox::AcceptRole);
      btnOk->setFocus();
      msgBox.exec();
      return;
    }

    if (QFile(iniFile).exists()) QFile(iniFile).remove();
    QTextEdit* txtEdit = new QTextEdit;
    txtEdit->setPlainText(txt);
    TextEditToFile(txtEdit, iniFile);
    isImport = true;
    loading = true;
    init_TabData();
    loading = false;

    while (!isReadTWEnd)
      QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

    startSave("alltab");
    QFile::remove(iniDir + "mainnotes.ini");
    QSettings RegTotalIni(iniFile, QSettings::IniFormat);
    RegTotalIni.setIniCodec("utf-8");
    QSettings Reg(iniDir + "mainnotes.ini", QSettings::IniFormat);
    Reg.setIniCodec("utf-8");
    Reg.setValue("/MainNotes/UserKey",
                 RegTotalIni.value("/MainNotes/UserKey").toString());
    Reg.setValue("/MainNotes/Text",
                 RegTotalIni.value("/MainNotes/Text").toString());
    Reg.setValue("/MainNotes/CurPos",
                 RegTotalIni.value("/MainNotes/CurPos").toLongLong());
    Reg.setValue("/MainNotes/SlidePos",
                 RegTotalIni.value("/MainNotes/SlidePos").toLongLong());
    Reg.setValue("/MainNotes/FileName",
                 RegTotalIni.value("/MainNotes/FileName").toString());

    // TextReader
    QString strReader = iniDir + "reader.ini";
    QFile::remove(strReader);
    QSettings Reg1(strReader, QSettings::IniFormat);
    Reg1.setIniCodec("utf-8");
    RegTotalIni.beginGroup("Reader");
    QStringList list = RegTotalIni.allKeys();
    // qDebug() << "Reader--allKeys : " << list << endl;
    foreach (QString key, list) {
      QString value = RegTotalIni.value(key).toString();
      // qDebug() << key << " = " << value << endl;
      Reg1.setValue("/Reader/" + key, value);
    }
    RegTotalIni.endGroup();
    mydlgReader->initReader();
  }
}

int MainWindow::get_Day(QString date) {
  QStringList list = date.split(" ");
  if (list.count() == 4) {
    QString strDay = list.at(2);
    return strDay.toInt();
  }
  return 0;
}

QString MainWindow::get_Month(QString date) {
  QStringList list = date.split(" ");
  if (list.count() == 4) {
    QString str = list.at(1);
    return str;
  }
  return "";
}

QString MainWindow::get_Year(QString date) {
  QStringList list = date.split(" ");
  if (list.count() == 4) {
    QString str = list.at(3);
    return str;
  }
  return "";
}

QTreeWidget* MainWindow::get_tw(int tabIndex) {
  QTreeWidget* tw = (QTreeWidget*)ui->tabWidget->widget(tabIndex);
  return tw;
}

void MainWindow::on_actionView_App_Data_triggered() {
  mydlgNotes->init_Notes();

  bakData(iniFile, false);

  mydlgNotes->ui->textBrowser->clear();
  QSettings Reg(iniFile, QSettings::IniFormat);
  Reg.setIniCodec("utf-8");
  int keys = Reg.allKeys().count();

  mydlgNotes->ui->textBrowser->append("[" + appName + "]");
  mydlgNotes->ui->textBrowser->append("Ver: " + ver);
  mydlgNotes->ui->textBrowser->append("All Keys: " + QString::number(keys));
  mydlgNotes->ui->textBrowser->append("File Size: " +
                                      getFileSize(QFile(iniFile).size(), 2));
  mydlgNotes->ui->textBrowser->append("File: " + iniFile);

  mydlgNotes->ui->textBrowser->append("");
  mydlgNotes->ui->textBrowser->setHidden(true);
  mydlgNotes->ui->textEdit->setHidden(true);

  mydlgNotes->ui->lblTitle->setText("");
  mydlgNotes->ui->lblAbout->setText(mydlgNotes->ui->textBrowser->toPlainText());
  mydlgNotes->ui->frameAbout->show();
  mydlgNotes->setGeometry(0, 0, this->width(), this->height());
  mydlgNotes->setModal(true);
  mydlgNotes->show();
}

void MainWindow::on_btnFind_clicked() {
  if (ui->frame_find->isHidden()) {
    ui->frame_find->show();
    ui->frameYear->show();
    ui->frameBtn->hide();
    // ui->frame_charts->setMaximumHeight(frameChartHeight +
    //                                    ui->frameYear->height());

  } else {
    // ui->frame_charts->setMaximumHeight(frameChartHeight);
    ui->frame_find->hide();
    ui->frameYear->hide();
    ui->frameBtn->show();
  }
}

QStringList MainWindow::get_MonthList(QString strY, QString strM) {
  QStringList listMonth;
  if (loading) return listMonth;
  // 格式：记录第一个子项的时间
  PointList.clear();
  doubleList.clear();

  QTreeWidget* tw = (QTreeWidget*)tabData->currentWidget();
  for (int i = 0; i < tw->topLevelItemCount(); i++) {
    if (isBreak) break;
    QTreeWidgetItem* topItem = tw->topLevelItem(i);
    QString str0 = topItem->text(0);
    QString y, m, d;
    y = get_Year(str0);
    m = get_Month(str0);
    d = QString::number(get_Day(str0));
    double x0 = d.toDouble();
    if (y == strY) {
      if (m == strM) {
        if (isrbFreq) {
          if (topItem->childCount() > 0)
            listMonth.append(
                topItem->child(0)->text(0));  //记录第一个子项的时间

          double y0 = topItem->text(1).toDouble();
          doubleList.append(y0);

          PointList.append(QPointF(x0, y0));
        } else {
          double y0 = topItem->text(2).toDouble();
          doubleList.append(y0);

          PointList.append(QPointF(x0, y0));
        }
      }
    }
  }

  return listMonth;
}

void MainWindow::on_cboxYear_currentTextChanged(const QString& arg1) {
  Q_UNUSED(arg1);

  series->clear();
  m_scatterSeries->clear();
  barSeries->clear();
  series2->clear();
  m_scatterSeries2->clear();
  QTreeWidget* tw = (QTreeWidget*)tabData->currentWidget();
  int count = tw->topLevelItemCount();
  for (int i = 0; i < count; i++) {
    QString str = tw->topLevelItem(i)->text(0);
    if (get_Year(str) == ui->btnYear->text()) {
      if (get_Month(str) == ui->btnMonth->text()) {
        if (tabChart->currentIndex() == 0) {
          startRead(str);
          break;
        }
        if (tabChart->currentIndex() == 1) {
          if (get_Day(str) == ui->btnDay->text().toInt()) {
            tw->setCurrentItem(tw->topLevelItem(i));
            tw->setFocus();

            parentItem = NULL;
            on_twItemClicked();

            break;
          }
        }
      }
    }
  }
  startSave("ymd");
}

void MainWindow::on_btnGo_clicked() {
  ui->lblStats->setText("");
  QTreeWidget* tw = get_tw(ui->tabWidget->currentIndex());
  if (isFindTextChange) findItemList = findDisc();
  int total = findItemList.count();
  if (total > 0) {
    QTreeWidgetItem* item = findItemList.at(findPos);
    tw->setCurrentItem(item);
    ui->lblStats->setText(tr("Search Results : ") + item->parent()->text(0) +
                          "  ( " + QString::number(findPos + 1) + " -> " +
                          QString::number(total) + " ) ");
    findPos++;
    if (findPos == total) findPos = 0;
  }
}

QList<QTreeWidgetItem*> MainWindow::findDisc() {
  findItemList.clear();
  QTreeWidget* tw = get_tw(ui->tabWidget->currentIndex());
  for (int i = 0; i < tw->topLevelItemCount(); i++) {
    QTreeWidgetItem* topItem = tw->topLevelItem(i);
    int count = topItem->childCount();
    for (int j = 0; j < count; j++) {
      QString str = topItem->child(j)->text(2).trimmed().toLower();
      QString strFind = ui->editFind->text().trimmed().toLower();
      if (str.contains(strFind)) {
        findItemList.append(topItem->child(j));
      }
    }
  }
  isFindTextChange = false;
  findPos = 0;
  if (findItemList.count() > 0)
    setLineEditQss(ui->editFind, 4, 1, "#4169E1", "#4169E1");
  else
    setLineEditQss(ui->editFind, 4, 1, "#FA0000", "##FA0000");
  return findItemList;
}

void MainWindow::on_editFind_textChanged(const QString& arg1) {
  Q_UNUSED(arg1);
  setLineEditQss(ui->editFind, 4, 1, "#4169E1", "#4169E1");
  isFindTextChange = true;
}

QString MainWindow::setLineEditQss(QLineEdit* txt, int radius, int borderWidth,
                                   const QString& normalColor,
                                   const QString& focusColor) {
  QStringList list;
  list.append(QString("QLineEdit{border-style:none;padding:3px;border-radius:%"
                      "1px;border:%2px solid %3;}")
                  .arg(radius)
                  .arg(borderWidth)
                  .arg(normalColor));
  list.append(QString("QLineEdit:focus{border:%1px solid %2;}")
                  .arg(borderWidth)
                  .arg(focusColor));

  QString qss = list.join("");
  txt->setStyleSheet(qss);
  return qss;
}

QString MainWindow::setComboBoxQss(QComboBox* txt, int radius, int borderWidth,
                                   const QString& normalColor,
                                   const QString& focusColor) {
  QStringList list;
  list.append(QString("QComboBox{border-style:none;padding:3px;border-radius:%"
                      "1px;border:%2px solid %3;}")
                  .arg(radius)
                  .arg(borderWidth)
                  .arg(normalColor));
  list.append(QString("QComboBox:focus{border:%1px solid %2;}")
                  .arg(borderWidth)
                  .arg(focusColor));
  list.append(
      QString("QComboBox::down-arrow{image:url(:/icon/"
              "add_bottom.png);width:10px;height:10px;right:2px;}"));
  list.append(QString(
      "QComboBox::drop-down{subcontrol-origin:padding;subcontrol-position:"
      "top "
      "right;width:15px;border-left-width:0px;border-left-style:solid;border-"
      "top-right-radius:3px;border-bottom-right-radius:3px;border-left-color:"
      "#"
      "B6B6B6;}"));
  list.append(QString("QComboBox::drop-down:on{top:1px;}"));

  QString qss = list.join("");
  txt->setStyleSheet(qss);
  return qss;
}

void MainWindow::on_btnHide_clicked() { on_btnFind_clicked(); }

void MainWindow::on_actionFind_triggered() { on_btnFind_clicked(); }

void MainWindow::on_btnTodo_clicked() {
  mydlgTodo->setFixedHeight(this->height());
  mydlgTodo->setFixedWidth(this->width());
  mydlgTodo->setModal(true);

  mydlgTodo->show();
}

void MainWindow::on_rbFreq_clicked() {
  tabChart->setTabEnabled(1, true);
  isrbFreq = true;
  startRead(strDate);
  chartMonth->setTitle(tr("Freq"));
  chartDay->setTitle(tr("Freq"));
}

void MainWindow::on_rbAmount_clicked() {
  tabChart->setTabEnabled(1, true);
  isrbFreq = false;
  startRead(strDate);
  chartMonth->setTitle(tr("Amount"));
  chartDay->setTitle(tr("Amount"));
}

void MainWindow::paintEvent(QPaintEvent* event) {
  Q_UNUSED(event);

  //获取背景色
  QPalette pal = ui->btnFind->palette();
  QBrush brush = pal.window();
  int c_red = brush.color().red();
  if (c_red != red) {
    red = c_red;
    if (red < 55) {
      mydlgTodo->ui->listWidget->setStyleSheet(mydlgTodo->styleDark);
      chartMonth->setTheme(QChart::ChartThemeDark);
      chartDay->setTheme(QChart::ChartThemeDark);
    }

    else {
      mydlgTodo->ui->listWidget->setStyleSheet(
          mydlgTodo->ui->listWidget->styleSheet());
      chartMonth->setTheme(QChart::ChartThemeLight);
      chartDay->setTheme(QChart::ChartThemeLight);
    }
  }
}

void MainWindow::on_btnMax_clicked() {
  if (ui->frame_tab->isHidden()) return;
  if (ui->btnMax->text() == tr("Max")) {
    ui->frame_tab->setMaximumHeight(this->height());
    ui->frame_charts->setHidden(true);
    ui->frame_find->show();
    ui->frameYear->hide();
    ui->btnMax->setText(tr("Min"));
  } else if (ui->btnMax->text() == tr("Min")) {
    ui->frame_tab->setMaximumHeight(this->height());
    ui->frame_charts->setHidden(false);
    ui->frame_find->hide();
    ui->frameYear->hide();
    ui->btnMax->setText(tr("Max"));
  }
}

void MainWindow::on_btnYear_clicked() {
  int w = ui->btnYear->width();
  QListWidget* list = new QListWidget(this);
  list->setStyleSheet(listStyle);
  QFont font;
  font.setPointSize(fontSize);
  list->setFont(font);
  int year = 2022;
  QStringList strList;
  for (int i = 0; i < 10; i++) {
    strList.append(QString::number(year + i));
  }

  for (int i = 0; i < strList.count(); i++) {
    QListWidgetItem* item = new QListWidgetItem;
    item->setSizeHint(QSize(w, 30));  // item->sizeHint().width()
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    item->setText(strList.at(i));
    list->addItem(item);
  }
  connect(list, &QListWidget::itemClicked, [=]() {
    ui->btnYear->setText(list->currentItem()->text());
    btnYText = ui->btnYear->text();

    list->close();
    on_cboxYear_currentTextChanged("");
  });

  int h = 30 * list->count() + 4;
  int y = ui->frame_find->y() - h / 2;

  list->setGeometry(ui->frameYear->x(), y, w + 15, h);

  list->show();

  QString str = ui->btnYear->text();
  for (int i = 0; i < list->count(); i++) {
    if (str == list->item(i)->text()) {
      list->setCurrentRow(i);
      break;
    }
  }
}

void MainWindow::on_btnMonth_clicked() {
  int w = ui->btnYear->width();
  QListWidget* list = new QListWidget(this);
  list->setStyleSheet(listStyle);
  QFont font;
  font.setPointSize(fontSize);
  list->setFont(font);

  QStringList strList = listMonth;

  for (int i = 0; i < strList.count(); i++) {
    QListWidgetItem* item = new QListWidgetItem;
    item->setSizeHint(QSize(w, 30));  // item->sizeHint().width()
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    item->setText(strList.at(i));
    list->addItem(item);
  }
  connect(list, &QListWidget::itemClicked, [=]() {
    ui->btnMonth->setText(list->currentItem()->text());
    btnMText = ui->btnMonth->text();
    list->close();
    on_cboxYear_currentTextChanged("");
  });

  int h = 30 * list->count() + 4;
  int y = ui->frame_find->y() - h / 2;

  list->setGeometry(ui->btnMonth->x() + ui->frameYear->x() + 5, y, w + 5, h);

  list->show();

  QString str = ui->btnMonth->text();
  for (int i = 0; i < list->count(); i++) {
    if (str == list->item(i)->text()) {
      list->setCurrentRow(i);
      break;
    }
  }
}

void MainWindow::on_btnDay_clicked() {
  int w = ui->btnDay->width();
  QListWidget* list = new QListWidget(this);
  list->setStyleSheet(listStyle);
  list->verticalScrollBar()->setStyleSheet(vsbarStyleSmall);
  list->setVerticalScrollMode(QListWidget::ScrollPerPixel);
  QScroller::grabGesture(list, QScroller::LeftMouseButtonGesture);
  QFont font;
  font.setPointSize(fontSize);
  list->setFont(font);
  QStringList strList;
  for (int i = 0; i < 31; i++) {
    if (i <= 8)
      strList.append("0" + QString::number(i + 1));
    else
      strList.append(QString::number(i + 1));
  }

  for (int i = 0; i < strList.count(); i++) {
    QListWidgetItem* item = new QListWidgetItem;
    item->setSizeHint(QSize(w - 20, 30));  // item->sizeHint().width()
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    item->setText(strList.at(i));
    list->addItem(item);
  }

  connect(list, &QListWidget::itemClicked, [=]() {
    ui->btnDay->setText(list->currentItem()->text());
    btnDText = ui->btnDay->text();
    list->close();
    on_cboxYear_currentTextChanged("");
  });

  int h = 13 * 30;
  int y = ui->frame_find->y() - h / 2;
  int x = ui->btnDay->x() + ui->frameYear->x();
  list->setGeometry(x, y, w + 15, h);
  list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  list->show();

  QString str = ui->btnDay->text();
  for (int i = 0; i < list->count(); i++) {
    if (str == list->item(i)->text()) {
      list->setCurrentRow(i);
      break;
    }
  }
}

void MainWindow::on_actionReport_triggered() {
  QTreeWidget* tw = get_tw(ui->tabWidget->currentIndex());
  //测试专用
  if (isTesting) {
    startSave("tab");
  }
  double freq, amount;
  freq = 0;
  amount = 0;
  mydlgReport->ui->tableReport->setRowCount(0);
  for (int i = 0; i < tw->topLevelItemCount(); i++) {
    int count = mydlgReport->ui->tableReport->rowCount();
    mydlgReport->ui->tableReport->setRowCount(count + 1);

    mydlgReport->ui->tableReport->setColumnWidth(0, 10);
    mydlgReport->ui->tableReport->setRowHeight(i, 30);

    QTableWidgetItem* tableItem =
        new QTableWidgetItem(tw->topLevelItem(i)->text(0));
    mydlgReport->ui->tableReport->setItem(i, 0, tableItem);

    QString txt1 = tw->topLevelItem(i)->text(1);
    freq = freq + txt1.toDouble();
    tableItem = new QTableWidgetItem(txt1);
    tableItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    mydlgReport->ui->tableReport->setItem(i, 1, tableItem);

    QString txt2 = tw->topLevelItem(i)->text(2);
    amount = amount + txt2.toDouble();
    tableItem = new QTableWidgetItem(txt2);
    mydlgReport->ui->tableReport->setItem(i, 2, tableItem);

    mydlgReport->ui->tableReport->item(i, 0)->setFlags(Qt::NoItemFlags);
    mydlgReport->ui->tableReport->item(i, 1)->setFlags(Qt::NoItemFlags);
    mydlgReport->ui->tableReport->item(i, 2)->setFlags(Qt::NoItemFlags);
  }

  int count = mydlgReport->ui->tableReport->rowCount();
  if (count > 0) {
    mydlgReport->ui->tableReport->setRowCount(count + 1);
    QTableWidgetItem* tableItem = new QTableWidgetItem(tr("Total"));
    mydlgReport->ui->tableReport->setItem(count, 0, tableItem);

    tableItem = new QTableWidgetItem(QString::number(freq));
    tableItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    mydlgReport->ui->tableReport->setItem(count, 1, tableItem);

    QString strAmount = QString("%1").arg(amount, 0, 'f', 2);
    tableItem = new QTableWidgetItem(strAmount);
    mydlgReport->ui->tableReport->setItem(count, 2, tableItem);

    mydlgReport->ui->tableReport->setColumnWidth(0, 10);
    mydlgReport->ui->tableReport->setRowHeight(count, 30);

    mydlgReport->ui->tableReport->item(count, 0)->setFlags(Qt::NoItemFlags);
    mydlgReport->ui->tableReport->item(count, 1)->setFlags(Qt::NoItemFlags);
    mydlgReport->ui->tableReport->item(count, 2)->setFlags(Qt::NoItemFlags);

    mydlgReport->on_tableReport_cellClicked(count, 0);
    mydlgReport->ui->tableReport->scrollToBottom();
  }

  mydlgReport->ui->lblTitle->setText(
      ui->tabWidget->tabText(ui->tabWidget->currentIndex()));
  mydlgReport->ui->tableDetails->setRowCount(0);

  mydlgReport->sel_Year();
  mydlgReport->sel_Month();

  mydlgReport->setFixedHeight(this->height());
  mydlgReport->setFixedWidth(this->width());
  mydlgReport->setModal(true);
  mydlgReport->show();
}

void MainWindow::on_btnReport_clicked() { on_actionReport_triggered(); }

void MainWindow::on_actionPreferences_triggered() {
  mydlgPre->setFixedHeight(this->height());
  mydlgPre->setFixedWidth(this->width());
  mydlgPre->setModal(true);
  mydlgPre->show();
}

void MainWindow::on_tabCharts_currentChanged(int index) {
  int count = tabChart->tabBar()->count();
  if (index >= 0) {
    for (int i = 0; i < count; i++) {
      if (i == index)
        tabChart->tabBar()->setTabTextColor(i, Qt::blue);
      else
        tabChart->tabBar()->setTabTextColor(i, Qt::black);
    }
  }

  if (index == 0) {
    startRead(strDate);
  }

  if (index == 1) {
    QTreeWidget* tw = (QTreeWidget*)tabData->currentWidget();
    int topCount = tw->topLevelItemCount();
    if (topCount == 0) {
      series2->clear();
      m_scatterSeries2->clear();
      return;
    }

    if (topCount > 0) {
      if (!tw->currentIndex().isValid()) {
        QTreeWidgetItem* topItem = tw->topLevelItem(topCount - 1);
        tw->setCurrentItem(topItem);
      }
    }
    on_twItemClicked();
  }
}

void MainWindow::on_btnSteps_clicked() {
  if (isHardStepSensor == 1) updateHardSensorSteps();
  mydlgSteps->setFixedHeight(this->height());
  mydlgSteps->setFixedWidth(this->width());
  mydlgSteps->ui->tableWidget->scrollToBottom();
  mydlgSteps->ui->tableWidget->setFocus();
  mydlgSteps->setModal(true);
  mydlgSteps->show();
}

void MainWindow::changeEvent(QEvent* event) {
  if (event->type() == QEvent::WindowStateChange) {
  }
}

void MainWindow::on_rbSteps_clicked() {
  tabChart->setCurrentIndex(0);
  tabChart->setTabEnabled(1, false);
  QTableWidget* t = mydlgSteps->ui->tableWidget;
  PointList.clear();
  doubleList.clear();
  int count = t->rowCount();
  QString sm = get_Month(QDate::currentDate().toString("ddd MM dd yyyy"));
  for (int i = 0; i < count; i++) {
    QString strD = t->item(i, 0)->text();
    if (sm == get_Month(strD)) {
      int day = get_Day(strD);
      int steps = t->item(i, 1)->text().toInt();
      PointList.append(QPointF(day, steps));
      doubleList.append(steps);
    }
  }

  initChartMonth("", sm);
  chartMonth->setTitle(tr("Steps"));
}

void MainWindow::Sleep(int msec) {
  QTime dieTime = QTime::currentTime().addMSecs(msec);
  while (QTime::currentTime() < dieTime)
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

/* The main algorithm, processes the sensor input data in sensor input buffer
 *  stored by preproc function to detect step type and estimate step count.
 *  The algorithm estimates frequency and amplitude of the accel y-axis data
 *  by finding max and min values.
 *  Max and min values have derivate of 0, thus a lead-lag filter is used to
 *  take derivative of the input data. The zero-crossings of derivative data
 *  provides max and min values of input data.
 *  The  zero crossing is computed by finding points where the data changes
 * sign. Input: Pointer to the algo output data Output: None
 */
static void step_algo_run(algo_out_t* step_algo_output) {
#define VERY_HIGH_VAL (100000)
#define NO_DETECT_DUR_SEC \
  (0.2f) /* Duration to avoid very close peaks not related to step */
#define CLOSE_TO_ZERO \
  (1.5) /* Threshold to detect if peak is reasonable height       */
#define MAX_TIME_PERIOD_SEC \
  (1.5f) /* Maximum time duration of one step                      */
#define SMALL_AMP \
  (5.0) /* Lower threshold for change in accel during a step      */
#define LARGE_AMP \
  (15.0) /* Higher threshold for change in accel during a step     */
#define SLOW_FREQ \
  (0.5) /* Lower threshold for step rate during a step            */
#define FAST_FREQ \
  (2.2) /* Lower threshold for step rate during a step            */

  /* Local Variables */
  unsigned int i;
  float prev_max_val, prev_max_ts, prev_min_val, prev_min_ts;
  float new_max_val, new_max_ts, new_min_val, new_min_ts;
  float avg_max_val = 0.0f, avg_min_val = 0.0f, avg_time_period = 0.0f;
  float freq_est = 0.0f, amp_est = 0.0f;
  unsigned int delta = 1; /* distance between two samples to be compared   */
  unsigned int count_max_det = 0, count_min_det = 0;
  unsigned int TC_samples = 0;

  /* Local Static Variables to maintain state of algo and static array
   * allocation  */
  static float prev_amp_est = 0.0f, prev_freq_est = 0.0f;
  static unsigned int amp_est_hold = 0, freq_est_hold = 0;
  static float prevAccDer = 0.0f;
  static float AccFilt[SAMP_BUFF_LEN + MAX_TC_SAMPLES] = {0};
  static float TimeStamps[SAMP_BUFF_LEN + MAX_TC_SAMPLES] = {0};
  static float AccDer[SAMP_BUFF_LEN] = {0}; /* Derivative of Accel Data */

  /* Initialize variables */
  new_max_val = -VERY_HIGH_VAL;
  new_min_val = VERY_HIGH_VAL;
  new_max_ts = 0.0f;
  new_min_ts = 0.0f;

  /* Prev max and min values and their timestamps are   */
  /* needed to estimate one step in cases where data for*/
  /* one step span accross multiple calls to this algo  */
  prev_max_val = step_algo_output->prev_max;
  prev_max_ts = step_algo_output->prev_max_ts;
  prev_min_val = step_algo_output->prev_min;
  prev_min_ts = step_algo_output->prev_min_ts;

  /* Delay between derivative and filtered data due to  */
  /* derivate (lead0lag) fiter delay                    */
  TC_samples = ll_filter_y.TC_samples;

  for (i = 0; i < SAMP_BUFF_LEN; i++) {
    /* Compute the derivative of filtered Y-axis Acc Data  */
    AccDer[i] = apply_filter(&ll_filter_y, AccBuff[CHY][i]);
    /* AccFilt and TimeStamps are extended buffers to save */
    /* prev TC_samples along with new Y-axis Acc Data      */
    AccFilt[TC_samples + i] = AccBuff[CHY][i];
    TimeStamps[TC_samples + i] = AccBuff[CHZ + 1][i];
  }

  /* Find the new max/min values of Filtered Y-axis Acc Data and timestamp */
  /* This is done by detecting rising/falling zero crossings in derivative of
   * Acc Data */
  for (i = 0; i < SAMP_BUFF_LEN; i = i + delta) {
    if (i >= delta) prevAccDer = AccDer[i - delta];

    if (prev_max_ts <= prev_min_ts) {
      /* need to find the next max val(falling ZC) */
      if ((AccDer[i] < -EPSILON) && (prevAccDer >= 0.0f)) {
        /* Avoid searching very close to already found max value */
        if (TimeStamps[i] - prev_max_ts > NO_DETECT_DUR_SEC) {
          new_max_val = AccFilt[i];
          if (fabs(new_max_val) > CLOSE_TO_ZERO) {
            new_max_ts = TimeStamps[i];
            /* Clamp diff betweeen new_max_ts and prev_max_ts to avoid */
            /* distortion in estimation of step frequency              */
            if (new_max_ts > prev_max_ts + MAX_TIME_PERIOD_SEC)
              prev_max_ts = new_max_ts - MAX_TIME_PERIOD_SEC;
            /* Avoid max vlaue which is very close to prev min value   */
            if (new_max_val - prev_min_val > CLOSE_TO_ZERO) {
              avg_time_period = avg_time_period + (new_max_ts - prev_max_ts);
              avg_max_val = avg_max_val + new_max_val;
              count_max_det = count_max_det + 1;
              prev_max_ts = new_max_ts;
              prev_max_val = new_max_val;
            }
          }
        }
      }
    } else {
      /* need to find the next min val(rising ZC) */
      if ((AccDer[i] > EPSILON) && (prevAccDer <= 0.0f)) {
        /* Avoid searching very close to already found min value */
        if (TimeStamps[i] - prev_min_ts > NO_DETECT_DUR_SEC) {
          new_min_val = AccFilt[i];
          new_min_ts = TimeStamps[i];
          /* Clamp diff betweeen new_max_ts and prev_max_ts to avoid */
          /* distortion in estimation of step frequency              */
          if (new_min_ts > prev_min_ts + MAX_TIME_PERIOD_SEC)
            prev_min_ts = new_min_ts - MAX_TIME_PERIOD_SEC;
          /* Avoid min vlaue which is very close to prev max value   */
          if (prev_max_val - new_min_val > CLOSE_TO_ZERO) {
            avg_time_period = avg_time_period + (new_min_ts - prev_min_ts);
            avg_min_val = avg_min_val + new_min_val;
            amp_est += prev_max_val - new_min_val;
            /* A pair of max and min is one step and is equal to   */
            /* the value of count_min_det                          */
            count_min_det = count_min_det + 1;
            prev_min_ts = new_min_ts;
            prev_min_val = new_min_val;
          }
        }
      }
    }
  }

  if (count_min_det > 0) {
    amp_est = amp_est / count_min_det;
    amp_est_hold = 0;
  } else {
    amp_est = prev_amp_est;
    amp_est_hold++;
  }
  if (amp_est_hold > BUFF_FACTOR) {
    amp_est_hold = 0;
    amp_est = 0.0f;
  }

  if (count_max_det + count_min_det > 0) {
    avg_time_period = avg_time_period / (count_max_det + count_min_det);
  }
  if (avg_time_period > EPSILON) {
    freq_est = 1 / avg_time_period;
    freq_est_hold = 0;
  } else {
    freq_est = prev_freq_est;
    freq_est_hold++;
  }
  if (freq_est_hold > BUFF_FACTOR) {
    freq_est_hold = 0;
    freq_est = 0.0f;
  }

  /* Save the last TC_samples for next run */
  for (i = 0; i < TC_samples; i++) {
    AccFilt[i] = AccBuff[CHY][SAMP_BUFF_LEN - TC_samples + i];
    TimeStamps[i] = AccBuff[CHZ + 1][SAMP_BUFF_LEN - TC_samples + i];
  }
  /* Save selected algo data for the next run */
  prevAccDer = AccDer[SAMP_BUFF_LEN - 1];
  prev_amp_est = amp_est;
  prev_freq_est = freq_est;

  /* Update the algo output */
  step_algo_output->prev_max = prev_max_val;
  step_algo_output->prev_max_ts = prev_max_ts;
  step_algo_output->prev_min = prev_min_val;
  step_algo_output->prev_min_ts = prev_min_ts;
  step_algo_output->step_count += count_min_det;
  if (amp_est <= SMALL_AMP) {
    if (freq_est <= SLOW_FREQ) /* STATIONARY */
      step_algo_output->step_type = STATIC;
    else {
      /* WALKING */
      step_algo_output->step_type = WALK;
      num_steps_walk += count_min_det;
    }
  } else if (amp_est >= LARGE_AMP) {
    if (freq_est >= FAST_FREQ) {
      /* RUNNING */
      step_algo_output->step_type = RUN;
      num_steps_run += count_min_det;
    } else {
      /* HOPPINNG */
      step_algo_output->step_type = HOP;
      num_steps_hop += count_min_det;
    }
  } else {
    if (freq_est >= FAST_FREQ) {
      /* RUNNING */
      step_algo_output->step_type = RUN;
      num_steps_run += count_min_det;
    } else {
      /* WALKING */
      step_algo_output->step_type = WALK;
      num_steps_walk += count_min_det;
    }
  }
}

/* Apply second order filter on input data
 *  Update filter state for the next run
 *  Input: Pointer to filter state var, Input data to filter
 *  Output: Filtered data
 */
static float apply_filter(filter_t* filt_data, float in_data) {
  float out_data;

  /* compute new output */
  out_data = (filt_data->b0 * in_data) + (filt_data->b1 * filt_data->prev_in) +
             (filt_data->b2 * filt_data->prev_prev_in) -
             (filt_data->a1 * filt_data->prev_out) -
             (filt_data->a2 * filt_data->prev_prev_out);

  /* update state of filter */
  filt_data->prev_prev_in = filt_data->prev_in;
  filt_data->prev_in = in_data;
  filt_data->prev_prev_out = filt_data->prev_out;
  filt_data->prev_out = out_data;

  return out_data;
}

/* Algo date preprocessing function called by Main to do some preprocessing of
 * sensor input data and store the preprocessed data in a buffer. Only call
 * algorithm to run when the sensor input buffer is full, this saves power
 *  Input: Timestamp in sec, AccX, AccY, AccZ, GyroX, GyroY, GyroZ data
 *  Output: 1 if sensor input buffer is full, 0 otherwise
 */
static unsigned int step_algo_preproc(float timestamp, float arx, float ary,
                                      float arz, float grx, float gry,
                                      float grz) {
  static unsigned int count = 0;
  unsigned int ret_val = 0;
  float ary_flt = 0.0f;

  /* Only use y-axis accelerometer data for algo      */
  /* Filter input sensor data before saving in buffer */
  ary_flt = apply_filter(&lp_filter_y, ary);
  AccBuff[CHY][count] = ary_flt;
  AccBuff[CHZ + 1][count] = timestamp;
  count = count + 1;
  if (count == SAMP_BUFF_LEN) {
    /* buffer is full, signal algo to run */
    ret_val = 1;
    count = 0;
  }

  return ret_val;
}

void MainWindow::getSteps2() {
#define MAX_CHAR_PER_LINE (120)

  // FILE *fpin, *fpout;
  // unsigned int rec_id = 0, sen_id = 0;
  // char date[12] = {0}, time[12] = {0};
  float arx = 0.0f, ary = 0.0f, arz = 0.0f; /* m/s^2 */
  float grx = 0.0f, gry = 0.0f, grz = 0.0f; /* rad/s */
  // float timestamp = 0.0f;                   /* sec */
  char line_buff[MAX_CHAR_PER_LINE] = {0};
  unsigned int skip_lines = 2;
  unsigned int run_step_algo = 0;
  unsigned int timeconst_samp = 0;
  char* step_type;

  /*if( argc != 3) {
    printf("Usage: %s inputfile outputfile\n", argv[0]);
    exit(1);
  }

  fpin = fopen(argv[1], "r");
  if(fpin == NULL) {
    printf("Cannot open input file: %s\n", argv[1]);
    exit(1);
  }

  fpout = fopen(argv[2], "w");
  if(fpout == NULL) {
    printf("Cannot open output file: %s\n", argv[2]);
    exit(1);
  }*/

  /* Algorithm uses a 2nd order low pass filter to smoothen the input sensor
   * data   */
  /* and a 2nd order lead-lag filter to estimate time derivate of input sensor
   * data */

  /* Initialize 2nd order low pass fitler data, 3Hz cut-off */
  lp_filter_x.b0 = lp_filter_y.b0 = lp_filter_z.b0 = 7.2269463E-03f;
  lp_filter_x.b1 = lp_filter_y.b1 = lp_filter_z.b1 = 1.4453893E-02f;
  lp_filter_x.b2 = lp_filter_y.b2 = lp_filter_z.b2 = 7.2269463E-03f;
  lp_filter_x.a1 = lp_filter_y.a1 = lp_filter_z.a1 = -1.7455322E+0f;
  lp_filter_x.a2 = lp_filter_y.a2 = lp_filter_z.a2 = 7.7444003E-01f;
  lp_filter_x.prev_in = lp_filter_y.prev_in = lp_filter_z.prev_in = 0.0f;
  lp_filter_x.prev_prev_in = lp_filter_y.prev_prev_in =
      lp_filter_z.prev_prev_in = 0.0f;
  lp_filter_x.prev_out = lp_filter_y.prev_out = lp_filter_z.prev_out = 0.0f;
  lp_filter_x.prev_prev_out = lp_filter_y.prev_prev_out =
      lp_filter_z.prev_prev_out = 0.0f;
  timeconst_samp = (unsigned int)(SENSOR_SAMP_FREQ * 0.075f) + 1;
  if (timeconst_samp > MAX_TC_SAMPLES) timeconst_samp = MAX_TC_SAMPLES;
  lp_filter_x.TC_samples = lp_filter_y.TC_samples = lp_filter_z.TC_samples =
      timeconst_samp;

  /* Initialize 2nd order lead lag fitler data, 4Hz cut-off */
  ll_filter_x.b0 = ll_filter_y.b0 = ll_filter_z.b0 = 2.5369363f;
  ll_filter_x.b1 = ll_filter_y.b1 = ll_filter_z.b1 = 0.0f;
  ll_filter_x.b2 = ll_filter_y.b2 = ll_filter_z.b2 = -2.5369363f;
  ll_filter_x.a1 = ll_filter_y.a1 = ll_filter_z.a1 = -1.6641912f;
  ll_filter_x.a2 = ll_filter_y.a2 = ll_filter_z.a2 = 0.71297842f;
  ll_filter_x.prev_in = ll_filter_y.prev_in = ll_filter_z.prev_in = 0.0f;
  ll_filter_x.prev_prev_in = ll_filter_y.prev_prev_in =
      ll_filter_z.prev_prev_in = 0.0f;
  ll_filter_x.prev_out = ll_filter_y.prev_out = ll_filter_z.prev_out = 0.0f;
  ll_filter_x.prev_prev_out = ll_filter_y.prev_prev_out =
      ll_filter_z.prev_prev_out = 0.0f;
  timeconst_samp = (unsigned int)(SENSOR_SAMP_FREQ * 0.06f) + 1;
  if (timeconst_samp > MAX_TC_SAMPLES) timeconst_samp = MAX_TC_SAMPLES;
  ll_filter_x.TC_samples = ll_filter_y.TC_samples = ll_filter_z.TC_samples =
      timeconst_samp;

  /* Initialize algo output data struct */
  step_algo_output.prev_max = 0.0;
  step_algo_output.prev_max_ts = 0.0;
  step_algo_output.prev_min = 0.0;
  step_algo_output.prev_min_ts = 0.0;
  step_algo_output.step_count = 0;
  step_algo_output.step_type = motion_type_t::STATIC;

  /* Skip the first two lines of input file */
  // while ((skip_lines > 0) &&
  //(NULL != fgets(line_buff, MAX_CHAR_PER_LINE, fpin))) {
  // skip_lines--;
  //}
  // if (skip_lines > 0) {
  // printf("Cannot read first two lines of input file: %s\n", argv[2]);
  // exit(1);
  //}

  // fprintf(fpout,
  //         "RECORD, TYPE, DATE, TIME, arx, ary, arz, grx, gry, grz, "
  //         "timestamp(sec), step_count, step_type, step_type_num\n");
  /* Process input sensor data from input file and save result in output file */
  // while (NULL != fgets(line_buff, MAX_CHAR_PER_LINE, fpin)) {
  // sscanf(line_buff, "%d, %d, %[^,], %[^,], %f, %f, %f, %f, %f, %f\n",
  // &rec_id,
  //        &sen_id, date, time, &arx, &ary, &arz, &grx, &gry, &grz);

  for (int i = 0; i < MAX_CHAR_PER_LINE; i++) {
    arx = rlistX.at(i);
    ary = rlistY.at(i);
    arz = rlistZ.at(i);
    grx = glistX.at(i);
    gry = glistY.at(i);
    grz = glistZ.at(i);

    memcpy(line_buff, &arx, sizeof(line_buff));
    memcpy(line_buff, &ary, sizeof(line_buff));
    memcpy(line_buff, &arz, sizeof(line_buff));
    memcpy(line_buff, &grx, sizeof(line_buff));
    memcpy(line_buff, &gry, sizeof(line_buff));
    memcpy(line_buff, &grz, sizeof(line_buff));

    timestamp += SENSOR_SAMP_INTVL;

    run_step_algo = step_algo_preproc(timestamp, arx, ary, arz, grx, gry, grz);
    if (run_step_algo == 1) {
      /* Collected enough sensor data to run step detect and count */
      step_algo_run(&step_algo_output);
    }

    if (step_algo_output.step_type == STATIC)
      step_type = "STATIONARY";
    else if (step_algo_output.step_type == WALK)
      step_type = "WALKING";
    else if (step_algo_output.step_type == RUN)
      step_type = "RUNNING";
    else if (step_algo_output.step_type == HOP)
      step_type = "HOPPING";

    // fprintf(fpout, "%d, %d, %s, %s, %f, %f, %f, %f, %f, %f, %f, %d, %s,
    // %d\n",
    //         rec_id, sen_id, date, time, arx, ary, arz, grx, gry, grz,
    //         timestamp, step_algo_output.step_count, step_type,
    //         step_algo_output.step_type);
  }

  printf(
      "Total motion duration is %f sec, which contains approximatly:\n %d "
      "Total number of steps including\n |---> %d steps of WALKING, \n |---> "
      "%d steps of RUNNING, and \n |---> %d steps of HOPPING.\n",
      timestamp, num_steps_walk + num_steps_run + num_steps_hop, num_steps_walk,
      num_steps_run, num_steps_hop);
  printf("Done.\n");

  QString str0 = tr("Duration") + " : " + mw_one->secondsToTime(timestamp);
  QString str1 = tr("Calculated Frequency") + " : " +
                 QString::number(step_algo_output.step_count);
  QString str2 = tr("Walk") + " : " + QString::number(num_steps_walk);
  QString str3 = tr("Run") + " : " + QString::number(num_steps_run);
  QString str4 = tr("Hop") + " : " + QString::number(num_steps_hop);

  mydlgSteps->ui->lblSteps->setText(str0 + "\n" + str1 + "\n" + str2 + "  " +
                                    str3 + "  " + str4);
  mw_one->updateSteps();
  rlistX.clear();
  rlistY.clear();
  rlistZ.clear();
  glistX.clear();
  glistY.clear();
  glistZ.clear();
}

QString MainWindow::secondsToTime(ulong totalTime) {
  // 输入为秒数则ss=1，输入为毫秒数则ss=1000
  qint64 ss = 1;
  qint64 mi = ss * 60;
  qint64 hh = mi * 60;
  qint64 dd = hh * 24;

  qint64 day = totalTime / dd;
  qint64 hour = (totalTime - day * dd) / hh;
  qint64 minute = (totalTime - day * dd - hour * hh) / mi;
  qint64 second = (totalTime - day * dd - hour * hh - minute * mi) / ss;

  QString hou = QString::number(hour, 10);
  QString min = QString::number(minute, 10);
  QString sec = QString::number(second, 10);

  hou = hou.length() == 1 ? QString("0%1").arg(hou) : hou;
  min = min.length() == 1 ? QString("0%1").arg(min) : min;
  sec = sec.length() == 1 ? QString("0%1").arg(sec) : sec;
  return hou + ":" + min + ":" + sec;
}

void MainWindow::updateHardSensorSteps() {
  timeTest = timeTest + 1;
  mydlgSteps->ui->lblTotalRunTime->setText(tr("Number of Operations") + " : " +
                                           QString::number(timeTest));

  if (strDate != QDate::currentDate().toString("ddd MM dd yyyy")) {
    initTodayInitSteps();
    strDate = QDate::currentDate().toString("ddd MM dd yyyy");
  }

  qlonglong steps = 0;
#ifdef Q_OS_ANDROID

  QAndroidJniObject m_activity = QtAndroid::androidActivity();
  m_activity.callMethod<void>("initStepSensor");

  QAndroidJniObject jo = QAndroidJniObject::fromString("getSteps");
  tc = jo.callStaticMethod<float>("com.x/MyActivity", "getSteps", "()F");
#endif
  steps = tc - initTodaySteps;

  if (steps < 0) return;
  if (steps > 100000000) return;
  CurrentSteps = tc - resetSteps;
  mydlgSteps->ui->lcdNumber->display(QString::number(steps));
  mydlgSteps->ui->lblSingle->setText(QString::number(CurrentSteps));
  mydlgSteps->setTableSteps(steps);

  sendMsg(steps);
}

void MainWindow::on_actionMemos_triggered() {
  mydlgMainNotes->move(0, 0);
  mydlgMainNotes->setFixedHeight(this->height());
  mydlgMainNotes->setFixedWidth(this->width());
  mydlgMainNotes->setModal(true);
  mydlgMainNotes->ui->textBrowser->hide();
  mydlgMainNotes->ui->textEdit->show();
  mydlgMainNotes->ui->btnSetKey->show();
  mydlgMainNotes->ui->frameSetKey->hide();

  QSettings Reg(iniDir + "mainnotes.ini", QSettings::IniFormat);
  Reg.setIniCodec("utf-8");
  QString file = iniDir + "mainnotes.txt";

  QString strPw = Reg.value("/MainNotes/UserKey").toString();
  if (strPw != "") {
    QByteArray baPw = strPw.toUtf8();
    for (int i = 0; i < baPw.size(); i++) {
      baPw[i] = baPw[i] - 66;  //解密User的密码
    }
    strPw = baPw;

    bool ok;
    QInputDialog* idlg = new QInputDialog(this);
    QString style =
        "QDialog{background: "
        "rgb(244,237,241);border-radius:0px;border:2px solid gray;}";
    idlg->setStyleSheet(style);
    idlg->setOkButtonText(tr("Ok"));
    idlg->setCancelButtonText(tr("Cancel"));
    idlg->setWindowTitle(tr("Please enter your password : "));
    idlg->setTextValue("");
    idlg->setLabelText(tr("Password : "));
    QLineEdit::EchoMode echoMode = QLineEdit::Password;
    idlg->setTextEchoMode(echoMode);
    QString text;

    if (QDialog::Accepted == idlg->exec()) {
      ok = true;
      text = idlg->textValue();
    } else
      ok = false;

    if (ok && !text.isEmpty()) {
      if (text.trimmed() == strPw) {
        decMemos(file);
        mydlgMainNotes->setCursorPosition();
        mydlgMainNotes->show();

      } else {
        QMessageBox msgBox;
        msgBox.setText("Knot");
        msgBox.setInformativeText(tr("The entered password does not match."));
        QPushButton* btnOk =
            msgBox.addButton(tr("Ok"), QMessageBox::AcceptRole);
        btnOk->setFocus();
        msgBox.exec();
      }
    }

  } else {
    decMemos(file);
    mydlgMainNotes->setCursorPosition();
    mydlgMainNotes->show();
  }
}

void MainWindow::decMemos(QString file) {
  TextEditToFile(mydlgMainNotes->ui->textEdit, file);
  if (QFile(file).exists()) {
    mydlgMainNotes->decode(file);
    mydlgMainNotes->ui->textEdit->setPlainText(
        mydlgMainNotes->Deciphering(file));
    QFile::remove(file);
  }
}

void MainWindow::init_Sensors() {
  accel_pedometer = new SpecialAccelerometerPedometer(this);
  // connect(accel_pedometer, SIGNAL(readingChanged()), this, SLOT(newDatas()));
  connect(accel_pedometer, SIGNAL(stepCountChanged()), this,
          SLOT(updateSteps()));

  accel_pedometer->setTangentLineIntercept(
      mydlgSteps->ui->editTangentLineIntercept->text().toFloat());
  accel_pedometer->setTangentLineSlope(
      mydlgSteps->ui->editTangentLineSlope->text().toFloat());
  accel_pedometer->setDataRate(100);
  accel_pedometer->setAccelerationMode(QAccelerometer::User);
  accel_pedometer->setAlwaysOn(true);

  gyroscope = new QGyroscope(this);
}

void MainWindow::init_UIWidget() {
  mw_one = this;

  strDate = QDate::currentDate().toString("ddd MM dd yyyy");
  isReadEnd = true;
  tabData = new QTabWidget;
  tabData = ui->tabWidget;
  tabChart = new QTabWidget;
  tabChart = ui->tabCharts;

  this->layout()->setMargin(0);
  ui->centralwidget->layout()->setMargin(1);
  ui->centralwidget->layout()->setContentsMargins(1, 0, 1, 7);
  ui->centralwidget->layout()->setSpacing(1);
  ui->frame_charts->setContentsMargins(0, 0, 0, 0);
  ui->frame_charts->layout()->setMargin(0);
  ui->frame_charts->layout()->setContentsMargins(0, 0, 0, 0);
  ui->frame_charts->layout()->setSpacing(0);
  ui->frame_charts->setMaximumHeight(frameChartHeight);

  this->installEventFilter(this);
  ui->tabWidget->tabBar()->installEventFilter(this);
  ui->tabWidget->installEventFilter(this);
  ui->frame_tab->setMouseTracking(true);
  ui->tabWidget->setMouseTracking(true);
  ui->progBar->setMaximumHeight(2);
  mydlgNotes = new dlgNotes(this);
  mydlgNotes->ui->textEdit->verticalScrollBar()->setStyleSheet(vsbarStyleSmall);
  mydlgNotes->ui->textBrowser->verticalScrollBar()->setStyleSheet(
      vsbarStyleSmall);
  mydlgRename = new dlgRename(this);
  mydlgSetTime = new dlgSetTime(this);
  mydlgTodo = new dlgTodo(this);
  mydlgTodo->setStyleSheet(vsbarStyleSmall);
  mydlgList = new dlgList(this);
  mydlgReport = new dlgReport(this);
  mydlgPre = new dlgPreferences(this);
  mydlgMainNotes = new dlgMainNotes(this);
  mydlgSteps = new dlgSteps(this);
  ui->lblStats->adjustSize();
  ui->lblStats->setWordWrap(true);
  mydlgReader = new dlgReader(this);

  timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
  timerStep = new QTimer(this);
  connect(timerStep, SIGNAL(timeout()), this, SLOT(updateHardSensorSteps()));

  ui->statusbar->setHidden(true);

  myReadTWThread = new ReadTWThread();
  connect(myReadTWThread, &ReadTWThread::isDone, this, &MainWindow::readTWDone);

  myReadThread = new ReadThread();
  connect(myReadThread, &ReadThread::isDone, this, &MainWindow::readDone);

  mySearchThread = new SearchThread();
  connect(mySearchThread, &SearchThread::isDone, this, &MainWindow::dealDone);

  ui->frame_find->setHidden(true);
  ui->frameYear->hide();

  ui->progBar->setStyleSheet(
      "QProgressBar{border:0px solid #FFFFFF;"
      "height:30;"
      "background:rgba(25,255,25,0);"
      "text-align:right;"
      "color:rgb(255,255,255);"
      "border-radius:0px;}"

      "QProgressBar:chunk{"
      "border-radius:0px;"
      "background-color:rgba(25,25,255,250);"
      "}");

  setLineEditQss(ui->editFind, 4, 1, "#4169E1", "#4169E1");

  int iz = 25;
  ui->btnFind->setIconSize(QSize(iz, iz));
  ui->btnReport->setIconSize(QSize(iz, iz));
  ui->btnNotes->setIconSize(QSize(iz, iz));
  ui->btnSelTab->setIconSize(QSize(iz, iz));
  ui->btnPause->setIconSize(QSize(iz, iz));

  int s = 28;
  if (isIOS) {
  }
  ui->btnPlus->setIconSize(QSize(s, s));
  ui->btnLess->setIconSize(QSize(s, s));
  ui->btnTodo->setIconSize(QSize(s, s));
  ui->btnMax->setIconSize(QSize(s, s));
  ui->btnSteps->setIconSize(QSize(s, s));
  ui->btnReader->setIconSize(QSize(s, s));
  ui->btnPlus->setIcon(QIcon(":/src/1.png"));
  ui->btnLess->setIcon(QIcon(":/src/2.png"));
  ui->btnTodo->setIcon(QIcon(":/src/todo.png"));
  ui->btnMax->setIcon(QIcon(":/src/zoom.png"));
  ui->btnSteps->setIcon(QIcon(":/src/step.png"));
  ui->btnReader->setIcon(QIcon(":/src/one.png"));
  ui->frame_tab->setMaximumHeight(this->height());

  ui->btnPlus->setStyleSheet("border:none");
  ui->btnLess->setStyleSheet("border:none");
  ui->btnTodo->setStyleSheet("border:none");
  ui->btnSteps->setStyleSheet("border:none");
  ui->btnMax->setStyleSheet("border:none");
  ui->btnReader->setStyleSheet("border:none");
}

void MainWindow::on_btnSelTab_clicked() {
  QListWidget* list = new QListWidget(this);
  list->setStyleSheet(listStyle);
  list->verticalScrollBar()->setStyleSheet(vsbarStyleSmall);
  list->setVerticalScrollMode(QListWidget::ScrollPerPixel);
  QScroller::grabGesture(list, QScroller::LeftMouseButtonGesture);
  QFont font;
  font.setPointSize(fontSize);
  list->setFont(font);

  int count = tabData->tabBar()->count();
  for (int i = 0; i < count; i++) {
    QListWidgetItem* item = new QListWidgetItem;
    item->setSizeHint(QSize(130, 30));  // item->sizeHint().width()
    item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    item->setText(" " + QString::number(i + 1) + " . " + tabData->tabText(i));
    list->addItem(item);
  }
  connect(list, &QListWidget::itemClicked, [=]() {
    tabData->setCurrentIndex(list->currentRow());
    list->close();
  });

  int h = 0;
  if (count <= 15)
    h = count * 30 + 4;
  else
    h = 15 * 30 + 4;

  int w = 220;
  int y = (this->height() - h) / 2;
  int x = (this->width() - w) / 2 + 3;
  list->setGeometry(x, y, w, h);
  list->setCurrentRow(tabData->currentIndex());
  list->show();
  list->setFocus();
}

void MainWindow::init_Menu() {
  ui->frameMenu->setStyleSheet("background-color: rgb(243,243,243);");
  ui->btnFind->setStyleSheet("border:none");
  ui->btnMenu->setStyleSheet("border:none");
  ui->btnReport->setStyleSheet("border:none");
  ui->btnNotes->setStyleSheet("border:none");
  ui->btnPause->setStyleSheet("border:none");
  ui->btnSelTab->setStyleSheet("border:none");
  ui->lblIcon->setText("");
  ui->lblIcon->setFixedHeight(22);
  ui->lblIcon->setFixedWidth(22);
  ui->lblIcon->setStyleSheet(
      "QLabel{"
      "border-image:url(:/src/icon.png) 4 4 4 4 stretch stretch;"
      "}");

  mainMenu = new QMenu(this);
  QAction* actAddTab = new QAction(tr("Add Tab"));
  QAction* actDelTab = new QAction(tr("Del Tab"));
  QAction* actRenameTab = new QAction(tr("Rename Tab"));
  QAction* actFind = new QAction(tr("Find"));
  QAction* actReport = new QAction(tr("Report"));
  QAction* actRemarks = new QAction(tr("Remarks"));
  QAction* actExportData = new QAction(tr("Export Data"));
  QAction* actImportData = new QAction(tr("Import Data"));
  QAction* actPreferences = new QAction(tr("Preferences"));
  QAction* actMemos = new QAction(tr("Memos"));
  QAction* actViewAppData = new QAction(tr("About") + " (" + ver + ")");
  QAction* actAbout = new QAction(tr("Check for New Releases"));
  actAbout->setVisible(false);
  QAction* actBakData = new QAction(tr("One Click Data Backup"));

  mainMenu->addAction(actAddTab);
  mainMenu->addAction(actDelTab);
  mainMenu->addAction(actRenameTab);
  mainMenu->addSeparator();

  mainMenu->addAction(actFind);
  mainMenu->addAction(actReport);
  mainMenu->addAction(actRemarks);
  mainMenu->addSeparator();

  mainMenu->addAction(actExportData);
  mainMenu->addAction(actImportData);
  mainMenu->addAction(actBakData);
  mainMenu->addSeparator();

  mainMenu->addAction(actPreferences);
  mainMenu->addSeparator();

  mainMenu->addAction(actMemos);
  mainMenu->addSeparator();
  mainMenu->addAction(actViewAppData);
  mainMenu->addAction(actAbout);

  connect(actAddTab, &QAction::triggered, this,
          &MainWindow::on_actionAdd_Tab_triggered);
  connect(actDelTab, &QAction::triggered, this,
          &MainWindow::on_actionDel_Tab_triggered);
  connect(actRenameTab, &QAction::triggered, this,
          &MainWindow::on_actionRename_triggered);
  connect(actFind, &QAction::triggered, this,
          &MainWindow::on_actionFind_triggered);
  connect(actReport, &QAction::triggered, this,
          &MainWindow::on_actionReport_triggered);
  connect(actRemarks, &QAction::triggered, this,
          &MainWindow::on_actionNotes_triggered);
  connect(actExportData, &QAction::triggered, this,
          &MainWindow::on_actionExport_Data_triggered);
  connect(actBakData, &QAction::triggered, this,
          &MainWindow::on_actionOneClickBakData);
  connect(actImportData, &QAction::triggered, this,
          &MainWindow::on_actionImport_Data_triggered);
  connect(actPreferences, &QAction::triggered, this,
          &MainWindow::on_actionPreferences_triggered);
  connect(actMemos, &QAction::triggered, this,
          &MainWindow::on_actionMemos_triggered);
  connect(actViewAppData, &QAction::triggered, this,
          &MainWindow::on_actionView_App_Data_triggered);
  connect(actAbout, &QAction::triggered, this,
          &MainWindow::on_actionAbout_triggered);
}

void MainWindow::on_btnMenu_clicked() {
  int x = ui->btnMenu->x();
  int y = ui->frameMenu->y() + ui->frameMenu->height();
  QPoint pos(x, y);
  mainMenu->exec(pos);
}

void MainWindow::on_btnZoom_clicked() {
  if (!ui->frame_tab->isHidden()) {
    ui->frame_tab->hide();
    ui->frame_charts->setMaximumHeight(this->height());
  } else {
    ui->frame_tab->show();
    ui->frame_charts->setMaximumHeight(frameChartHeight);
  }
}

void MainWindow::stopJavaTimer() {
#ifdef Q_OS_ANDROID
  QAndroidJniObject jo = QAndroidJniObject::fromString("StopWin");
  jo.callStaticMethod<int>("com.x/MyService", "stopTimer", "()I");
#endif
  accel_pedometer->stop();
  accel_pedometer->setActive(false);
  gyroscope->stop();
  gyroscope->setActive(false);
}

#ifdef Q_OS_ANDROID
static void JavaNotify_1() {
  mw_one->newDatas();
  // qDebug() << "C++ JavaNotify_1";
}
static void JavaNotify_2() {
  mw_one->updateHardSensorSteps();

  // qDebug() << "C++ JavaNotify_2";
}
static void JavaNotify_3() {
  mw_one->mydlgTodo->on_Alarm();

  // qDebug() << "C++ JavaNotify_3";
}
static const JNINativeMethod gMethods[] = {
    {"CallJavaNotify_1", "()V", (void*)JavaNotify_1},
    {"CallJavaNotify_2", "()V", (void*)JavaNotify_2},
    {"CallJavaNotify_3", "()V", (void*)JavaNotify_3}};

void RegJni() {
  QtAndroid::runOnAndroidThreadSync([=]() {
    QAndroidJniEnvironment Environment;
    const char* mClassName = "com/x/MyService";
    jclass j_class;
    j_class = Environment->FindClass(mClassName);
    if (j_class == nullptr) {
      qDebug() << "erro clazz";
      return;
    }
    jint mj = Environment->RegisterNatives(
        j_class, gMethods, sizeof(gMethods) / sizeof(gMethods[0]));
    if (mj != JNI_OK) {
      qDebug() << "register native method failed!";
      return;
    } else {
      qDebug() << "RegisterNatives success!";
    }
  });
  qDebug() << "++++++++++++++++++++++++";
}

void RegJniMyActivity() {
  QtAndroid::runOnAndroidThreadSync([=]() {
    QAndroidJniEnvironment Environment;
    const char* mClassName = "com/x/MyActivity";
    jclass j_class;
    j_class = Environment->FindClass(mClassName);
    if (j_class == nullptr) {
      qDebug() << "erro clazz";
      return;
    }
    jint mj = Environment->RegisterNatives(
        j_class, gMethods, sizeof(gMethods) / sizeof(gMethods[0]));
    if (mj != JNI_OK) {
      qDebug() << "register native method failed!";
      return;
    } else {
      qDebug() << "RegisterNatives success!";
    }
  });
  qDebug() << "++++++++++++++++++++++++";
}
#endif

void MainWindow::on_btnPause_clicked() { mydlgSteps->ui->btnPause->click(); }

QString MainWindow::getYMD(QString date) {
  QStringList list = date.split(" ");
  QString str;
  if (list.count() == 4) {
    str = list.at(1) + list.at(2) + list.at(3);
  }
  return str;
}

void MainWindow::on_btnReader_clicked() {
  if (!isOne) {
    ui->quickWidget->setSource(QUrl(QStringLiteral("qrc:/text.qml")));
    mydlgReader->goPostion();
    isOne = true;
    mwh = this->height();
    setFixedHeight(mwh);
  }
  ui->frameMain->hide();
  ui->frameQML->show();
}

void MainWindow::setSCrollPro(QObject* obj) {
  QScrollerProperties sp;
  sp.setScrollMetric(QScrollerProperties::DragStartDistance, 0.001);
  sp.setScrollMetric(QScrollerProperties::ScrollingCurve, QEasingCurve::Linear);
  QScroller* qs = QScroller::scroller(obj);
  qs->setScrollerProperties(sp);
}

void MainWindow::on_btnBack_clicked() {
  ui->frameQML->hide();
  ui->frameMain->show();
  mydlgReader->saveReader();
}

void MainWindow::on_btnOpen_clicked() { mydlgReader->on_btnOpen_clicked(); }

void MainWindow::on_btnPageUp_clicked() { mydlgReader->on_btnPageUp_clicked(); }

void MainWindow::on_btnPageNext_clicked() {
  mydlgReader->on_btnPageNext_clicked();
}

void MainWindow::on_btnPage_clicked() { mydlgReader->on_btnPage_clicked(); }

void MainWindow::on_btnLines_clicked() { mydlgReader->on_btnLines_clicked(); }

void MainWindow::on_hSlider_sliderReleased() {
  mydlgReader->on_hSlider_sliderMoved(ui->hSlider->value());
}

void MainWindow::on_btnFontPlus_clicked() {
  textFontSize++;
  int FontSize = textFontSize;
  ui->quickWidget->rootContext()->setContextProperty("FontSize", FontSize);
}

void MainWindow::on_btnFontLess_clicked() {
  textFontSize--;
  int FontSize = textFontSize;
  ui->quickWidget->rootContext()->setContextProperty("FontSize", FontSize);
}

void MainWindow::on_hSlider_sliderMoved(int position) {
  ui->btnLines->setText(
      tr("Pages") + "\n" + QString::number(position + 1) + " / " +
      QString::number(mydlgReader->totallines / mydlgReader->baseLines));
}
