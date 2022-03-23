#include "mainwindow.h"

#include <QMessageBox>

#include "ui_mainwindow.h"

QString appName = "Xcounter";
QString iniFile = "/data/data/org.qtproject.example.Xcount/Xcount.ini";
MainWindow* mw_one;
bool loading = false;
extern bool isAndroid, isIOS, zh_cn;
int fontSize, red;
QRegularExpression regxNumber("^-?\[0-9.]*$");
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  QString ver = "1.0.01";
  ui->actionAbout->setText(tr("About") + " (" + ver + ")");
  fontSize = this->font().pixelSize();

  chart = new Chart(this, tr("History Data"));
  ui->pLayout->setMargin(0);
  ui->pLayout->setSpacing(0);
  ui->pLayout->setContentsMargins(0, 0, 0, 0);
  ui->pLayout->addWidget(chart);
  chartTimeLine = new Chart(this, tr("Time Line"));
  ui->glTimeLine->setMargin(0);
  ui->glTimeLine->setSpacing(0);
  ui->glTimeLine->setContentsMargins(0, 0, 0, 0);
  ui->glTimeLine->addWidget(chartTimeLine);
  mw_one = this;
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
  mydlgList->ui->listWidget->setFont(this->font());
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

  this->layout()->setMargin(0);
  this->layout()->setSpacing(0);
  this->layout()->setContentsMargins(1, 1, 1, 1);
  loading = true;
  ui->statusbar->setHidden(true);
  this->setWindowTitle("");
  tmer = new QTimer(this);
  connect(tmer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
  // tmer->start(1000);
  strDate = QDate::currentDate().toString();  //"yyyy-MM-dd");
  ui->frame_tab->setMaximumHeight(this->height() / 2 - 30);
  ui->lcdNumber->setHidden(true);
  ui->tabCharts->setCornerWidget(ui->frame_cw);
  ui->frame_find->setHidden(true);
  QStringList listMonth;
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
  ui->cboxMonth->clear();
  ui->cboxMonth->addItems(listMonth);
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
  QSettings Reg(iniFile, QSettings::IniFormat);
  ui->cboxYear->setCurrentText(Reg.value("/YMD/Y").toString());
  ui->cboxMonth->setCurrentText(Reg.value("/YMD/M").toString());
  ui->cboxDay->setCurrentText(Reg.value("/YMD/D").toString());
  // Custom Desc
  mw_one->mydlgList->ui->listWidget->clear();
  int descCount = Reg.value("/CustomDesc/Count").toInt();
  for (int i = 0; i < descCount; i++) {
    mw_one->mydlgList->ui->listWidget->addItem(
        Reg.value("/CustomDesc/Item" + QString::number(i)).toString());
  }

  mydlgTodo->init_Items();
  init_Data();

  QTreeWidget* tw = (QTreeWidget*)ui->tabWidget->currentWidget();
  get_Today(tw);
  init_Stats(tw);

  initMonthChart();
  initChartTimeLine(tw, true);

  isInit = true;
  loading = false;
}

void MainWindow::init_Data() {
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
    qDebug() << "strNotes" << strNotes;

    readData(tw);
  }

  if (TabCount == 0) {
    ui->tabWidget->addTab(init_TreeWidget("tab1"),
                          tr("Counter") + " " + QString::number(1));
    ui->tabWidget->setTabToolTip(0, "");
  }

  ui->tabWidget->setCurrentIndex(Reg.value("CurrentIndex").toInt());
}

