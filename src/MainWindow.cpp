#include "MainWindow.h"

#include "it/ltdev/qt/cpp/components/qtpdfviewerinitializer.h"
#include "src/onedrive/qtonedriveauthorizationdialog.h"
#include "ui_MainWindow.h"

QList<QPointF> PointList;
QList<double> doubleList;

QString ver = "1.1.21";
QGridLayout *gl1;
QTreeWidgetItem *parentItem;
bool isrbFreq = true;
bool isEBook, isReport, isUpData, isZipOK, isMenuImport, isTimeMachine,
    isDownData;

QString appName = "Knot";
QString iniFile, iniDir, privateDir, strDate, readDate, noteText, strStats,
    SaveType, strY, strM, btnYText, btnMText, btnDText, CurrentYearMonth,
    zipfile, txt, infoStr, searchStr;
QStringList listM;

int curPos, today, fontSize, red, currentTabIndex;
int chartMax = 5;
double yMaxMonth, yMaxDay;
MainWindow *mw_one;
QTabWidget *tabData, *tabChart;
bool loading, isReadEnd, isReadTWEnd;
bool isReadEBookEnd = true;
bool isSaveEnd = true;
bool isBreak = false;

QRegularExpression regxNumber("^-?\[0-9.]*$");

extern bool isAndroid, isIOS, zh_cn, isEpub, isText, isPDF, del, isWholeMonth,
    isDateSection;
extern QString btnYearText, btnMonthText, strPage, ebookFile, strTitle,
    fileName, strOpfPath, fontname;
extern int iPage, sPos, totallines, baseLines, htmlIndex, s_y1, s_m1, s_d1,
    s_y2, s_m2, s_d2;
extern QStringList readTextList, htmlFiles, listCategory;
extern void setTableNoItemFlags(QTableWidget *t, int row);
extern QtOneDriveAuthorizationDialog *dialog_;
extern dlgList *m_List;

void RegJni(const char *myClassName);

#ifdef Q_OS_ANDROID
static void JavaNotify_0();
static void JavaNotify_1();
static void JavaNotify_2();
static void JavaNotify_3();
static void JavaNotify_4();
#endif

BakDataThread::BakDataThread(QObject *parent) : QThread{parent} {}
void BakDataThread::run() {
  if (isUpData)
    mw_one->bakData(zipfile, false);
  else
    mw_one->bakData(zipfile, true);

  emit isDone();
}

void MainWindow::bakDataDone() {
  closeProgress();

  if (isUpData) {
    m_CloudBackup->uploadData();
  } else {
    if (QFile(infoStr).exists()) {
      QMessageBox msgBox;
      msgBox.setText(appName);
      msgBox.setInformativeText(tr("The data was exported successfully.") +
                                +"\n\n" + infoStr);
      QPushButton *btnOk = msgBox.addButton(tr("Ok"), QMessageBox::AcceptRole);
      btnOk->setFocus();

      msgBox.exec();

      mydlgPre->appendBakFile(
          QDateTime::currentDateTime().toString("yyyy-M-d HH:mm:ss") + "\n" +
              tr("Manual Backup") + "\n" + strLatestModify,
          infoStr);
    }
  }

  isUpData = false;
}

ImportDataThread::ImportDataThread(QObject *parent) : QThread{parent} {}
void ImportDataThread::run() {
  if (isMenuImport || isDownData)
    mw_one->importBakData(zipfile, true, false, false);

  if (isTimeMachine) mw_one->importBakData(zipfile, true, false, true);

  emit isDone();
}

void MainWindow::importDataDone() {
  if (!zipfile.isNull() && isZipOK) {
    m_NotesList->initNotesList();
    m_NotesList->initRecycle();
    m_Todo->init_Todo();

    loading = true;
    init_TotalData();
    loading = false;

    while (!isReadTWEnd)
      QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

    while (!isSaveEnd)
      QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

    on_tabWidget_currentChanged(tabData->currentIndex());
  }

  closeProgress();

  if (isTimeMachine) {
  }

  if (isMenuImport) {
    if (!isZipOK) {
      QMessageBox msgBox;
      msgBox.setText(appName);
      msgBox.setInformativeText(tr("Invalid data file."));

      QPushButton *btnOk = msgBox.addButton(tr("Ok"), QMessageBox::AcceptRole);
      btnOk->setFocus();

      msgBox.exec();
    }
  }
}

SearchThread::SearchThread(QObject *parent) : QThread{parent} {}
void SearchThread::run() {
  mw_one->m_Method->startSearch();

  emit isDone();
}

void MainWindow::searchDone() {
  mw_one->m_Method->initSearchResults();
  closeProgress();
}

ReadEBookThread::ReadEBookThread(QObject *parent) : QThread{parent} {}
void ReadEBookThread::run() {
  isReadEBookEnd = false;

  if (isEBook) {
    mw_one->m_Reader->openFile(ebookFile);
    // for (int i = 0; i < htmlFiles.count(); i++)
    //   mw_one->m_Reader->processHtml(i);
  }

  if (isReport) {
    mw_one->m_Report->getMonthData();
  }

  emit isDone();
}

void MainWindow::readEBookDone() {
  if (isEBook) {
    ui->lblBookName->setText(strTitle);

    ui->btnReader->setEnabled(true);
    ui->frameReaderFun->setEnabled(true);
    ui->btnBackDir->setEnabled(false);
    this->repaint();

    if (isText || isEpub) {
      qDebug() << "Read  Text or Epub End... ...";

      ui->qwPdf->hide();
      ui->qwReader->show();
      ui->frameReaderFun->show();
      ui->lblBookName->show();
      ui->progReader->show();
      mw_one->ui->qwReader->rootContext()->setContextProperty("isWebViewShow",
                                                              false);
      mw_one->ui->qwReader->rootContext()->setContextProperty("strText", "");
      mw_one->ui->qwReader->rootContext()->setContextProperty(
          "isSelText", mw_one->isSelText);
      mw_one->ui->qwReader->rootContext()->setContextProperty("isAni", true);
      mw_one->ui->qwReader->rootContext()->setContextProperty("aniW",
                                                              mw_one->width());
      mw_one->ui->qwReader->rootContext()->setContextProperty("toW", 0);
      mw_one->ui->qwReader->setSource(
          QUrl(QStringLiteral("qrc:/src/qmlsrc/reader.qml")));

      if (isEpub)
        ui->qwReader->rootContext()->setContextProperty("htmlPath", strOpfPath);
    }

    if (isPDF) {
      qDebug() << "Read Pdf... ..." << fileName;
      ui->lblBookName->hide();
      ui->progReader->hide();
      ui->qwReader->hide();
      ui->frameReaderFun->hide();
      ui->qwPdf->show();

      QString PDFJS, str;

// https://mozilla.github.io/pdf.js/web/viewer.html?file=compressed.tracemonkey-pldi-09.pdf
// "https://mozilla.github.io/pdf.js/web/viewer.html";
#ifdef Q_OS_ANDROID
      PDFJS = "file:///android_asset/pdfjs/web/viewer.html?file=";
      PDFJS = "https://mozilla.github.io/pdf.js/web/viewer.html";
      str =
          "https://mozilla.github.io/pdf.js/web/"
          "viewer.html?file=compressed.tracemonkey-pldi-09.pdf";
      str = PDFJS + "?file=" + fileName;
      if (QFile("assets:/web/viewer.html").exists())
        qDebug() << "viewer.html exists......";
#else
      PDFJS = "file://" + privateDir + "pdfjs/web/viewer.html";
      str = PDFJS + "?file=file://" + fileName;
#endif
      QUrl url;
      url.setUrl(str);

      if (isPdfNewMothod) {
        QQuickItem *root = ui->qwPdf->rootObject();
        QMetaObject::invokeMethod((QObject *)root, "setPdfPath",
                                  Q_ARG(QVariant, url));

      } else {
        QQuickItem *root = ui->qwPdf->rootObject();
#ifdef Q_OS_WIN
        QMetaObject::invokeMethod((QObject *)root, "setViewEnd",
                                  Q_ARG(QVariant, true));
#endif

        // QMetaObject::invokeMethod((QObject *)root, "loadPDFWin",
        //                          Q_ARG(QVariant, PDFJS + "?file="),
        //                          Q_ARG(QVariant, fileName));

        QMetaObject::invokeMethod((QObject *)root, "loadPDF",
                                  Q_ARG(QVariant, fileName));
      }
    }

    m_Reader->goPostion();

    for (int i = 0; i < m_Reader->bookList.count(); i++) {
      QString str = m_Reader->bookList.at(i);
      if (str.contains(fileName)) {
        m_Reader->bookList.removeAt(i);
        break;
      }
    }

    m_Reader->bookList.insert(0, strTitle + "|" + fileName);

    isEBook = false;
  }

  if (isReport) {
    m_Report->updateTable();
    ui->lblTitle->setText(tabData->tabText(tabData->currentIndex()));

    ui->btnCategory->hide();
    if (listCategory.count() > 0) ui->btnCategory->setHidden(false);

    isReport = false;
  }

  closeProgress();

  isReadEBookEnd = true;
}

ReadTWThread::ReadTWThread(QObject *parent) : QThread{parent} {}
void ReadTWThread::run() {
  isReadTWEnd = false;
  MainWindow::readDataInThread(currentTabIndex);
  emit isDone();
}

void MainWindow::readTWDone() {
  for (int i = 0; i < tabData->tabBar()->count(); i++) {
    QTreeWidget *tw = (QTreeWidget *)tabData->widget(i);
    tw->setCurrentItem(tw->topLevelItem(tw->topLevelItemCount() - 1));
  }

  ui->actionImport_Data->setEnabled(true);
  ui->actionExport_Data->setEnabled(true);
  ui->actionDel_Tab->setEnabled(true);
  ui->actionAdd_Tab->setEnabled(true);
  ui->actionView_App_Data->setEnabled(true);
  isReadTWEnd = true;

  ui->progBar->setMaximum(100);
}

ReadThread::ReadThread(QObject *parent) : QThread{parent} {}

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
  QTreeWidget *tw = (QTreeWidget *)tabData->widget(index);

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

void MainWindow::readChartDone() {
  if (tabChart->currentIndex() == 0) {
    initChartMonth();
  }
  if (tabChart->currentIndex() == 1) {
    initChartDay();
  }

  if (isShowDetails)
    ui->lblStats->setText(strShowDetails);
  else
    ui->lblStats->setText(strStats);
  isReadEnd = true;
}

SaveThread::SaveThread(QObject *parent) : QThread{parent} {}
void SaveThread::run() {
  isSaveEnd = false;
  MainWindow::SaveFile(SaveType);
  emit isDone();
}

void MainWindow::saveDone() {
  if (isBreak) {
    isSaveEnd = true;
    return;
  }

  addRedo();

  isSaveEnd = true;

  ui->progBar->setMaximum(100);

  if (SaveType == "tab" || SaveType == "alltab") startRead(strDate);
}

void MainWindow::SaveFile(QString SaveType) {
  if (SaveType == "tab") {
    EditRecord::saveOne();
    saveTab();
    if (!del) EditRecord::saveCustomDesc();
  }

  if (SaveType == "alltab") {
    for (int i = 0; i < tabData->tabBar()->count(); i++) {
      if (isBreak) break;

      QTreeWidget *tw = (QTreeWidget *)tabData->widget(i);
      QString name = tw->objectName();
      QString ini_file = iniDir + name + ".ini";
      if (QFile(ini_file).exists()) QFile(ini_file).remove();

      saveData(tw, i);
      saveNotes(i);
    }

    saveTab();
    EditRecord::saveCustomDesc();
  }

  if (SaveType == "todo") {
  }

  if (SaveType == "notes") {
    saveNotes(tabData->currentIndex());
  }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  initMain = true;

  if (!isAndroid) {
#ifdef Q_OS_MAC
    this->setGeometry(800, 0, 390, this->height() - 60);
#endif

#ifdef Q_OS_WIN
    this->setGeometry(730, 25, 450, 620);
#endif
  }

  qRegisterMetaType<QVector<int>>("QVector<int>");
  loading = true;
  init_UIWidget();
  init_ChartWidget();
  init_Options();
  init_Sensors();
  init_TotalData();
  m_Reader->initReader();
  loading = false;

  QTreeWidget *tw = (QTreeWidget *)tabData->currentWidget();
  startRead(strDate);
  get_Today(tw);
  init_Stats(tw);

  initHardStepSensor();

  reloadMain();
  clickData();
  resetWinPos();
  initMain = false;
  addFilesWatch();
  m_Reader->setPdfViewVisible(false);

  m_Todo->refreshTableListsFromIni();
  m_Todo->refreshAlarm();

  // load note
  m_NotesList->currentMDFile = m_NotesList->getCurrentMDFile();
  mw_one->m_Notes->MD2Html(m_NotesList->currentMDFile);
  mw_one->m_Notes->loadMemoQML();
}

void MainWindow::initHardStepSensor() {
#ifdef Q_OS_ANDROID

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  QAndroidJniObject jo = QAndroidJniObject::fromString("CheckSensorWin");
  isHardStepSensor =
      jo.callStaticMethod<int>("com.x/MyActivity", "getHardStepCounter", "()I");
#else
  QJniObject jo = QJniObject::fromString("CheckSensorWin");
  isHardStepSensor =
      jo.callStaticMethod<int>("com.x/MyActivity", "getHardStepCounter", "()I");
#endif

  if (isHardStepSensor == 0) {
    ui->btnPause->click();
    ui->btnPause->setHidden(true);
    ui->btnSteps->setHidden(true);
    if (ui->rbAlg1->isChecked()) m_Steps->on_rbAlg1_clicked();
  }
  if (isHardStepSensor == 1) {
    ui->btnPause->click();
    ui->gboxAlg->hide();
    ui->lblSteps->hide();
    ui->btnPauseSteps->hide();
    ui->lblTotalRunTime->hide();
    mydlgPre->ui->chkDebug->setChecked(false);
    mydlgPre->on_chkDebug_clicked();
    mydlgPre->ui->chkDebug->hide();
    ui->btnPause->hide();
    initTodayInitSteps();
    resetSteps = tc;
  }
#endif
}

