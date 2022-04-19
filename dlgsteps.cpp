#include "dlgsteps.h"

#include "mainwindow.h"
#include "ui_dlgsteps.h"
#include "ui_mainwindow.h"

extern MainWindow* mw_one;
extern QRegularExpression regxNumber;
extern QList<float> rlistX, rlistY, rlistZ, glistX, glistY, glistZ;
extern unsigned int num_steps_walk, num_steps_run, num_steps_hop;
extern bool loading;
extern bool isImport;
extern QString iniFile, iniDir;

dlgSteps::dlgSteps(QWidget* parent) : QDialog(parent), ui(new Ui::dlgSteps) {
  ui->setupUi(this);
  this->installEventFilter(this);

  ui->lblSingle->adjustSize();

  QFont font1;
  font1.setPointSize(15);
  ui->lblX->setFont(font1);
  ui->lblY->setFont(font1);
  ui->lblZ->setFont(font1);
  ui->lblSteps->setFont(font1);

  lblStyleNormal = ui->lblX->styleSheet();

  QValidator* validator =
      new QRegularExpressionValidator(regxNumber, ui->editTangentLineIntercept);
  ui->editTangentLineIntercept->setValidator(validator);
  ui->editTangentLineSlope->setValidator(validator);
  ui->editStepLength->setValidator(validator);

  for (int y = 0; y < ui->tableWidget->columnCount(); y++) {
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(
        y, QHeaderView::ResizeToContents);
  }

  ui->tableWidget->setAlternatingRowColors(true);
  ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
  ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
  ui->tableWidget->setVerticalScrollMode(QTableWidget::ScrollPerPixel);
  QScroller::grabGesture(ui->tableWidget, QScroller::LeftMouseButtonGesture);
  ui->tableWidget->verticalScrollBar()->setStyleSheet(mw_one->vsbarStyleSmall);
}

dlgSteps::~dlgSteps() { delete ui; }

void dlgSteps::keyReleaseEvent(QKeyEvent* event) { Q_UNUSED(event) }

bool dlgSteps::eventFilter(QObject* watch, QEvent* evn) {
  if (evn->type() == QEvent::KeyPress) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      on_btnBack_clicked();
      return true;
    }
  }

  return QWidget::eventFilter(watch, evn);
}

void dlgSteps::on_btnBack_clicked() {
  saveSteps();
  close();
}

void dlgSteps::on_btnPause_clicked() {
  if (ui->btnPause->text() == tr("Pause")) {
    ui->btnPause->setText(tr("Start"));
    mw_one->accel_pedometer->stop();

  } else if (ui->btnPause->text() == tr("Start")) {
    ui->btnPause->setText(tr("Pause"));
    mw_one->accel_pedometer->start();
  }
}

void dlgSteps::on_btnReset_clicked() {
  mw_one->accel_pedometer->resetStepCount();
  num_steps_walk = 0;
  num_steps_run = 0;
  num_steps_hop = 0;
  mw_one->CurrentSteps = 0;
  ui->lblSingle->setText("0");
  mw_one->ui->btnMainNotes->setText(tr("Steps"));
  toDayInitSteps = getCurrentSteps();
}

void dlgSteps::saveSteps() {
  QSettings Reg(iniDir + "steps.ini", QSettings::IniFormat);
  Reg.setValue("/Steps/Intercept",
               ui->editTangentLineIntercept->text().trimmed());
  Reg.setValue("/Steps/Slope", ui->editTangentLineSlope->text().trimmed());
  Reg.setValue("/Steps/Length", ui->editStepLength->text().trimmed());
  Reg.setValue("/Steps/Alg1", ui->rbAlg1->isChecked());
  Reg.setValue("/Steps/Alg2", ui->rbAlg2->isChecked());

  int i = 0;
  int count = ui->tableWidget->rowCount();
  if (count >= 30) {
    count = 30;
    i = count - 30;
  }
  Reg.setValue("/Steps/Count", count);
  for (; i < count; i++) {
    Reg.setValue("/Steps/Table-" + QString::number(i) + "-0",
                 ui->tableWidget->item(i, 0)->text());
    Reg.setValue("/Steps/Table-" + QString::number(i) + "-1",
                 ui->tableWidget->item(i, 1)->text());
  }
}

