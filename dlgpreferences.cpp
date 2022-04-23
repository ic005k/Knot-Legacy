#include "dlgpreferences.h"

#include <QKeyEvent>

#include "mainwindow.h"
#include "ui_dlgpreferences.h"
#include "ui_mainwindow.h"
extern QString iniFile, iniDir;
extern MainWindow* mw_one;
extern bool isBreak;

dlgPreferences::dlgPreferences(QWidget* parent)
    : QDialog(parent), ui(new Ui::dlgPreferences) {
  ui->setupUi(this);
  this->installEventFilter(this);
  ui->gBoxSensor->hide();
  ui->frameAutoPause->hide();
}

dlgPreferences::~dlgPreferences() { delete ui; }

void dlgPreferences::keyReleaseEvent(QKeyEvent* event) { Q_UNUSED(event); }

bool dlgPreferences::eventFilter(QObject* watch, QEvent* evn) {
  if (evn->type() == QEvent::KeyPress) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      on_btnBack_clicked();
      return true;
    }
  }

  return QWidget::eventFilter(watch, evn);
}

void dlgPreferences::on_btnBack_clicked() {
  saveOptions();
  close();
}

void dlgPreferences::saveOptions() {
  if (isBreak) return;
  QSettings Reg(iniDir + "options.ini", QSettings::IniFormat);
  Reg.setValue("/Options/rb0", ui->rb0->isChecked());
  Reg.setValue("/Options/rb1", ui->rb1->isChecked());
  Reg.setValue("/Options/rb2", ui->rb2->isChecked());
  Reg.setValue("/Options/Close", ui->chkClose->isChecked());
  Reg.setValue("/Options/AutoTimeY", ui->chkAutoTime->isChecked());
  Reg.setValue("/Options/SM1", ui->rbSM1->isChecked());
  Reg.setValue("/Options/SM2", ui->rbSM2->isChecked());
  Reg.setValue("/Options/Debug", ui->chkDebug->isChecked());
}

void dlgPreferences::on_rb0_clicked() { isFontChange = true; }

void dlgPreferences::on_rb1_clicked() { isFontChange = true; }

void dlgPreferences::on_rb2_clicked() { isFontChange = true; }

void dlgPreferences::on_rbSM1_clicked() {
  mw_one->accel_pedometer->setAccelerationMode(QAccelerometer::Combined);
}

void dlgPreferences::on_rbSM2_clicked() {
  mw_one->accel_pedometer->setAccelerationMode(QAccelerometer::User);
}

void dlgPreferences::on_chkDebug_clicked() {
  if (ui->chkDebug->isChecked()) {
    mw_one->mydlgSteps->ui->btnLogs->show();
    mw_one->mydlgSteps->ui->lblX->show();
    mw_one->mydlgSteps->ui->lblY->show();
    mw_one->mydlgSteps->ui->lblZ->show();

    if (mw_one->mydlgSteps->ui->rbAlg1->isChecked())
      mw_one->mydlgSteps->ui->frameWay1->show();
    if (mw_one->mydlgSteps->ui->rbAlg2->isChecked())
      mw_one->mydlgSteps->ui->frameWay1->hide();
  } else if (!ui->chkDebug->isChecked()) {
    mw_one->mydlgSteps->ui->btnLogs->hide();
    mw_one->mydlgSteps->ui->lblX->hide();
    mw_one->mydlgSteps->ui->lblY->hide();
    mw_one->mydlgSteps->ui->lblZ->hide();
    mw_one->mydlgSteps->ui->frameWay1->hide();
  }
}
