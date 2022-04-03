#include "dlgsteps.h"

#include "mainwindow.h"
#include "ui_dlgsteps.h"
#include "ui_mainwindow.h"

extern MainWindow* mw_one;
extern QRegularExpression regxNumber;
extern QString iniDir;

dlgSteps::dlgSteps(QWidget* parent) : QDialog(parent), ui(new Ui::dlgSteps) {
  ui->setupUi(this);

  QValidator* validator =
      new QRegularExpressionValidator(regxNumber, ui->editTangentLineIntercept);
  ui->editTangentLineIntercept->setValidator(validator);
  ui->editTangentLineSlope->setValidator(validator);
}

dlgSteps::~dlgSteps() { delete ui; }

void dlgSteps::keyReleaseEvent(QKeyEvent* event) { event->accept(); }

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
}

void dlgSteps::saveSteps() {
  QSettings Reg(iniDir + "steps.ini", QSettings::IniFormat);
  Reg.setValue("/Steps/Intercept", ui->editTangentLineIntercept->text());
  Reg.setValue("/Steps/Slope", ui->editTangentLineSlope->text());
  Reg.setValue("/Steps/Text", ui->textEdit->toPlainText());
}

void dlgSteps::init_Steps() {
  QSettings Reg(iniDir + "steps.ini", QSettings::IniFormat);
  ui->editTangentLineIntercept->setText(
      Reg.value("/Steps/Intercept", 10).toString());
  ui->editTangentLineSlope->setText(Reg.value("/Steps/Slope", 25).toString());
  ui->textEdit->setPlainText(Reg.value("/Steps/Text").toString());
}
