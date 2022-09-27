#include "dlgrename.h"

#include "mainwindow.h"
#include "ui_dlgrename.h"
#include "ui_mainwindow.h"

extern MainWindow* mw_one;
extern QTabWidget* tabData;

dlgRename::dlgRename(QWidget* parent) : QDialog(parent), ui(new Ui::dlgRename) {
  ui->setupUi(this);
  setFixedHeight(mw_one->height() / 3);
}

dlgRename::~dlgRename() { delete ui; }

void dlgRename::keyReleaseEvent(QKeyEvent* event) { event->accept(); }

void dlgRename::on_btnCancel_clicked() {
  close();
  mw_one->closeGrayWindows();
}

void dlgRename::on_btnOk_clicked() {
  if (ui->editName->text().trimmed() != "") {
    mw_one->addUndo(tr("Rename Tab") + " ( " + mw_one->getTabText() + " ) ");

    int index = mw_one->ui->tabWidget->currentIndex();
    mw_one->ui->tabWidget->setTabText(index, ui->editName->text().trimmed());
    mw_one->saveTab();
    mw_one->addRedo();
    close();
    mw_one->closeGrayWindows();
  }
}
