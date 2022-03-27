#include "mainwindow.h"

#include <QMessageBox>

#include "ui_mainwindow.h"

QVector<double> doubleList;
QSplineSeries* series;
QScatterSeries* m_scatterSeries;
QSplineSeries* series2;
QScatterSeries* m_scatterSeries2;
QGridLayout* gl1;
QTreeWidgetItem* parentItem;
bool isrbFreq = true;
Chart* chartTimeLine;
QChart *chartMonth, *chartDay;
QString appName = "Xcounter";
QString iniFile, ver, strDate, noteText;
int curPos, sliderPos, today, fontSize, red, yMaxMonth, yMaxDay;
MainWindow* mw_one;
QTabWidget *tabData, *tabChart;
bool loading, SaveAll, isReadEnd;
extern bool isAndroid, isIOS, zh_cn;
QRegularExpression regxNumber("^-?\[0-9.]*$");

ReadThread::ReadThread(QObject* parent) : QThread{parent} {}
void ReadThread::run() {
  isReadEnd = false;
  int index = tabData->currentIndex();
  QTreeWidget* tw = (QTreeWidget*)tabData->widget(index);

  qDebug() << "currentTW: " << tw;
  MainWindow::get_Today(tw);
  if (tabChart->currentIndex() == 0) MainWindow::drawMonthChart();
  if (tabChart->currentIndex() == 1) MainWindow::initChartTimeLine(tw, true);
  emit isDone();
}
void MainWindow::readDone() {
  if (tabChart->currentIndex() == 0) {
    chartMonth->createDefaultAxes();
    chartMonth->axes(Qt::Horizontal).first()->setRange(0, 31);
    chartMonth->axes(Qt::Vertical).first()->setRange(0, yMaxMonth);
  }
  if (tabChart->currentIndex() == 1) {
    chartDay->createDefaultAxes();
    chartDay->axes(Qt::Horizontal).first()->setRange(0, 24);
    chartDay->axes(Qt::Vertical).first()->setRange(0, yMaxDay + 2);
  }
  QTreeWidget* tw = (QTreeWidget*)tabData->currentWidget();
  init_Stats(tw);
  init_NavigateBtnColor();
  isReadEnd = true;
}

