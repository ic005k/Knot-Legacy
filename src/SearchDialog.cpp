#include "SearchDialog.h"

#include "mainwindow.h"
#include "ui_SearchDialog.h"

extern MainWindow* mw_one;

SearchDialog::SearchDialog(QWidget* parent)
    : QDialog(parent), ui(new Ui::SearchDialog) {
  ui->setupUi(this);
  this->installEventFilter(this);
  ui->btnBack->setStyleSheet(mw_one->btnStyle);
  ui->btnSearch->setStyleSheet(mw_one->btnStyle);
  mw_one->setLineEditQss(ui->editSearchText, 10, 1, "#4169E1", "#4169E1");

  ui->tableSearch->setColumnCount(4);
  ui->tableSearch->setHorizontalHeaderItem(0, new QTableWidgetItem(tr("Time")));
  ui->tableSearch->setHorizontalHeaderItem(1,
                                           new QTableWidgetItem(tr("Amount")));
  ui->tableSearch->setHorizontalHeaderItem(
      2, new QTableWidgetItem(tr("Category")));
  ui->tableSearch->setHorizontalHeaderItem(3,
                                           new QTableWidgetItem(tr("Details")));
}

SearchDialog::~SearchDialog() { delete ui; }

bool SearchDialog::eventFilter(QObject* watch, QEvent* evn) {
  if (evn->type() == QEvent::KeyPress) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
    }
  }

  return QWidget::eventFilter(watch, evn);
}

void SearchDialog::on_btnBack_clicked() { close(); }
