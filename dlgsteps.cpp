#include "dlgsteps.h"

#include "mainwindow.h"
#include "ui_dlgsteps.h"
#include "ui_mainwindow.h"

extern MainWindow* mw_one;
extern QRegularExpression regxNumber;
extern QString iniDir;

dlgSteps::dlgSteps(QWidget* parent) : QDialog(parent), ui(new Ui::dlgSteps) {
  ui->setupUi(this);
  this->installEventFilter(this);

  lblStyleNormal = ui->lblX->styleSheet();
  QValidator* validator =
      new QRegularExpressionValidator(regxNumber, ui->editTangentLineIntercept);
  ui->editTangentLineIntercept->setValidator(validator);
  ui->editTangentLineSlope->setValidator(validator);

  ui->textEdit->verticalScrollBar()->setStyleSheet(mw_one->vsbarStyleSmall);
  QScroller::grabGesture(ui->textEdit, QScroller::LeftMouseButtonGesture);
  ui->textEdit->setHidden(true);

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
  mw_one->CurrentSteps = 0;
  ui->lcdNumber->display("0");
  mw_one->ui->btnMainNotes->setText(tr("Steps"));
}

void dlgSteps::saveSteps() {
  QSettings Reg(iniDir + "steps.ini", QSettings::IniFormat);
  Reg.setValue("/Steps/Intercept", ui->editTangentLineIntercept->text());
  Reg.setValue("/Steps/Slope", ui->editTangentLineSlope->text());
  Reg.setValue("/Steps/Text", ui->textEdit->toPlainText());

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
  QSettings Reg(iniDir + "steps.ini", QSettings::IniFormat);
  ui->editTangentLineIntercept->setText(
      Reg.value("/Steps/Intercept", 9.5).toString());
  ui->editTangentLineSlope->setText(Reg.value("/Steps/Slope", 9.5).toString());
  ui->textEdit->setPlainText(Reg.value("/Steps/Text").toString());

  int count = Reg.value("/Steps/Count").toInt();
  for (int i = 0; i < count; i++) {
    QString str0 =
        Reg.value("/Steps/Table-" + QString::number(i) + "-0").toString();
    qlonglong str1 =
        Reg.value("/Steps/Table-" + QString::number(i) + "-1").toLongLong();
    addRecord(str0, str1);
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
