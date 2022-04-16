#include "dlglist.h"

#include "mainwindow.h"
#include "ui_dlglist.h"
#include "ui_mainwindow.h"
extern MainWindow* mw_one;
extern int fontSize, red;
dlgList::dlgList(QWidget* parent) : QDialog(parent), ui(new Ui::dlgList) {
  ui->setupUi(this);
  this->installEventFilter(this);
  ui->listWidget->verticalScrollBar()->setStyleSheet(mw_one->vsbarStyleSmall);
  // ui->listWidget->setVerticalScrollMode(QListWidget::ScrollPerPixel);
  // QScroller::grabGesture(ui->listWidget, QScroller::LeftMouseButtonGesture);
  ui->listWidget->horizontalScrollBar()->setHidden(true);
}

dlgList::~dlgList() { delete ui; }

void dlgList::keyReleaseEvent(QKeyEvent* event) { Q_UNUSED(event) }

bool dlgList::eventFilter(QObject* watch, QEvent* evn) {
  if (evn->type() == QEvent::KeyPress) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      on_btnBack_clicked();
      return true;
    }
  }

  return QWidget::eventFilter(watch, evn);
}

void dlgList::on_listWidget_itemClicked(QListWidgetItem* item) {
  Q_UNUSED(item);
  int row = ui->listWidget->currentRow();
  if (row >= 0) {
    mw_one->mydlgSetTime->ui->editDesc->setText(
        ui->listWidget->item(row)->text());
  }
  close();
}

void dlgList::on_btnClear_clicked() {
  // ui->listWidget->clear();
  int row = ui->listWidget->currentRow();
  if (row >= 0) ui->listWidget->takeItem(row);
  mw_one->mydlgSetTime->saveCustomDesc();
}

void dlgList::on_btnBack_clicked() { close(); }
