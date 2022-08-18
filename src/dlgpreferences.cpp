#include "dlgpreferences.h"

#include <QKeyEvent>

#include "mainwindow.h"
#include "ui_dlgpreferences.h"
#include "ui_mainwindow.h"
extern QString iniFile, iniDir, hsStyle;
extern MainWindow* mw_one;
extern bool isBreak;
extern int fontSize;

dlgPreferences::dlgPreferences(QWidget* parent)
    : QDialog(parent), ui(new Ui::dlgPreferences) {
  ui->setupUi(this);
  this->installEventFilter(this);
  ui->gBoxSensor->hide();
  ui->chkEM->hide();
  ui->sliderFontSize->setValue(fontSize);
  ui->lblFontDemo->setText(tr("Font Size") + " : " + QString::number(fontSize));
  isFontChange = false;
  ui->chkMute->setStyleSheet(ui->chkClose->styleSheet());
  hsStyle = ui->sliderFontSize->styleSheet();
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
  Reg.setValue("/Options/FontSize", ui->sliderFontSize->value());
  Reg.setValue("/Options/ReaderFont", ui->chkReaderFont->isChecked());
  Reg.setValue("/Options/Close", ui->chkClose->isChecked());
  Reg.setValue("/Options/AutoTimeY", ui->chkAutoTime->isChecked());
  Reg.setValue("/Options/SM1", ui->rbSM1->isChecked());
  Reg.setValue("/Options/SM2", ui->rbSM2->isChecked());
  Reg.setValue("/Options/Debug", ui->chkDebug->isChecked());
  Reg.setValue("/Options/Mute", ui->chkMute->isChecked());
}

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

void dlgPreferences::on_sliderFontSize_sliderMoved(int position) {
  QFont font;
  font.setPointSize(position);
  ui->lblFontDemo->setText(tr("Font Size") + " : " + QString::number(position));
  ui->lblFontDemo->setFont(font);
  isFontChange = true;
}

void dlgPreferences::on_chkReaderFont_clicked() { isFontChange = true; }

void dlgPreferences::on_chkMute_clicked() { mw_one->mydlgTodo->refreshAlarm(); }
