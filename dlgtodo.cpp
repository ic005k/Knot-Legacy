#include "dlgtodo.h"

#include "mainwindow.h"
#include "ui_dlgtodo.h"
extern QString iniFile;

dlgTodo::dlgTodo(QWidget* parent) : QDialog(parent), ui(new Ui::dlgTodo) {
  ui->setupUi(this);
}

dlgTodo::~dlgTodo() { delete ui; }

void dlgTodo::keyReleaseEvent(QKeyEvent* event) { event->accept(); }

void dlgTodo::on_btnBack_clicked() {
  QSettings Reg(iniFile, QSettings::IniFormat);
  int count = ui->listWidget->count();
  Reg.setValue("/Todo/Count", count);
  for (int i = 0; i < count; i++) {
    QString str = ui->listWidget->item(i)->text();
    bool chk = ui->listWidget->item(i)->checkState();
    Reg.setValue("/Todo/Item" + QString::number(i), str);
    Reg.setValue("/Todo/ItemCheckState" + QString::number(i), chk);
  }

  close();
}

void dlgTodo::init_Items() {
  QSettings Reg(iniFile, QSettings::IniFormat);
  int count = Reg.value("/Todo/Count").toInt();
  for (int i = 0; i < count; i++) {
    QString str = Reg.value("/Todo/Item" + QString::number(i)).toString();
    bool chk = Reg.value("/Todo/ItemCheckState" + QString::number(i)).toBool();
    QListWidgetItem* pItem = new QListWidgetItem;
    pItem->setSizeHint(QSize(ui->listWidget->width(), 45));
    pItem->setText(str);
    if (!chk)
      pItem->setCheckState(Qt::Unchecked);
    else
      pItem->setCheckState(Qt::Checked);
    ui->listWidget->addItem(pItem);
  }
}

void dlgTodo::on_btnAdd_clicked() {
  QString str = ui->lineEdit->text().trimmed();
  for (int i = 0; i < ui->listWidget->count(); i++) {
    if (ui->listWidget->item(i)->text() == str) {
      ui->listWidget->setCurrentRow(i);
      return;
    }
  }

  if (str != "") {
    int count = ui->listWidget->count();
    QListWidgetItem* pItem = new QListWidgetItem;
    pItem->setSizeHint(QSize(this->width(), 45));
    pItem->setText(str);
    pItem->setCheckState(Qt::Unchecked);
    ui->listWidget->addItem(pItem);
    ui->listWidget->setCurrentRow(count);
  }
}

void dlgTodo::on_listWidget_itemClicked(QListWidgetItem* item) {
  if (item->checkState() == Qt::Checked) {
    ui->listWidget->setCurrentItem(item);
    int row = ui->listWidget->currentRow();
    ui->listWidget->takeItem(row);
    qDebug() << item->checkState();
  }
}
