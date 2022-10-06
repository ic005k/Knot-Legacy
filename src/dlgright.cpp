#include "dlgright.h"

#include "src/mainwindow.h"
#include "ui_dlgright.h"
#include "ui_mainwindow.h"

extern MainWindow* mw_one;

dlgRight::dlgRight(QWidget* parent) : QDialog(parent), ui(new Ui::dlgRight) {
  ui->setupUi(this);
}

dlgRight::~dlgRight() { delete ui; }