void dlgSteps::init_Steps() {
  QString ini_file;
  if (isImport)
    ini_file = iniFile;
  else
    ini_file = iniDir + "steps.ini";
  QSettings Reg(ini_file, QSettings::IniFormat);

  ui->editTangentLineIntercept->setText(
      Reg.value("/Steps/Intercept", dleInter).toString());
  ui->editTangentLineSlope->setText(
      Reg.value("/Steps/Slope", dleSlope).toString());
  ui->editStepLength->setText(Reg.value("/Steps/Length", "35").toString());
  ui->rbAlg1->setChecked(Reg.value("Steps/Alg1", true).toBool());
  ui->rbAlg2->setChecked(Reg.value("Steps/Alg2", false).toBool());

  int count = Reg.value("/Steps/Count").toInt();
  for (int i = 0; i < count; i++) {
    QString str0 =
        Reg.value("/Steps/Table-" + QString::number(i) + "-0").toString();
    qlonglong str1 =
        Reg.value("/Steps/Table-" + QString::number(i) + "-1").toLongLong();
    addRecord(str0, str1);
  }

  for (int i = 0; i < count; i++) {
    if (QDate::currentDate().toString() ==
        ui->tableWidget->item(i, 0)->text()) {
      toDayInitSteps = ui->tableWidget->item(i, 1)->text().toInt();
      break;
    }
  }
}

void dlgSteps::on_editTangentLineIntercept_textChanged(const QString& arg1) {
  mw_one->accel_pedometer->setTangentLineIntercept(arg1.toFloat());
}

void dlgSteps::on_editTangentLineSlope_textChanged(const QString& arg1) {
  mw_one->accel_pedometer->setTangentLineSlope(arg1.toFloat());
}

void dlgSteps::addRecord(QString date, qlonglong steps) {
  bool isYes = false;
  int count = ui->tableWidget->rowCount();
  for (int i = 0; i < count; i++) {
    QString str = ui->tableWidget->item(i, 0)->text();
    if (str == date) {
      QTableWidgetItem* item = new QTableWidgetItem(date);
      ui->tableWidget->setItem(i, 0, item);

      item = new QTableWidgetItem(QString::number(steps));
      item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
      ui->tableWidget->setItem(i, 1, item);

      ui->tableWidget->item(i, 0)->setFlags(Qt::NoItemFlags);
      ui->tableWidget->item(i, 1)->setFlags(Qt::NoItemFlags);
      isYes = true;
      break;
    }
  }

  if (!isYes) {
    ui->tableWidget->setRowCount(count + 1);
    QTableWidgetItem* item = new QTableWidgetItem(date);
    ui->tableWidget->setItem(count, 0, item);

    item = new QTableWidgetItem(QString::number(steps));
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tableWidget->setItem(count, 1, item);

    ui->tableWidget->item(count, 0)->setFlags(Qt::NoItemFlags);
    ui->tableWidget->item(count, 1)->setFlags(Qt::NoItemFlags);
  }
}

qlonglong dlgSteps::getCurrentSteps() {
  int count = ui->tableWidget->rowCount();
  if (count == 0) return 0;

  return ui->tableWidget->item(count - 1, 1)->text().toLongLong();
}

void dlgSteps::setTableSteps(qlonglong steps) {
  int count = ui->tableWidget->rowCount();
  if (count == 0) {
    addRecord(QDate::currentDate().toString(), 1);
  }
  if (count > 0) {
    QString strDate = ui->tableWidget->item(count - 1, 0)->text();
    if (strDate == QDate::currentDate().toString()) {
      QTableWidgetItem* item = new QTableWidgetItem(QString::number(steps));
      item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
      ui->tableWidget->setItem(count - 1, 1, item);
    } else
      addRecord(QDate::currentDate().toString(), 1);
  }
}

void dlgSteps::on_btnDefaultIntercept_clicked() {
  ui->editTangentLineIntercept->setText(QString::number(dleInter));
}

void dlgSteps::on_btnDefaultSlope_clicked() {
  ui->editTangentLineSlope->setText(QString::number(dleSlope));
}

void dlgSteps::on_rbAlg1_clicked() {
  if (loading) return;
  ui->frameWay1->show();
  ui->lblSteps->setText("");
  rlistX.clear();
  rlistY.clear();
  rlistZ.clear();
  glistX.clear();
  glistY.clear();
  glistZ.clear();

#ifdef Q_OS_ANDROID
  QAndroidJniObject jo = QAndroidJniObject::fromString("Sleep1Win");
  jo.callStaticMethod<int>("com.x/MyService", "setSleep1", "()I");
#endif
}

void dlgSteps::on_rbAlg2_clicked() {
  if (loading) return;
  ui->frameWay1->hide();
  rlistX.clear();
  rlistY.clear();
  rlistZ.clear();
  glistX.clear();
  glistY.clear();
  glistZ.clear();

#ifdef Q_OS_ANDROID
  QAndroidJniObject jo = QAndroidJniObject::fromString("Sleep2Win");
  jo.callStaticMethod<int>("com.x/MyService", "setSleep2", "()I");
#endif
}

void dlgSteps::on_btnLogs_clicked() {
  mw_one->mydlgMainNotes->setFixedHeight(mw_one->height());
  mw_one->mydlgMainNotes->setFixedWidth(mw_one->width());
  mw_one->mydlgMainNotes->ui->textBrowser->show();
  mw_one->mydlgMainNotes->ui->textEdit->hide();
  mw_one->mydlgMainNotes->setModal(true);
  mw_one->mydlgMainNotes->show();
}