SearchThread::SearchThread(QObject* parent) : QThread{parent} {}
void SearchThread::run() {
  MainWindow::saveFile(SaveAll);
  emit isDone();
}
void MainWindow::dealDone() {
  QTreeWidget* tw = (QTreeWidget*)tabData->currentWidget();
  get_Today(tw);
  init_Stats(tw);
  if (tabChart->currentIndex() == 0) drawMonthChart();
  if (tabChart->currentIndex() == 1) initChartTimeLine(tw, true);
}
void MainWindow::saveFile(bool all) {
  if (!all) {
    QTreeWidget* tw = (QTreeWidget*)tabData->currentWidget();
    int index = tabData->currentIndex();
    saveData(tw, index);
  }

  if (all) {
    QFile(iniFile).remove();
    for (int i = 0; i < tabData->tabBar()->count(); i++) {
      QTreeWidget* tw = (QTreeWidget*)tabData->widget(i);
      tw->setObjectName("tab" + QString::number(i + 1));
      saveData(tw, i);
    }
  }
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  mw_one = this;
  ver = "1.0.02";
  ui->actionAbout->setText(tr("About") + " (" + ver + ")");

  isReadEnd = true;
  tabData = new QTabWidget;
  tabData = ui->tabWidget;
  tabChart = new QTabWidget;
  tabChart = ui->tabCharts;
  gl1 = new QGridLayout(this);
  gl1 = ui->pLayout;

  chartMonth = new QChart();
  QChartView* chartview = new QChartView(chartMonth);
  ui->pLayout->addWidget(chartview);
  chartview->setRenderHint(QPainter::Antialiasing);  //防止图形走样
  // chartMonth->setTitle("Line chart");
  chartMonth->legend()->hide();
  chartMonth->setMargins(QMargins(0, 0, 0, 0));
  chartMonth->setContentsMargins(0, 0, 0, 0);
  chartMonth->setAnimationOptions(QChart::SeriesAnimations);  //设置曲线动画模式

  chartDay = new QChart();
  QChartView* chartview1 = new QChartView(chartDay);
  ui->glTimeLine->addWidget(chartview1);
  chartview1->setRenderHint(QPainter::Antialiasing);  //防止图形走样
  // chartDay->setTitle("Line chart");
  chartDay->legend()->hide();
  chartDay->setMargins(QMargins(0, 0, 0, 0));
  chartDay->setContentsMargins(0, 0, 0, 0);
  chartDay->setAnimationOptions(QChart::SeriesAnimations);  //设置曲线动画模式

  series = new QSplineSeries(chartMonth);
  series->setPen(QPen(Qt::blue, 3, Qt::SolidLine));
  m_scatterSeries = new QScatterSeries(chartMonth);  //创建散点
  m_scatterSeries->setMarkerShape(
      QScatterSeries::MarkerShapeCircle);  //设置散点样式
  m_scatterSeries->setMarkerSize(10);      //设置散点大小
  chartMonth->addSeries(series);
  chartMonth->addSeries(m_scatterSeries);

  series2 = new QSplineSeries(chartDay);
  series2->setPen(QPen(Qt::blue, 3, Qt::SolidLine));
  m_scatterSeries2 = new QScatterSeries(chartDay);  //创建散点
  m_scatterSeries2->setMarkerShape(
      QScatterSeries::MarkerShapeCircle);  //设置散点样式
  m_scatterSeries2->setMarkerSize(10);     //设置散点大小
  chartDay->addSeries(series2);
  chartDay->addSeries(m_scatterSeries2);

  // ui->glTimeLine->setMargin(0);
  // ui->glTimeLine->setSpacing(0);
  // ui->glTimeLine->setContentsMargins(0, 0, 0, 0);

  this->installEventFilter(this);
  ui->tabWidget->tabBar()->installEventFilter(this);
  ui->tabWidget->installEventFilter(this);
  ui->frame_tab->setMouseTracking(true);
  ui->tabWidget->setMouseTracking(true);
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
  ui->lblStats->adjustSize();
  ui->lblStats->setWordWrap(true);
  // 获取背景色
  QPalette pal = this->palette();
  QBrush brush = pal.window();
  red = brush.color().red();
  if (red < 55) {
    mydlgTodo->ui->listWidget->setStyleSheet(mydlgTodo->styleDark);

  } else {
    mydlgTodo->ui->listWidget->setStyleSheet(
        mydlgTodo->ui->listWidget->styleSheet());
  }

  loading = true;
  ui->statusbar->setHidden(true);
  this->setWindowTitle("");
  tmer = new QTimer(this);
  connect(tmer, SIGNAL(timeout()), this, SLOT(timerUpdate()));

  myReadThread = new ReadThread();
  connect(myReadThread, &ReadThread::isDone, this, &MainWindow::readDone);

  mySearchThread = new SearchThread();
  connect(mySearchThread, &SearchThread::isDone, this, &MainWindow::dealDone);

  strDate = QDate::currentDate().toString();  //"yyyy-MM-dd");

  ui->tabCharts->setCornerWidget(ui->frame_cw);
  ui->frame_find->setHidden(true);

  if (zh_cn) {
    listMonth = QStringList() << "1月"
                              << "2月"
                              << "3月"
                              << "4月"
                              << "5月"
                              << "6月"
                              << "7月"
                              << "8月"
                              << "9月"
                              << "10月"
                              << "11月"
                              << "12月";
  } else {
    listMonth = QStringList() << "Jan"
                              << "Feb"
                              << "Mar"
                              << "Apr"
                              << "May"
                              << "Jun"
                              << "Jul"
                              << "Aug"
                              << "Sep"
                              << "Oct"
                              << "Nov"
                              << "Dec";
  }

  // setComboBoxQss(ui->cboxYear, 4, 1, "#C0C0C0", "#4169E1");
  setLineEditQss(ui->editFind, 4, 1, "#4169E1", "#4169E1");

  QDir dir;
  QString path;
  path = dir.currentPath();
  qDebug() << "Path:" << path;
  if (isIOS) {
    QString str1 = QDir::homePath() + "/" + appName + "/";
    QDir dir0;
    dir0.mkpath(str1);
    iniFile = str1 + appName + ".ini";
    bakFile = str1 + appName + ".bak";
  }
  if (isAndroid) {
    iniFile = path + "/" + appName + ".ini";
    bakFile = path + "/" + appName + ".bak";
  }

  int iz = 22;
  ui->btnFind->setIconSize(QSize(iz, iz));
  ui->btnReport->setIconSize(QSize(iz, iz));
  int s = 35;
  if (isIOS) {
  }
  ui->btnPlus->setIconSize(QSize(s, s));
  ui->btnLess->setIconSize(QSize(s, s));
  ui->btnTodo->setIconSize(QSize(s, s));
  ui->btnPlus->setIcon(QIcon(":/src/1.png"));
  ui->btnLess->setIcon(QIcon(":/src/2.png"));
  // ui->btnTodo->setIcon(QIcon(":/src/todo.png"));
  ui->btnTodo->setFixedHeight(s + 6);
  ui->btnMax->setFixedHeight(s + 6);
  ui->frame_tab->setMaximumHeight(this->height() / 2 - ui->btnTodo->height());
  QSettings Reg(iniFile, QSettings::IniFormat);
  ui->btnYear->setText(Reg.value("/YMD/Y", 2022).toString());
  ui->btnMonth->setText(Reg.value("/YMD/M", tr("Month")).toString());
  ui->btnDay->setText(Reg.value("/YMD/D", 1).toString());

  // Custom Desc
  mw_one->mydlgList->ui->listWidget->clear();
  int descCount = Reg.value("/CustomDesc/Count").toInt();
  for (int i = 0; i < descCount; i++) {
    mw_one->mydlgList->ui->listWidget->addItem(
        Reg.value("/CustomDesc/Item" + QString::number(i)).toString());
  }

  // Font Size
  QFont font(this->font());
  QFontInfo fInfo(font);
  fontSize = fInfo.pointSize();
  qDebug() << "fontSize:" << fontSize;
  mydlgPre->ui->rb0->setChecked(Reg.value("/FontSize/rb0", 1).toBool());
  mydlgPre->ui->rb1->setChecked(Reg.value("/FontSize/rb1", 0).toBool());
  mydlgPre->ui->rb2->setChecked(Reg.value("/FontSize/rb2", 0).toBool());
  if (mydlgPre->ui->rb1->isChecked()) fontSize = fontSize + 4;
  if (mydlgPre->ui->rb2->isChecked()) fontSize = fontSize + 8;

  loading = false;
  init_TabData();
  QTreeWidget* tw = (QTreeWidget*)tabData->currentWidget();
  if (tabChart->currentIndex() == 0) drawMonthChart();
  if (tabChart->currentIndex() == 1) initChartTimeLine(tw, true);
  mydlgTodo->init_Items();

  get_Today(tw);
  init_Stats(tw);
  init_NavigateBtnColor();

  isInit = true;

  // drawMonth();
}