void MainWindow::initTodayInitSteps() {
  qlonglong a = 0;
  qlonglong b = 0;

#ifdef Q_OS_ANDROID

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  QAndroidJniObject jo = QAndroidJniObject::fromString("getSteps");
  a = jo.callStaticMethod<float>("com.x/MyActivity", "getSteps", "()F");
#else
  QJniObject jo = QJniObject::fromString("getSteps");
  a = jo.callStaticMethod<float>("com.x/MyActivity", "getSteps", "()F");
#endif

#endif

  tc = a;

  QSettings Reg(iniDir + "initsteps.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif
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
}

void MainWindow::newDatas() {
  ax = ay = az = gx = gy = gz = 0;
  az = accel_pedometer->reading()->z();

  updateRunTime();

  countOne++;
  if (ui->rbAlg1->isChecked()) {
    if (countOne >= 2) {
      aoldZ = az;
      countOne = 0;
    }
  }

  if (qAbs(qAbs(az) - qAbs(aoldZ)) < 0.5) {
    return;
  }

  if (ui->rbAlg1->isChecked()) {
    accel_pedometer->runStepCountAlgorithm();
    timeCount++;
    ui->lblSteps->setText(tr("Number of Operations") + " : " +
                          QString::number(timeCount));
  }

  showSensorValues();
}

void MainWindow::showSensorValues() {
  if (mydlgPre->ui->chkDebug->isChecked()) {
    ui->lblX->setText("AX:" + QString::number(ax) + "\n" +
                      "GX:" + QString::number(gx));
    ui->lblY->setText("AY:" + QString::number(ay) + "\n" +
                      "GY:" + QString::number(gy));
    ui->lblZ->setText("AZ:" + QString::number(az) + "\n" +
                      "GZ:" + QString::number(gz));

    if (ui->lblX->isHidden()) {
      ui->lblX->show();
      ui->lblY->show();
      ui->lblZ->show();
    }
  } else {
    if (!ui->lblX->isHidden()) {
      ui->lblX->hide();
      ui->lblY->hide();
      ui->lblZ->hide();
    }
  }
}

void MainWindow::updateRunTime() {
  smallCount++;
  if (ui->rbAlg1->isChecked()) {
    if (smallCount >= 5) {
      timeTest++;
      smallCount = 0;
      pausePedometer();
    }
  }

  ui->lblTotalRunTime->setText(tr("Total Working Hours") + " : " +
                               secondsToTime(timeTest));
}

void MainWindow::pausePedometer() {
  if (QTime::currentTime().toString("hh-mm-ss") == "22-00-00") {
    if (ui->btnPauseSteps->text() == tr("Pause")) ui->btnPauseSteps->click();
  }
}

void MainWindow::updateSteps() {
  // CurrentSteps = accel_pedometer->stepCount();
  if (ui->rbAlg1->isChecked()) {
    CurrentSteps++;
    CurTableCount = m_Steps->getCurrentSteps();
    CurTableCount++;
    m_Steps->toDayInitSteps++;
  }

  ui->lcdNumber->display(QString::number(CurTableCount));
  ui->lblSingle->setText(QString::number(CurrentSteps));
  m_Steps->setTableSteps(CurTableCount);

  if (CurrentSteps == 0) return;
  sendMsg(CurTableCount);
}

void MainWindow::sendMsg(int CurTableCount) {
  Q_UNUSED(CurTableCount);
#ifdef Q_OS_ANDROID
  double sl = ui->editStepLength->text().toDouble();
  double d0 = sl / 100;
  double x = CurTableCount * d0;
  double gl = x / 1000;
  QString strNotify = tr("Today") + " : " + QString::number(CurTableCount) +
                      "  ( " + QString::number(gl) + " " + tr("km") + " )";

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  QAndroidJniObject javaNotification = QAndroidJniObject::fromString(strNotify);
  QAndroidJniObject::callStaticMethod<void>(
      "com/x/MyService", "notify",
      "(Landroid/content/Context;Ljava/lang/String;)V",
      QtAndroid::androidContext().object(), javaNotification.object<jstring>());
#else
  QJniObject javaNotification = QJniObject::fromString(strNotify);
  QJniObject::callStaticMethod<void>(
      "com/x/MyService", "notify",
      "(Landroid/content/Context;Ljava/lang/String;)V",
      QNativeInterface::QAndroidApplication::context(),
      javaNotification.object<jstring>());
#endif

#endif
}

void MainWindow::init_Options() {
  QSettings Reg(privateDir + "options.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif

#ifdef Q_OS_UNIX
  Reg.setValue("/Options/macIniDir", iniDir);
#endif

#ifdef Q_OS_ANDROID
  Reg.setValue("/Options/androidIniDir", iniDir);
#endif
  androidIniDir = Reg.value("/Options/androidIniDir", "").toString();
  macIniDir = Reg.value("/Options/macIniDir", "").toString();

  QSettings Reg2(iniDir + "ymd.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg2.setIniCodec("utf-8");
#endif
  btnYText = Reg2.value("/YMD/btnYText", 2022).toString();

  btnMText = Reg2.value("/YMD/btnMText", tr("Month")).toString();

  btnDText = Reg2.value("/YMD/btnDText", 1).toString();

  btnYearText = Reg2.value("/YMD/btnYearText", "2022").toString();
  ui->btnYear->setText(btnYearText);
  btnMonthText = Reg2.value("/YMD/btnMonthText", "01").toString();
  ui->btnMonth->setText(btnMonthText);

  s_y1 = Reg2.value("/YMD/Y1", 2022).toInt();
  s_y2 = Reg2.value("/YMD/Y2", 2022).toInt();
  s_m1 = Reg2.value("/YMD/M1", 1).toInt();
  s_m2 = Reg2.value("/YMD/M2", 12).toInt();
  s_d1 = Reg2.value("/YMD/D1", 1).toInt();
  s_d2 = Reg2.value("/YMD/D2", 1).toInt();

  ui->btnStartDate->setText(QString::number(s_y1) + "  " +
                            QString("%1").arg(s_m1, 2, 10, QLatin1Char('0')) +
                            "  " +
                            QString("%1").arg(s_d1, 2, 10, QLatin1Char('0')));
  ui->btnEndDate->setText(QString::number(s_y2) + "  " +
                          QString("%1").arg(s_m2, 2, 10, QLatin1Char('0')) +
                          "  " +
                          QString("%1").arg(s_d2, 2, 10, QLatin1Char('0')));

  isWholeMonth = Reg2.value("/YMD/isWholeMonth", 1).toBool();
  isDateSection = Reg2.value("/YMD/isDateSection", 0).toBool();

  // time machine
  QSettings RegTime(privateDir + "timemachine.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  RegTime.setIniCodec("utf-8");
#endif
  int countTime = RegTime.value("/TimeLines/Count", 0).toInt();
  for (int i = 0; i < countTime; i++)
    timeLines.append(
        RegTime.value("/TimeLines/Files" + QString::number(i)).toString());

  mydlgPre->initValues();
  mydlgPre->ui->btnReStart->hide();
  QString style =
      "QToolButton {background-color: rgb(255, 0, 0); color: rgb(255,255,255); "
      "border-radius:10px; "
      "border:0px solid gray; } QToolButton:pressed { background-color: "
      "rgb(220,220,230);}";
  mydlgPre->ui->btnReStart->setStyleSheet(style);
}

void MainWindow::init_ChartWidget() {
  CurrentYear = QString::number(QDate::currentDate().year());
  ui->tabCharts->setCornerWidget(ui->frame_cw);
  ui->glMonth->layout()->setContentsMargins(0, 0, 0, 0);
  ui->glMonth->layout()->setSpacing(0);
  ui->glDay->layout()->setContentsMargins(0, 0, 0, 0);
  ui->glDay->layout()->setSpacing(0);

  int a0 = 0;
  int a1 = -6;
  // Month
  chartMonth = new QChart();
  chartview = new QChartView(chartMonth);
  chartview->installEventFilter(this);
  ui->glMonth->addWidget(chartview);
  chartview->setRenderHint(QPainter::Antialiasing);
  chartMonth->legend()->hide();
  chartMonth->setMargins(QMargins(a0, a0, a0, a0));
  chartMonth->setContentsMargins(a1, a1, a1, a1);
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
  chartDay->setMargins(QMargins(a0, a0, a0, a0));
  chartDay->setContentsMargins(a1, a1, a1, a1);
  chartDay->setAnimationOptions(QChart::SeriesAnimations);

  series2 = new QSplineSeries(chartDay);
  series2->setPen(QPen(Qt::blue, 2, Qt::SolidLine));
  m_scatterSeries2 = new QScatterSeries();
  m_scatterSeries2_1 = new QScatterSeries();

  // 散点图(用于边框)
  m_scatterSeries2->setMarkerShape(
      QScatterSeries::MarkerShapeCircle);                 // 圆形的点
  m_scatterSeries2->setBorderColor(QColor(255, 0, 0));    // 边框颜色
  m_scatterSeries2->setBrush(QBrush(QColor(255, 0, 0)));  // 背景颜色
  m_scatterSeries2->setMarkerSize(5);                     // 点大小

  // 散点图(用于中心)
  m_scatterSeries2_1->setMarkerShape(
      QScatterSeries::MarkerShapeCircle);         // 圆形的点
  m_scatterSeries2_1->setBorderColor(Qt::red);    // 边框颜色
  m_scatterSeries2_1->setBrush(QBrush(Qt::red));  // 背景颜色
  m_scatterSeries2_1->setMarkerSize(4);           // 点大小
  connect(m_scatterSeries2_1, &QScatterSeries::hovered, this,
          &MainWindow::slotPointHoverd);  // 用于鼠标移动到点上显示数值
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

  QFont font1;
  font1.setPointSize(12);
  font1.setBold(true);
  chartMonth->setTitleFont(font1);
  chartDay->setTitleFont(font1);
  axisX->setLabelsFont(font1);
  axisY->setLabelsFont(font1);
  axisY->setTickCount(yScale);
  axisX2->setLabelsFont(font1);
  axisY2->setLabelsFont(font1);
  axisY2->setTickCount(yScale);

  // Get the background color to fit the dark mode
  QPalette pal = this->palette();
  QBrush brush = pal.window();
  red = brush.color().red();
  if (red < 55) {
    chartMonth->setTheme(QChart::ChartThemeDark);
    chartDay->setTheme(QChart::ChartThemeDark);
  } else {
    chartMonth->setTheme(QChart::ChartThemeLight);
    chartDay->setTheme(QChart::ChartThemeLight);
  }
}

void MainWindow::slotPointHoverd(const QPointF &point, bool state) {
  if (state) {
    m_valueLabel->setText(QString::asprintf("%1.0f", point.y()));

    QPoint curPos = mapFromGlobal(QCursor::pos());
    m_valueLabel->move(curPos.x() - m_valueLabel->width() / 2,
                       curPos.y() - m_valueLabel->height() * 1.5);  // 移动数值

    m_valueLabel->show();
  } else
    m_valueLabel->hide();
}

void MainWindow::init_TotalData() {
  ui->progBar->setHidden(false);
  ui->progBar->setMaximum(0);

  int count = ui->tabWidget->tabBar()->count();
  for (int i = 0; i < count; i++) {
    ui->tabWidget->removeTab(0);
  }
  QString ini_file, ini_file1;

  ini_file = iniDir + "tab.ini";
  QSettings RegTab(ini_file, QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  RegTab.setIniCodec("utf-8");
#endif

  ini_file1 = iniDir + "notes.ini";
  QSettings RegNotes(ini_file1, QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  RegNotes.setIniCodec("utf-8");
#endif
  int TabCount = RegTab.value("TabCount", 0).toInt();

  for (int i = 0; i < TabCount; i++) {
    QString name;
    name = RegTab.value("twName" + QString::number(i)).toString();
    if (name.trimmed().length() == 0) name = "tab" + QString::number(i + 1);
    QTreeWidget *tw = init_TreeWidget(name);

    ui->tabWidget->addTab(tw, RegTab
                                  .value("TabName" + QString::number(i),
                                         tr("Tab") + QString::number(i + 1))
                                  .toString());
    RegTab.setValue("twName" + QString::number(i), name);

    QString strNotes = RegNotes.value("/" + name + "/Note").toString();
    ui->tabWidget->setTabToolTip(i, strNotes);
  }

  if (TabCount == 0) {
    QTreeWidget *tw = init_TreeWidget("20220303_101010_1");

    ui->tabWidget->addTab(tw, tr("Tab") + " " + QString::number(1));

    ui->tabWidget->setTabToolTip(0, "");
  }

  myEditRecord->init_Desc();

  m_Steps->init_Steps();
  m_Steps->saveSteps();

  currentTabIndex = RegTab.value("CurrentIndex").toInt();
  ui->tabWidget->setCurrentIndex(currentTabIndex);
  QTreeWidget *twCur = (QTreeWidget *)tabData->currentWidget();
  readData(twCur);
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
      QTreeWidget *tw = (QTreeWidget *)tabData->widget(i);
      readData(tw);
    }
  }
}

void MainWindow::timerUpdate() {
  ui->lblTotalRunTime->setText(tr("Total Working Hours") + " : " +
                               secondsToTime(timeTest++));
  if (QTime::currentTime().toString("hh-mm-ss") == "00-30-00") {
    mydlgPre->isFontChange = true;
    this->close();
  }
}

void MainWindow::on_timerSyncData() {
  qDebug() << ".......Sync.......";
  timerSyncData->stop();
  on_btnSync_clicked();
}

void MainWindow::startSyncData() {
  if (initMain) return;
  timerSyncData->start(10000);
}

void MainWindow::removeFilesWatch() {
#ifdef Q_OS_ANDROID
#else
  FileSystemWatcher::removeWatchPath(iniDir + "todo.ini");
  FileSystemWatcher::removeWatchPath(iniDir + "mainnotes.ini");
  qDebug() << QTime::currentTime().toString() << "remove file watch......";
#endif
}

void MainWindow::addFilesWatch() {
#ifdef Q_OS_ANDROID
#else
  FileSystemWatcher::addWatchPath(iniDir + "todo.ini");
  FileSystemWatcher::addWatchPath(iniDir + "mainnotes.ini");

  qDebug() << QTime::currentTime().toString()
           << "add file watch...... isSelf=" << isSelf;
#endif
}

MainWindow::~MainWindow() {
  delete ui;
  mySaveThread->quit();
  mySaveThread->wait();

  myReadThread->quit();
  myReadThread->wait();

  myReadTWThread->quit();
  myReadTWThread->wait();

  myReadEBookThread->quit();
  myReadEBookThread->wait();
}

void MainWindow::startSave(QString str_type) {
  if (!isSaveEnd) {
    isBreak = true;
    mySaveThread->quit();
    mySaveThread->wait();

    while (!isSaveEnd)
      QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
  }
  if (isSaveEnd) {
    isBreak = false;
    SaveType = str_type;

    ui->progBar->setHidden(false);
    ui->progBar->setMaximum(0);

    mySaveThread->start();
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

void MainWindow::add_Data(QTreeWidget *tw, QString strTime, QString strAmount,
                          QString strDesc) {
  bool isYes = false;

  strDate = QDate::currentDate().toString("ddd MM dd yyyy");

  int topc = tw->topLevelItemCount();
  for (int i = 0; i < topc; i++) {
    QString str = tw->topLevelItem(topc - 1 - i)->text(0) + " " +
                  tw->topLevelItem(topc - 1 - i)->text(3);
    if (getYMD(str) == getYMD(strDate)) {
      isYes = true;

      QTreeWidgetItem *topItem = tw->topLevelItem(topc - 1 - i);
      QTreeWidgetItem *item11 = new QTreeWidgetItem(topItem);
      item11->setText(0, strTime);
      if (strAmount == "")
        item11->setText(1, "");
      else
        item11->setText(1, QString("%1").arg(strAmount.toDouble(), 0, 'f', 2));

      item11->setText(2, strDesc);
      item11->setText(3,
                      myEditRecord->ui->editDetails->toPlainText().trimmed());

      int childCount = topItem->childCount();

      topItem->setTextAlignment(1, Qt::AlignHCenter | Qt::AlignVCenter);
      topItem->setTextAlignment(2, Qt::AlignRight | Qt::AlignVCenter);
      item11->setTextAlignment(1, Qt::AlignRight | Qt::AlignVCenter);

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
    } else
      break;
  }

  if (!isYes) {
    QTreeWidgetItem *topItem = new QTreeWidgetItem;

    QStringList lista = strDate.split(" ");
    if (lista.count() == 4) {
      QString a = lista.at(0) + " " + lista.at(1) + " " + lista.at(2);
      topItem->setText(0, a);
      topItem->setText(3, lista.at(3));
    }

    tw->addTopLevelItem(topItem);
    QTreeWidgetItem *item11 = new QTreeWidgetItem(topItem);
    item11->setText(0, strTime);
    if (strAmount == "")
      item11->setText(1, "");
    else
      item11->setText(1, QString("%1").arg(strAmount.toDouble(), 0, 'f', 2));
    item11->setText(2, strDesc);
    item11->setText(3, myEditRecord->ui->editDetails->toPlainText().trimmed());

    topItem->setTextAlignment(1, Qt::AlignHCenter | Qt::AlignVCenter);
    topItem->setTextAlignment(2, Qt::AlignRight | Qt::AlignVCenter);
    item11->setTextAlignment(1, Qt::AlignRight | Qt::AlignVCenter);

    //  Amount
    int child = topItem->childCount();
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

  int topCount = tw->topLevelItemCount();
  QTreeWidgetItem *topItem = tw->topLevelItem(topCount - 1);
  tw->setCurrentItem(topItem);
  sort_childItem(topItem->child(0));
  tw->setCurrentItem(topItem->child(topItem->childCount() - 1));

  reloadMain();
}

void MainWindow::showDelMsgBox(QString title, QString info) {
  m_widget = new QWidget(this);

  QDialog *dlg = new QDialog(this);
  QVBoxLayout *vbox0 = new QVBoxLayout;
  dlg->setLayout(vbox0);
  dlg->setModal(true);
  dlg->setWindowFlag(Qt::FramelessWindowHint);
  dlg->setAttribute(Qt::WA_TranslucentBackground);

  QFrame *frame = new QFrame(this);
  vbox0->addWidget(frame);
  frame->setStyleSheet(
      "QFrame{background-color: rgb(255, 255, 255);border-radius:10px; "
      "border:0px solid gray;}");

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setContentsMargins(12, 12, 12, 12);
  vbox->setSpacing(12);
  frame->setLayout(vbox);

  QLabel *lblTitle = new QLabel(this);
  lblTitle->adjustSize();
  lblTitle->setWordWrap(true);
  lblTitle->setText(title);
  vbox->addWidget(lblTitle);

  QFrame *hframe = new QFrame(this);
  hframe->setFrameShape(QFrame::HLine);
  hframe->setStyleSheet("QFrame{background:red;min-height:2px}");
  vbox->addWidget(hframe);

  QLabel *lbl = new QLabel(this);
  lbl->adjustSize();
  lbl->setWordWrap(true);
  lbl->setText(info);
  vbox->addWidget(lbl);

  QToolButton *btnCancel = new QToolButton(this);
  QToolButton *btnOk = new QToolButton(this);
  btnCancel->setText(tr("Cancel"));
  btnOk->setText(tr("Delete"));
  btnOk->setStyleSheet(
      "QToolButton {background-color: rgb(255, 0, 0);color: rgb(255, "
      "255, 255);border-radius:10px;border:0px solid gray;} "
      "QToolButton:pressed "
      "{ background-color: "
      "rgb(220,220,230);color: black}");

  btnCancel->setStyleSheet(btnStyle);
  btnOk->setFixedHeight(35);
  btnCancel->setFixedHeight(35);

  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->addWidget(btnCancel);
  hbox->addWidget(btnOk);
  btnCancel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  btnOk->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

  QSpacerItem *sparcer_item =
      new QSpacerItem(0, 160, QSizePolicy::Fixed, QSizePolicy::Expanding);
  vbox->addItem(sparcer_item);

  vbox->addLayout(hbox, 0);

  isOK = false;
  connect(btnCancel, &QToolButton::clicked, [=]() mutable {
    isOK = false;
    dlg->close();
  });
  connect(dlg, &QDialog::rejected, [=]() mutable { closeGrayWindows(); });
  connect(btnOk, &QToolButton::clicked, [=]() mutable {
    isOK = true;
    dlg->close();
  });

  int x, y, w, h;
  w = mw_one->width() - 40;
  x = mw_one->geometry().x() + (mw_one->width() - w) / 2;
  h = calcStringPixelHeight(this->font(), fontSize) * 15;
  y = geometry().y() + (height() - h) / 2;
  dlg->setGeometry(x, y, w, h);

  showGrayWindows();

  dlg->exec();
}

int MainWindow::calcStringPixelWidth(QString s_str, QFont font,
                                     int n_font_size) {
  font.setPointSize(n_font_size);
  QFontMetrics fm(font);
  return fm.horizontalAdvance(s_str);
}

int MainWindow::calcStringPixelHeight(QFont font, int n_font_size) {
  font.setPointSize(n_font_size);
  QFontMetrics fm(font);
  return fm.height();
}

void MainWindow::del_Data(QTreeWidget *tw) {
  if (tw->topLevelItemCount() == 0) return;

  bool isNo = true;
  strDate = QDate::currentDate().toString("ddd MM dd yyyy");
  for (int i = 0; i < tw->topLevelItemCount(); i++) {
    QString str =
        tw->topLevelItem(i)->text(0) + " " + tw->topLevelItem(i)->text(3);
    if (getYMD(str) == getYMD(strDate)) {
      isNo = false;
      QTreeWidgetItem *topItem = tw->topLevelItem(i);
      int childCount = topItem->childCount();
      if (childCount > 0) {
        QString str = ui->tabWidget->tabText(ui->tabWidget->currentIndex());
        QString str1 =
            tr("Time") + " : " + topItem->child(childCount - 1)->text(0) +
            "\n" + tr("Amount") + " : " +
            topItem->child(childCount - 1)->text(1) + "\n" + tr("Category") +
            " : " + topItem->child(childCount - 1)->text(2) + "\n";

        showDelMsgBox(
            str + " : ",
            tr("The last record added today will be deleted!") + "\n\n" + str1);
        if (!isOK) return;

        isNeedAutoBackup = true;
        strLatestModify = tr("Del Item") + " ( " + getTabText() + " ) ";

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

        if (topItem->childCount() == 0)
          tw->takeTopLevelItem(tw->topLevelItemCount() - 1);

        reloadMain();

        break;
      }
    }
  }

  if (isNo) {
    QString str = ui->tabWidget->tabText(ui->tabWidget->currentIndex());
    showMsgBox(str, tr("Only the reduction of the day's records is allowed."),
               "", 1);
    return;
  }

  int topCount = tw->topLevelItemCount();
  if (topCount > 0) {
    QTreeWidgetItem *topItem = tw->topLevelItem(topCount - 1);
    tw->setCurrentItem(topItem);
  }

  del = true;
  startSave("tab");
}

void MainWindow::on_AddRecord() {
  isAdd = true;

  myEditRecord->ui->lblTitle->setText(
      tr("Add") + "  : " + tabData->tabText(tabData->currentIndex()));
  myEditRecord->ui->editDetails->clear();

  myEditRecord->ui->hsH->setValue(QTime::currentTime().hour());
  myEditRecord->ui->hsM->setValue(QTime::currentTime().minute());
  myEditRecord->getTime(myEditRecord->ui->hsH->value(),
                        myEditRecord->ui->hsM->value());

  myEditRecord->init();
}

void MainWindow::on_DelRecord() {
  del_Data((QTreeWidget *)ui->tabWidget->currentWidget());
}

void MainWindow::set_btnStyle(QObject *parent) {
  QObjectList btnList = getAllToolButton(getAllUIControls(parent));
  for (int i = 0; i < btnList.count(); i++) {
    QToolButton *btn = (QToolButton *)btnList.at(i);

    if (btn != ui->btnBackNotes && btn != ui->btnEdit &&
        btn != ui->btnNotesList && btn != ui->btnSetKey && btn != ui->btnPDF)
      setPushButtonQss(btn, 5, 3, "#3498DB", "#FFFFFF", "#3498DB", "#FFFFFF",
                       "#2483C7", "#A0DAFB");  // #5DACE4 #E5FEFF
  }
}

QObjectList MainWindow::getAllTreeWidget(QObjectList lstUIControls) {
  QObjectList lst;
  foreach (QObject *obj, lstUIControls) {
    if (obj->metaObject()->className() == QStringLiteral("QTreeWidget")) {
      lst.append(obj);
    }
  }
  return lst;
}

QObjectList MainWindow::getAllToolButton(QObjectList lstUIControls) {
  QObjectList lst;
  foreach (QObject *obj, lstUIControls) {
    if (obj->metaObject()->className() == QStringLiteral("QToolButton")) {
      lst.append(obj);
    }
  }
  return lst;
}

QObjectList MainWindow::getAllUIControls(QObject *parent) {
  QObjectList lstOfChildren, lstTemp;
  if (parent) {
    lstOfChildren = parent->children();
  }
  if (lstOfChildren.isEmpty()) {
    return lstOfChildren;
  }

  lstTemp = lstOfChildren;

  foreach (QObject *obj, lstTemp) {
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
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif
  int TabCount = tabData->tabBar()->count();
  Reg.setValue("TabCount", TabCount);
  int CurrentIndex = tabData->currentIndex();
  Reg.setValue("CurrentIndex", CurrentIndex);
  for (int i = 0; i < TabCount; i++) {
    if (isBreak) break;
    Reg.setValue("TabName" + QString::number(i), tabData->tabText(i));

    QTreeWidget *tw = (QTreeWidget *)tabData->widget(i);
    Reg.setValue("twName" + QString::number(i), tw->objectName());
  }
}

QString MainWindow::getFileSize(const qint64 &size, int precision) {
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

void MainWindow::saveData(QTreeWidget *tw, int tabIndex) {
  Q_UNUSED(tabIndex);

  QString name = tw->objectName();
  QString ini_file = iniDir + name + ".ini";
  QSettings Reg(ini_file, QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif

  if (!QFile::exists(ini_file)) {
    Reg.setValue("/" + name + "/" + "CreatedTime",
                 QDateTime::currentDateTime().toString());
  }

  int count = tw->topLevelItemCount();
  int abc = count;

  QString flag;
  QString group = Reg.childGroups().at(0);
  if (group.trimmed().length() == 0)
    flag = "/" + name + "/";
  else
    flag = "/" + group + "/";

  for (int i = 0; i < count; i++) {
    if (isBreak) break;
    int childCount = tw->topLevelItem(i)->childCount();

    if (childCount > 0) {
      Reg.setValue(flag + QString::number(i + 1) + "-topDate",
                   tw->topLevelItem(i)->text(0));
      Reg.setValue(flag + QString::number(i + 1) + "-topYear",
                   tw->topLevelItem(i)->text(3));
      Reg.setValue(flag + QString::number(i + 1) + "-topFreq",
                   tw->topLevelItem(i)->text(1));
      Reg.setValue(flag + QString::number(i + 1) + "-topAmount",
                   tw->topLevelItem(i)->text(2));

      Reg.setValue(flag + QString::number(i + 1) + "-childCount", childCount);
      for (int j = 0; j < childCount; j++) {
        if (isBreak) return;
        Reg.setValue(
            flag + QString::number(i + 1) + "-childTime" + QString::number(j),
            tw->topLevelItem(i)->child(j)->text(0));
        Reg.setValue(
            flag + QString::number(i + 1) + "-childAmount" + QString::number(j),
            tw->topLevelItem(i)->child(j)->text(1));
        Reg.setValue(
            flag + QString::number(i + 1) + "-childDesc" + QString::number(j),
            tw->topLevelItem(i)->child(j)->text(2));
        Reg.setValue(flag + QString::number(i + 1) + "-childDetails" +
                         QString::number(j),
                     tw->topLevelItem(i)->child(j)->text(3));
      }
    } else
      abc--;

    Reg.setValue(flag + "TopCount", abc);
  }
}

void MainWindow::drawMonthChart() {
  listM.clear();
  listM = get_MonthList(strY, strM);
  CurrentYearMonth = strY + strM;
}

void MainWindow::drawDayChart() {
  QTreeWidget *tw = (QTreeWidget *)tabData->currentWidget();
  if (loading) return;
  PointList.clear();

  int topCount = tw->topLevelItemCount();
  if (topCount == 0) {
    return;
  }

  if (topCount > 0) {
    if (!tw->currentIndex().isValid()) {
      QTreeWidgetItem *topItem = tw->topLevelItem(topCount - 1);
      tw->setCurrentItem(topItem);
    }
  }

  QTreeWidgetItem *item = tw->currentItem();
  bool child;
  int childCount;
  if (item->parent() == NULL)
    child = false;
  else
    child = true;

  QString month;

  if (child) {
    childCount = item->parent()->childCount();
    parentItem = item->parent();
    month = get_Month(item->parent()->text(0));
  } else {
    childCount = item->childCount();
    parentItem = item;
    month = get_Month(item->text(0));
  }

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
    int a = chartMax;
    if (childCount > a)
      yMaxDay = childCount;
    else
      yMaxDay = a;
  } else {
    yMaxDay = *std::max_element(dList.begin(), dList.end());
  }
}

void MainWindow::readData(QTreeWidget *tw) {
  tw->clear();
  QString name = tw->objectName();
  QString ini_file = iniDir + name + ".ini";

  if (!QFile::exists(ini_file)) return;

  QSettings Reg(ini_file, QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif

  QString group = Reg.childGroups().at(0);

  int rowCount = Reg.value("/" + group + "/TopCount").toInt();
  for (int i = 0; i < rowCount; i++) {
    int childCount =
        Reg.value("/" + group + "/" + QString::number(i + 1) + "-childCount")
            .toInt();

    // 不显示子项为0的数据
    if (childCount > 0) {
      QTreeWidgetItem *topItem = new QTreeWidgetItem;
      QString strD0 =
          Reg.value("/" + group + "/" + QString::number(i + 1) + "-topDate")
              .toString();

      QStringList lista = strD0.split(" ");
      if (lista.count() == 4) {
        QString a0 = lista.at(0) + " " + lista.at(1) + " " + lista.at(2);
        topItem->setText(0, a0);
        topItem->setText(3, lista.at(3));
      } else {
        topItem->setText(0, strD0);
        QString year =
            Reg.value("/" + group + "/" + QString::number(i + 1) + "-topYear")
                .toString();
        topItem->setText(3, year);
      }

      tw->addTopLevelItem(topItem);

      topItem->setTextAlignment(1, Qt::AlignHCenter | Qt::AlignVCenter);
      topItem->setTextAlignment(2, Qt::AlignRight | Qt::AlignVCenter);

      topItem->setText(
          1, Reg.value("/" + group + "/" + QString::number(i + 1) + "-topFreq")
                 .toString());
      topItem->setText(2, Reg.value("/" + group + "/" + QString::number(i + 1) +
                                    "-topAmount")
                              .toString());

      for (int j = 0; j < childCount; j++) {
        QTreeWidgetItem *item11 = new QTreeWidgetItem(topItem);
        item11->setText(0,
                        Reg.value("/" + group + "/" + QString::number(i + 1) +
                                  "-childTime" + QString::number(j))
                            .toString());
        item11->setText(1,
                        Reg.value("/" + group + "/" + QString::number(i + 1) +
                                  "-childAmount" + QString::number(j))
                            .toString());
        item11->setText(2,
                        Reg.value("/" + group + "/" + QString::number(i + 1) +
                                  "-childDesc" + QString::number(j))
                            .toString());
        item11->setText(3,
                        Reg.value("/" + group + "/" + QString::number(i + 1) +
                                  "-childDetails" + QString::number(j))
                            .toString());

        item11->setTextAlignment(1, Qt::AlignRight | Qt::AlignVCenter);
      }
    }
  }
}

void MainWindow::get_Today(QTreeWidget *tw) {
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
  bool isExists = QFile(textFile).exists();
  if (isExists) {
    QFile file(textFile);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
      qDebug() << tr("Cannot read file %1:\n%2.")
                      .arg(QDir::toNativeSeparators(textFile),
                           file.errorString());

    } else {
      QTextStream in(&file);

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
      in.setCodec("UTF-8");
#endif

      QString text = in.readAll();
      return text;
    }
    file.close();
  }

  return "";
}

void MainWindow::TextEditToFile(QTextEdit *txtEdit, QString fileName) {
  QFile *file;
  file = new QFile;
  file->setFileName(fileName);
  bool ok = file->open(QIODevice::WriteOnly | QIODevice::Text);
  if (ok) {
    QTextStream out(file);
    out << txtEdit->toPlainText();
    file->close();
    delete file;
  } else
    qDebug() << "Write failure!" << fileName;
}

void MainWindow::closeEvent(QCloseEvent *event) {
  if (!ui->frameNoteRecycle->isHidden()) {
    on_btnBackNoteRecycle_clicked();
    event->ignore();
    return;
  }

  if (!ui->frameNoteList->isHidden()) {
    on_btnBackNoteList_clicked();
    event->ignore();
    return;
  }

  if (!ui->frameNotes->isHidden()) {
    on_btnBackNotes_clicked();
    event->ignore();
    return;
  }

  if (!ui->frameTodoRecycle->isHidden()) {
    on_btnReturnRecycle_clicked();
    event->ignore();
    return;
  }

  if (!ui->frameTodo->isHidden()) {
    on_btnBackTodo_clicked();
    event->ignore();
    return;
  }

#ifdef Q_OS_ANDROID
  if (mydlgPre->isFontChange) {
    stopJavaTimer();
    event->accept();
    return;
  }
#else
  QSettings Reg(privateDir + "winpos.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif
  Reg.setValue("x", this->geometry().x());
  Reg.setValue("y", this->geometry().y());
  Reg.setValue("w", this->geometry().width());
  Reg.setValue("h", this->geometry().height());

  event->accept();
#endif
}

void MainWindow::setMini() {
#ifdef Q_OS_ANDROID

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  QAndroidJniObject jo = QAndroidJniObject::fromString("MiniWin");
  jo.callStaticMethod<int>("com.x/MyActivity", "mini", "()I");

#else
  QJniObject jo = QJniObject::fromString("MiniWin");
  jo.callStaticMethod<int>("com.x/MyActivity", "mini", "()I");

#endif

#endif
}

void MainWindow::resetWinPos() {
  QSettings Reg(privateDir + "winpos.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif
  int x, y, w, h;
  x = Reg.value("x").toInt();
  y = Reg.value("y").toInt();
  w = Reg.value("w").toInt();
  h = Reg.value("h").toInt();

  if (x < 0) x = 0;
  if (y < 0) y = 0;

  if (x > 0 && y > 0 && w > 0 && h > 0) this->setGeometry(x, y, w, h);
}

void MainWindow::init_Stats(QTreeWidget *tw) {
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

void MainWindow::initChartMonth() {
  if (loading) return;

  int count = PointList.count();
  if (count == 0) {
    return;
  }

  barSeries->clear();
  series->clear();
  m_scatterSeries->clear();
  bool isOne = true;

  QBarSet *setY = new QBarSet("Y");
  QStringList categories;

  for (int i = 0; i < count; i++) {
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

      tempPointList.append(QPointF(PointList.at(i).x(), y0));
    }
    PointList.clear();
    PointList = tempPointList;
  }

  double maxValue = *std::max_element(doubleList.begin(), doubleList.end());
  double max;
  if (isrbFreq) {
    max = chartMax;
    if (maxValue >= max) {
      max = maxValue;
    }

  } else {
    max = 50.00;
    if (maxValue >= max) max = maxValue;
  }

  yMaxMonth = max;

  QList<double> dList, tempDList;
  for (int i = 0; i < PointList.count(); i++) {
    tempDList.append(PointList.at(i).y());
    categories.append(QString::number(PointList.at(i).x()));
  }
  for (int i = 0; i < max_day; i++) {
    dList.append(0);
  }
  for (int i = 0; i < categories.count(); i++) {
    for (int n = 0; n < max_day; n++) {
      if (categories.at(i) == QString::number(n + 1)) {
        dList.removeAt(n);
        dList.insert(n, PointList.at(i).y());
      }
    }
  }

  for (int i = 0; i < max_day; i++) setY->append(dList.at(i));
  categories.clear();
  for (int i = 0; i < max_day; i++) categories.append(QString::number(i + 1));
  barSeries->append(setY);
  axisX->setRange("", QString::number(max_day));
  axisX->append(categories);
  axisY->setRange(0, yMaxMonth);

  if (isOne && mydlgPre->ui->chkAutoTime->isChecked()) {
    axisY->setRange(0, 24);
    chartMonth->setTitle(CurrentYear + "  Y:" + tr("Time") +
                         "    X:" + tr("Days"));
  } else {
    axisY->setRange(0, yMaxMonth);
    if (ui->rbFreq->isChecked())
      chartMonth->setTitle(CurrentYear + "  Y:" + tr("Freq") +
                           "    X:" + tr("Days"));

    if (ui->rbAmount->isChecked())
      chartMonth->setTitle(CurrentYear + "  Y:" + tr("Amount") +
                           "    X:" + tr("Days"));
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

  axisX2->setRange(0, 24);
  axisX2->setTickType(QValueAxis::TicksFixed);
  axisX2->setTickCount(7);

  axisY2->setRange(0, yMaxDay + 1);

  if (ui->rbFreq->isChecked())
    chartDay->setTitle(CurrentYear + "  Y:" + tr("Freq") +
                       "    X:" + tr("Time"));

  if (ui->rbAmount->isChecked())
    chartDay->setTitle(CurrentYear + "  Y:" + tr("Amount") +
                       "    X:" + tr("Time"));
}

void MainWindow::on_actionRename_triggered() {
  int index = ui->tabWidget->currentIndex();
  bool ok = false;
  QString text;
  QFrame *frame = new QFrame(this);
  QVBoxLayout *vbox = new QVBoxLayout;
  frame->setLayout(vbox);
  QInputDialog *idlg = new QInputDialog(this);
  idlg->hide();
  vbox->addWidget(idlg);

  idlg->setWindowFlag(Qt::FramelessWindowHint);
  QString style =
      "QDialog{background: "
      "rgb(244,237,241);border-radius:10px;border:2px solid red;}";

  idlg->setStyleSheet(style);
  idlg->setOkButtonText(tr("Ok"));
  idlg->setCancelButtonText(tr("Cancel"));
  idlg->setContentsMargins(10, 10, 10, 10);

  idlg->setWindowTitle(tr("Rename tab name : "));
  idlg->setTextValue(ui->tabWidget->tabText(index));
  idlg->setLabelText(tr("Tab name : "));

  frame->setGeometry(50, -100, mw_one->width() - 100, this->height());
  idlg->show();
  frame->show();

  if (QDialog::Accepted == idlg->exec()) {
    ok = true;
    text = idlg->textValue();
    frame->close();
  } else {
    frame->close();
    return;
  }

  if (ok && !text.isEmpty()) {
    ui->tabWidget->setTabText(index, text);
    saveTab();
  }

  isNeedAutoBackup = true;
  strLatestModify = tr("Rename Tab");
}

void MainWindow::on_actionAdd_Tab_triggered() {
  int count = ui->tabWidget->tabBar()->count();
  QString twName = m_Notes->getDateTimeStr() + "_" + QString::number(count + 1);
  QString ini_file = iniDir + twName + ".ini";
  if (QFile(ini_file).exists()) QFile(ini_file).remove();

  QTreeWidget *tw = init_TreeWidget(twName);

  ui->tabWidget->addTab(tw, tr("Tab") + " " + QString::number(count + 1));
  ui->tabWidget->setCurrentIndex(count);

  ui->tabCharts->setTabText(0, tr("Month"));
  ui->tabCharts->setTabText(1, tr("Day"));

  on_actionRename_triggered();
  reloadMain();

  saveTab();

  isNeedAutoBackup = true;
  strLatestModify = tr("Add Tab") + " ( " + getTabText() + " ) ";
}

void MainWindow::on_actionDel_Tab_triggered() {
  int index = ui->tabWidget->currentIndex();
  if (index < 0) return;

  QString tab_name = ui->tabWidget->tabText(index);

  if (!showMsgBox("Knot", tr("Whether to remove") + "  " + tab_name + " ? ", "",
                  2))
    return;

  isNeedAutoBackup = true;
  strLatestModify = tr("Del Tab") + " ( " + tab_name + " ) ";

  QTreeWidget *tw = (QTreeWidget *)tabData->currentWidget();
  QString twName = tw->objectName();
  QString tab_file = iniDir + twName + ".ini";
  QString date_time = m_Notes->getDateTimeStr();
  QFile::copy(tab_file,
              iniDir + "recycle_" + tab_name + "_" + date_time + ".ini");

  QFile file(tab_file);
  file.remove();

  int TabCount = ui->tabWidget->tabBar()->count();
  if (TabCount > 1) ui->tabWidget->removeTab(index);
  if (TabCount == 1) {
    QTreeWidget *tw = (QTreeWidget *)ui->tabWidget->currentWidget();
    tw->clear();

    ui->tabWidget->setTabToolTip(0, "");

    reloadMain();
  }

  saveTab();
}

QTreeWidget *MainWindow::init_TreeWidget(QString name) {
  QTreeWidget *tw = new QTreeWidget(this);
  tw->setFixedHeight(0);
  tw->setObjectName(name);

  QString ini_file = iniDir + name + ".ini";
  if (!QFile::exists(ini_file)) {
    QSettings RegTab(ini_file, QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    RegTab.setIniCodec("utf-8");
#endif
    RegTab.setValue("/" + name + "/" + "CreatedTime",
                    QDateTime::currentDateTime().toString());
  }

  QFont font;
  font.setPointSize(fontSize);
  tw->setFont(font);
  font.setBold(true);
  tw->header()->setFont(font);

  font.setPointSize(fontSize + 1);
  ui->lblKnot->setFont(font);

  tw->setColumnCount(4);
  tw->headerItem()->setText(0, "  " + tr("Date") + "  ");
  tw->headerItem()->setText(1, "  " + tr("Freq") + "  ");
  tw->headerItem()->setText(2, tr("Amount"));
  tw->headerItem()->setText(3, tr("Year"));
  tw->setColumnHidden(3, true);

  tw->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
  tw->header()->setDefaultAlignment(Qt::AlignCenter);
  tw->headerItem()->setTextAlignment(2, Qt::AlignRight);
  tw->setAlternatingRowColors(true);
  tw->setFrameShape(QTreeWidget::NoFrame);
  tw->installEventFilter(this);
  tw->viewport()->installEventFilter(this);
  tw->setUniformRowHeights(true);  // 加快展开速度
  connect(tw, &QTreeWidget::itemClicked, this, &MainWindow::on_twItemClicked);
  connect(tw, &QTreeWidget::itemDoubleClicked, this,
          &MainWindow::on_twItemDoubleClicked);
  connect(tw, &QTreeWidget::itemPressed, [=]() {});

  connect(tw->verticalScrollBar(), &QScrollBar::valueChanged, [=]() {});

  // tw->setUniformRowHeights(false); //对速度可能有影响，数据量大时
  QScrollBar *SB = tw->verticalScrollBar();
  SB->setStyleSheet(vsbarStyleSmall);
  tw->setStyleSheet(treeStyle);
  tw->setVerticalScrollMode(QTreeWidget::ScrollPerPixel);
  QScroller::grabGesture(tw, QScroller::LeftMouseButtonGesture);
  setSCrollPro(tw);
  return tw;
}

void MainWindow::on_twItemClicked() {
  QTreeWidget *tw = (QTreeWidget *)ui->tabWidget->currentWidget();
  if (!tw->currentIndex().isValid()) return;

  QTreeWidgetItem *item = tw->currentItem();
  if (item->parent() == NULL && item->childCount() == 0) return;

  QTreeWidgetItem *pItem = NULL;

  QString stra;
  if (item->parent() == NULL) {
    CurrentYear = item->text(3);
    stra = item->text(0);
  } else {
    CurrentYear = item->parent()->text(3);
    stra = item->parent()->text(0);
  }
  tw->headerItem()->setText(0, "" + tr("Date") + "  " + CurrentYear);
  ui->tabCharts->setTabText(0, stra.split(" ").at(1));
  ui->tabCharts->setTabText(1, stra.split(" ").at(2));

  // top item
  if (item->childCount() > 0) {
    pItem = item;
    QString sy = pItem->text(3);
    QString sm = pItem->text(0).split(" ").at(1);
    max_day = getMaxDay(sy, sm);

    isShowDetails = false;
    ui->lblStats->setStyleSheet(myEditRecord->ui->lblTitle->styleSheet());
    ui->lblStats->setText(strStats);
  }

  // child items
  if (item->childCount() == 0 && item->parent()->childCount() > 0) {
    pItem = item->parent();
    QString sy = pItem->text(3);
    QString sm = pItem->text(0).split(" ").at(1);
    max_day = getMaxDay(sy, sm);
  }

  if (tabChart->currentIndex() == 0) {
    QString str = stra + " " + CurrentYear;
    QString strYearMonth = get_Year(str) + get_Month(str);
    if (!isTabChanged) {
      if (strYearMonth == CurrentYearMonth) return;
    } else
      isTabChanged = false;
    startRead(str);
  }

  if (tabChart->currentIndex() == 1) {
    if (!isTabChanged) {
      if (parentItem != pItem) {
        startRead(strDate);
      }
    } else {
      startRead(strDate);
      isTabChanged = false;
    }
  }
}

void MainWindow::set_Time() {
  QTreeWidget *tw = (QTreeWidget *)ui->tabWidget->currentWidget();
  QTreeWidgetItem *item = tw->currentItem();
  QTreeWidgetItem *topItem = item->parent();
  QString newtime = myEditRecord->ui->lblTime->text().trimmed();
  if (item->childCount() == 0 && item->parent()->childCount() > 0) {
    item->setText(0, newtime);
    QString sa = myEditRecord->ui->editAmount->text().trimmed();
    if (sa == "")
      item->setText(1, "");
    else
      item->setText(1, QString("%1").arg(sa.toDouble(), 0, 'f', 2));
    item->setText(2, myEditRecord->ui->editCategory->text().trimmed());
    item->setText(3, myEditRecord->ui->editDetails->toPlainText().trimmed());
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

    int childRow0 = tw->currentIndex().row();
    sort_childItem(item);

    int childRow1 = 0;
    for (int i = 0; i < topItem->childCount(); i++) {
      QTreeWidgetItem *childItem = topItem->child(i);

      QString time = childItem->text(0).split(".").at(1);
      time = time.trimmed();

      if (time == newtime) {
        childRow1 = i;
        break;
      }
    }

    int newrow;
    int row = m_Method->getCurrentIndexBakList(ui->qwMainEvent);
    if (childRow0 - childRow1 == 0) newrow = row;
    if (childRow0 - childRow1 < 0) newrow = row + childRow1 - childRow0;
    if (childRow0 - childRow1 > 0) newrow = row - (childRow0 - childRow1);

    int maindateIndex = m_Method->getCurrentIndexBakList(ui->qwMainDate);
    reloadMain();
    m_Method->setCurrentIndexBakList(ui->qwMainDate, maindateIndex);
    m_Method->clickMainDate();
    m_Method->setCurrentIndexBakList(ui->qwMainEvent, newrow);
  }
}

void MainWindow::sort_childItem(QTreeWidgetItem *item) {
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
            [](const QString &s1, const QString &s2) { return s1 < s2; });

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
    QTreeWidgetItem *childItem = item->parent()->child(i);
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
  QTreeWidget *tw = (QTreeWidget *)ui->tabWidget->currentWidget();
  QTreeWidgetItem *item = tw->currentItem();
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
    myEditRecord->ui->lblTitle->setText(
        tr("Modify") + "  : " + tabData->tabText(tabData->currentIndex()));

    myEditRecord->ui->hsH->setValue(sh.toInt());
    myEditRecord->ui->hsM->setValue(sm.toInt());

    myEditRecord->ui->lblTime->setText(t.trimmed());

    QString str = item->text(1);
    if (str == "0.00")
      myEditRecord->ui->editAmount->setText("");
    else
      myEditRecord->ui->editAmount->setText(str);

    myEditRecord->ui->editCategory->setText(item->text(2));
    myEditRecord->ui->editDetails->setText(item->text(3));
    myEditRecord->ui->frame->setFocus();

    isAdd = false;
    myEditRecord->init();
  }

  if (item == tw->topLevelItem(tw->topLevelItemCount() - 1)) {
    if (item->childCount() > 0) {
    }
  }
}

void MainWindow::on_tabWidget_currentChanged(int index) {
  int count = ui->tabWidget->tabBar()->count();

  if (isSlide || loading || count <= 0) {
    return;
  }

  QTreeWidget *tw = (QTreeWidget *)tabData->widget(index);
  tw->setFocus();

  if (!loading) {
    QSettings Reg(iniDir + "tab.ini", QSettings::IniFormat);

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    Reg.setIniCodec("utf-8");
#endif

    Reg.setValue("CurrentIndex", index);
  }

  reloadMain();

  series->clear();
  m_scatterSeries->clear();
  barSeries->clear();

  series2->clear();
  m_scatterSeries2->clear();
  m_scatterSeries2_1->clear();

  isTabChanged = true;
  // clickData();
  m_Method->clickMainDateData();
}

void MainWindow::saveNotes(int tabIndex) {
  if (loading) return;
  QSettings Reg(iniDir + "notes.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif
  QTreeWidget *tw = (QTreeWidget *)tabData->widget(tabIndex);
  QString name = tw->objectName();
  Reg.setValue("/" + name + "/Note", tabData->tabToolTip(tabIndex));
}

void MainWindow::on_btnRemarks_clicked() {
  m_Reader->setPdfViewVisible(false);

  m_Notes->m_SetEditText->close();
  m_Notes->m_SetEditText = new dlgSetEditText(m_Remarks);

  m_Remarks->setGeometry(mw_one->geometry().x(), mw_one->geometry().y(),
                         this->width(), this->height() / 2);

  m_Remarks->ui->textEdit->clear();

  m_Remarks->ui->frameAbout->hide();
  m_Remarks->ui->textEdit->setHidden(false);
  m_Remarks->ui->lblTitle->show();
  m_Remarks->ui->btnPaste->show();
  m_Remarks->ui->lblTitle->setText(tr("Remarks") + " : " +
                                   tabData->tabText(tabData->currentIndex()));

  m_Remarks->init_Notes();

  m_Remarks->show();
}

bool MainWindow::eventFilter(QObject *watch, QEvent *evn) {
  if (loading) return QWidget::eventFilter(watch, evn);

  if (watch == ui->editTodo->viewport()) {
    mw_one->m_Notes->getEditPanel(ui->editTodo, evn);
  }

  QMouseEvent *event = static_cast<QMouseEvent *>(evn);  // 将之转换为鼠标事件
  QTreeWidget *tw = (QTreeWidget *)ui->tabWidget->currentWidget();

  if (evn->type() == QEvent::ToolTip) {
    QToolTip::hideText();
    evn->ignore();
    return true;
  }

  if (watch == ui->lblStats) {
    if (event->type() == QEvent::MouseButtonDblClick) {
      on_btnSelTab_clicked();
    }
  }

  if (watch == ui->qwReader) {
    if (event->type() == QEvent::MouseButtonDblClick) {
      on_SetReaderFunVisible();
    }
  }

  if (watch == tw->viewport()) {
    if (event->type() == QEvent::MouseButtonPress) {
    }

    if (event->type() == QEvent::MouseButtonRelease) {
    }
  }

  if (watch == ui->textBrowser->viewport()) {
    if (event->type() == QEvent::MouseButtonPress) {
      isMousePress = true;
    }

    if (event->type() == QEvent::MouseButtonRelease) {
      isMousePress = false;

      QString str = ui->textBrowser->textCursor().selectedText().trimmed();
      if (str == "") {
        mydlgSetText->close();
      } else {
        mydlgSetText->setFixedWidth(width() * 2 / 3);

        int y1;
        int a = 30;
        if (event->globalY() - a - mydlgSetText->height() >= 0)
          y1 = event->globalY() - a - mydlgSetText->height();
        else
          y1 = event->globalY() + a;

        mydlgSetText->init(
            geometry().x() + (width() - mydlgSetText->width()) / 2, y1,
            mydlgSetText->width(), mydlgSetText->height());
      }
    }

    if (event->type() == QEvent::MouseMove) {
      if (isMousePress) {
        QString str = ui->textBrowser->textCursor().selectedText().trimmed();
        if (str != "") {
          mydlgSetText->setFixedWidth(width() * 2 / 3);

          int y1;
          int a = 30;
          if (event->globalY() - a - mydlgSetText->height() >= 0)
            y1 = event->globalY() - a - mydlgSetText->height();
          else
            y1 = event->globalY() + a;

          mydlgSetText->init(
              geometry().x() + (width() - mydlgSetText->width()) / 2, y1,
              mydlgSetText->width(), mydlgSetText->height());
        }
      }
    }

    if (event->type() == QEvent::MouseButtonDblClick) {
      QTextCursor cursor = ui->textBrowser->textCursor();
      cursor.setPosition(cursor.anchor());
      ui->textBrowser->setTextCursor(cursor);

      return true;
    }
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
        QTreeWidget *tw = (QTreeWidget *)ui->tabWidget->widget(i);
        QString name = tw->objectName();
        if (name != "tab" + QString::number(i + 1)) {
          move = true;
        }
      }
      if (move) {
        qDebug() << "ok save";
        saveTab();
      }
    }
  }

  if (watch == ui->lblStats) {
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
    }

    if (event->type() == QEvent::MouseButtonRelease) {
      relea_x = event->globalX();
      relea_y = event->globalY();
    }

    // Right
    if ((relea_x - press_x) > 75 &&
        event->type() == QEvent::MouseButtonRelease &&
        qAbs(relea_y - press_y) < 35) {
      int current_page = ui->tabWidget->currentIndex();
      if (current_page < count - 1) {
        isSlide = true;

        QPropertyAnimation *animation1 =

            new QPropertyAnimation(ui->tabWidget->currentWidget(), "geometry");
        animation1->setDuration(350);
        animation1->setStartValue(QRect(x, y, w, h));
        animation1->setEndValue(QRect(w * 2, y, w, h));

        ui->tabWidget->setCurrentIndex(current_page + 1);

        QPropertyAnimation *animation2 =
            new QPropertyAnimation(ui->tabWidget->currentWidget(), "geometry");
        animation2->setDuration(350);
        animation2->setStartValue(QRect(-w * 2, y, w, h));
        animation2->setEndValue(QRect(x, y, w, h));

        QParallelAnimationGroup *group = new QParallelAnimationGroup;
        group->addAnimation(animation1);
        group->addAnimation(animation2);
        group->start();
        QElapsedTimer t;
        t.start();
        while (t.elapsed() < 600) {
          QCoreApplication::processEvents();
        }

        isSlide = false;

        on_tabWidget_currentChanged(tabData->currentIndex());
      }
    }

    // Left
    if ((press_x - relea_x) > 75 &&
        event->type() == QEvent::MouseButtonRelease &&
        qAbs(relea_y - press_y) < 35 && index > 0) {
      int current_page = ui->tabWidget->currentIndex();
      if (current_page >= 0) {
        isSlide = true;

        QPropertyAnimation *animation1 =

            new QPropertyAnimation(ui->tabWidget->currentWidget(), "geometry");
        animation1->setDuration(350);
        animation1->setStartValue(QRect(x, y, w, h));
        animation1->setEndValue(QRect(-w, y, w, h));

        ui->tabWidget->setCurrentIndex(current_page - 1);

        QPropertyAnimation *animation2 =
            new QPropertyAnimation(ui->tabWidget->currentWidget(), "geometry");
        animation2->setDuration(350);
        animation2->setStartValue(QRect(w * 2, y, w, h));
        animation2->setEndValue(QRect(x, y, w, h));

        QParallelAnimationGroup *group = new QParallelAnimationGroup;
        group->addAnimation(animation1);
        group->addAnimation(animation2);
        group->start();
        QElapsedTimer t;
        t.start();
        while (t.elapsed() < 500) {
          QCoreApplication::processEvents();
        }

        isSlide = false;

        on_tabWidget_currentChanged(tabData->currentIndex());
      }
    }
  }

  if (evn->type() == QEvent::KeyRelease) {
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(evn);

    if (watch == ui->editSearchText && keyEvent->key() == Qt::Key_Return) {
      on_btnStartSearch_clicked();
      return true;
    }

    if (watch == ui->editFindNote && keyEvent->key() == Qt::Key_Return) {
      on_btnFindNextNote_clicked();
      return true;
    }

    if (keyEvent->key() == Qt::Key_Back) {
      if (!ui->frameReader->isHidden()) {
        if (!listSelFont->isHidden()) {
          listSelFont->close();
          return true;

        } else if (!mydlgReaderFun->isHidden()) {
          mydlgReaderFun->close();
          return true;

        } else if (!mydlgSetText->isHidden()) {
          mydlgSetText->close();
          return true;

        } else if (!ui->textBrowser->isHidden()) {
          on_btnSelText_clicked();
          return true;
        }

        else {
          on_btnBack_clicked();
          return true;
        }
      }

      if (!ui->f_ImgView->isHidden()) {
        if (isReaderVisible) {
          ui->f_ImgView->hide();
          ui->frameReader->show();
          return true;
        } else if (isMemoVisible) {
          ui->f_ImgView->hide();
          ui->frameNotes->show();
          return true;
        }
      }

      if (!dlgTimeMachine->isHidden()) {
        dlgTimeMachine->close();
        return true;
      }

      if (!ui->frameMain->isHidden()) {
        setMini();
        mydlgPre->autoBakData();

        return true;
      }

      if (!ui->frameOne->isHidden()) {
        on_btnBack_One_clicked();
        return true;
      }

      if (!ui->frameNoteRecycle->isHidden()) {
        on_btnBackNoteRecycle_clicked();
        return true;
      }

      if (!ui->frameNoteList->isHidden()) {
        on_btnBackNoteList_clicked();
        return true;
      }

      if (!ui->frameNotes->isHidden()) {
        on_btnBackNotes_clicked();
        return true;
      }

      if (!ui->frameTodo->isHidden()) {
        if (!mw_one->m_Notes->m_SetEditText->isHidden()) {
          mw_one->m_Notes->m_SetEditText->close();
          return true;
        } else {
          on_btnBackTodo_clicked();
          return true;
        }
      }

      if (!ui->frameTodoRecycle->isHidden()) {
        on_btnReturnRecycle_clicked();
        return true;
      }

      if (!ui->frameTabRecycle->isHidden()) {
        on_btnBackTabRecycle_clicked();
        return true;
      }

      if (!ui->frameSteps->isHidden()) {
        on_btnBackSteps_clicked();
        return true;
      }

      if (!ui->frameViewCate->isHidden()) {
        ui->frameViewCate->hide();
        ui->frameReport->show();
        return true;
      }

      if (!ui->frameReport->isHidden()) {
        on_btnBack_Report_clicked();
        return true;
      }

      if (!ui->frameSearch->isHidden()) {
        on_btnBackSearch_clicked();
        return true;
      }

      if (!ui->frameBakList->isHidden()) {
        on_btnBackBakList_clicked();
        return true;
      }

      if (!ui->frameCategory->isHidden()) {
        on_btnCancelType_clicked();
        return true;
      }

      if (!ui->frameSetTab->isHidden()) {
        on_btnBackSetTab_clicked();
        return true;
      }
    }
  }

  if (watch == ui->qwReader) {
    static int press_x;
    static int press_y;
    static int relea_x;
    static int relea_y;
    int length = 75;

    if (ui->textBrowser->isHidden()) {
      if (event->type() == QEvent::MouseButtonPress) {
        isMousePress = true;
        isMouseMove = false;
        if (!isMouseMove) timerMousePress->start(1300);
      }

      if (event->type() == QEvent::MouseButtonRelease) {
        isMousePress = false;
        isMouseMove = false;
      }

      if (event->type() == QEvent::MouseMove) {
        isMouseMove = true;

        if (isMousePress) {
          if ((relea_x - press_x) > length && qAbs(relea_y - press_y) < 35) {
            // qDebug() << "book right...";
            int cn = mw_one->ui->btnPages->text().split("\n").at(1).toInt();
            if (cn != 1) {
              mydlgFloatFun->setPicRight();
              mydlgFloatFun->init();
            }
          } else if ((press_x - relea_x) > length &&
                     qAbs(relea_y - press_y) < 35) {
            // qDebug() << "book left...";
            int cn = mw_one->ui->btnPages->text().split("\n").at(1).toInt();
            int tn = mw_one->ui->btnPages->text().split("\n").at(2).toInt();
            if (cn != tn) {
              mydlgFloatFun->setPicLeft();
              mydlgFloatFun->init();
            }
          } else
            mydlgFloatFun->close();
        }
      }
    }

    if (event->type() == QEvent::MouseButtonPress) {
      isMousePress = true;

      press_x = event->globalX();
      press_y = event->globalY();
      x = 0;
      y = 0;
      w = ui->qwReader->width();
      h = ui->qwReader->height();
    }

    if (event->type() == QEvent::MouseButtonRelease) {
      relea_x = event->globalX();
      relea_y = event->globalY();
      ui->lblTitle->hide();
      QQuickItem *root = ui->qwReader->rootObject();

      isTurnThePage = false;
      isMousePress = false;
      isMouseMove = false;

      // Right Slide
      if ((relea_x - press_x) > length && qAbs(relea_y - press_y) < 35) {
        if (isText) {
          if (iPage - baseLines <= 0) {
            if (isText || isEpub) {
              QMetaObject::invokeMethod((QObject *)root, "setX",
                                        Q_ARG(QVariant, 0));
              return QWidget::eventFilter(watch, evn);
            }
          }
        } else if (isEpub) {
          if (htmlIndex <= 0) {
            if (isText || isEpub) {
              QMetaObject::invokeMethod((QObject *)root, "setX",
                                        Q_ARG(QVariant, 0));
              return QWidget::eventFilter(watch, evn);
            }
          }
        }
        isTurnThePage = true;

        on_btnPageUp_clicked();
        mydlgFloatFun->close();
      }

      // Left Slide
      if ((press_x - relea_x) > length && qAbs(relea_y - press_y) < 35) {
        if (isText) {
          if (iPage + baseLines > totallines) {
            if (isText || isEpub) {
              QMetaObject::invokeMethod((QObject *)root, "setX",
                                        Q_ARG(QVariant, 0));
              return QWidget::eventFilter(watch, evn);
            }
          }
        } else if (isEpub) {
          if (htmlIndex + 1 >= htmlFiles.count()) {
            if (isText || isEpub) {
              QMetaObject::invokeMethod((QObject *)root, "setX",
                                        Q_ARG(QVariant, 0));
              return QWidget::eventFilter(watch, evn);
            }
          }
        }
        isTurnThePage = true;

        on_btnPageNext_clicked();
        mydlgFloatFun->close();
      }

      if (isText || isEpub)
        QMetaObject::invokeMethod((QObject *)root, "setX", Q_ARG(QVariant, 0));

      curx = 0;
    }

    if (event->type() == QEvent::MouseMove) {
      relea_x = event->globalX();
      relea_y = event->globalY();
      if (isMousePress && qAbs(relea_x - press_x) > 20 &&
          qAbs(relea_y - press_y) < 20) {
        isMouseMove = true;
      }
    }
  }

  if (watch == ui->qwPdf) {
    if (event->type() == QEvent::MouseButtonPress) {
    }
  }

  return QWidget::eventFilter(watch, evn);
}

void MainWindow::on_actionExport_Data_triggered() {
  if (!isSaveEnd) return;

#ifdef Q_OS_ANDROID
  zipfile = "android";
#else
  QFileDialog fd;
  zipfile = fd.getSaveFileName(this, tr("KnotBak"), "", tr("Zip File(*.zip)"));
#endif

  isUpData = false;
  showProgress();
  myBakDataThread->start();
}

void MainWindow::bakIniData(QString unredoFile, bool unre) {
  QTextEdit *edit = new QTextEdit;
  edit->append("[" + appName + "]");
  edit->append("Ver: " + ver);
  edit->append(loadText(iniDir + "tab.ini"));
  edit->append(loadText(iniDir + "desc.ini"));
  edit->append(loadText(iniDir + "ymd.ini"));
  edit->append(loadText(iniDir + "notes.ini"));
  edit->append(loadText(iniDir + "steps.ini"));

  for (int i = 0; i < tabData->tabBar()->count(); i++) {
    QString tabIniFile = iniDir + "tab" + QString::number(i + 1) + ".ini";
    if (QFile(tabIniFile).exists()) edit->append(loadText(tabIniFile));
  }

  TextEditToFile(edit, iniDir + "memo/KnotSync.ini");
  if (unre) {
    TextEditToFile(edit, unredoFile);
  }
}

QString MainWindow::bakData(QString fileName, bool msgbox) {
  Q_UNUSED(msgbox);
  if (!fileName.isNull()) {
    isSelf = true;

    m_NotesList->clearFiles();

    // Remove old ini files
    QStringList iniFiles;
    QStringList fmt;
    fmt.append("ini");
    m_NotesList->getAllFiles(iniDir + "memo/", iniFiles, fmt);
    for (int i = 0; i < iniFiles.count(); i++) {
      QFile::remove(iniFiles.at(i));
    }

    // Copy new ini files
    iniFiles.clear();
    m_NotesList->getAllFiles(iniDir, iniFiles, fmt);
    for (int i = 0; i < iniFiles.count(); i++) {
      QString strf = iniFiles.at(i);
      QFileInfo fi(strf);
      QFile::copy(strf, iniDir + "memo/" + fi.fileName());
    }

    QString zipfile = iniDir + "memo.zip";
    QFile(zipfile).remove();
    mw_one->m_Notes->zipMemo();

    if (fileName != zipfile) {
      if (QFile::exists(fileName)) QFile::remove(fileName);

#ifdef Q_OS_ANDROID
      QDir *folder = new QDir;
      QString path = "/storage/emulated/0/KnotBak/";
      folder->mkdir(path);
      QString str = m_Notes->getDateTimeStr();
      infoStr = path + str + "_Knot.zip";
      m_Notes->androidCopyFile(zipfile, infoStr);
      if (!QFile(infoStr).exists()) {
        QMessageBox box;
        box.setText(tr("Please turn on the storage permission of the app."));
        box.exec();
      }

#else
      QFile::copy(zipfile, fileName);
      infoStr = fileName;
#endif
    }

    // Remove old ini files
    iniFiles.clear();
    m_NotesList->getAllFiles(iniDir + "memo/", iniFiles, fmt);
    for (int i = 0; i < iniFiles.count(); i++) {
      QFile::remove(iniFiles.at(i));
    }

    isSelf = false;
    return infoStr;
  }
  return "";
}

void MainWindow::on_actionImport_Data_triggered() {
  if (!isSaveEnd) return;

  zipfile = "";
#ifdef Q_OS_ANDROID
  QString path = "/storage/emulated/0/KnotBak/";
  zipfile = QFileDialog::getOpenFileName(this, tr("KnotBak"), path,
                                         tr("Zip File (*.*)"));
#else
  zipfile = QFileDialog::getOpenFileName(this, tr("KnotBak"), "",
                                         tr("Zip File (*.zip);;All(*.*)"));
#endif

  if (QFile(zipfile).exists()) addUndo(tr("Import Data"));

  if (!zipfile.isNull()) {
    if (!mw_one->showMsgBox("Kont",
                            tr("Import this data?") + "\n" +
                                mw_one->m_Reader->getUriRealPath(zipfile),
                            "", 2)) {
      isZipOK = false;
      return;
    }
  }

  showProgress();

  isMenuImport = true;
  isTimeMachine = false;
  isDownData = false;
  myImportDataThread->start();
}

bool MainWindow::importBakData(QString fileName, bool msg, bool book,
                               bool unre) {
  if (!fileName.isNull()) {
    if (msg) {
    }

    if (unre) {
    }

    if (fileName != iniDir + "memo.zip") {
      QFile::remove(iniDir + "memo.zip");

      QString oldPath = iniDir + "memo";
      QDir dirOld(oldPath);
      dirOld.rename(oldPath, iniDir + "memo_bak");

      QFile::copy(fileName, iniDir + "memo.zip");
    }
    mw_one->m_Notes->unzip(iniDir + "memo.zip");

    QFile file(iniDir + "memo/tab.ini");
    if (!file.exists()) {
      QString oldPath = iniDir + "memo_bak";
      QDir dirOld(oldPath);
      dirOld.rename(oldPath, iniDir + "memo");

      isZipOK = false;
      return false;
    }

    isZipOK = true;
    mw_one->m_Reader->deleteDirfile(iniDir + "memo_bak");

    // Remove old ini files
    QStringList iniFiles;
    QStringList fmt;
    fmt.append("ini");
    m_NotesList->getAllFiles(iniDir, iniFiles, fmt);
    for (int i = 0; i < iniFiles.count(); i++) {
      QString file = iniFiles.at(i);
      if (!file.contains("/memo/")) QFile::remove(file);
    }

    // Copy new ini files
    iniFiles.clear();
    m_NotesList->getAllFiles(iniDir + "memo/", iniFiles, fmt);
    for (int i = 0; i < iniFiles.count(); i++) {
      QString strf = iniFiles.at(i);
      QFileInfo fi(strf);
      QFile::copy(strf, iniDir + fi.fileName());

      // Del ini bak files
      QFile::remove(strf);
    }

    if (book) {
    }
  }

  return true;
}

bool MainWindow::copyFileToPath(QString sourceDir, QString toDir,
                                bool coverFileIfExist) {
  toDir.replace("\\", "/");
  if (sourceDir == toDir) {
    return true;
  }
  if (!QFile::exists(sourceDir)) {
    return false;
  }
  QDir *createfile = new QDir;
  bool exist = createfile->exists(toDir);
  if (exist) {
    if (coverFileIfExist) {
      createfile->remove(toDir);
    }
  }  // end if

  if (!QFile::copy(sourceDir, toDir)) {
    return false;
  }
  return true;
}

QDialog *MainWindow::getProgBar() {
  QDialog *dlg;
  dlg = new QDialog(this);
  dlg->setModal(true);
  dlg->setFixedHeight(200);
  dlg->setFixedWidth(geometry().width() - 50);
  QVBoxLayout *vbox = new QVBoxLayout;
  dlg->setLayout(vbox);
  dlg->setGeometry(geometry().x() + (width() - dlg->width()) / 2,
                   (height() - dlg->height()) / 2 + 0, dlg->width(),
                   dlg->height());

  QLabel *lbl = new QLabel(dlg);
  lbl->setText(tr("Reading, please wait..."));
  lbl->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  dlg->layout()->addWidget(lbl);

  if (nProgressBarType == 1) {
    QProgressBar *prog = new QProgressBar(this);
    prog->setStyleSheet(
        "QProgressBar{border:0px solid #FFFFFF;"
        "height:25;"
        "background:rgb(25,255,25);"
        "text-align:right;"
        "color:rgb(255,255,255);"
        "border-radius:0px;}"

        "QProgressBar:chunk{"
        "border-radius:0px;"
        "background-color:rgba(18,150,219,255);"
        "}");
    prog->setMaximum(0);
    prog->setMinimum(0);
    dlg->layout()->addWidget(prog);
  }

  if (nProgressBarType == 2) {
    QtMaterialCircularProgress *qmProgress =
        new QtMaterialCircularProgress(this);
    dlg->layout()->addWidget(qmProgress);
  }

  return dlg;
}

void MainWindow::showProgress() {
  dlgProg = new QDialog();
  dlgProg = getProgBar();

  if (!initMain) dlgProg->show();
}

void MainWindow::closeProgress() {
  if (!initMain) {
    dlgProg->close();
    delete dlgProg;
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
  if (list.count() > 1) {
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

QTreeWidget *MainWindow::get_tw(int tabIndex) {
  QTreeWidget *tw = (QTreeWidget *)tabData->widget(tabIndex);
  return tw;
}

void MainWindow::on_about() {
  m_Remarks->init_Notes();

  QTextBrowser *textBrowser = new QTextBrowser;
  textBrowser->append("");
  textBrowser->append(appName + "  Ver: " + ver);

  textBrowser->append("");
  textBrowser->append("Launched: " + loginTime);
  textBrowser->append("");
  textBrowser->setHidden(true);
  m_Remarks->ui->textEdit->setHidden(true);

  m_Remarks->ui->lblTitle->hide();
  m_Remarks->ui->btnPaste->hide();
  m_Remarks->ui->lblAbout->setText(textBrowser->toPlainText());
  m_Remarks->ui->frameAbout->show();
  m_Remarks->setGeometry(this->geometry().x(), this->geometry().y(),
                         this->width(), this->height());
  m_Remarks->setModal(true);
  m_Remarks->show();
}

void MainWindow::on_btnFind_clicked() {
  ui->frameMain->hide();
  ui->frameSearch->show();
  ui->editSearchText->setFocus();
}

QStringList MainWindow::get_MonthList(QString strY, QString strM) {
  QStringList listMonth;
  if (loading) return listMonth;
  // 格式：记录第一个子项的时间
  PointList.clear();
  doubleList.clear();

  QTreeWidget *tw = (QTreeWidget *)tabData->currentWidget();
  for (int i = 0; i < tw->topLevelItemCount(); i++) {
    if (isBreak) break;
    QTreeWidgetItem *topItem = tw->topLevelItem(i);
    QString str0 = topItem->text(0) + " " + topItem->text(3);
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
                topItem->child(0)->text(0));  // 记录第一个子项的时间

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

void MainWindow::gotoMainItem(QTreeWidgetItem *item) {
  int count = getCount();
  for (int i = 0; i < count; i++) {
    QString text0 = getText0(i);
    QString text1 = getText1(i);
    QString text2 = getText2(i);
    if (item->text(0) == text0 && item->text(1) == text1 &&
        item->text(2) == text2) {
      gotoIndex(i);
      setCurrentIndex(i);
      break;
    }
  }
}

QString MainWindow::setLineEditQss(QLineEdit *txt, int radius, int borderWidth,
                                   const QString &normalColor,
                                   const QString &focusColor) {
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

QString MainWindow::setPushButtonQss(QToolButton *btn, int radius, int padding,
                                     const QString &normalColor,
                                     const QString &normalTextColor,
                                     const QString &hoverColor,
                                     const QString &hoverTextColor,
                                     const QString &pressedColor,
                                     const QString &pressedTextColor) {
  QStringList list;
  list.append(QString("QToolButton{border-style:none;padding:%1px;border-"
                      "radius:%2px;color:%3;background:%4;}")
                  .arg(padding)
                  .arg(radius)
                  .arg(normalTextColor)
                  .arg(normalColor));
  list.append(QString("QToolButton:hover{color:%1;background:%2;}")
                  .arg(hoverTextColor)
                  .arg(hoverColor));
  list.append(QString("QToolButton:pressed{color:%1;background:%2;}")
                  .arg(pressedTextColor)
                  .arg(pressedColor));

  QString qss = list.join("");
  btn->setStyleSheet(qss);
  return qss;
}

QString MainWindow::setComboBoxQss(QComboBox *txt, int radius, int borderWidth,
                                   const QString &normalColor,
                                   const QString &focusColor) {
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

void MainWindow::on_actionFind_triggered() { on_btnFind_clicked(); }

void MainWindow::on_btnTodo_clicked() {
  removeFilesWatch();
  isSelf = true;
  m_Notes->m_SetEditText->close();
  m_Notes->m_SetEditText = new dlgSetEditText(m_Todo);

  m_Todo->setGeometry(this->geometry().x(), this->geometry().y(), this->width(),
                      this->height());
  mw_one->ui->qwTodo->rootContext()->setContextProperty("m_width",
                                                        mw_one->width());

  ui->frameMain->hide();
  ui->frameTodo->setGeometry(this->geometry().x(), this->geometry().y(),
                             this->width(), this->height());
  ui->frameTodo->show();
  m_Todo->init_Todo();

  m_Todo->refreshAlarm();
}

void MainWindow::on_rbFreq_clicked() {
  tabChart->setTabEnabled(1, true);
  isrbFreq = true;
  CurrentYearMonth = "";
  parentItem = NULL;
  m_Method->clickMainDateData();
}

void MainWindow::on_rbAmount_clicked() {
  tabChart->setTabEnabled(1, true);
  isrbFreq = false;
  CurrentYearMonth = "";
  parentItem = NULL;
  m_Method->clickMainDateData();
}

void MainWindow::paintEvent(QPaintEvent *event) {
  Q_UNUSED(event);
  if (floatfun && !initMain) {
    floatfun = false;
  }
  // 获取背景色
  QPalette pal = ui->btnFind->palette();
  QBrush brush = pal.window();
  int c_red = brush.color().red();

  if (c_red != red) {
    red = c_red;
    if (red < 55) {
      chartMonth->setTheme(QChart::ChartThemeDark);
      chartDay->setTheme(QChart::ChartThemeDark);
    }

    else {
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
    ui->btnMax->setText(tr("Normal"));
  } else if (ui->btnMax->text() == tr("Normal")) {
    ui->frame_tab->setMaximumHeight(this->height());
    ui->frame_charts->setHidden(false);
    ui->btnMax->setText(tr("Max"));
  }
}

void MainWindow::on_actionReport_triggered() {
  if (isEBook || !isSaveEnd || !isReadEBookEnd) return;

  if (isReadEBookEnd) {
    m_Report->init();
    startInitReport();
  }
}

void MainWindow::startInitReport() {
  showProgress();

  isReport = true;
  myReadEBookThread->start();
}

void MainWindow::on_actionPreferences_triggered() {
  mydlgPre->setFixedWidth(this->width());
  mydlgPre->setGeometry(geometry().x(), geometry().y(), mydlgPre->width(),
                        height());
  mydlgPre->setModal(true);
  mydlgPre->ui->sliderFontSize->setStyleSheet(
      myEditRecord->ui->hsM->styleSheet());
  mydlgPre->show();
}

void MainWindow::on_tabCharts_currentChanged(int index) {
  if (ui->rbSteps->isChecked() || loading || index < 0) return;

  m_Method->clickMainDateData();
}

void MainWindow::on_btnSteps_clicked() {
  m_Steps->setGeometry(this->geometry().x(), this->geometry().y(),
                       this->width(), this->height());

  ui->frameMain->hide();
  ui->frameSteps->show();

  if (isHardStepSensor == 1) updateHardSensorSteps();

  m_Steps->init_Steps();

  m_Steps->setScrollBarPos(1.00);
  m_Steps->setMaxMark();
}

void MainWindow::changeEvent(QEvent *event) {
  if (event->type() == QEvent::WindowStateChange) {
  }
}

void MainWindow::on_rbSteps_clicked() {
  int count = m_Steps->getCount();
  if (count <= 0) return;

  tabChart->setCurrentIndex(0);
  tabChart->setTabEnabled(1, false);

  PointList.clear();
  doubleList.clear();

  QString sm = get_Month(QDate::currentDate().toString("ddd MM dd yyyy"));
  for (int i = 0; i < count; i++) {
    QString strD = m_Steps->getDate(i);
    if (sm == get_Month(strD)) {
      int day = get_Day(strD);
      int steps = m_Steps->getSteps(i);
      PointList.append(QPointF(day, steps));
      doubleList.append(steps);
    }
  }

  initChartMonth();
  chartMonth->setTitle("Y:" + tr("Steps") + "    X:" + tr("Days"));
}

void MainWindow::Sleep(int msec) {
  QTime dieTime = QTime::currentTime().addMSecs(msec);
  while (QTime::currentTime() < dieTime)
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
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
  qDebug() << "Started updating the hardware sensor steps...";

  timeTest = timeTest + 1;
  ui->lblTotalRunTime->setText(tr("Number of Operations") + " : " +
                               QString::number(timeTest));

  if (strDate != QDate::currentDate().toString("ddd MM dd yyyy")) {
    initTodayInitSteps();
    strDate = QDate::currentDate().toString("ddd MM dd yyyy");
  }

  qlonglong steps = 0;
#ifdef Q_OS_ANDROID

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  QAndroidJniObject m_activity = QtAndroid::androidActivity();
  m_activity.callMethod<void>("initStepSensor");

  QAndroidJniObject jo = QAndroidJniObject::fromString("getSteps");
  tc = jo.callStaticMethod<float>("com.x/MyActivity", "getSteps", "()F");
#else
  QJniObject m_activity = QNativeInterface::QAndroidApplication::context();
  m_activity.callMethod<void>("initStepSensor");

  QJniObject jo = QJniObject::fromString("getSteps");
  tc = jo.callStaticMethod<float>("com.x/MyActivity", "getSteps", "()F");
#endif

#endif
  steps = tc - initTodaySteps;

  if (steps < 0) return;
  if (steps > 100000000) return;
  CurrentSteps = tc - resetSteps;
  ui->lcdNumber->display(QString::number(steps));
  ui->lblSingle->setText(QString::number(CurrentSteps));
  m_Steps->setTableSteps(steps);

  sendMsg(steps);
}

void MainWindow::on_btnNotes_clicked() {
  removeFilesWatch();
  isSelf = true;

  QSettings Reg(iniDir + "mainnotes.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif

  QString strPw = Reg.value("/MainNotes/UserKey").toString();
  if (strPw != "") {
    QByteArray baPw = strPw.toUtf8();
    for (int i = 0; i < baPw.size(); i++) {
      baPw[i] = baPw[i] - 66;  // 解密User的密码
    }
    strPw = baPw;

    bool ok = false;
    QString text;
    QFrame *frame = new QFrame(this);
    QVBoxLayout *vbox = new QVBoxLayout;
    frame->setLayout(vbox);
    QInputDialog *idlg = new QInputDialog(this);
    vbox->addWidget(idlg);

    idlg->setWindowFlag(Qt::FramelessWindowHint);
    QString style =
        "QDialog{background: "
        "rgb(244,237,241);border-radius:10px;border:2px solid red;}";
    idlg->setStyleSheet(style);
    idlg->setOkButtonText(tr("Ok"));
    idlg->setCancelButtonText(tr("Cancel"));
    idlg->setContentsMargins(10, 10, 10, 10);
    idlg->setWindowTitle(tr("Please enter your password : "));
    idlg->setTextValue("");
    idlg->setLabelText(tr("Password : "));
    QLineEdit::EchoMode echoMode = QLineEdit::Password;
    idlg->setTextEchoMode(echoMode);

    frame->setGeometry(50, -100, mw_one->width() - 100, this->height());
    frame->show();

    if (QDialog::Accepted == idlg->exec()) {
      ok = true;
      text = idlg->textValue();
      frame->close();
    } else {
      frame->close();
      return;
    }

    if (ok && !text.isEmpty()) {
      if (text.trimmed() == strPw) {
        showNotes();

      } else {
        QMessageBox msgBox;
        msgBox.setText("Knot");
        msgBox.setInformativeText(tr("The entered password does not match."));
        QPushButton *btnOk =
            msgBox.addButton(tr("Ok"), QMessageBox::AcceptRole);
        btnOk->setFocus();
        msgBox.exec();
        return;
      }
    }

  } else {
    showNotes();
  }

  if (mw_one->isHardStepSensor == 1) mw_one->updateHardSensorSteps();
}

void MainWindow::showNotes() {
  mydlgFloatFun->close();
  isMemoVisible = true;
  isReaderVisible = false;

  QFont f(this->font());
  f.setPointSize(fontSize);
  m_Notes->ui->editSource->setFont(f);

  memoHeight = ui->qwNotes->height();

  m_Notes->ui->btnUndo->setEnabled(false);
  m_Notes->ui->btnRedo->setEnabled(false);

  ui->frameMain->hide();
  ui->frameSetKey->hide();
  ui->frameNotes->show();
  m_Notes->setVPos();

  return;

  m_NotesList->close();
  ui->btnNotesList->click();
  m_NotesList->on_treeWidget_itemClicked(
      m_NotesList->ui->treeWidget->currentItem(), 0);
  m_NotesList->ui->btnClose->click();
}

QString MainWindow::decMemos(QString strDec, QString file) {
  QString text;
  QTextEdit *edit = new QTextEdit;
  edit->setPlainText(strDec);
  TextEditToFile(edit, file);
  if (QFile(file).exists()) {
    m_Notes->decode(file);
    text = m_Notes->Deciphering(file);

    QFile::remove(file);
  }

  return text;
}

void MainWindow::init_Sensors() {
  accel_pedometer = new SpecialAccelerometerPedometer(this);
  // connect(accel_pedometer, SIGNAL(readingChanged()), this,
  // SLOT(newDatas()));
  connect(accel_pedometer, SIGNAL(stepCountChanged()), this,
          SLOT(updateSteps()));

  accel_pedometer->setTangentLineIntercept(
      ui->editTangentLineIntercept->text().toFloat());
  accel_pedometer->setTangentLineSlope(
      ui->editTangentLineSlope->text().toFloat());
  accel_pedometer->setDataRate(100);
  accel_pedometer->setAccelerationMode(QAccelerometer::User);
  accel_pedometer->setAlwaysOn(true);

  gyroscope = new QGyroscope(this);
}

void MainWindow::initQW() {
  ui->qwReader->rootContext()->setContextProperty("myW", this->width());
  ui->qwReader->rootContext()->setContextProperty("myH", this->height());
  ui->qwReader->rootContext()->setContextProperty("mw_one", mw_one);

  ui->qw_Img->rootContext()->setContextProperty("myW", this->width());
  ui->qw_Img->rootContext()->setContextProperty("myH", this->height());

  ui->qwNotes->rootContext()->setContextProperty("m_Notes", m_Notes);
  ui->qwNotes->rootContext()->setContextProperty("FontSize", fontSize);
  ui->qwNotes->rootContext()->setContextProperty("strText", "");
  ui->qwNotes->setSource(QUrl(QStringLiteral("qrc:/src/qmlsrc/notes.qml")));

  ui->qwTodo->rootContext()->setContextProperty("m_Todo", m_Todo);
  ui->qwTodo->rootContext()->setContextProperty("FontSize", fontSize);
  ui->qwTodo->setSource(QUrl(QStringLiteral("qrc:/src/qmlsrc/todo.qml")));

  ui->qwRecycle->rootContext()->setContextProperty("FontSize", fontSize);
  ui->qwRecycle->setSource(
      QUrl(QStringLiteral("qrc:/src/qmlsrc/todorecycle.qml")));

  ui->qwMain->rootContext()->setContextProperty("mw_one", mw_one);
  ui->qwMain->setSource(QUrl(QStringLiteral("qrc:/src/qmlsrc/main.qml")));

  ui->qwSteps->rootContext()->setContextProperty("myW", this->width());
  ui->qwSteps->setSource(QUrl(QStringLiteral("qrc:/src/qmlsrc/steps.qml")));

  ui->qwReport->rootContext()->setContextProperty("m_Report", m_Report);
  ui->qwReport->setSource(QUrl(QStringLiteral("qrc:/src/qmlsrc/report.qml")));
  ui->qwReportSub->setSource(
      QUrl(QStringLiteral("qrc:/src/qmlsrc/details.qml")));

  ui->qwOneDriver->rootContext()->setContextProperty("m_CloudBackup",
                                                     m_CloudBackup);

  ui->qwSearch->rootContext()->setContextProperty("m_Method", m_Method);
  ui->qwSearch->setSource(QUrl(QStringLiteral("qrc:/src/qmlsrc/search.qml")));

  ui->qwBakList->rootContext()->setContextProperty("m_Method", m_Method);
  ui->qwBakList->setSource(QUrl(QStringLiteral("qrc:/src/qmlsrc/baklist.qml")));

  ui->qwViewCate->rootContext()->setContextProperty("m_Report", m_Report);
  ui->qwViewCate->setSource(
      QUrl(QStringLiteral("qrc:/src/qmlsrc/viewcate.qml")));

  ui->qwTabRecycle->rootContext()->setContextProperty("m_Report", m_Report);
  ui->qwTabRecycle->setSource(
      QUrl(QStringLiteral("qrc:/src/qmlsrc/tabrecycle.qml")));

  ui->qwNoteBook->rootContext()->setContextProperty("m_Method", m_Method);
  ui->qwNoteBook->setSource(
      QUrl(QStringLiteral("qrc:/src/qmlsrc/notebook.qml")));

  ui->qwNoteList->rootContext()->setContextProperty("m_Method", m_Method);
  ui->qwNoteList->setSource(
      QUrl(QStringLiteral("qrc:/src/qmlsrc/notelist.qml")));

  ui->qwNoteRecycle->rootContext()->setContextProperty("m_Method", m_Method);
  ui->qwNoteRecycle->setSource(
      QUrl(QStringLiteral("qrc:/src/qmlsrc/noterecycle.qml")));

  ui->qwMainDate->rootContext()->setContextProperty("m_Method", m_Method);
  ui->qwMainDate->setSource(
      QUrl(QStringLiteral("qrc:/src/qmlsrc/maindate.qml")));

  ui->qwMainEvent->rootContext()->setContextProperty("m_Method", m_Method);
  ui->qwMainEvent->setSource(
      QUrl(QStringLiteral("qrc:/src/qmlsrc/mainevent.qml")));

  ui->qwCategory->rootContext()->setContextProperty("m_Method", m_Method);
  ui->qwCategory->setSource(QUrl(QStringLiteral("qrc:/src/qmlsrc/type.qml")));

  ui->qwSelTab->rootContext()->setContextProperty("mw_one", mw_one);
  ui->qwSelTab->setSource(QUrl(QStringLiteral("qrc:/src/qmlsrc/seltab.qml")));

  ui->qwPdf->engine()->addImportPath("qrc:/");
  ui->qwPdf->engine()->addImportPath(":/");
  ui->qwPdf->rootContext()->setContextProperty("mw_one", mw_one);
  if (!isPdfNewMothod)
    ui->qwPdf->setSource(QUrl(QStringLiteral("qrc:/pdf_module/PdfPage.qml")));
  else
    ui->qwPdf->setSource(QUrl(QStringLiteral("qrc:/src/qmlsrc/pdf.qml")));
}

void MainWindow::init_UIWidget() {
  set_btnStyle(this);
  tabData = new QTabWidget;
  tabData = ui->tabWidget;

  tabChart = new QTabWidget;
  tabChart = ui->tabCharts;

  if (fontname == "") fontname = this->font().family();

  qmlRegisterType<File>("MyModel1", 1, 0, "File");
  qmlRegisterType<DocumentHandler>("MyModel2", 1, 0, "DocumentHandler");

  QFontMetrics fontMetrics(font());
  int nFontHeight = fontMetrics.height();
  ui->tabWidget->tabBar()->setFixedHeight(nFontHeight * 1.5);
  ui->tabWidget->setStyleSheet(ui->tabCharts->styleSheet());
  ui->tabWidget->setFixedHeight(ui->tabWidget->tabBar()->height() + 0);

  connect(pAndroidKeyboard, &QInputMethod::visibleChanged, this,
          &MainWindow::on_KVChanged);

  mw_one = this;
  listSelFont = new QListWidget();
  loginTime = QDateTime::currentDateTime().toString();

  strDate = QDate::currentDate().toString("ddd MM dd yyyy");
  isReadEnd = true;

  ui->lblIcon->hide();
  ui->lblKnot->hide();
  ui->frameReader->hide();
  ui->frameTodo->hide();
  ui->frameTodoRecycle->hide();
  ui->frameSteps->hide();
  ui->frameDebug->hide();
  ui->frameReport->hide();
  ui->frameSearch->hide();
  ui->frameBakList->hide();
  ui->qwPdf->hide();
  ui->frameViewCate->hide();
  ui->frameTabRecycle->hide();
  ui->frameNoteList->hide();
  ui->frameNoteRecycle->hide();
  ui->f_FindNotes->hide();
  ui->btnFindNextNote->setEnabled(false);
  ui->btnFindPreviousNote->setEnabled(false);
  ui->qwMain->hide();
  ui->frameCategory->hide();
  ui->frameSetTab->hide();

  ui->frameReader->layout()->setContentsMargins(0, 0, 0, 1);
  ui->frameReader->setContentsMargins(0, 0, 0, 1);
  ui->frameReader->layout()->setSpacing(1);
  ui->f_ImgView->hide();

  ui->frameMain->layout()->setContentsMargins(1, 0, 1, 0);
  ui->frameMain->setContentsMargins(1, 0, 1, 0);
  ui->frameMain->layout()->setSpacing(1);

  ui->frameOne->hide();
  ui->frameFunWeb->hide();
  ui->btnStorageInfo->hide();
  ui->editCode->setLineWrapMode(QTextEdit::NoWrap);

  ui->frameNotes->hide();
  ui->frameNotes->layout()->setContentsMargins(1, 1, 1, 1);

  ui->edit1->setEchoMode(QLineEdit::EchoMode::Password);
  ui->edit2->setEchoMode(QLineEdit::EchoMode::Password);

  ui->centralwidget->layout()->setContentsMargins(1, 0, 1, 2);
  ui->centralwidget->layout()->setSpacing(1);
  ui->frame_charts->setContentsMargins(0, 0, 0, 0);

  ui->frame_charts->layout()->setContentsMargins(0, 0, 0, 0);
  ui->frame_charts->layout()->setSpacing(0);
  frameChartHeight = 150;
  ui->frame_charts->setFixedHeight(frameChartHeight);
  tabChart->setCurrentIndex(0);

  ui->frame_tab->layout()->setContentsMargins(0, 0, 0, 0);
  ui->frame_tab->setContentsMargins(0, 0, 0, 0);
  ui->frame_tab->layout()->setSpacing(1);

  this->installEventFilter(this);
  ui->textBrowser->installEventFilter(this);
  ui->textBrowser->setMouseTracking(true);
  ui->textBrowser->viewport()->installEventFilter(this);
  ui->textBrowser->viewport()->setMouseTracking(true);
  ui->qwReader->installEventFilter(this);
  ui->qwPdf->installEventFilter(this);
  ui->tabWidget->tabBar()->installEventFilter(this);
  ui->tabWidget->installEventFilter(this);
  ui->frame_tab->setMouseTracking(true);
  ui->tabWidget->setMouseTracking(true);
  ui->lblStats->installEventFilter(this);
  ui->editSearchText->installEventFilter(this);
  ui->editFindNote->installEventFilter(this);

  myfile = new File();
  m_Remarks = new dlgRemarks(this);
  m_Remarks->ui->textEdit->verticalScrollBar()->setStyleSheet(vsbarStyleSmall);

  myEditRecord = new EditRecord(this);
  m_Todo = new dlgTodo(this);
  m_Todo->setStyleSheet(vsbarStyleSmall);
  m_Report = new dlgReport(this);
  mydlgPre = new dlgPreferences(this);
  m_Notes = new Notes(this);
  m_Steps = new dlgSteps(this);
  ui->lblStats->adjustSize();
  ui->lblStats->setWordWrap(true);
  m_Reader = new dlgReader(this);
  m_TodoAlarm = new TodoAlarm(this);
  m_CloudBackup = new CloudBackup;
  mydlgFloatFun = new dlgFloatFun(this);
  mydlgFloatFun->close();
  mydlgReaderFun = new dlgReaderFun(this);
  mydlgSetText = new dlgSetText(this);
  m_widget = new QWidget(this);
  m_widget->close();
  m_NotesList = new dlgNotesList(this);
  m_SyncInfo = new SyncInfo(this);
  dlgTimeMachine = new QFrame();
  dlgTimeMachine->close();
  m_Method = new Method(this);

  timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
  timerStep = new QTimer(this);
  connect(timerStep, SIGNAL(timeout()), this, SLOT(updateHardSensorSteps()));
  timerSyncData = new QTimer(this);
  connect(timerSyncData, SIGNAL(timeout()), this, SLOT(on_timerSyncData()));
  timerMousePress = new QTimer(this);
  connect(timerMousePress, SIGNAL(timeout()), this, SLOT(on_timerMousePress()));

  ui->statusbar->setHidden(true);

  myReadEBookThread = new ReadEBookThread();
  connect(myReadEBookThread, &ReadEBookThread::isDone, this,
          &MainWindow::readEBookDone);

  myReadTWThread = new ReadTWThread();
  connect(myReadTWThread, &ReadTWThread::isDone, this, &MainWindow::readTWDone);

  myReadThread = new ReadThread();
  connect(myReadThread, &ReadThread::isDone, this, &MainWindow::readChartDone);

  mySaveThread = new SaveThread();
  connect(mySaveThread, &SaveThread::isDone, this, &MainWindow::saveDone);

  myBakDataThread = new BakDataThread();
  connect(myBakDataThread, &BakDataThread::isDone, this,
          &MainWindow::bakDataDone);

  myImportDataThread = new ImportDataThread();
  connect(myImportDataThread, &ImportDataThread::isDone, this,
          &MainWindow::importDataDone);

  mySearchThread = new SearchThread();
  connect(mySearchThread, &SearchThread::isDone, this, &MainWindow::searchDone);

  ui->progBar->setMaximumHeight(4);
  ui->progBar->hide();
  ui->progBar->setStyleSheet(
      "QProgressBar{border:0px solid #FFFFFF;"
      "height:30;"
      "background:rgba(25,255,25,0);"
      "text-align:right;"
      "color:rgb(255,255,255);"
      "border-radius:0px;}"

      "QProgressBar:chunk{"
      "border-radius:0px;"
      "background-color:rgba(18,150,219,255);"
      "}");
  ui->progReader->setStyleSheet(ui->progBar->styleSheet());
  ui->progReader->setFixedHeight(4);

  setLineEditQss(ui->edit1, 0, 1, "#4169E1", "#4169E1");
  ui->edit2->setStyleSheet(ui->edit1->styleSheet());

  if (isIOS) {
  }

  ui->frame_tab->setMaximumHeight(this->height());

  ui->frameMenu->setStyleSheet("background-color: rgb(243,243,243);");
  ui->btnFind->setStyleSheet("border:none");
  ui->btnMenu->setStyleSheet("border:none");
  ui->btnRemarks->setStyleSheet("border:none");

  ui->btnPause->setStyleSheet("border:none");
#ifdef Q_OS_ANDROID
#else
  ui->btnPause->hide();
#endif

  ui->btnSelTab->setStyleSheet("border:none");
  ui->btnTodo->setStyleSheet("border:none");
  ui->btnSteps->setStyleSheet("border:none");
  ui->btnMax->setStyleSheet("border:none");
  ui->btnReader->setStyleSheet("border:none");
  ui->btnNotes->setStyleSheet("border:none");
  ui->btnAdd->setStyleSheet("border:none");
  ui->btnDel->setStyleSheet("border:none");
  ui->btnReport->setStyleSheet("border:none");
  ui->btnPasteTodo->setStyleSheet("border:none");
  ui->btnSync->setStyleSheet("border:none");

  QFont f = this->font();
  f.setPointSize(11);
  ui->btnTodo->setFont(f);
  ui->btnSteps->setFont(f);
  ui->btnMax->setFont(f);
  ui->btnReader->setFont(f);
  ui->btnNotes->setFont(f);

  QString lblStyle = myEditRecord->ui->lblTitle->styleSheet();
  ui->lblTotal->setStyleSheet(lblStyle);
  ui->lblDetails->setStyleSheet(lblStyle);
  ui->lblTitle->setStyleSheet(lblStyle);
  ui->lblTitle_Report->setStyleSheet(lblStyle);
  ui->lblStats->setStyleSheet(lblStyle);

  ui->lblIcon->setText("");
  ui->lblIcon->setFixedHeight(22);
  ui->lblIcon->setFixedWidth(22);
  ui->lblIcon->setStyleSheet(
      "QLabel{"
      "border-image:url(:/res/icon.png) 4 4 4 4 stretch stretch;"
      "}");

  initQW();
}

void MainWindow::selTab() {
  int index = m_Method->getCurrentIndexBakList(ui->qwSelTab);
  tabData->setCurrentIndex(index);
  on_btnBackSetTab_clicked();
}

void MainWindow::on_btnSelTab_clicked() {
  ui->frameMain->hide();
  ui->frameSetTab->show();
  m_Method->clearAllBakList(ui->qwSelTab);
  int tab_count = tabData->tabBar()->count();
  for (int i = 0; i < tab_count; i++) {
    QString text0 = tabData->tabText(i);
    m_Method->addItemBakList(ui->qwSelTab, text0, "", "", "", 0);
  }

  int index = ui->tabWidget->currentIndex();
  m_Method->setCurrentIndexBakList(ui->qwSelTab, index);

  ui->lblSelTabInfo->setText(tr("Total") + " : " + QString::number(tab_count) +
                             " ( " + QString::number(index + 1) + " ) ");

  return;

  mydlgFloatFun->close();
  m_widget = new QWidget(this);

  QDialog *dlg = new QDialog(this);
  QVBoxLayout *vbox0 = new QVBoxLayout;
  dlg->setLayout(vbox0);

  dlg->setModal(true);
  dlg->setWindowFlag(Qt::FramelessWindowHint);
  dlg->setAttribute(Qt::WA_TranslucentBackground);

  QFrame *frame = new QFrame(this);
  vbox0->addWidget(frame);
  frame->setStyleSheet(
      "QFrame{background-color: rgb(255, 255, "
      "255);border-radius:10px;border:1px solid gray;}");

  QVBoxLayout *vbox = new QVBoxLayout;
  frame->setLayout(vbox);
  vbox->setContentsMargins(1, 1, 1, 1);

  QListWidget *list = new QListWidget(this);
  vbox->addWidget(list);
  list->setSpacing(8);
  list->setAlternatingRowColors(false);
  list->setViewMode(QListView::IconMode);
  list->setMovement(QListView::Static);
  list->setStyleSheet(listStyleMain);
  list->verticalScrollBar()->setStyleSheet(vsbarStyleSmall);
  list->setVerticalScrollMode(QListWidget::ScrollPerPixel);
  QScroller::grabGesture(list, QScroller::LeftMouseButtonGesture);
  setSCrollPro(list);
  QFont font;
  font.setPointSize(fontSize + 3);
  list->setFont(font);

  QLabel *lblTotal = new QLabel;
  vbox->addWidget(lblTotal);

  int count = tabData->tabBar()->count();
  for (int i = 0; i < count; i++) {
    QListWidgetItem *item = new QListWidgetItem;
    item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    item->setText(tabData->tabText(i));
    list->addItem(item);
  }
  connect(list, &QListWidget::itemClicked, [=]() {
    dlg->close();
    tabData->setCurrentIndex(list->currentRow());
  });
  connect(dlg, &QDialog::rejected, [=]() { dlg->close(); });

  int h = geometry().height() * 3 / 4;
  int w = geometry().width() - 10;
  int y = geometry().y() + (this->height() - h) / 2;
  int x = geometry().x() + (this->width() - w) / 2;
  dlg->setGeometry(x, y, w, h);
  list->setCurrentRow(tabData->currentIndex());
  lblTotal->setText(tr("Total") + " : " + QString::number(list->count()) +
                    " ( " + QString::number(list->currentRow() + 1) + " ) ");

  dlg->show();
  list->setFocus();
}

void MainWindow::init_Menu(QMenu *mainMenu) {
  QAction *actAddTab = new QAction(tr("Add Tab"));
  QAction *actDelTab = new QAction(tr("Del Tab"));
  QAction *actRenameTab = new QAction(tr("Rename Tab"));

  QAction *actOpenKnotBakDir = new QAction(tr("Open KnotBak Dir"));

  QAction *actReport = new QAction(tr("Report"));
  actReport->setVisible(false);

  QAction *actExportData = new QAction(tr("Export Data"));
  QAction *actImportData = new QAction(tr("Import Data"));

  QAction *actPreferences = new QAction(tr("Preferences"));

  QAction *actAbout = new QAction(tr("About") + " (" + ver + ")");
  QAction *actOneDrive = new QAction(tr("OneDrive Backup Data"));

  QAction *actUndo = new QAction(tr("Undo"));
  QAction *actRedo = new QAction(tr("Redo"));
  actUndo->setVisible(false);
  actRedo->setVisible(false);

  QAction *actBakFileList = new QAction(tr("Backup File List"));
  QAction *actTabRecycle = new QAction(tr("Tab Recycle"));

  connect(actAddTab, &QAction::triggered, this,
          &MainWindow::on_actionAdd_Tab_triggered);
  connect(actDelTab, &QAction::triggered, this,
          &MainWindow::on_actionDel_Tab_triggered);
  connect(actRenameTab, &QAction::triggered, this,
          &MainWindow::on_actionRename_triggered);

  connect(actUndo, &QAction::triggered, this, &MainWindow::undo);
  connect(actRedo, &QAction::triggered, this, &MainWindow::redo);

  connect(actBakFileList, &QAction::triggered, this,
          &MainWindow::on_actionBakFileList);

  connect(actTabRecycle, &QAction::triggered, this,
          &MainWindow::on_actionTabRecycle);

  connect(actOpenKnotBakDir, &QAction::triggered, this,
          &MainWindow::on_openKnotBakDir);
  connect(actReport, &QAction::triggered, this,
          &MainWindow::on_actionReport_triggered);

  connect(actExportData, &QAction::triggered, this,
          &MainWindow::on_actionExport_Data_triggered);

  connect(actImportData, &QAction::triggered, this,
          &MainWindow::on_actionImport_Data_triggered);
  connect(actPreferences, &QAction::triggered, this,
          &MainWindow::on_actionPreferences_triggered);

  connect(actOneDrive, &QAction::triggered, this,
          &MainWindow::on_actionOneDriveBackupData);
  connect(actAbout, &QAction::triggered, this, &MainWindow::on_about);

  QString qss =
      "QMenu {"
      "border: 1px solid rgb(172, 172, 172);"
      "border-radius: 3px; }"
      "QMenu::item {"
      "border-bottom: 1px solid rgb(172, 172, 172);"
      "padding:10px 10px;"
      "margin:0px 0px; }"
      "QMenu::item:selected {"
      "background-color: rgb(62, 186, 231); }";

  mainMenu->addAction(actAddTab);
  mainMenu->addAction(actDelTab);
  mainMenu->addAction(actRenameTab);

  mainMenu->addAction(actReport);

  mainMenu->addAction(actUndo);
  mainMenu->addAction(actRedo);

  mainMenu->addAction(actExportData);
  mainMenu->addAction(actImportData);

#ifdef Q_OS_ANDROID
  mainMenu->addAction(actOpenKnotBakDir);
  actOpenKnotBakDir->setVisible(false);
#else
  if (!mydlgPre->devMode) {
    actAddTab->setVisible(false);
    actDelTab->setVisible(false);
    actRenameTab->setVisible(false);
    actBakFileList->setVisible(false);
    actExportData->setVisible(false);
    actImportData->setVisible(false);
    actOneDrive->setVisible(false);
    actTabRecycle->setVisible(false);
  }
#endif

  mainMenu->addAction(actPreferences);

  mainMenu->addAction(actOneDrive);
  mainMenu->addAction(actBakFileList);
  mainMenu->addAction(actTabRecycle);
  mainMenu->addAction(actAbout);

  mainMenu->setStyleSheet(qss);
}

void MainWindow::on_openKnotBakDir() {
#ifdef Q_OS_ANDROID

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  QAndroidJniObject m_activity = QtAndroid::androidActivity();
  m_activity.callMethod<void>("openKnotBakDir");
#else
  QJniObject m_activity = QNativeInterface::QAndroidApplication::context();
  m_activity.callMethod<void>("openKnotBakDir");
#endif

#endif
}

void MainWindow::on_actionOneDriveBackupData() {
  floatfun = false;
  mydlgFloatFun->close();

  ui->frameMain->hide();
  ui->frameReader->hide();
  ui->frameOne->show();
  delete m_CloudBackup;
  m_CloudBackup = new CloudBackup;
  m_CloudBackup->loadLogQML();
}

void MainWindow::undo() {
  importBakData(iniDir + "undoFile", true, false, true);
}

void MainWindow::redo() {
  importBakData(iniDir + "redoFile", true, false, true);
}

void MainWindow::addUndo(QString log) {
  QString undoFile = privateDir + m_Notes->getDateTimeStr();
  bakIniData(undoFile, true);

  for (int i = 0; i < timeLines.count(); i++) {
    QString str = timeLines.at(i);
    if (str.contains(LatestTime)) {
      timeLines.removeAt(i);
      break;
    }
  }

  timeLines.insert(0, undoFile + "\n" + log);
  timeLines.insert(0, privateDir + LatestTime);
  int count = timeLines.count();
  if (count > 50) {
    count = 50;
    QString str = timeLines.at(count);
    QString oldFile = str.split("\n").at(0);
    if (QFile().remove(oldFile))
      qDebug() << oldFile << "addUndo del oldFile...";
    timeLines.removeAt(count);
  }
  QSettings Reg(privateDir + "timemachine.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif
  Reg.setValue("/TimeLines/Count", count);
  for (int i = 0; i < count; i++)
    Reg.setValue("/TimeLines/Files" + QString::number(i), timeLines.at(i));
}

void MainWindow::addRedo() {
  QString redoFile = privateDir + LatestTime;
  bakIniData(redoFile, true);
}

void MainWindow::on_actionTabRecycle() {
  ui->frameMain->hide();
  ui->frameTabRecycle->show();

  m_Method->clearAllBakList(ui->qwTabRecycle);

  QString tab_name, tab_time;
  QStringList iniFiles;
  QStringList fmt;
  fmt.append("ini");
  m_NotesList->getAllFiles(iniDir, iniFiles, fmt);
  for (int i = 0; i < iniFiles.count(); i++) {
    QString ini_file = iniFiles.at(i);
    if (ini_file.contains("recycle_")) {
      QFileInfo fi(ini_file);
      QString ini_filename = fi.fileName();
      ini_filename = ini_filename.replace(".ini", "");
      tab_name = ini_filename.split("_").at(1);
      tab_time =
          ini_filename.split("_").at(2) + "  " + ini_filename.split("_").at(3);

      m_Method->addItemBakList(ui->qwTabRecycle, tab_name, tab_time, "",
                               ini_file, 0);
    }
  }

  int t_count = m_Method->getCountBakList(ui->qwTabRecycle);
  if (t_count > 0) {
    m_Method->setCurrentIndexBakList(ui->qwTabRecycle, 0);
  }

  ui->lblTitleTabRecycle->setText(tr("Tab Recycle") + "    " + tr("Total") +
                                  " : " + QString::number(t_count));
}

void MainWindow::on_actionBakFileList() {
  ui->frameMain->hide();
  ui->frameBakList->show();

  m_Method->clearAllBakList(ui->qwBakList);

  QStringList bakFileList = mydlgPre->getBakFilesList();
  int bakCount = bakFileList.count();

  for (int i = 0; i < bakCount; i++) {
    QString action, bakfile;
    QString str = bakFileList.at(bakCount - 1 - i);
    action = str.split("-===-").at(0);
    bakfile = str.split("-===-").at(1);

    QString item = action + "\n" + getFileSize(QFile(bakfile).size(), 2);

    m_Method->addItemBakList(ui->qwBakList, item, "", "", bakfile, 0);
  }

  if (m_Method->getCountBakList(ui->qwBakList) > 0)
    m_Method->setCurrentIndexBakList(ui->qwBakList, 0);

  ui->lblBakListTitle->setText(
      tr("Backup File List") + "    " + tr("Total") + " : " +
      QString::number(m_Method->getCountBakList(ui->qwBakList)));
}

void MainWindow::on_btnMenu_clicked() {
  QMenu *mainMenu = new QMenu(this);
  init_Menu(mainMenu);
  int x = mw_one->geometry().x() + 2;
  int y = geometry().y() + ui->frameMenu->height() + 2;
  QPoint pos(x, y);
  mainMenu->exec(pos);
}

void MainWindow::on_btnZoom_clicked() {
  if (!ui->frame_tab->isHidden()) {
    axisY->setTickCount(13);
    axisY2->setTickCount(13);
    ui->frame_tab->hide();
    ui->frame_charts->setMaximumHeight(this->height());
    floatfun = false;
    mydlgFloatFun->close();
  } else {
    axisY->setTickCount(yScale);
    axisY2->setTickCount(yScale);
    ui->frame_tab->show();
    ui->frame_charts->setFixedHeight(frameChartHeight);
  }
}

void MainWindow::stopJavaTimer() {
#ifdef Q_OS_ANDROID

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  QAndroidJniObject jo = QAndroidJniObject::fromString("StopWin");
  jo.callStaticMethod<int>("com.x/MyService", "stopTimer", "()I");
#else
  QJniObject jo = QJniObject::fromString("StopWin");
  jo.callStaticMethod<int>("com.x/MyService", "stopTimer", "()I");
#endif

#endif
  accel_pedometer->stop();
  accel_pedometer->setActive(false);
  gyroscope->stop();
  gyroscope->setActive(false);
}

#ifdef Q_OS_ANDROID
static void JavaNotify_0() {
  mw_one->m_SyncInfo->runSync("");
  // qDebug() << "C++ JavaNotify_0";
}

static void JavaNotify_1() {
  mw_one->newDatas();
  // qDebug() << "C++ JavaNotify_1";
}

static void JavaNotify_2() {
  if (!mw_one->ui->frameMain->isHidden()) {
    // When the screen lights up.
    mw_one->updateHardSensorSteps();
  }
  qDebug() << "C++ JavaNotify_2";
}

static void JavaNotify_3() {
  mw_one->alertWindowsCount++;
  mw_one->m_Todo->refreshAlarm();
  qDebug() << "C++ JavaNotify_3";
}

static void JavaNotify_4() {
  mw_one->alertWindowsCount--;

  return;

  if (mw_one->alertWindowsCount == 0) {
    if (!mw_one->m_Report->myDateSelector->isHidden())
      mw_one->m_Report->myDateSelector->close();

    if (!mw_one->ui->frameViewCate->isHidden()) {
      mw_one->ui->frameViewCate->hide();
      mw_one->ui->frameReport->show();
    }

    if (!mw_one->ui->frameReport->isHidden())
      mw_one->ui->btnBack_Report->click();

    if (!mw_one->ui->frameReader->isHidden()) mw_one->ui->btnBack->click();

    if (!mw_one->ui->frameNoteRecycle->isHidden())
      mw_one->on_btnBackNoteRecycle_clicked();

    if (!mw_one->ui->frameNoteList->isHidden())
      mw_one->on_btnBackNoteList_clicked();

    if (!mw_one->ui->frameNotes->isHidden()) mw_one->on_btnNotes_clicked();

    if (!mw_one->ui->frameOne->isHidden()) mw_one->ui->btnBack_One->click();

    if (!mw_one->ui->frameSteps->isHidden()) mw_one->ui->btnBackSteps->click();

    if (!mw_one->ui->frameBakList->isHidden())
      mw_one->ui->btnBackBakList->click();

    if (!mw_one->ui->frameTabRecycle->isHidden())
      mw_one->ui->btnBackTabRecycle->click();

    mw_one->ui->btnTodo->click();
  }

  qDebug() << "alertWindowsCount=" << mw_one->alertWindowsCount;
  qDebug() << "C++ JavaNotify_4";
}

static const JNINativeMethod gMethods[] = {
    {"CallJavaNotify_0", "()V", (void *)JavaNotify_0},
    {"CallJavaNotify_1", "()V", (void *)JavaNotify_1},
    {"CallJavaNotify_2", "()V", (void *)JavaNotify_2},
    {"CallJavaNotify_3", "()V", (void *)JavaNotify_3},
    {"CallJavaNotify_4", "()V", (void *)JavaNotify_4}};

void RegJni(const char *myClassName) {
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  QtAndroid::runOnAndroidThreadSync([=]() {
    QAndroidJniEnvironment Environment;
    const char *mClassName = myClassName;  //"com/x/MyService";
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
#else
  QNativeInterface::QAndroidApplication::runOnAndroidMainThread([=]() {
    QJniEnvironment Environment;
    const char *mClassName = myClassName;  //"com/x/MyService";
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
#endif
}

#endif

void MainWindow::on_btnPause_clicked() { ui->btnPauseSteps->click(); }

QString MainWindow::getYMD(QString date) {
  QStringList list = date.split(" ");
  QString str;
  if (list.count() == 4) {
    str = list.at(1) + list.at(2) + list.at(3);
  }
  return str;
}

void MainWindow::on_btnReader_clicked() {
  floatfun = false;
  mydlgFloatFun->close();
  isReaderVisible = true;
  isMemoVisible = false;

  if (!isOne) {
    mwh = this->height();
    setFixedHeight(mwh);
    ui->qwReader->rootContext()->setContextProperty("myW", this->width());
    ui->qwReader->rootContext()->setContextProperty("myH", mwh);
  }

  if (isPDF) m_Reader->setPdfViewVisible(true);

  ui->frameMain->hide();
  ui->frameReader->show();

  if (!isOne) {
    isOne = true;
    m_Reader->setPageVPos();
  }

  if (mw_one->isHardStepSensor == 1) mw_one->updateHardSensorSteps();
}

void MainWindow::setSCrollPro(QObject *obj) {
  QScrollerProperties sp;
  sp.setScrollMetric(QScrollerProperties::DragStartDistance, 0.001);
  sp.setScrollMetric(QScrollerProperties::ScrollingCurve, QEasingCurve::Linear);
  QScroller *qs = QScroller::scroller(obj);
  qs->setScrollerProperties(sp);
}

void MainWindow::on_btnBack_clicked() {
  mydlgReaderFun->close();
  m_Reader->setPdfViewVisible(false);

  if (isSelText) on_btnSelText_clicked();

  m_Reader->saveReader();
  m_Reader->savePageVPos();

  ui->frameReader->hide();
  ui->frameMain->show();
}

void MainWindow::on_btnOpen_clicked() {
  mw_one->mydlgReaderFun->close();
  m_Reader->on_btnOpen_clicked();
}

void MainWindow::on_btnPageUp_clicked() { m_Reader->on_btnPageUp_clicked(); }

void MainWindow::on_btnPageNext_clicked() {
  m_Reader->on_btnPageNext_clicked();
}

void MainWindow::on_btnPages_clicked() {
  if (!mydlgReaderFun->isHidden()) {
    mydlgReaderFun->close();
  } else
    mydlgReaderFun->init();
}

void MainWindow::on_hSlider_sliderMoved(int position) {
  if (isText) {
    ui->btnPages->setText(tr("Pages") + "\n" + QString::number(position + 1) +
                          "\n" + QString::number(totallines / baseLines));
    ui->progReader->setMinimum(0);
    ui->progReader->setMaximum(totallines / baseLines);
    ui->progReader->setValue(position + 1);
  }

  if (isEpub) {
    ui->btnPages->setText(tr("Pages") + "\n" + QString::number(position) +
                          "\n" + QString::number(htmlFiles.count()));
    ui->progReader->setMinimum(1);
    ui->progReader->setMaximum(htmlFiles.count());
    if (position == 0) position = 1;
    ui->progReader->setValue(position);
  }
}

void MainWindow::on_btnReadList_clicked() {
  mydlgReaderFun->close();
  m_Reader->getReadList();
}

void MainWindow::on_btnBackDir_clicked() { m_Reader->backDir(); }

QString MainWindow::getTabText() {
  return tabData->tabText(tabData->currentIndex());
}

void MainWindow::refreshMainUI() {
  this->update();
  this->repaint();
  qApp->processEvents();
}

bool MainWindow::showMsgBox(QString title, QString info, QString copyText,
                            int buttonCount) {
  QMessageBox msgBox;
  QPushButton *btnCopy = NULL;
  QPushButton *btnCancel = NULL;
  QPushButton *btnOk = NULL;

  msgBox.setText(title);
  msgBox.setInformativeText(info);

  QPixmap pix(":/res/info.png");
  pix.setDevicePixelRatio(8);
  msgBox.setIconPixmap(pix);

  if (buttonCount >= 1) {
    btnOk = msgBox.addButton(tr("Ok"), QMessageBox::AcceptRole);
    btnOk->setFixedHeight(35);
    btnOk->setFixedWidth(100);
    btnOk->setStyleSheet(pushbtnStyle);
  }

  if (buttonCount >= 2) {
    btnCancel = msgBox.addButton(tr("Cancel"), QMessageBox::RejectRole);

    btnCancel->setFixedHeight(35);
    btnCancel->setFixedWidth(100);
    btnCancel->setStyleSheet(pushbtnStyle);
  }

  if (buttonCount == 3) {
    btnCopy = msgBox.addButton(tr("Copy"), QMessageBox::AcceptRole);
    btnCopy->setFixedHeight(35);
    btnCopy->setFixedWidth(100);
    btnCopy->setStyleSheet(pushbtnStyle);
  }

  if (buttonCount >= 1) btnOk->setFocus();

  msgBox.exec();
  if (buttonCount >= 2) {
    if (msgBox.clickedButton() == btnCancel) {
      return false;
    }
  }

  if (msgBox.clickedButton() == btnCopy && buttonCount == 3) {
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(copyText);
    return false;
  }

  return true;
}

void MainWindow::on_btnSelText_clicked() {
  mydlgReaderFun->close();

  if (!isSelText) {
    ui->btnSelText->setIcon(QIcon(":/res/choice1.png"));
    isSelText = true;

    ui->textBrowser->setReadOnly(true);
    QFont font = ui->qwReader->font();
    font.setPixelSize(textFontSize);
    font.setFamily(fontname);
    font.setLetterSpacing(QFont::AbsoluteSpacing, 2);
    ui->textBrowser->setFont(font);

    if (isText) ui->textBrowser->setHtml(m_Reader->currentTxt);

    if (isEpub) {
      QString str = loadText(m_Reader->currentHtmlFile);
      str.replace("..", strOpfPath);
      QDir dir;
      dir.setCurrent(strOpfPath);
      ui->textBrowser->setHtml(str);
    }

    ui->qwReader->hide();
    ui->textBrowser->show();

    m_Reader->getVPos();
    ui->textBrowser->verticalScrollBar()->setSliderPosition(m_Reader->textPos);

    mydlgSetText->setFixedWidth(width() * 2 / 3);
    mydlgSetText->init(geometry().x() + (width() - mydlgSetText->width()) / 2,
                       geometry().y(), mydlgSetText->width(),
                       mydlgSetText->height());

  } else {
    ui->btnSelText->setIcon(QIcon(":/res/choice0.png"));

    isSelText = false;

    ui->textBrowser->hide();
    ui->qwReader->show();

    mydlgSetText->close();
  }
}

void MainWindow::on_btnSignIn_clicked() {
  m_CloudBackup->on_pushButton_SignIn_clicked();
}

void MainWindow::on_btnSignOut_clicked() {
  m_CloudBackup->on_pushButton_SingOut_clicked();
}

void MainWindow::on_btnUpload_clicked() {
  m_CloudBackup->on_pushButton_upload2_clicked();
}

void MainWindow::on_btnDownload_clicked() {
  m_CloudBackup->on_pushButton_downloadFile_clicked();
}

void MainWindow::on_btnBack_One_clicked() {
  if (!ui->frameOne->isHidden()) {
    if (ui->frameOneFun->isHidden()) {
      ui->frameOneFun->show();
      ui->frameFunWeb->hide();

      m_CloudBackup->loadLogQML();
    } else {
      ui->frameOne->hide();
      ui->frameMain->show();
    }
  }
}

void MainWindow::on_btnRefreshToken_clicked() {
  m_CloudBackup->on_pushButton_clicked();
}

void MainWindow::on_btnStorageInfo_clicked() {
  m_CloudBackup->on_pushButton_storageInfo_clicked();
}

void MainWindow::on_btnRefreshWeb_clicked() {
  ui->qwOneDriver->rootContext()->setContextProperty("initialUrl",
                                                     strRefreshUrl);
}

void MainWindow::on_btnUserInfo_clicked() {
  // 获取openssl相关信息并自行编译安装 Qt6.4(1.1.1.m版本的openssl）
  // openssl下载网址：https://www.openssl.org/source/old/
  QNetworkAccessManager *manager = new QNetworkAccessManager(this);
  qDebug() << manager->supportedSchemes();
  qDebug() << QSslSocket::sslLibraryBuildVersionString();

  m_CloudBackup->on_pushButton_GetUserInfo_clicked();
}

void MainWindow::on_btnBackNotes_clicked() {
  m_Notes->saveQMLVPos();

  ui->frameNotes->hide();
  ui->frameMain->show();
  isSelf = false;
  addFilesWatch();
}

void MainWindow::on_btnSetKey_clicked() {
  if (ui->frameSetKey->isHidden())
    ui->frameSetKey->show();
  else
    ui->frameSetKey->hide();
}

void MainWindow::on_btnSetKeyOK_clicked() {
  isSelf = true;

  QSettings Reg(iniDir + "mainnotes.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif
  if (ui->edit1->text().trimmed() == "" && ui->edit2->text().trimmed() == "") {
    Reg.remove("/MainNotes/UserKey");
    ui->frameSetKey->hide();
    QMessageBox msgBox;
    msgBox.setText("Knot");
    msgBox.setInformativeText(tr("The password is removed."));
    QPushButton *btnOk = msgBox.addButton(tr("Ok"), QMessageBox::AcceptRole);
    btnOk->setFocus();
    msgBox.exec();

    return;
  }

  if (ui->edit1->text().trimmed() == ui->edit2->text().trimmed()) {
    QString strPw = ui->edit1->text().trimmed();
    QByteArray baPw = strPw.toUtf8();
    for (int i = 0; i < baPw.size(); i++) {
      baPw[i] = baPw[i] + 66;  // 加密User的密码
    }
    strPw = baPw;
    Reg.setValue("/MainNotes/UserKey", strPw);

    QMessageBox msgBox;
    msgBox.setText("Knot");
    msgBox.setInformativeText(tr("The password is set successfully."));
    QPushButton *btnOk = msgBox.addButton(tr("Ok"), QMessageBox::AcceptRole);
    btnOk->setFocus();
    msgBox.exec();
    ui->frameSetKey->hide();
    ui->edit1->clear();
    ui->edit2->clear();

  } else {
    QMessageBox msgBox;
    msgBox.setText("Knot");
    msgBox.setInformativeText(tr("The entered password does not match."));
    QPushButton *btnOk = msgBox.addButton(tr("Ok"), QMessageBox::AcceptRole);
    btnOk->setFocus();
    msgBox.exec();
  }
}

void MainWindow::on_btnEdit_clicked() {
  isSelf = true;

  m_Notes->m_SetEditText->close();
  delete m_Notes->m_SetEditText;
  m_Notes->m_SetEditText = new dlgSetEditText(m_Notes);
  m_Notes->m_SetEditText->close();

  QSettings Reg(iniDir + "mainnotes.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif

  QString mdfile = mw_one->loadText(m_NotesList->currentMDFile);

  m_Notes->init();
  m_Notes->ui->editSource->setPlainText(mdfile);
  new MarkdownHighlighter(m_Notes->ui->editSource->document());

  mainHeight = mw_one->height();
  ui->frameNotes->hide();
  m_Notes->show();
  m_Notes->isShow = true;

  QString a = m_NotesList->currentMDFile;
  a.replace(iniDir, "");
  int vpos = Reg.value("/MainNotes/editVPos" + a).toInt();
  int cpos = Reg.value("/MainNotes/editCPos" + a).toInt();
  m_Notes->ui->editSource->verticalScrollBar()->setSliderPosition(vpos);
  QTextCursor tmpCursor = m_Notes->ui->editSource->textCursor();
  tmpCursor.setPosition(cpos);
  m_Notes->ui->editSource->setTextCursor(tmpCursor);

  m_Notes->isSave = false;
}

void MainWindow::on_btnCode_clicked() {
  QString str = ui->editCode->toPlainText().trimmed();
  if (str != "" && str.contains("?code=")) {
    dialog_->sendMsg(str);
  }
}

void MainWindow::clearSelectBox() {
  QString tempFile = iniDir + "memo/texteditor.html";
  if (!mw_one->ui->frameReader->isHidden()) {
    mw_one->m_Reader->savePageVPos();
    bool isAni = false;
    mw_one->ui->qwReader->rootContext()->setContextProperty("isAni", isAni);
    QQuickItem *root = mw_one->ui->qwReader->rootObject();
    QMetaObject::invokeMethod((QObject *)root, "loadHtml",
                              Q_ARG(QVariant, tempFile));
    Sleep(50);
    if (isEpub) {
      QMetaObject::invokeMethod(
          (QObject *)root, "loadHtml",
          Q_ARG(QVariant, mw_one->m_Reader->currentHtmlFile));
    } else {
      ui->qwReader->rootContext()->setContextProperty("strText",
                                                      m_Reader->currentTxt);
    }
    mw_one->m_Reader->setPageVPos();
  }

  if (!mw_one->ui->frameNotes->isHidden()) {
    m_Notes->getVPos();
    int pos = m_Notes->sliderPos;
    QQuickItem *root = mw_one->ui->qwNotes->rootObject();
    QMetaObject::invokeMethod((QObject *)root, "loadHtml",
                              Q_ARG(QVariant, tempFile));

    // QMetaObject::invokeMethod((QObject*)root, "loadHtml",
    //                           Q_ARG(QVariant, file));
    QMetaObject::invokeMethod((QObject *)root, "loadHtmlBuffer",
                              Q_ARG(QVariant, m_Notes->htmlBuffer));
    QMetaObject::invokeMethod((QObject *)root, "setVPos", Q_ARG(QVariant, pos));
  }
}

void MainWindow::on_btnCopy_clicked() {
  QClipboard *clipboard = QApplication::clipboard();
  clipboard->setText(mydlgSetText->ui->lineEdit->text().trimmed());
  on_btnCancelSel_clicked();
}

QString MainWindow::getSelectedText() {
  QString str;
  QVariant returnedValue;
  QQuickItem *root = ui->qwReader->rootObject();
  QMetaObject::invokeMethod((QObject *)root, "getSelectedText",
                            Q_RETURN_ARG(QVariant, returnedValue));
  str = returnedValue.toString();
  return str.trimmed();
}

void MainWindow::on_btnSearch_clicked() {
  QString str = mydlgSetText->ui->lineEdit->text().trimmed();
  if (str == "") return;

  QString strurl;
  /*if (zh_cn)
    strurl =
        "https://wap.baidu.com/"
        "s?ie=utf-8&f=8&rsv_bp=1&rsv_idx=1&tn=baidu&wd=" +
        str;
  else*/
  strurl = "https://bing.com/search?q=" + str;

  QUrl url(strurl);
  QDesktopServices::openUrl(url);
  on_btnCancelSel_clicked();
}

void MainWindow::on_btnCancelSel_clicked() { ui->btnSelText->click(); }

void MainWindow::on_textBrowser_selectionChanged() {
  QString str = ui->textBrowser->textCursor().selectedText().trimmed();
  ui->editSetText->setText(str);
  mydlgSetText->ui->lineEdit->setText(str);
}

void MainWindow::on_SetReaderFunVisible() {
  if (!isTurnThePage) {
    if (ui->frameReaderFun->isHidden())
      ui->frameReaderFun->show();
    else {
      ui->frameReaderFun->hide();
      mydlgReaderFun->hide();
    }
  }
}

void MainWindow::on_timerMousePress() {
  timerMousePress->stop();
  if (!isMouseMove && isMousePress) on_btnSelText_clicked();
}

void MainWindow::showGrayWindows() {
  mydlgFloatFun->close();

  m_widget->resize(this->width(), this->height());
  m_widget->move(0, 0);
  m_widget->setStyleSheet("background-color:rgba(0, 0, 0,25%);");

  QPropertyAnimation *m_pAnimation = new QPropertyAnimation();
  m_pAnimation->setTargetObject(m_widget);
  m_pAnimation->setDuration(50);
  QGraphicsOpacityEffect *m_pOpacity = new QGraphicsOpacityEffect();
  m_widget->setGraphicsEffect(m_pOpacity);
  m_pOpacity->setOpacity(1);
  m_pAnimation->setTargetObject(m_pOpacity);
  m_pAnimation->setPropertyName("opacity");
  m_pAnimation->setStartValue(0);
  m_pAnimation->setEndValue(1);
  m_pAnimation->start();

  m_widget->show();
}

void MainWindow::closeGrayWindows() { m_widget->close(); }

void MainWindow::on_btnNotesList_clicked() {
  ui->frameNotes->hide();
  ui->frameNoteList->show();

  m_NotesList->loadAllNoteBook();

  int notebookCount = m_NotesList->getNoteBookCount();
  if (notebookCount > 0) {
    int notebookIndex = m_Method->getCurNoteIndex().at(0);
    int noteIndex = m_Method->getCurNoteIndex().at(1);

    if (notebookIndex + 1 >= notebookCount) notebookIndex = notebookCount - 1;
    if (notebookIndex < 0) notebookIndex = 0;

    m_Method->setCurrentIndexBakList(mw_one->ui->qwNoteBook, notebookIndex);
    m_Method->clickNoteBook();

    int noteCount = m_NotesList->getNotesListCount();
    if (noteIndex + 1 >= noteCount) noteIndex = noteCount - 1;
    if (noteIndex < 0) noteIndex = 0;
    m_Method->setCurrentIndexBakList(mw_one->ui->qwNoteList, noteIndex);
  }

  m_NotesList->setNoteLabel();

  return;

  m_NotesList->show();
  m_NotesList->setWinPos();
  m_NotesList->tw->setFocus();
  m_NotesList->isSave = false;
  m_NotesList->tw->scrollToItem(m_NotesList->tw->currentItem());
}

void MainWindow::on_btnBackImg_clicked() {
  ui->f_ImgView->hide();
  if (isReaderVisible) ui->frameReader->show();
  if (isMemoVisible) ui->frameNotes->show();
}

void MainWindow::on_btnZoomIn_clicked() {
  imgSize = imgSize + 1000;
  mw_one->ui->qw_Img->rootContext()->setContextProperty("imgW", imgSize);
  mw_one->ui->qw_Img->rootContext()->setContextProperty("imgH", imgSize);

  QQuickItem *root = ui->qw_Img->rootObject();
  QMetaObject::invokeMethod((QObject *)root, "zoomin");
}

void MainWindow::on_btnZoomOut_clicked() {
  QQuickItem *root = ui->qw_Img->rootObject();
  QMetaObject::invokeMethod((QObject *)root, "zoomout");
}

void MainWindow::on_btnReport_clicked() { on_actionReport_triggered(); }

void MainWindow::on_btnPasteCode_clicked() {
  QClipboard *clipboard = QApplication::clipboard();
  QString originalText = clipboard->text();
  ui->editCode->setPlainText(originalText);
}

void MainWindow::on_btnAdd_clicked() {
  on_AddRecord();
  if (mw_one->isHardStepSensor == 1) mw_one->updateHardSensorSteps();
}

void MainWindow::on_btnDel_clicked() {
  m_Reader->setPdfViewVisible(false);

  on_DelRecord();
}

void MainWindow::resizeEvent(QResizeEvent *event) {
  Q_UNUSED(event);
  ui->qwReader->rootContext()->setContextProperty("myW", this->width());
  ui->qwReader->rootContext()->setContextProperty("myH", this->height());

  ui->qwSteps->rootContext()->setContextProperty("myW", this->width());
}

void MainWindow::on_KVChanged() {}

void MainWindow::on_btnAddTodo_clicked() { m_Todo->on_btnAdd_clicked(); }

void MainWindow::on_btnBackTodo_clicked() {
  m_Todo->saveTodo();
  ui->frameTodo->hide();
  ui->frameMain->show();

  m_Todo->refreshTableLists();
  m_Todo->refreshAlarm();
  isSelf = false;
  addFilesWatch();
}

void MainWindow::on_btnHigh_clicked() { m_Todo->on_btnHigh_clicked(); }

void MainWindow::on_btnLow_clicked() { m_Todo->on_btnLow_clicked(); }

void MainWindow::on_btnSetTime_clicked() { m_Todo->on_btnSetTime_clicked(); }

void MainWindow::on_btnRecycle_clicked() { m_Todo->on_btnRecycle_clicked(); }

void MainWindow::on_btnReturnRecycle_clicked() {
  m_Todo->on_btnReturn_clicked();
}

void MainWindow::on_btnClearRecycle_clicked() { m_Todo->on_btnClear_clicked(); }

void MainWindow::on_btnDelRecycle_clicked() { m_Todo->on_btnDel_clicked(); }

void MainWindow::on_btnRestoreRecycle_clicked() {
  m_Todo->on_btnRestore_clicked();
}

void MainWindow::on_editTodo_textChanged() {
  m_Todo->on_editTodo_textChanged();
}

void MainWindow::setItemHeight(int h) {
  QQuickItem *root = mw_one->ui->qwMain->rootObject();
  QMetaObject::invokeMethod((QObject *)root, "setItemHeight",
                            Q_ARG(QVariant, h));
}

void MainWindow::addItem(QString text0, QString text1, QString text2, int type,
                         QString topitem, int itemH) {
  QQuickItem *root = mw_one->ui->qwMain->rootObject();
  QMetaObject::invokeMethod((QObject *)root, "addItem", Q_ARG(QVariant, text0),
                            Q_ARG(QVariant, text1), Q_ARG(QVariant, text2),
                            Q_ARG(QVariant, type), Q_ARG(QVariant, topitem),
                            Q_ARG(QVariant, itemH));
}

QString MainWindow::getTop(int index) {
  QQuickItem *root = mw_one->ui->qwMain->rootObject();
  QVariant itemTime;
  QMetaObject::invokeMethod((QObject *)root, "getTop",
                            Q_RETURN_ARG(QVariant, itemTime),
                            Q_ARG(QVariant, index));
  return itemTime.toString();
}

QString MainWindow::getText0(int index) {
  QQuickItem *root = mw_one->ui->qwMain->rootObject();
  QVariant item;
  QMetaObject::invokeMethod((QObject *)root, "getText0",
                            Q_RETURN_ARG(QVariant, item),
                            Q_ARG(QVariant, index));
  return item.toString();
}

QString MainWindow::getText1(int index) {
  QQuickItem *root = mw_one->ui->qwMain->rootObject();
  QVariant item;
  QMetaObject::invokeMethod((QObject *)root, "getText1",
                            Q_RETURN_ARG(QVariant, item),
                            Q_ARG(QVariant, index));
  return item.toString();
}

QString MainWindow::getText2(int index) {
  QQuickItem *root = mw_one->ui->qwMain->rootObject();
  QVariant item;
  QMetaObject::invokeMethod((QObject *)root, "getText2",
                            Q_RETURN_ARG(QVariant, item),
                            Q_ARG(QVariant, index));
  return item.toString();
}

int MainWindow::getItemType(int index) {
  QQuickItem *root = mw_one->ui->qwMain->rootObject();
  QVariant itemType;
  QMetaObject::invokeMethod((QObject *)root, "getType",
                            Q_RETURN_ARG(QVariant, itemType),
                            Q_ARG(QVariant, index));
  return itemType.toInt();
}

void MainWindow::delItem(int index) {
  QQuickItem *root = mw_one->ui->qwMain->rootObject();
  QMetaObject::invokeMethod((QObject *)root, "delItem", Q_ARG(QVariant, index));
}

int MainWindow::getCount() {
  QQuickItem *root = mw_one->ui->qwMain->rootObject();
  QVariant itemCount;
  QMetaObject::invokeMethod((QObject *)root, "getItemCount",
                            Q_RETURN_ARG(QVariant, itemCount));
  return itemCount.toInt();
}

void MainWindow::clearAll() {
  int count = getCount();
  for (int i = 0; i < count; i++) {
    delItem(0);
  }
}

void MainWindow::setCurrentIndex(int index) {
  QQuickItem *root = mw_one->ui->qwMain->rootObject();
  QMetaObject::invokeMethod((QObject *)root, "setCurrentItem",
                            Q_ARG(QVariant, index));
}

void MainWindow::gotoEnd() {
  QQuickItem *root = mw_one->ui->qwMain->rootObject();
  QMetaObject::invokeMethod((QObject *)root, "gotoEnd");
}

void MainWindow::gotoIndex(int index) {
  QQuickItem *root = mw_one->ui->qwMain->rootObject();
  QMetaObject::invokeMethod((QObject *)root, "gotoIndex",
                            Q_ARG(QVariant, index));
}

int MainWindow::getCurrentIndex() {
  QQuickItem *root = mw_one->ui->qwMain->rootObject();
  QVariant itemIndex;
  QMetaObject::invokeMethod((QObject *)root, "getCurrentIndex",
                            Q_RETURN_ARG(QVariant, itemIndex));
  return itemIndex.toInt();
}

void MainWindow::setScrollBarPos(double pos) {
  QQuickItem *root = mw_one->ui->qwMain->rootObject();
  QMetaObject::invokeMethod((QObject *)root, "setScrollBarPos",
                            Q_ARG(QVariant, pos));
}

void MainWindow::reloadMainOld() {
  clearAll();

  QFontMetrics fontMetrics(font());
  int nFontHeight = fontMetrics.height();

  QTreeWidget *tw = get_tw(tabData->currentIndex());

  int total = tw->topLevelItemCount();
  int a;

  if (total - days > 0)
    a = total - days;
  else
    a = 0;

  int nullrows = 0;
  QString text0, text1, text2, text3, topitem;
  for (int i = a; i < total; i++) {
    QTreeWidgetItem *topItem = tw->topLevelItem(i);

    text0 = topItem->text(0) + "  " + topItem->text(3);
    text1 = topItem->text(1);
    text2 = topItem->text(2);

    nullrows = 1;
    if (text1.length() > 0) {
      text1 = tr("Freq") + " : " + text1;
      nullrows++;
    }

    if (text2.length() > 0) {
      text2 = tr("Amount") + " : " + text2;
      nullrows++;
    }

    topitem = text0;
    addItem(text0, text1, text2, 1, topitem, nFontHeight * (nullrows + 1));

    int childCount = topItem->childCount();
    for (int j = 0; j < childCount; j++) {
      QTreeWidgetItem *childItem = topItem->child(j);
      text0 = childItem->text(0);
      text1 = childItem->text(1);
      text2 = childItem->text(2);
      text3 = childItem->text(3);

      if (text3.trimmed().length() > 0) {
        text2 = "*" + text2;
      }

      nullrows = 1;
      text0 = "  " + text0;
      if (text1.length() > 0) {
        text1 = "  " + tr("Amount") + " : " + text1;
        nullrows++;
      }

      if (text2.length() > 0) {
        text2 = "  " + tr("Category") + " : " + text2;
        nullrows++;
      }

      addItem(text0, text1, text2, 0, topitem, nFontHeight * (nullrows + 1));
    }
  }

  gotoEnd();
  int count = getCount();
  setCurrentIndex(count - 1);
}

void MainWindow::reloadMain() {
  m_Method->clearAllBakList(ui->qwMainDate);

  // QFontMetrics fontMetrics(font());
  // int nFontHeight = fontMetrics.height();

  QTreeWidget *tw = get_tw(tabData->currentIndex());

  int total = tw->topLevelItemCount();

  if (total == 0) {
    m_Method->clearAllBakList(ui->qwMainEvent);
    return;
  }

  int a;

  if (total - days > 0)
    a = total - days;
  else
    a = 0;

  int nullrows = 0;
  QString text0, text1, text2, text3, topitem;
  for (int i = a; i < total; i++) {
    QTreeWidgetItem *topItem = tw->topLevelItem(i);

    text0 = topItem->text(0) + "  " + topItem->text(3);
    text1 = topItem->text(1);
    text2 = topItem->text(2);

    nullrows = 1;
    if (text1.length() > 0) {
      text1 = tr("Freq") + " : " + text1;
      nullrows++;
    }

    if (text2.length() > 0) {
      text2 = tr("Amount") + " : " + text2;
      nullrows++;
    }

    topitem = text0;

    m_Method->addItemBakList(ui->qwMainDate, text0, text1, text2, text3, 0);
  }

  m_Method->gotoEnd(ui->qwMainDate);
  int count = m_Method->getCountBakList(ui->qwMainDate);
  m_Method->setCurrentIndexBakList(ui->qwMainDate, count - 1);

  m_Method->clickMainDate();
}

void MainWindow::reeditData() {
  if (setTWCurrentItem()) {
    on_twItemDoubleClicked();
  }
}

void MainWindow::clickData() {
  if (setTWCurrentItem()) on_twItemClicked();
}

bool MainWindow::setTWCurrentItem() {
  bool isSel = false;
  int row = getCurrentIndex();
  if (row < 0) return false;

  // type==0 child; type==1 top
  int type = getItemType(row);

  QString textTop = getTop(row);
  QString text0 = getText0(row);
  QStringList list = text0.split(".");
  int childIndex = 0;
  if (list.count() > 0) {
    childIndex = list.at(0).toInt() - 1;
  }

  if (type == 0) {
    if (childIndex < 0) return false;
  }

  QTreeWidget *tw = get_tw(tabData->currentIndex());
  int count = tw->topLevelItemCount();
  for (int i = 0; i < count; i++) {
    QTreeWidgetItem *topItem = tw->topLevelItem(count - 1 - i);
    if (topItem->text(0) + "  " + topItem->text(3) == textTop) {
      if (type == 0) {
        QTreeWidgetItem *childItem = topItem->child(childIndex);
        tw->setCurrentItem(childItem, 0);
      }

      if (type == 1) {
        tw->setCurrentItem(topItem);
      }

      isSel = true;
      break;
    }
  }

  return isSel;
}

void MainWindow::on_btnBackSteps_clicked() { m_Steps->on_btnBack_clicked(); }

void MainWindow::on_btnPauseSteps_clicked() { m_Steps->on_btnPause_clicked(); }

void MainWindow::on_btnReset_clicked() { m_Steps->on_btnReset_clicked(); }

void MainWindow::on_btnBack_Report_clicked() { m_Report->on_btnBack_clicked(); }

void MainWindow::on_btnYear_clicked() { m_Report->on_btnYear_clicked(); }

void MainWindow::on_btnMonth_clicked() { m_Report->on_btnMonth_clicked(); }

void MainWindow::on_btnOut2Img_clicked() { m_Report->on_btnOut2Img_clicked(); }

void MainWindow::on_btnCategory_clicked() {
  m_Report->on_btnCategory_clicked();
}

void MainWindow::on_btnSync_clicked() {
  m_Reader->setPdfViewVisible(false);
  ui->btnUpload->click();
}

void MainWindow::on_btnPDF_clicked() { m_Notes->on_btnPDF_clicked(); }

void MainWindow::on_btnPasteTodo_clicked() { ui->editTodo->paste(); }

int MainWindow::getMaxDay(QString sy, QString sm) {
  int maxDay = 0;
  for (int i = 0; i < 50; i++) {
    QString strDate = sy + "-" + sm + "-" + QString::number(i + 1);
    QDate date = QDate::fromString(strDate, "yyyy-M-d");
    if (date.dayOfWeek() == 0) {
      maxDay = i;
      break;
    }
  }

  return maxDay;
}

void MainWindow::on_btnStartDate_clicked() {
  m_Report->myDateSelector->initStartEndDate("start");
}

void MainWindow::on_btnEndDate_clicked() {
  m_Report->myDateSelector->initStartEndDate("end");
}

void MainWindow::on_btnBackSearch_clicked() {
  ui->frameSearch->hide();
  ui->frameMain->show();
}

void MainWindow::on_btnClearSearchText_clicked() {
  ui->editSearchText->setText("");
  ui->editSearchText->setFocus();
}

void MainWindow::on_btnStartSearch_clicked() {
  searchStr = ui->editSearchText->text().trimmed();
  if (searchStr.length() == 0) return;

  showProgress();
  mySearchThread->start();
}

void MainWindow::on_btnBackBakList_clicked() {
  ui->frameBakList->hide();
  ui->frameMain->show();
}

void MainWindow::on_btnImportBakList_clicked() {
  if (m_Method->getCountBakList(ui->qwBakList) == 0) return;

  int cur_index = m_Method->getCurrentIndexBakList(ui->qwBakList);
  QString str = m_Method->getText3(ui->qwBakList, cur_index);
  zipfile = str.trimmed();

  if (!zipfile.isNull()) {
    if (!mw_one->showMsgBox("Kont",
                            tr("Import this data?") + "\n" +
                                mw_one->m_Reader->getUriRealPath(zipfile),
                            "", 2)) {
      isZipOK = false;
      return;
    }
  }

  isZipOK = true;
  ui->btnBackBakList->clicked();
  showProgress();

  isMenuImport = false;
  isTimeMachine = true;
  isDownData = false;
  myImportDataThread->start();
}

void MainWindow::on_btnOkViewCate_clicked() { m_Report->on_CateOk(); }

void MainWindow::on_btnBackTabRecycle_clicked() {
  ui->frameTabRecycle->hide();
  ui->frameMain->show();
}

void MainWindow::on_btnDelTabRecycle_clicked() {
  if (m_Method->getCountBakList(ui->qwTabRecycle) == 0) return;
  int index = m_Method->getCurrentIndexBakList(ui->qwTabRecycle);
  QString tab_file = m_Method->getText3(ui->qwTabRecycle, index);
  if (!showMsgBox("Knot", tr("Whether to remove") + "  " + tab_file + " ? ", "",
                  2))
    return;

  QFile file(tab_file);
  file.remove();
  m_Method->delItemBakList(ui->qwTabRecycle, index);

  ui->lblTitleTabRecycle->setText(
      tr("Tab Recycle") + "    " + tr("Total") + " : " +
      QString::number(m_Method->getCountBakList(ui->qwTabRecycle)));
}

void MainWindow::on_btnRestoreTab_clicked() {
  if (m_Method->getCountBakList(ui->qwTabRecycle) == 0) return;

  int count = ui->tabWidget->tabBar()->count();
  QString twName = m_Notes->getDateTimeStr() + "_" + QString::number(count + 1);
  QString ini_file = iniDir + twName + ".ini";
  if (QFile(ini_file).exists()) QFile(ini_file).remove();

  int index = m_Method->getCurrentIndexBakList(ui->qwTabRecycle);
  QString recycle = m_Method->getText3(ui->qwTabRecycle, index);
  QFile::copy(recycle, ini_file);

  QString tab_name = m_Method->getText0(ui->qwTabRecycle, index);
  QTreeWidget *tw = init_TreeWidget(twName);
  ui->tabWidget->addTab(tw, tab_name);

  ui->tabWidget->setCurrentIndex(count);

  readData(tw);

  QFile recycle_file(recycle);
  recycle_file.remove();
  on_btnBackTabRecycle_clicked();

  saveTab();

  reloadMain();
  clickData();

  isNeedAutoBackup = true;
  strLatestModify = tr("Restore Tab") + "(" + tab_name + ")";
}

void MainWindow::on_btnDelBakFile_clicked() {
  if (m_Method->getCountBakList(ui->qwBakList) == 0) return;

  int index = m_Method->getCurrentIndexBakList(ui->qwBakList);
  QString bak_file = m_Method->getText3(ui->qwBakList, index);
  if (!showMsgBox("Knot", tr("Whether to remove") + "  " + bak_file + " ? ", "",
                  2))
    return;

  QFile file(bak_file);
  file.remove();
  m_Method->delItemBakList(ui->qwBakList, index);

  ui->lblBakListTitle->setText(
      tr("Backup File List") + "    " + tr("Total") + " : " +
      QString::number(m_Method->getCountBakList(ui->qwBakList)));
}

void MainWindow::on_btnBackNoteList_clicked() {
  ui->frameNoteList->hide();
  ui->frameNotes->show();

  m_NotesList->saveNotesList();
}

void MainWindow::on_btnBackNoteRecycle_clicked() {
  m_NotesList->saveRecycle();
  ui->frameNoteRecycle->hide();
  ui->frameNoteList->show();
}

void MainWindow::on_btnNoteRecycle_clicked() {
  ui->frameNoteList->hide();
  ui->frameNoteRecycle->show();

  m_Method->clearAllBakList(ui->qwNoteRecycle);
  int childCount = m_NotesList->twrb->topLevelItem(0)->childCount();
  for (int i = 0; i < childCount; i++) {
    QTreeWidgetItem *childItem = m_NotesList->twrb->topLevelItem(0)->child(i);
    QString text0 = childItem->text(0);
    QString text3 = childItem->text(1);

    m_Method->addItemBakList(ui->qwNoteRecycle, text0, "", "", text3, 0);
  }
}

void MainWindow::on_btnDelNoteRecycle_clicked() {
  int count = m_Method->getCountBakList(ui->qwNoteRecycle);
  if (count == 0) return;

  int index = m_Method->getCurrentIndexBakList(ui->qwNoteRecycle);
  QString file = m_Method->getText0(ui->qwNoteRecycle, index);

  if (!showMsgBox("Knot", tr("Whether to remove") + "  " + file + " ? ", "", 2))
    return;

  QTreeWidgetItem *topItem = m_NotesList->twrb->topLevelItem(0);
  m_NotesList->twrb->setCurrentItem(topItem->child(index));
  m_NotesList->ui->btnDel_2->click();

  m_Method->delItemBakList(ui->qwNoteRecycle, index);
}

void MainWindow::on_btnRestoreNoteRecycle_clicked() {
  int count = m_Method->getCountBakList(ui->qwNoteRecycle);
  if (count == 0) return;

  if (m_NotesList->getNoteBookCount() == 0) return;

  int indexRecycle = m_Method->getCurrentIndexBakList(ui->qwNoteRecycle);
  m_NotesList->twrb->setCurrentItem(
      m_NotesList->twrb->topLevelItem(0)->child(indexRecycle));

  int indexTop = m_Method->getCurrentIndexBakList(ui->qwNoteBook);

  m_NotesList->tw->setCurrentItem(m_NotesList->tw->topLevelItem(indexTop));

  m_NotesList->on_btnRestore_clicked();

  m_Method->delItemBakList(ui->qwNoteRecycle, indexRecycle);

  int noteCount = m_Method->getCountBakList(ui->qwNoteList);
  m_Method->setCurrentIndexBakList(ui->qwNoteList, noteCount - 1);
  m_Method->clickNoteList();

  on_btnBackNoteRecycle_clicked();
}

void MainWindow::on_btnFindNotes_clicked() {
  m_NotesList->on_btnFind_clicked();
}

void MainWindow::on_editFindNote_textChanged(const QString &arg1) {
  m_NotesList->ui->editFind->setText(arg1.trimmed());
}

void MainWindow::on_btnFindPreviousNote_clicked() {
  m_NotesList->on_btnPrev_clicked();
}

void MainWindow::on_btnFindNextNote_clicked() {
  m_NotesList->on_btnNext_clicked();
}

void MainWindow::on_btnClearNoteFindText_clicked() {
  ui->editFindNote->setText("");
}

void MainWindow::on_btnShowFindNotes_clicked() {
  if (ui->btnShowFindNotes->text() == tr("Show Find")) {
    ui->f_FindNotes->show();
    ui->editFindNote->setFocus();
    ui->btnShowFindNotes->setText(tr("Hide Find"));
  } else {
    if (ui->btnShowFindNotes->text() == tr("Hide Find")) {
      ui->f_FindNotes->hide();
      ui->btnShowFindNotes->setText(tr("Show Find"));
    }
  }
}

void MainWindow::on_btnNoteBookMenu_clicked() {
  m_Method->showNoteBookMenu(ui->qwNoteBook->x(), ui->qwNoteBook->y());
}

void MainWindow::on_btnNoteMenu_clicked() {
  m_Method->showNotsListMenu(ui->qwNoteList->x(), ui->qwNoteList->y());
}

void MainWindow::on_btnCancelType_clicked() { m_List->on_btnCancel_clicked(); }

void MainWindow::on_btnOkType_clicked() { m_List->on_btnOk_clicked(); }

void MainWindow::on_btnDelType_clicked() { m_List->on_btnDel_clicked(); }

void MainWindow::on_btnRenameType_clicked() {
  m_List->ui->editRename->setText(ui->editRenameType->text().trimmed());
  m_List->on_btnRename_clicked();
}

void MainWindow::on_btnBackSetTab_clicked() {
  ui->frameSetTab->hide();
  ui->frameMain->show();
}
