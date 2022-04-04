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
}

dlgPreferences::~dlgPreferences() { delete ui; }

void dlgPreferences::keyReleaseEvent(QKeyEvent* event) { event->accept(); }

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
}