void MainWindow::init_TabData() {
  int count = ui->tabWidget->tabBar()->count();
  for (int i = 0; i < count; i++) {
    ui->tabWidget->removeTab(0);
  }

  QSettings Reg(iniFile, QSettings::IniFormat);
  int TabCount = Reg.value("TabCount", 0).toInt();
  for (int i = 0; i < TabCount; i++) {
    QString name = "tab" + QString::number(i + 1);
    QTreeWidget* tw = init_TreeWidget(name);
    ui->tabWidget->addTab(tw, Reg.value("TabName" + QString::number(i),
                                        tr("Counter") + QString::number(i + 1))
                                  .toString());
    QString strNotes = Reg.value("/" + name + "/Note").toString();
    ui->tabWidget->setTabToolTip(i, strNotes);

    readData(tw);
  }

  if (TabCount == 0) {
    ui->tabWidget->addTab(init_TreeWidget("tab1"),
                          tr("Counter") + " " + QString::number(1));
    ui->tabWidget->setTabToolTip(0, "");
  }

  ui->tabWidget->setCurrentIndex(Reg.value("CurrentIndex").toInt());

  init_TabNavigate();
}

void MainWindow::init_TabNavigate() {
  if (listNBtn.count() > 0) {
    for (int i = 0; i < listNBtn.count(); i++) {
      delete listNBtn.at(i);
    }
    listNBtn.clear();
  }
  for (int i = 0; i < ui->tabWidget->tabBar()->count(); i++) {
    QToolButton* btn = new QToolButton(this);
    btn->setFixedWidth(25);
    listNBtn.append(btn);
    QFont font;
    font.setPointSize(13);
    btn->setFont(font);
    btn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    connect(btn, &QToolButton::clicked, [=]() {
      ui->tabWidget->setCurrentIndex(btn->text().toInt() - 1);
      init_NavigateBtnColor();
    });
    QString str = ui->tabWidget->tabBar()->tabText(i);
    QString str1;
    for (int j = 0; j < str.length(); j++) {
      str1 = str1 + str.mid(j, 1) + "\n";
    }

    btn->setText(QString::number(i + 1));
    btn->setToolTip(str);
    ui->vl->addWidget(btn);
  }
}

void MainWindow::init_NavigateBtnColor() {
  QPalette p1, p2;
  p1.setColor(QPalette::Button, Qt::red);
  p1.setColor(QPalette::Button, Qt::gray);

  for (int n = 0; n < listNBtn.count(); n++) {
    if (n == ui->tabWidget->currentIndex())
      listNBtn.at(n)->setPalette(p1);
    else
      listNBtn.at(n)->setPalette(p2);
  }
}

void MainWindow::timerUpdate() {}

MainWindow::~MainWindow() {
  delete ui;
  mySearchThread->quit();
  mySearchThread->wait();
}

