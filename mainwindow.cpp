#include "mainwindow.h"

#include <QMessageBox>

#include "ui_mainwindow.h"

QString appName = "Xcount";
QString iniFile = "/data/data/org.qtproject.example.Xcount/Xcount.ini";
QString txtFile = "assets:/data/Xcount.txt";

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  ui->statusbar->setHidden(true);
  this->setWindowTitle("");
  tmer = new QTimer(this);
  connect(tmer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
  tmer->start(1000);
  strDate = QDate::currentDate().toString();  //"yyyy-MM-dd");

  ui->tableWidget->setColumnCount(2);
  ui->tableWidget->setAlternatingRowColors(true);
  ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
  for (int i = 0; i < ui->tableWidget->columnCount(); i++) {
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(
        i, QHeaderView::ResizeToContents);
  }

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

  readData();

  initChart();
  isInit = true;
}

void MainWindow::timerUpdate() {
  QDateTime dateTime = QDateTime::currentDateTime();
  ui->lcdNumber->display(dateTime.toString("HH:mm:ss") + " - " +
                         QString::number(today));
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_btnPlus_clicked() {
  bool isYes = false;
  for (int i = 0; i < ui->tableWidget->rowCount(); i++) {
    QString str = ui->tableWidget->item(i, 0)->text();
    if (strDate == str) {
      isYes = true;
      int t = ui->tableWidget->item(i, 1)->text().toInt();
      QTableWidgetItem* item = new QTableWidgetItem(QString::number(t + 1));
      item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
      ui->tableWidget->setItem(i, 1, item);
      break;
    }
  }

  if (!isYes) {
    int n = ui->tableWidget->rowCount();
    ui->tableWidget->setRowCount(n + 1);
    ui->tableWidget->setItem(n, 0, new QTableWidgetItem(strDate));
    QTableWidgetItem* item = new QTableWidgetItem("1");
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tableWidget->setItem(n, 1, item);
  }

  saveData();
}

void MainWindow::on_btnLess_clicked() {
  for (int i = 0; i < ui->tableWidget->rowCount(); i++) {
    QString str = ui->tableWidget->item(i, 0)->text();
    if (strDate == str) {
      int t = ui->tableWidget->item(i, 1)->text().toInt();
      if (t > 0) {
        QTableWidgetItem* item = new QTableWidgetItem(QString::number(t - 1));
        item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget->setItem(i, 1, item);
      }
      break;
    }
  }

  saveData();
}

void MainWindow::saveData() {
  QSettings Reg(iniFile, QSettings::IniFormat);
  int count = ui->tableWidget->rowCount();
  Reg.setValue("RowCount", count);
  for (int i = 0; i < count; i++) {
    Reg.setValue(QString::number(i + 1) + "1",
                 ui->tableWidget->item(i, 0)->text());
    Reg.setValue(QString::number(i + 1) + "2",
                 ui->tableWidget->item(i, 1)->text());
  }

  TextEditToFile(ui->textEdit, txtFile);
  qDebug() << iniFile << Reg.value("RowCount").toInt();

  get_Today(count);

  init_Stats();
  initChart();
}

void MainWindow::readData() {
  QSettings Reg(iniFile, QSettings::IniFormat);
  int rowCount = Reg.value("RowCount").toInt();
  qDebug() << iniFile << rowCount;
  ui->tableWidget->setRowCount(rowCount);
  for (int i = 0; i < rowCount; i++) {
    ui->tableWidget->setItem(
        i, 0,
        new QTableWidgetItem(
            Reg.value(QString::number(i + 1) + "1").toString()));

    QTableWidgetItem* item = new QTableWidgetItem(
        Reg.value(QString::number(i + 1) + "2").toString());
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tableWidget->setItem(i, 1, item);
  }

  // ui->textEdit->setPlainText(loadText("assets:/data/readme.txt"));
  ui->textEdit->setPlainText(loadText(txtFile));

  get_Today(rowCount);

  init_Stats();
}

void MainWindow::get_Today(int rowCount) {
  ui->tableWidget->setCurrentCell(rowCount - 1, 0);
  QString str = ui->tableWidget->item(rowCount - 1, 1)->text();
  QString str1 = ui->tableWidget->item(rowCount - 1, 0)->text();
  if (strDate == str1) {
    today = str.toInt();
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
}

void MainWindow::init_Stats() {
  int count = ui->tableWidget->rowCount();
  int tatol = 0;
  for (int i = 0; i < count; i++) {
    int n = ui->tableWidget->item(i, 1)->text().toInt();
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

void MainWindow::initChart() {
  if (isInit) delete chart;
  QStringList strList;
  for (int i = 0; i < ui->tableWidget->rowCount(); i++) {
    strList.append(ui->tableWidget->item(i, 1)->text());
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
  int count = ui->tableWidget->rowCount();
  int start = 0;
  if (count > 30) start = count - 30;
  for (int i = start; i < count; i++) {
    int x, y;
    x = i;
    y = ui->tableWidget->item(i, 1)->text().toInt();
    QPointF pf(x, y);
    pointlist.append(pf);
  }

  //绘制
  chart->buildChart(pointlist);
}

void MainWindow::drawChart() {
  //设置离散点数据
  QList<QPointF> pointlist = {
      QPointF(0, 8),  QPointF(1, 2),  QPointF(3, 4), QPointF(4, 8),
      QPointF(5, 16), QPointF(6, 16), QPointF(7, 8), QPointF(8, 4),
      QPointF(9, 2),  QPointF(10, 1),
  };

  // pointlist.clear();
  int count = ui->tableWidget->rowCount();
  int start = 0;
  if (count > 30) start = count - 30;
  for (int i = start; i < count; i++) {
    int x, y;
    x = i + 1;
    y = ui->tableWidget->item(i, 1)->text().toInt();
    QPointF pf(x, y);
    pointlist.append(pf);
  }

  //绘制
  chart->close();
  chart->buildChart(pointlist);
}
