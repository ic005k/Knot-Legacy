#include "dlgpreferences.h"

#include <QKeyEvent>

#include "mainwindow.h"
#include "ui_dlgpreferences.h"
#include "ui_mainwindow.h"
extern QString iniFile;
extern MainWindow* mw_one;
static bool is_rb0;
static bool is_rb1;
static bool is_rb2;
extern bool isBreak;

dlgPreferences::dlgPreferences(QWidget* parent)
    : QDialog(parent), ui(new Ui::dlgPreferences) {
  ui->setupUi(this);
}

dlgPreferences::~dlgPreferences() { delete ui; }

void dlgPreferences::keyReleaseEvent(QKeyEvent* event) { event->accept(); }

void dlgPreferences::on_btnBack_clicked() {
  mw_one->startSave("font");
  close();
}

void dlgPreferences::saveFontSize() {
  if (isBreak) return;
  QSettings Reg(iniFile, QSettings::IniFormat);
  Reg.setValue("/FontSize/rb0", is_rb0);
  Reg.setValue("/FontSize/rb1", is_rb1);
  Reg.setValue("/FontSize/rb2", is_rb2);
}

void dlgPreferences::on_rb0_clicked(bool checked) { is_rb0 = checked; }

void dlgPreferences::on_rb1_clicked(bool checked) { is_rb1 = checked; }

void dlgPreferences::on_rb2_clicked(bool checked) { is_rb2 = checked; }