void MainWindow::add_Data(QTreeWidget* tw, QString strTime, QString strAmount,
                          QString strDesc) {
  bool isYes = false;

  strDate = QDate::currentDate().toString();
  if (isTesting) {
    strDate = strDate.replace("3", "3");
  }
  for (int i = 0; i < tw->topLevelItemCount(); i++) {
    QString str = tw->topLevelItem(i)->text(0);
    if (str == strDate) {
      isYes = true;

      QTreeWidgetItem* topItem = tw->topLevelItem(i);
      QTreeWidgetItem* item11 = new QTreeWidgetItem(topItem);
      item11->setText(0, strTime);
      if (strAmount == "")
        item11->setText(1, "");
      else
        item11->setText(1, QString("%1").arg(strAmount.toDouble(), 0, 'f', 2));

      // QLabel* lblDesc = new QLabel(this);
      // lblDesc->setText(strDesc);
      // lblDesc->adjustSize();
      // lblDesc->setWordWrap(true);
      // tw->setItemWidget(item11, 2, lblDesc);
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
    // saveData(tw, ui->tabWidget->currentIndex());
    SaveAll = false;
    mySearchThread->start();
  }
}

void MainWindow::del_Data(QTreeWidget* tw) {
  bool isNo = true;
  strDate = QDate::currentDate().toString();
  for (int i = 0; i < tw->topLevelItemCount(); i++) {
    QString str = tw->topLevelItem(i)->text(0);
    if (str == strDate) {
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

  SaveAll = false;
  mySearchThread->start();
}

void MainWindow::on_btnPlus_clicked() {
  isAdd = true;
  mydlgSetTime->setFixedHeight(this->height());
  mydlgSetTime->setFixedWidth(this->width());
  mydlgSetTime->setModal(true);
  mydlgSetTime->ui->lblTitle->setText(tr("Add"));
  mydlgSetTime->ui->timeEdit->setTime(QTime::currentTime());
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
  QSettings Reg(iniFile, QSettings::IniFormat);
  int TabCount = tabData->tabBar()->count();
  Reg.setValue("TabCount", TabCount);
  int CurrentIndex = tabData->currentIndex();
  Reg.setValue("CurrentIndex", CurrentIndex);
  for (int i = 0; i < TabCount; i++) {
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
  QSettings Reg(iniFile, QSettings::IniFormat);
  int count = tw->topLevelItemCount();
  QString name = "tab" + QString::number(tabIndex + 1);
  tw->setObjectName(name);
  Reg.setValue("/" + name + "/TopCount", count);
  for (int i = 0; i < count; i++) {
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

  Reg.setValue("/" + name + "/" + "Note", tabData->tabToolTip(tabIndex));
  int lines = Reg.allKeys().count();
  Reg.setValue("/" + ver + "-" + appName + "/AllKeys", lines);
  Reg.setValue("/" + ver + "-" + appName + "/FileSize",
               getFileSize(QFile(iniFile).size(), 2));
  Reg.setValue("/" + ver + "-" + appName + "/File", iniFile);

  saveTab();
  dlgTodo::saveTodo();
  dlgSetTime::saveCustomDesc();
  dlgPreferences::saveFontSize();
}

void MainWindow::drawMonthChart() {
  QString strY, strM;
  strY = get_Year(strDate);
  strM = get_Month(strDate);
  QStringList listM = get_MonthList(strY, strM);

  // double maxValue = *std::max_element(doubleList.begin(), doubleList.end());
  // yMaxMonth = maxValue;
  // chartMonth->createDefaultAxes();
  // chartMonth->axes(Qt::Horizontal).first()->setRange(0, 31);
  // chartMonth->axes(Qt::Vertical).first()->setRange(0, yMaxMonth);

  initChart(strY, strM, listM);
}

void MainWindow::readData(QTreeWidget* tw) {
  tw->clear();
  QSettings Reg(iniFile, QSettings::IniFormat);
  QString name = tw->objectName();
  int rowCount = Reg.value("/" + name + "/TopCount").toInt();
  for (int i = 0; i < rowCount; i++) {
    int childCount =
        Reg.value("/" + name + "/" + QString::number(i + 1) + "-childCount")
            .toInt();
    // 不显示子项为0的数据
    if (childCount > 0) {
      QTreeWidgetItem* topItem = new QTreeWidgetItem;
      topItem->setText(
          0, Reg.value("/" + name + "/" + QString::number(i + 1) + "-topDate")
                 .toString());
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

  // get_Today(tw);
  // init_Stats(tw);
  //  initMonthChart();
  //  initChartTimeLine(tw, true);
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

void MainWindow::closeEvent(QCloseEvent* event) { Q_UNUSED(event); }

void MainWindow::init_Stats(QTreeWidget* tw) {
  int count = tw->topLevelItemCount();
  int tatol = 0;
  double amount = 0;
  for (int i = 0; i < count; i++) {
    QString str1 = tw->topLevelItem(i)->text(1);
    QString str2 = tw->topLevelItem(i)->text(2);
    tatol = tatol + str1.toInt();
    amount = amount + str2.toDouble();
  }

  double a = (double)tatol / 20;
  int a1, b, c;
  a1 = a;
  b = a1 / 10;
  c = a1 % 10;
  ui->lblStats->clear();
  /*if (ui->tabWidget->currentIndex() == 0)
    ui->lblStats->setText(tr("Total") + " : " + QString::number(a) + " " +
                          tr("Boxes") + " ( " + QString::number(b) + " " +
                          tr("Cartons") + " " + QString::number(c) + " " +
                          tr("Boxes") + " ) ");
  else*/
  QString strAmount = QString("%1").arg(amount, 0, 'f', 2);
  ui->lblStats->setText(tr("Total") + " : " + QString::number(tatol) + "    $" +
                        strAmount);
}

void MainWindow::initChart(QString strY, QString strM, QStringList listMonth) {
  if (loading) return;
  // listMonth Format:1.Day  2.Freq or Amount
  // QVector<double> doubleList;
  // double x, y;
  // series->clear();
  // m_scatterSeries->clear();

  /*QRandomGenerator rg(QTime(0, 0, 0).secsTo(QTime::currentTime()));
  for (int i = 0; i < 30; i++) {
    int a = rg() % (20);
    QPointF pf(i, a);
    pointlist.append(pf);

    chart->series->append(pf.x(), pf.y());
    chart->m_scatterSeries->append(pf.x(), pf.y());
  }*/

  int count = listMonth.count();
  if (count == 0) {
    return;
  }

  /*for (int i = 0; i < count; i++) {
    QString str = listMonth.at(i);
    QStringList list = str.split("|");

    if (list.count() == 2) {
      x = list.at(0).toInt();
      if (isrbFreq) {
        doubleList.append(list.at(1).toInt());
        y = list.at(1).toInt();
      } else {
        y = list.at(1).toDouble();
        doubleList.append(y);
      }

      series->append(x, y);
      m_scatterSeries->append(x, y);
    }
  }*/

  double maxValue = *std::max_element(doubleList.begin(), doubleList.end());
  qDebug() << "In table Max:" << maxValue;
  double max;
  if (isrbFreq) {
    max = 18;
    if (maxValue > 20) max = maxValue + 2;

    // chart->setAxis(strM + "  " + strY + "    " + tr("Today") + ": " +
    //                    QString::number(today),
    //                1, 31, 31, tr("Freq"), 0, max + 2, 5);

  } else {
    max = 80.00;
    if (maxValue > max) max = maxValue + 20;

    // chart->setAxis(strM + "  " + strY + "    " + tr("Today") + ": " +
    //                    QString::number(today),
    //                1, 31, 31, tr("Amount"), 0, max + 20, 5);
  }

  yMaxMonth = max;
  // chartMonth->createDefaultAxes();
  // chartMonth->axes(Qt::Horizontal).first()->setRange(0, 31);
  // chartMonth->axes(Qt::Vertical).first()->setRange(0, max);
}

void MainWindow::drawMonth() {
  QChart* chart = new QChart();
  chart->setTitle("Line chart");
  chart->legend()->hide();
  chart->setMargins(QMargins(0, 0, 0, 0));
  chart->setContentsMargins(0, 0, 0, 0);
  chart->setAnimationOptions(QChart::SeriesAnimations);  //设置曲线动画模式
  QSplineSeries* series;
  QScatterSeries* m_scatterSeries;
  series = new QSplineSeries(chart);
  series->setPen(QPen(Qt::blue, 3, Qt::SolidLine));
  m_scatterSeries = new QScatterSeries(chart);  //创建散点
  m_scatterSeries->setMarkerShape(
      QScatterSeries::MarkerShapeCircle);  //设置散点样式
  m_scatterSeries->setMarkerSize(10);      //设置散点大小
  QChartView* chartview = new QChartView(chart);
  chartview->setRenderHint(QPainter::Antialiasing);  //防止图形走样
  ui->pLayout->addWidget(chartview);

  QRandomGenerator rg(QTime(0, 0, 0).secsTo(QTime::currentTime()));
  for (int i = 0; i < 30; i++) {
    int a = rg() % (20);
    QPointF pf(i, a);

    series->append(pf.x(), pf.y());
    m_scatterSeries->append(pf.x(), pf.y());

    chart->addSeries(series);
    chart->addSeries(m_scatterSeries);
  }

  chart->createDefaultAxes();
  chart->axes(Qt::Horizontal).first()->setRange(0, 31);
  chart->axes(Qt::Vertical).first()->setRange(0, 20);
}

void MainWindow::initChartTimeLine(QTreeWidget* tw, bool isDay) {
  if (loading) return;
  series2->clear();
  m_scatterSeries2->clear();

  int topCount = tw->topLevelItemCount();
  if (topCount == 0) {
    chartDay->addSeries(series2);
    chartDay->addSeries(m_scatterSeries2);
    return;
  }
  if (!isDay) {
    chartDay->addSeries(series2);
    chartDay->addSeries(m_scatterSeries2);
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
  if (child) {
    childCount = item->parent()->childCount();
    parentItem = item->parent();
  } else {
    childCount = item->childCount();
    parentItem = item;
  }

  //设置坐标系
  int y0 = 0;
  if (isrbFreq) {
    y0 = 3;
    if (childCount > y0) y0 = childCount;

    yMaxDay = y0;
  }

  QVector<double> dList;
  if (!isrbFreq) {
    double y0 = 100;
    for (int i = 0; i < parentItem->childCount(); i++) {
      QString str = parentItem->child(i)->text(1);
      y0 = str.toDouble();
      dList.append(y0);
    }
    y0 = *std::max_element(dList.begin(), dList.end());

    yMaxDay = y0;
  }

  double x, y;
  for (int i = 0; i < childCount; i++) {
    QString str, str1;
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
      y = dList.at(i);
    }

    series2->append(x, y);
    m_scatterSeries2->append(x, y);
  }
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
  QSettings Reg(iniFile, QSettings::IniFormat);
  Reg.remove("/tab" + QString::number(count + 1));

  ui->tabWidget->addTab(init_TreeWidget("tab" + QString::number(count + 1)),
                        tr("Counter") + " " + QString::number(count + 1));
  ui->tabWidget->setCurrentIndex(count);
  init_TabNavigate();
  on_actionRename_triggered();
}

void MainWindow::on_actionDel_Tab_triggered() {
  int index = ui->tabWidget->currentIndex();
  if (index < 0) return;
  if (index == 0) {
    QMessageBox msgBox;
    msgBox.setText(appName);
    msgBox.setInformativeText(
        tr("The first tab is not allowed to be deleted."));

    QPushButton* btnOk = msgBox.addButton(tr("Ok"), QMessageBox::AcceptRole);
    btnOk->setFocus();
    // msgBox.exec();
    // return;
  }
  QString str1 = ui->tabWidget->tabText(index);
  QMessageBox msgBox;
  msgBox.setText("Xcounter");
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

  // Save all
  SaveAll = true;
  mySearchThread->start();

  int TabCount = ui->tabWidget->tabBar()->count();
  if (TabCount == 0) {
    ui->tabWidget->addTab(init_TreeWidget("tab1"),
                          tr("Counter") + " " + QString::number(1));
    ui->tabWidget->setTabToolTip(0, "");
  }

  init_TabNavigate();
}

QTreeWidget* MainWindow::init_TreeWidget(QString name) {
  QTreeWidget* tw = new QTreeWidget(this);
  if (name != "")
    tw->setObjectName(name);
  else
    tw->setObjectName("tw" + init_Objname());
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
  //这个不太好用，备研
  // QScroller::grabGesture(tw, QScroller::TouchGesture);
  QScroller::grabGesture(tw, QScroller::LeftMouseButtonGesture);
  return tw;
}

void MainWindow::on_twItemClicked() {
  QTreeWidget* tw = (QTreeWidget*)ui->tabWidget->currentWidget();
  QTreeWidgetItem* item = tw->currentItem();
  if (item->parent() == NULL && item->childCount() == 0) return;
  QTreeWidgetItem* pItem;

  if (item->childCount() > 0) pItem = item;
  if (item->childCount() == 0 && item->parent()->childCount() > 0)
    pItem = item->parent();
  if (item->parent() != NULL) {
    QString str = item->text(2);
    if (str.length() > 0)
      ui->lblStats->setText(str);
    else
      init_Stats(tw);
  } else
    init_Stats(tw);
  if (parentItem == pItem) return;
  // initChartTimeLine(tw, true);
  if (tabChart->currentIndex() == 1) {
    if (isReadEnd) myReadThread->start();
  }
}

void MainWindow::set_Time() {
  QTreeWidget* tw = (QTreeWidget*)ui->tabWidget->currentWidget();
  QTreeWidgetItem* item = tw->currentItem();
  if (item->childCount() == 0 && item->parent()->childCount() > 0) {
    QString time = mydlgSetTime->ui->timeEdit->text();
    item->setText(0, time);
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
    saveData(tw, ui->tabWidget->currentIndex());
  }
}

void MainWindow::sort_childItem(QTreeWidgetItem* item) {
  QStringList keys, list;
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
  }

  std::sort(keys.begin(), keys.end(),
            [](const QString& s1, const QString& s2) { return s1 < s2; });

  for (int i = 0; i < childCount; i++) {
    QTreeWidgetItem* childItem = item->parent()->child(i);
    QString str = keys.at(i);
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
    QTime time;
    time.setHMS(sh.toInt(), sm.toInt(), ss.toInt());
    mydlgSetTime->ui->lblTitle->setText(tr("Modify"));
    mydlgSetTime->ui->timeEdit->setTime(time);

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
}

void MainWindow::on_tabWidget_currentChanged(int index) {
  int count = ui->tabWidget->tabBar()->count();
  if (index == 0) {
    ui->btnLeft->setEnabled(false);
    ui->btnRight->setEnabled(true);
  }
  if (index > 0 && index < count - 1) {
    ui->btnLeft->setEnabled(true);
    ui->btnRight->setEnabled(true);
  }
  if (index == count - 1) {
    ui->btnLeft->setEnabled(true);
    ui->btnRight->setEnabled(false);
  }

  if (isSlide || loading || count <= 0) {
    return;
  }

  if (isReadEnd) myReadThread->start();
  return;
  tabData->setCurrentIndex(index);
  QTreeWidget* tw = (QTreeWidget*)tabData->currentWidget();

  qDebug() << "currentTW: " << tw;
  MainWindow::get_Today(tw);

  if (tabChart->currentIndex() == 0) MainWindow::drawMonthChart();
  if (tabChart->currentIndex() == 1) MainWindow::initChartTimeLine(tw, true);

  init_Stats(tw);
  init_NavigateBtnColor();
}

void MainWindow::saveNotes() {
  if (loading) return;
  QSettings Reg(iniFile, QSettings::IniFormat);
  QTreeWidget* tw = (QTreeWidget*)tabData->currentWidget();
  QString name = tw->objectName();
  int index = tabData->currentIndex();

  QString text = QString::number(curPos) + "|" + QString::number(sliderPos) +
                 "|" + noteText;
  Reg.setValue("/" + name + "/Note", text);
  tabData->setTabToolTip(index, text);
}

void MainWindow::on_btnLeft_clicked() {
  if (ui->tabWidget->currentIndex() < 0) return;
  int index = ui->tabWidget->currentIndex();
  if (index == 0) return;
  ui->tabWidget->setCurrentIndex(index - 1);
}

void MainWindow::on_btnRight_clicked() {
  if (ui->tabWidget->currentIndex() < 0) return;
  int index = ui->tabWidget->currentIndex();
  if (index == ui->tabWidget->tabBar()->count() - 1) return;
  ui->tabWidget->setCurrentIndex(index + 1);
}

void MainWindow::on_actionNotes_triggered() {
  mydlgNotes->setGeometry(0, 0, this->width(), this->height() / 2);

  int index = ui->tabWidget->currentIndex();
  mydlgNotes->ui->textEdit->clear();

  mydlgNotes->ui->textBrowser->setHidden(true);
  mydlgNotes->ui->textEdit->setHidden(false);
  mydlgNotes->setModal(true);
  mydlgNotes->ui->lblTitle->setText(tr("Notes :"));
  mydlgNotes->show();

  QString str = ui->tabWidget->tabToolTip(index);
  QStringList list = str.split("|");
  if (list.count() > 2) {
    mydlgNotes->ui->textEdit->setPlainText(list.at(2));
    QTextCursor tmpCursor = mydlgNotes->ui->textEdit->textCursor();
    QString curPos = list.at(0);
    tmpCursor.setPosition(curPos.toInt());
    mydlgNotes->ui->textEdit->setTextCursor(tmpCursor);
    QString sliderPos = list.at(1);
    mydlgNotes->ui->textEdit->verticalScrollBar()->setSliderPosition(
        sliderPos.toInt());

  } else
    mydlgNotes->ui->textEdit->setPlainText(str);
}

QString MainWindow::init_Objname() {
  QString y = QString::number(QDate::currentDate().year());
  QString m = QString::number(QDate::currentDate().month());
  QString d = QString::number(QDate::currentDate().day());
  QString h = QString::number(QTime::currentTime().hour());
  QString mm = QString::number(QTime::currentTime().minute());
  QString s = QString::number(QTime::currentTime().second());
  QString CurrentDateTime = y + m + d + h + mm + s;
  QRandomGenerator rg(QTime(0, 0, 0).secsTo(QTime::currentTime()));
  int a = rg() % (1000);
  return CurrentDateTime + QString::number(a);
}

void MainWindow::on_btnNotes_clicked() { emit on_actionNotes_triggered(); }

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

  if (watch == ui->tabWidget->tabBar()) {
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
        SaveAll = true;
        mySearchThread->start();
      }
    }
  }

  if (watch != ui->tabWidget->tabBar() && watch != tw) {
    static int press_x;  //鼠标按下时的位置
    static int press_y;
    static int relea_x;  //鼠标释放时的位置
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
      qDebug() << "Press:" << press_x << press_y;
    }

    if (event->type() == QEvent::MouseButtonRelease) {
      relea_x = event->globalX();
      relea_y = event->globalY();
      qDebug() << "Release:" << relea_x << relea_y;
    }

    //判断滑动方向（右滑）

    if ((relea_x - press_x) > 20 &&
        event->type() == QEvent::MouseButtonRelease &&
        qAbs(relea_y - press_y) < 50) {
      int current_page = ui->tabWidget->currentIndex();
      if (current_page < count - 1) {
        isSlide = true;
        // ui->lblStats->setPixmap(ui->tabWidget->currentWidget()
        //                             ->grab());  //捕获当前界面并绘制到label上
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
        readData((QTreeWidget*)ui->tabWidget->currentWidget());
        init_NavigateBtnColor();
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
        readData((QTreeWidget*)ui->tabWidget->currentWidget());
        init_NavigateBtnColor();
        isSlide = false;
      }
    }
  }

  return QWidget::eventFilter(watch, evn);
}

void MainWindow::on_actionAbout_triggered() {
  QUrl url(QString("https://github.com/ic005k/Xcounter#readme"));
  QDesktopServices::openUrl(url);
}

void MainWindow::on_actionExport_Data_triggered() {
  QString fileName;
  QFileDialog fd;
  fileName =
      fd.getSaveFileName(this, tr("XcounterBak"), "", tr("Data Files(*.ini)"));

  if (!fileName.isNull()) {
    QTextEdit* txtEdit = new QTextEdit;
    txtEdit->setPlainText(loadText(iniFile));
    TextEditToFile(txtEdit, fileName);
  }
}

void MainWindow::on_actionImport_Data_triggered() {
  QString fileName;
  fileName = QFileDialog::getOpenFileName(this, tr("XcounterBak"), "",
                                          tr("Data Files (*.ini)"));
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
  if (!txt.contains(appName)) {
    QMessageBox msgBox;
    msgBox.setText(appName);
    msgBox.setInformativeText(tr("Invalid data file."));

    QPushButton* btnOk = msgBox.addButton(tr("Ok"), QMessageBox::AcceptRole);
    btnOk->setFocus();
    msgBox.exec();
    return;
  }

  if (!fileName.isNull()) {
    if (QFile(iniFile).exists()) QFile(iniFile).remove();
    bool ok = QFile::copy(fileName, iniFile);
    if (ok) init_TabData();
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
  mydlgNotes->ui->textBrowser->clear();
  QSettings Reg(iniFile, QSettings::IniFormat);
  QString keys = Reg.value("/" + ver + "-" + appName + "/AllKeys").toString();
  QString fs = Reg.value("/" + ver + "-" + appName + "/FileSize").toString();
  QString inif = Reg.value("/" + ver + "-" + appName + "/File").toString();
  if (keys.toInt() > 10000) {
    mydlgNotes->ui->textBrowser->append("AllKeys : " + keys);
    mydlgNotes->ui->textBrowser->append("FileSize : " + fs);
    mydlgNotes->ui->textBrowser->append("File : " + inif);
  } else
    mydlgNotes->ui->textBrowser->setPlainText(loadText(iniFile));

  mydlgNotes->ui->textBrowser->setHidden(false);
  mydlgNotes->ui->textEdit->setHidden(true);

  mydlgNotes->ui->lblTitle->setText(tr("App Data :"));
  mydlgNotes->setGeometry(0, 0, this->width(), this->height());
  mydlgNotes->setModal(true);
  mydlgNotes->show();
}

void MainWindow::on_btnFind_clicked() {
  if (ui->frame_find->isHidden()) {
    ui->frame_find->setHidden(false);
    ui->btnPlus->setHidden(true);
    ui->btnLess->setHidden(true);
    ui->btnTodo->setHidden(true);
    ui->btnMax->setHidden(true);
  } else {
    ui->frame_find->setHidden(true);
    ui->btnPlus->setHidden(false);
    ui->btnLess->setHidden(false);
    ui->btnTodo->setHidden(false);
    ui->btnMax->setHidden(false);
  }
}

void MainWindow::goResults() {
  if (loading) return;
  QStringList listYear, listMonth, listDay;
  bool isDay = false;
  QTreeWidget* tw = (QTreeWidget*)ui->tabWidget->currentWidget();
  for (int i = 0; i < tw->topLevelItemCount(); i++) {
    QTreeWidgetItem* topItem = tw->topLevelItem(i);
    QString str0 = topItem->text(0);
    QString y, m, d;
    y = get_Year(str0);
    m = get_Month(str0);
    d = QString::number(get_Day(str0));
    qDebug() << y << m << d;
    if (y == ui->btnYear->text()) {
      if (m == ui->btnMonth->text()) {
        if (d == ui->btnDay->text()) {
          tw->setCurrentItem(topItem);
          initChartTimeLine(tw, true);
          isDay = true;
          break;
        }
      }
    }
  }
  if (!isDay) {
    initChartTimeLine(tw, false);
  }
}

void MainWindow::goResultsMonth() {
  if (loading) return;
  QStringList listMonth;
  QString strY, strM;
  strY = ui->btnYear->text();
  strM = ui->btnMonth->text();
  listMonth = get_MonthList(strY, strM);
  initChart(strY, strM, listMonth);
}

QStringList MainWindow::get_MonthList(QString strY, QString strM) {
  // 格式：天 ｜ 总数（或金额）
  doubleList.clear();
  series->clear();
  m_scatterSeries->clear();

  QStringList listMonth;
  QTreeWidget* tw = (QTreeWidget*)tabData->currentWidget();
  for (int i = 0; i < tw->topLevelItemCount(); i++) {
    QTreeWidgetItem* topItem = tw->topLevelItem(i);
    QString str0 = topItem->text(0);
    QString y, m, d;
    y = get_Year(str0);
    m = get_Month(str0);
    d = QString::number(get_Day(str0));

    if (y == strY) {
      if (m == strM) {
        if (isrbFreq) {
          listMonth.append(d + "|" + topItem->text(1));  //记录天和总数

          int y0 = topItem->text(1).toInt();
          doubleList.append(y0);
          series->append(d.toInt(), y0);
          m_scatterSeries->append(d.toInt(), y0);
        } else {
          listMonth.append(d + "|" + topItem->text(2));

          double y0 = topItem->text(2).toDouble();
          doubleList.append(y0);
          series->append(d.toInt(), y0);
          m_scatterSeries->append(d.toInt(), y0);
        }
      }
    }
  }

  return listMonth;
}

void MainWindow::on_cboxYear_currentTextChanged(const QString& arg1) {
  Q_UNUSED(arg1);
  goResultsMonth();
  goResults();
  QSettings Reg(iniFile, QSettings::IniFormat);
  Reg.setValue("/YMD/Y", ui->btnYear->text());
}

void MainWindow::on_cboxMonth_currentTextChanged(const QString& arg1) {
  Q_UNUSED(arg1);
  goResultsMonth();
  goResults();
  QSettings Reg(iniFile, QSettings::IniFormat);
  Reg.setValue("/YMD/M", ui->btnMonth->text());
}

void MainWindow::on_cboxDay_currentTextChanged(const QString& arg1) {
  Q_UNUSED(arg1);
  goResultsMonth();
  goResults();
  QSettings Reg(iniFile, QSettings::IniFormat);
  Reg.setValue("/YMD/D", ui->btnDay->text());
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

QVector<QTreeWidgetItem*> MainWindow::findDisc() {
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
      "QComboBox::drop-down{subcontrol-origin:padding;subcontrol-position:top "
      "right;width:15px;border-left-width:0px;border-left-style:solid;border-"
      "top-right-radius:3px;border-bottom-right-radius:3px;border-left-color:#"
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
  isrbFreq = true;

  QTreeWidget* tw = (QTreeWidget*)tabData->currentWidget();
  if (tabChart->currentIndex() == 0) drawMonthChart();
  if (tabChart->currentIndex() == 1) initChartTimeLine(tw, true);
}

void MainWindow::on_rbAmount_clicked() {
  isrbFreq = false;
  QTreeWidget* tw = (QTreeWidget*)tabData->currentWidget();
  if (tabChart->currentIndex() == 0) drawMonthChart();
  if (tabChart->currentIndex() == 1) initChartTimeLine(tw, true);
}

void MainWindow::paintEvent(QPaintEvent* event) {
  Q_UNUSED(event);

  //获取背景色
  QPalette pal = ui->btnFind->palette();
  QBrush brush = pal.window();
  int c_red = brush.color().red();
  if (c_red != red) {
    red = c_red;
    if (red < 55)
      mydlgTodo->ui->listWidget->setStyleSheet(mydlgTodo->styleDark);
    else
      mydlgTodo->ui->listWidget->setStyleSheet(
          mydlgTodo->ui->listWidget->styleSheet());
  }
}

void MainWindow::on_btnZoom_clicked() {
  if (ui->btnZoom->text() == tr("Zoom")) {
    ui->frame_tab->setHidden(true);
    ui->btnZoom->setText(tr("Normal"));
  } else {
    if (ui->btnZoom->text() == tr("Normal")) {
      ui->frame_tab->setHidden(false);
      ui->btnZoom->setText(tr("Zoom"));
    }
  }
}

void MainWindow::on_btnMax_clicked() {
  if (ui->btnZoom->text() != tr("Zoom")) return;
  if (ui->btnMax->text() == tr("Max")) {
    ui->frame_tab->setMaximumHeight(this->height());
    ui->frame_charts->setHidden(true);
    ui->btnMax->setText(tr("Min"));
  } else if (ui->btnMax->text() == tr("Min")) {
    ui->frame_tab->setMaximumHeight(this->height() / 2 - ui->btnTodo->height());
    ui->frame_charts->setHidden(false);
    ui->btnMax->setText(tr("Max"));
  }
}

void MainWindow::on_btnYear_clicked() {
  int w = ui->btnYear->width();
  QListWidget* list = new QListWidget(this);
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
    list->close();
    on_cboxYear_currentTextChanged("");
  });

  int h = 30 * list->count() + 2;
  int y = ui->frame_find->y() - h / 2;

  list->setGeometry(ui->frame_find->x(), y, w + 15, h);

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
    list->close();
    on_cboxMonth_currentTextChanged("");
  });

  int h = 30 * list->count() + 2;
  int y = ui->frame_find->y() - h / 2;

  list->setGeometry(ui->btnMonth->x() + ui->frame_find->x(), y, w + 5, h);

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
  list->verticalScrollBar()->setStyleSheet(vsbarStyleSmall);
  list->setVerticalScrollMode(QListWidget::ScrollPerPixel);
  QScroller::grabGesture(list, QScroller::LeftMouseButtonGesture);
  QFont font;
  font.setPointSize(fontSize);
  list->setFont(font);
  int day = 1;
  QStringList strList;
  for (int i = 0; i < 31; i++) {
    strList.append(QString::number(day + i));
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
    list->close();
    on_cboxDay_currentTextChanged("");
  });

  int h = 13 * 30;
  int y = ui->frame_find->y() - h / 2;
  int x = ui->btnDay->x() + ui->frame_find->x();
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
  if (isTesting) saveData(tw, ui->tabWidget->currentIndex());  //测试专用
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
  if (isReadEnd) myReadThread->start();
}
