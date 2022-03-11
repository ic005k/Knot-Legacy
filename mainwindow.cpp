#include "mainwindow.h"

#include <QMessageBox>

#include "ui_mainwindow.h"

QString appName = "Xcount";
QString iniFile = "/data/data/org.qtproject.example.Xcount/Xcount.ini";
QString txtFile = "assets:/data/Xcount.txt";

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  loading = true;
  ui->statusbar->setHidden(true);
  this->setWindowTitle("");
  tmer = new QTimer(this);
  connect(tmer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
  tmer->start(1000);
  strDate = QDate::currentDate().toString();  //"yyyy-MM-dd");

  QDir dir;
  QString path;
  path = dir.currentPath();
  qDebug() << "Path:" << path;
  QString str0 = appName + ".app";
  if (path.contains(str0)) {
    isIOS = true;
    QString str1 = QDir::homePath() + "/" + appName + "/";
    QDir dir0;
    dir0.mkpath(str1);
    iniFile = str1 + appName + ".ini";
    txtFile = str1 + appName + ".txt";
  } else {
    QStringList list = path.split("/");
    QString last = list.at(list.count() - 1);
    QString str = path.replace(last.trimmed(), "");
    iniFile = str + appName + ".ini";
    txtFile = str + appName + ".txt";
  }

  int s = 90;
  if (isIOS) {
    ui->btnLess->setMinimumHeight(30);
    ui->btnPlus->setMinimumHeight(30);
    ui->lcdNumber->setMinimumHeight(30);
    ui->textEdit->setMaximumHeight(50);
    s = 50;
  }
  ui->btnPlus->setIconSize(QSize(s, s));
  ui->btnLess->setIconSize(QSize(s, s));
  ui->btnPlus->setIcon(QIcon(":/src/1.png"));
  ui->btnLess->setIcon(QIcon(":/src/2.png"));

  QSettings Reg(iniFile, QSettings::IniFormat);
  int TabCount = Reg.value("TabCount", 0).toInt();
  for (int i = 0; i < TabCount; i++) {
    QTreeWidget* tw = new QTreeWidget(this);
    init_TreeWidget(tw);
    tw->setObjectName("tab" + QString::number(i + 1));
    ui->tabWidget->addTab(tw, Reg.value("TabName" + QString::number(i),
                                        tr("Counter") + QString::number(i + 1))
                                  .toString());
  }

  if (TabCount == 0) {
    QTreeWidget* tw = new QTreeWidget(this);
    tw->setObjectName("tab1");
    init_TreeWidget(tw);
    ui->tabWidget->addTab(tw, tr("Counter") + " " + QString::number(1));
  }

  QTreeWidget* tw = (QTreeWidget*)ui->tabWidget->currentWidget();
  readData(tw);
  isInit = true;
  loading = false;
}

