#include "dlgsettime.h"

#include <QKeyEvent>

#include "mainwindow.h"
#include "ui_dlgsettime.h"
extern MainWindow* mw_one;

dlgSetTime::dlgSetTime(QWidget* parent)
    : QDialog(parent), ui(new Ui::dlgSetTime) {
  ui->setupUi(this);
}

dlgSetTime::~dlgSetTime() { delete ui; }

void dlgSetTime::on_btnBack_clicked() { close(); }

void dlgSetTime::keyReleaseEvent(QKeyEvent* event) { event->accept(); }

void dlgSetTime::on_btnOk_clicked() {
  mw_one->set_Time();
  close();
}