void MainWindow::timerUpdate() {
  QDateTime dateTime = QDateTime::currentDateTime();
  ui->lcdNumber->display(dateTime.toString("HH:mm:ss") + " - " +
                         QString::number(today));
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::add_Data(QTreeWidget* tw, QString strTime, QString strAmount,
                          QString strDesc) {
  bool isYes = false;
  strDate = QDate::currentDate().toString();
  for (int i = 0; i < tw->topLevelItemCount(); i++) {
    QString str = tw->topLevelItem(i)->text(0);
    if (str == strDate) {
      isYes = true;

      QTreeWidgetItem* topItem = tw->topLevelItem(i);
      QTreeWidgetItem* item11 = new QTreeWidgetItem(topItem);
      item11->setText(0, strTime);
      item11->setText(1, QString("%1").arg(strAmount.toDouble(), 0, 'f', 2));
      item11->setText(2, strDesc);
      int child = topItem->childCount();
      topItem->setTextAlignment(1, Qt::AlignHCenter | Qt::AlignVCenter);

      // Amount
      double amount = 0;
      for (int m = 0; m < child; m++) {
        QString str = topItem->child(m)->text(1);
        amount = amount + str.toDouble();
      }
      QString strAmount = QString("%1").arg(amount, 0, 'f', 2);
      topItem->setText(1, QString::number(child));
      topItem->setText(2, strAmount);

      break;
    }
  }
  if (!isYes) {
    QTreeWidgetItem* topItem = new QTreeWidgetItem;
    topItem->setText(0, strDate);
    tw->addTopLevelItem(topItem);
    QTreeWidgetItem* item11 = new QTreeWidgetItem(topItem);
    item11->setText(0, strTime);
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
    topItem->setText(2, strAmount);
  }

  QTreeWidgetItem* topItem = tw->topLevelItem(tw->topLevelItemCount() - 1);
  tw->setCurrentItem(topItem);
  sort_childItem(topItem->child(0));
  tw->setCurrentItem(topItem->child(topItem->childCount() - 1));

  saveData(tw, ui->tabWidget->currentIndex());
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

  saveData(tw, ui->tabWidget->currentIndex());
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
  int TabCount = ui->tabWidget->tabBar()->count();
  Reg.setValue("TabCount", TabCount);
  int CurrentIndex = ui->tabWidget->currentIndex();
  Reg.setValue("CurrentIndex", CurrentIndex);
  for (int i = 0; i < TabCount; i++) {
    Reg.setValue("TabName" + QString::number(i), ui->tabWidget->tabText(i));
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

  Reg.setValue("/" + name + "/" + "Note", ui->tabWidget->tabToolTip(tabIndex));
  int lines = Reg.allKeys().count();
  Reg.setValue("/" + appName + "/AllKeys", lines);
  Reg.setValue("/" + appName + "/FileSize",
               getFileSize(QFile(iniFile).size(), 2));
  Reg.setValue("/" + appName + "/File", iniFile);

  saveTab();
  mydlgTodo->saveTodo();
  mydlgSetTime->saveCustomDesc();
  get_Today(tw);
  init_Stats(tw);
  initMonthChart();
  initChartTimeLine(tw, true);
}

void MainWindow::initMonthChart() {
  QString strY, strM;
  strY = get_Year(strDate);
  strM = get_Month(strDate);
  QStringList listM = get_MonthList(strY, strM);
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

  get_Today(tw);
  init_Stats(tw);
  initMonthChart();
  initChartTimeLine(tw, true);
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

void MainWindow::closeEvent(QCloseEvent* event) {
  Q_UNUSED(event);

  saveTab();
}

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
  ui->lblStats->setText(tr("Total") + " : " + QString::number(tatol) + "    $" +
                        QString::number(amount));
}

void MainWindow::initChart(QString strY, QString strM, QStringList listMonth) {
  // listMonth Format:1.Day  2.Freq or Amount
  QVector<double> doubleList;
  QList<QPointF> pointlist;
  int count = listMonth.count();
  if (count == 0) {
    chart->buildChart(pointlist);
    return;
  }
  for (int i = 0; i < count; i++) {
    QString str = listMonth.at(i);
    QStringList list = str.split("|");

    if (ui->rbFreq->isChecked()) {
      doubleList.append(list.at(1).toInt());
    }
    if (ui->rbAmount->isChecked()) {
      doubleList.append(list.at(1).toDouble());
    }
  }
  double maxValue = *std::max_element(doubleList.begin(), doubleList.end());
  qDebug() << "In table Max:" << maxValue;
  double max;
  if (ui->rbFreq->isChecked()) {
    max = 18;
    if (maxValue > 20) max = maxValue;
    //设置坐标系
    chart->setAxis(strM + "  " + strY + "    " + tr("Today") + ": " +
                       QString::number(today),
                   1, 31, 31, tr("Freq"), 0, max + 2, 5);
  }
  if (ui->rbAmount->isChecked()) {
    max = 98.00;
    if (maxValue > max) max = maxValue;
    //设置坐标系
    chart->setAxis(strM + "  " + strY + "    " + tr("Today") + ": " +
                       QString::number(today),
                   1, 31, 31, tr("Amount"), 0, max + 20, 5);
  }

  //设置离散点数据
  pointlist = {
      QPointF(0, 8),  QPointF(1, 2),  QPointF(3, 4), QPointF(4, 8),
      QPointF(5, 16), QPointF(6, 16), QPointF(7, 8), QPointF(8, 4),
      QPointF(9, 2),  QPointF(10, 1),
  };
  pointlist.clear();

  QRandomGenerator rg(QTime(0, 0, 0).secsTo(QTime::currentTime()));
  for (int i = 0; i < 30; i++) {
    int a = rg() % (20);
    QPointF pf(i, a);
    pointlist.append(pf);
  }

  pointlist.clear();

  double x, y;
  QString strD;
  for (int i = 0; i < count; i++) {
    QString str = listMonth.at(i);
    QStringList list = str.split("|");
    strD = list.at(0);
    x = strD.toInt();
    y = doubleList.at(i);
    QPointF pf(x, y);
    pointlist.append(pf);
  }

  //绘制
  chart->buildChart(pointlist);
}

void MainWindow::initChartTimeLine(QTreeWidget* tw, bool isDay) {
  //设置离散点数据
  QList<QPointF> pointlist;
  int topCount = tw->topLevelItemCount();
  if (topCount == 0) {
    chartTimeLine->buildChart(pointlist);
    return;
  }
  if (!isDay) {
    chartTimeLine->buildChart(pointlist);
    return;
  }
  if (topCount > 0) {
    tw->setFocus();
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
  if (ui->rbFreq->isChecked()) {
    int y0 = 3;
    if (childCount > y0) y0 = childCount;
    chartTimeLine->setAxis(tr("24 hours") + "  " + tr("Total") + " : " +
                               QString::number(childCount),
                           0, 24, 1, tr("Freq"), 0, y0 + 2, 1);
  }
  QVector<double> dList;
  if (ui->rbAmount->isChecked()) {
    double y0 = 100;
    for (int i = 0; i < parentItem->childCount(); i++) {
      QString str = parentItem->child(i)->text(1);
      y0 = str.toDouble();
      dList.append(y0);
    }
    y0 = *std::max_element(dList.begin(), dList.end());

    chartTimeLine->setAxis(tr("24 hours") + "  " + tr("Total") + " : " +
                               QString::number(childCount),
                           0, 24, 1, tr("Amount"), 0, y0 + 10, 1);
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
    if (ui->rbFreq->isChecked()) y = i + 1;
    if (ui->rbAmount->isChecked()) {
      y = dList.at(i);
    }
    QPointF pf(x, y);
    pointlist.append(pf);
  }

  //绘制
  chartTimeLine->buildChart(pointlist);
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
  /*for (int i = 0; i < Reg.allKeys().count(); i++) {
    if (Reg.allKeys().at(i).contains("tab" + QString::number(count + 1))) {
      Reg.remove(Reg.allKeys().at(i));
      i--;
    }
  }*/
  Reg.remove("/tab" + QString::number(count + 1));

  ui->tabWidget->addTab(init_TreeWidget("tab" + QString::number(count + 1)),
                        tr("Counter") + " " + QString::number(count + 1));
  ui->tabWidget->setCurrentIndex(count);
  emit on_actionRename_triggered();
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

  QFile(iniFile).remove();
  for (int i = 0; i < ui->tabWidget->tabBar()->count(); i++) {
    QTreeWidget* tw = (QTreeWidget*)ui->tabWidget->widget(i);
    tw->setObjectName("tab" + QString::number(i + 1));
    saveData(tw, i);
  }

  int TabCount = ui->tabWidget->tabBar()->count();
  if (TabCount == 0) {
    ui->tabWidget->addTab(init_TreeWidget("tab1"),
                          tr("Counter") + " " + QString::number(1));
    ui->tabWidget->setTabToolTip(0, "");
  }
}

QTreeWidget* MainWindow::init_TreeWidget(QString name) {
  QTreeWidget* tw = new QTreeWidget(this);
  if (name != "")
    tw->setObjectName(name);
  else
    tw->setObjectName("tw" + init_Objname());
  tw->setColumnCount(3);
  tw->headerItem()->setText(0, "  " + tr("Date") + "  ");
  tw->headerItem()->setText(1, "  " + tr("Frequency") + "  ");
  tw->headerItem()->setText(2, tr("Amount"));
  tw->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
  tw->header()->setDefaultAlignment(Qt::AlignCenter);
  tw->setAlternatingRowColors(true);
  tw->setFrameShape(QFrame::NoFrame);
  tw->installEventFilter(this);
  tw->setMouseTracking(true);
  tw->setDragEnabled(false);
  connect(tw, &QTreeWidget::itemClicked, this, &MainWindow::on_twItemClicked);
  connect(tw, &QTreeWidget::itemDoubleClicked, this,
          &MainWindow::on_twItemDoubleClicked);
  QScrollBar* SB = tw->verticalScrollBar();
  SB->setStyleSheet(vsbarStyle);
  // QScroller::grabGesture(tw, QScroller::TouchGesture);
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
  initChartTimeLine(tw, true);
}

void MainWindow::set_Time() {
  QTreeWidget* tw = (QTreeWidget*)ui->tabWidget->currentWidget();
  QTreeWidgetItem* item = tw->currentItem();
  if (item->childCount() == 0 && item->parent()->childCount() > 0) {
    QString time = mydlgSetTime->ui->timeEdit->text();
    item->setText(0, time);
    QString sa = mydlgSetTime->ui->editAmount->text().trimmed();
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

    mydlgSetTime->ui->editAmount->setText(item->text(1));
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

  ui->tabWidget->setCurrentIndex(index);
  QTreeWidget* tw = (QTreeWidget*)ui->tabWidget->currentWidget();

  get_Today(tw);
  init_Stats(tw);
  initMonthChart();
  initChartTimeLine(tw, true);
}

void MainWindow::saveNotes() {
  if (loading) return;
  QSettings Reg(iniFile, QSettings::IniFormat);
  QTreeWidget* tw = (QTreeWidget*)ui->tabWidget->currentWidget();
  QString name = tw->objectName();
  int index = ui->tabWidget->currentIndex();
  QString text = mydlgNotes->ui->textEdit->toPlainText();
  int curPos = mydlgNotes->ui->textEdit->textCursor().position();
  int sliderPos =
      mydlgNotes->ui->textEdit->verticalScrollBar()->sliderPosition();
  text =
      QString::number(curPos) + "|" + QString::number(sliderPos) + "|" + text;
  Reg.setValue("/" + name + "/Note", text);
  ui->tabWidget->setTabToolTip(index, text);
  qDebug() << "curPos" << curPos << "sliderPos" << sliderPos;
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
    qDebug() << "curPos" << curPos << "sliderPos" << sliderPos;

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
    if (event->type() == QEvent::MouseButtonPress) {
    }
    qDebug() << "tw mousePress";
    // tw->setCurrentItem(tw->currentItem());
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
        QFile(iniFile).remove();
        for (int i = 0; i < ui->tabWidget->tabBar()->count(); i++) {
          QTreeWidget* tw = (QTreeWidget*)ui->tabWidget->widget(i);
          saveData(tw, i);
        }
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

    if (event->type() == QEvent::MouseButtonPress)  //如果鼠标按下
    {
      press_x = event->globalX();
      press_y = event->globalY();
      x = 0;
      y = 0;
      w = tw->width();
      h = tw->height();
      qDebug() << "Press:" << press_x << press_y;
    }

    if (event->type() == QEvent::MouseButtonRelease)  //如果鼠标释放
    {
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
    qDebug() << iniFile << fileName;
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
    if (ok) init_Data();
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

void MainWindow::on_tabWidget_tabBarClicked(int index) { Q_UNUSED(index) }

void MainWindow::on_tabCharts_tabBarClicked(int index) {
  if (index == 0) {
    initMonthChart();
  }
  if (index == 1) {
    initChartTimeLine(get_tw(ui->tabWidget->currentIndex()), true);
  }
}

QTreeWidget* MainWindow::get_tw(int tabIndex) {
  QTreeWidget* tw = (QTreeWidget*)ui->tabWidget->widget(tabIndex);
  return tw;
}

void MainWindow::on_actionView_App_Data_triggered() {
  mydlgNotes->ui->textBrowser->clear();
  mydlgNotes->ui->textBrowser->setText(loadText(iniFile));
  mydlgNotes->ui->textBrowser->setTextInteractionFlags(
      Qt::NoTextInteraction);  //鼠标不可选中文本
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
  } else {
    ui->frame_find->setHidden(true);
    ui->btnPlus->setHidden(false);
    ui->btnLess->setHidden(false);
    ui->btnTodo->setHidden(false);
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
    if (y == ui->cboxYear->currentText()) {
      if (m == ui->cboxMonth->currentText()) {
        if (d == ui->cboxDay->currentText()) {
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
  strY = ui->cboxYear->currentText();
  strM = ui->cboxMonth->currentText();
  listMonth = get_MonthList(strY, strM);
  initChart(strY, strM, listMonth);
}

QStringList MainWindow::get_MonthList(QString strY, QString strM) {
  QStringList listMonth;
  QTreeWidget* tw = (QTreeWidget*)ui->tabWidget->currentWidget();
  for (int i = 0; i < tw->topLevelItemCount(); i++) {
    QTreeWidgetItem* topItem = tw->topLevelItem(i);
    QString str0 = topItem->text(0);
    QString y, m, d;
    y = get_Year(str0);
    m = get_Month(str0);
    d = QString::number(get_Day(str0));

    if (y == strY) {
      if (m == strM) {
        if (ui->rbFreq->isChecked())
          listMonth.append(d + "|" + topItem->text(1));  //记录天和总数
        if (ui->rbAmount->isChecked())
          listMonth.append(d + "|" + topItem->text(2));
      }
    }
  }
  qDebug() << listMonth;
  return listMonth;
}

void MainWindow::on_cboxYear_currentTextChanged(const QString& arg1) {
  Q_UNUSED(arg1);
  goResultsMonth();
  goResults();
  QSettings Reg(iniFile, QSettings::IniFormat);
  Reg.setValue("/YMD/Y", ui->cboxYear->currentText());
}

void MainWindow::on_cboxMonth_currentTextChanged(const QString& arg1) {
  Q_UNUSED(arg1);
  goResultsMonth();
  goResults();
  QSettings Reg(iniFile, QSettings::IniFormat);
  Reg.setValue("/YMD/M", ui->cboxMonth->currentText());
}

void MainWindow::on_cboxDay_currentTextChanged(const QString& arg1) {
  Q_UNUSED(arg1);
  goResultsMonth();
  goResults();
  QSettings Reg(iniFile, QSettings::IniFormat);
  Reg.setValue("/YMD/D", ui->cboxDay->currentText());
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
  initMonthChart();
  QTreeWidget* tw = (QTreeWidget*)ui->tabWidget->currentWidget();
  initChartTimeLine(tw, true);
}

void MainWindow::on_rbAmount_clicked() {
  initMonthChart();
  QTreeWidget* tw = (QTreeWidget*)ui->tabWidget->currentWidget();
  initChartTimeLine(tw, true);
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
    ui->frame_tab->setMaximumHeight(this->height() / 2 - 30);
    ui->frame_charts->setHidden(false);
    ui->btnMax->setText(tr("Max"));
  }
}