void MainWindow::timerUpdate() {
  QDateTime dateTime = QDateTime::currentDateTime();
  ui->lcdNumber->display(dateTime.toString("HH:mm:ss") + " - " +
                         QString::number(today));
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::add_Data(QTreeWidget* tw) {
  bool isYes = false;
  for (int i = 0; i < tw->topLevelItemCount(); i++) {
    QString str = tw->topLevelItem(i)->text(0);
    if (str == strDate) {
      isYes = true;
      QTreeWidgetItem* topItem = new QTreeWidgetItem;
      topItem = tw->topLevelItem(i);
      QTreeWidgetItem* item11 = new QTreeWidgetItem(topItem);
      item11->setText(0, QTime::currentTime().toString());
      int child = topItem->childCount();
      topItem->setTextAlignment(1, Qt::AlignHCenter | Qt::AlignVCenter);
      topItem->setText(1, QString::number(child));
      break;
    }
  }
  if (!isYes) {
    QTreeWidgetItem* topItem = new QTreeWidgetItem;
    topItem->setText(0, strDate);
    tw->addTopLevelItem(topItem);
    QTreeWidgetItem* item11 = new QTreeWidgetItem(topItem);
    item11->setText(0, QTime::currentTime().toString());
    int child = topItem->childCount();
    topItem->setTextAlignment(1, Qt::AlignHCenter | Qt::AlignVCenter);
    topItem->setText(1, QString::number(child));
  }

  saveData(tw);
}

void MainWindow::del_Data(QTreeWidget* tw) {
  for (int i = 0; i < tw->topLevelItemCount(); i++) {
    QString str = tw->topLevelItem(i)->text(0);
    if (str == strDate) {
      QTreeWidgetItem* topItem = new QTreeWidgetItem;
      topItem = tw->topLevelItem(i);
      int child = topItem->childCount();
      if (child > 0) {
        topItem->removeChild(topItem->child(child - 1));
        topItem->setTextAlignment(1, Qt::AlignHCenter | Qt::AlignVCenter);
        topItem->setText(1, QString::number(child - 1));
        break;
      }
    }
  }

  saveData(tw);
}

void MainWindow::on_btnPlus_clicked() {
  add_Data((QTreeWidget*)ui->tabWidget->currentWidget());
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

void MainWindow::saveData(QTreeWidget* tw) {
  QSettings Reg(iniFile, QSettings::IniFormat);
  int count = tw->topLevelItemCount();
  QString name = tw->objectName();
  Reg.setValue(name + "TopCount", count);
  for (int i = 0; i < count; i++) {
    Reg.setValue(name + QString::number(i + 1) + "1",
                 tw->topLevelItem(i)->text(0));
    Reg.setValue(name + QString::number(i + 1) + "2",
                 tw->topLevelItem(i)->text(1));
    int childCount = tw->topLevelItem(i)->childCount();
    Reg.setValue(name + QString::number(i + 1) + "childCount", childCount);
    for (int j = 0; j < childCount; j++) {
      Reg.setValue(name + QString::number(i + 1) + "child" + QString::number(j),
                   tw->topLevelItem(i)->child(j)->text(0));
    }
  }

  TextEditToFile(ui->textEdit, txtFile);
  qDebug() << iniFile << Reg.value("TopCount").toInt();

  get_Today(tw);
  init_Stats(tw);
  initChart(tw);
}

void MainWindow::readData(QTreeWidget* tw) {
  tw->clear();
  QSettings Reg(iniFile, QSettings::IniFormat);
  QString name = tw->objectName();
  int rowCount = Reg.value(name + "TopCount").toInt();
  qDebug() << iniFile << rowCount;
  for (int i = 0; i < rowCount; i++) {
    QTreeWidgetItem* topItem = new QTreeWidgetItem;
    topItem->setText(0,
                     Reg.value(name + QString::number(i + 1) + "1").toString());
    tw->addTopLevelItem(topItem);
    int childCount =
        Reg.value(name + QString::number(i + 1) + "childCount").toInt();
    topItem->setTextAlignment(1, Qt::AlignHCenter | Qt::AlignVCenter);
    topItem->setText(1, QString::number(childCount));

    for (int j = 0; j < childCount; j++) {
      QTreeWidgetItem* item11 = new QTreeWidgetItem(topItem);
      item11->setText(0, Reg.value(name + QString::number(i + 1) + "child" +
                                   QString::number(j))
                             .toString());
    }
  }

  // ui->textEdit->setPlainText(loadText("assets:/data/readme.txt"));
  ui->textEdit->setPlainText(loadText(txtFile));

  get_Today(tw);
  init_Stats(tw);
  initChart(tw);
}

void MainWindow::get_Today(QTreeWidget* tw) {
  int count = tw->topLevelItemCount();
  if (count <= 0) return;
  QString str0 = tw->topLevelItem(count - 1)->text(0);
  QString str1 = tw->topLevelItem(count - 1)->text(1);
  if (strDate == str0) {
    today = str1.toInt();

  } else
    today = 0;
}

QString MainWindow::loadText(QString textFile) {
  QFileInfo fi(textFile);
  if (fi.exists()) {
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
  TextEditToFile(ui->textEdit, txtFile);

  QSettings Reg(iniFile, QSettings::IniFormat);
  int TabCount = ui->tabWidget->tabBar()->count();
  Reg.setValue("TabCount", TabCount);
  for (int i = 0; i < TabCount; i++) {
    Reg.setValue("TabName" + QString::number(i), ui->tabWidget->tabText(i));
  }
}

void MainWindow::init_Stats(QTreeWidget* tw) {
  int count = tw->topLevelItemCount();
  int tatol = 0;
  for (int i = 0; i < count; i++) {
    int n = tw->topLevelItem(i)->text(1).toInt();
    tatol = tatol + n;
  }

  double a = (double)tatol / 20;
  int a1, b, c;
  a1 = a;
  b = a1 / 10;
  c = a1 % 10;
  ui->lblStats->setText("总计：" + QString::number(a) + " 盒 ( " +
                        QString::number(b) + " 条 " + QString::number(c) +
                        " 盒 ) ");
}

void MainWindow::initChart(QTreeWidget* tw) {
  if (isInit) delete chart;
  QStringList strList;
  int count = tw->topLevelItemCount();
  for (int i = 0; i < count; i++) {
    strList.append(tw->topLevelItem(i)->text(1));
  }
  for (int i = 0; i < strList.count(); i++) {
    if (strList.count() > 1) {
      if (strList.at(0) < strList.at(1))
        strList.removeAt(0);
      else
        strList.removeAt(1);
      i--;
    }
  }
  int max = 20;
  if (strList.count() == 1) {
    QString str = strList.at(0);
    qDebug() << "In table Max:" << str;
    if (str.toInt() > 20) max = str.toInt();
  }

  //设置表头
  chart = new Chart(this, "历史数据");
  ui->pLayout->addWidget(chart);
  //设置坐标系
  chart->setAxis("天数(近30天)", 0, 30, 30, "频次", 0, max, 5);
  //设置离散点数据
  QList<QPointF> pointlist = {
      QPointF(0, 8),  QPointF(1, 2),  QPointF(3, 4), QPointF(4, 8),
      QPointF(5, 16), QPointF(6, 16), QPointF(7, 8), QPointF(8, 4),
      QPointF(9, 2),  QPointF(10, 1),
  };
  pointlist.clear();
  qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));
  for (int i = 0; i < 30; i++) {
    int a = qrand() % (20);
    QPointF pf(i, a);
    pointlist.append(pf);
  }

  pointlist.clear();

  int start = 0;
  if (count > 30) start = count - 30;
  for (int i = start; i < count; i++) {
    int x, y;
    x = i;
    y = tw->topLevelItem(i)->text(1).toInt();
    QPointF pf(x, y);
    pointlist.append(pf);
  }

  //绘制
  chart->buildChart(pointlist);
}

void MainWindow::on_actionRename_triggered() {
  int index = ui->tabWidget->currentIndex();
  bool ok;
  QString text = QInputDialog::getText(this, tr("Rename tab name"),
                                       tr("Tab name:"), QLineEdit::Normal,
                                       ui->tabWidget->tabText(index), &ok);
  if (ok && !text.isEmpty()) {
    ui->tabWidget->setTabText(index, text);
  }
}

void MainWindow::on_actionAdd_Tab_triggered() {
  int count = ui->tabWidget->tabBar()->count();
  QTreeWidget* tw = new QTreeWidget(this);
  init_TreeWidget(tw);
  ui->tabWidget->addTab(tw, tr("Counter") + " " + QString::number(count + 1));
  tw->setObjectName("tab" + QString::number(count + 1));
  ui->tabWidget->setCurrentIndex(count);
}

void MainWindow::on_actionDel_Tab_triggered() {
  int index = ui->tabWidget->currentIndex();
  if (index == 0) return;
  ui->tabWidget->removeTab(index);
}

void MainWindow::init_TreeWidget(QTreeWidget* tw) {
  tw->setColumnCount(2);
  tw->headerItem()->setText(0, tr("Date"));
  tw->headerItem()->setText(1, tr("Frequency"));
  tw->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
  tw->header()->setDefaultAlignment(Qt::AlignCenter);
  tw->setAlternatingRowColors(true);
}

void MainWindow::on_tabWidget_currentChanged(int index) {
  if (loading) return;
  ui->tabWidget->setCurrentIndex(index);
  QTreeWidget* tw = (QTreeWidget*)ui->tabWidget->currentWidget();
  readData(tw);
}
