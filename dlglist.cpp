#include "dlglist.h"

#include "mainwindow.h"
#include "ui_dlglist.h"
#include "ui_mainwindow.h"
extern MainWindow* mw_one;
dlgList::dlgList(QWidget* parent) : QDialog(parent), ui(new Ui::dlgList) {
  ui->setupUi(this);
}

dlgList::~dlgList() { delete ui; }
void dlgList::keyReleaseEvent(QKeyEvent* event) { event->accept(); }

void dlgList::on_listWidget_itemClicked(QListWidgetItem* item) {
  mw_one->mydlgSetTime->ui->editDesc->setText(item->text());
  close();
}

void dlgList::on_btnClear_clicked() {
  ui->listWidget->clear();
  close();
}

void dlgList::on_btnBack_clicked() { close(); }
