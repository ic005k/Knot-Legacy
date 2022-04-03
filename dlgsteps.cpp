#include "dlgsteps.h"

#include "mainwindow.h"
#include "ui_dlgsteps.h"
#include "ui_mainwindow.h"

extern MainWindow* mw_one;

dlgSteps::dlgSteps(QWidget* parent) : QDialog(parent), ui(new Ui::dlgSteps) {
  ui->setupUi(this);
  QFont font;
  font.setPointSize(25);
  ui->lblSteps->setFont(font);
  ui->lblSteps->adjustSize();
}

dlgSteps::~dlgSteps() { delete ui; }

void dlgSteps::keyReleaseEvent(QKeyEvent* event) { event->accept(); }

void dlgSteps::on_btnBack_clicked() { close(); }

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
  ui->lblSteps->setText(tr("Steps") + " : 0");
}
