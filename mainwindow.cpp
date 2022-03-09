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
  int s = 90;
  ui->btnPlus->setIconSize(QSize(s, s));
  ui->btnLess->setIconSize(QSize(s, s));
  ui->btnPlus->setIcon(QIcon(":/src/1.png"));
  ui->btnLess->setIcon(QIcon(":/src/2.png"));
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

  readData();
}

void MainWindow::timerUpdate() {
  QDateTime dateTime = QDateTime::currentDateTime();
  ui->lcdNumber->display(dateTime.toString("HH:mm:ss"));
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
  Reg.setValue("RowCount", ui->tableWidget->rowCount());
  for (int i = 0; i < ui->tableWidget->rowCount(); i++) {
    Reg.setValue(QString::number(i + 1) + "1",
                 ui->tableWidget->item(i, 0)->text());
    Reg.setValue(QString::number(i + 1) + "2",
                 ui->tableWidget->item(i, 1)->text());
  }

  TextEditToFile(ui->textEdit, txtFile);
  qDebug() << iniFile << Reg.value("RowCount").toInt();

  init_Stats();
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
  init_Stats();
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
  ui->lblStats->setText("统计：" + QString::number(a) + " 盒");
}
