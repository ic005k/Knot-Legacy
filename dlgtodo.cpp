#include "dlgtodo.h"

#include "mainwindow.h"
#include "ui_dlgtodo.h"
extern QString iniFile;

dlgTodo::dlgTodo(QWidget* parent) : QDialog(parent), ui(new Ui::dlgTodo) {
  ui->setupUi(this);
  QScroller::grabGesture(ui->listWidget, QScroller::TouchGesture);
  ui->listWidget->horizontalScrollBar()->setHidden(true);
}

dlgTodo::~dlgTodo() { delete ui; }

void dlgTodo::keyReleaseEvent(QKeyEvent* event) { event->accept(); }

void dlgTodo::on_btnBack_clicked() {
  saveTodo();
  close();
}

void dlgTodo::saveTodo() {
  QSettings Reg(iniFile, QSettings::IniFormat);
  int count = ui->listWidget->count();
  Reg.setValue("/Todo/Count", count);
  for (int i = 0; i < count; i++) {
    QString str = ui->listWidget->item(i)->text();
    Reg.setValue("/Todo/Item" + QString::number(i), str);
  }
}

void dlgTodo::init_Items() {
  QSettings Reg(iniFile, QSettings::IniFormat);
  int count = Reg.value("/Todo/Count").toInt();
  for (int i = 0; i < count; i++) {
    QString str = Reg.value("/Todo/Item" + QString::number(i)).toString();
    add_Item(str);
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
  add_Item(str);
}

void dlgTodo::add_Item(QString str) {
  if (str != "") {
    int count = ui->listWidget->count();
    QListWidgetItem* pItem = new QListWidgetItem;
    // pItem->setSizeHint(QSize(this->width() - 15, 45));
    pItem->setText(str);
    // pItem->setCheckState(Qt::Unchecked);
    ui->listWidget->addItem(pItem);

    QWidget* w = new QWidget;

    QHBoxLayout* layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    QToolButton* pushButton = new QToolButton(w);
    // pushButton->setFixedWidth(50);
    pushButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    pushButton->setText(tr("Done"));
    connect(pushButton, &QToolButton::clicked, [=]() {
      ui->listWidget->setCurrentItem(pItem);
      int row = ui->listWidget->currentRow();
      ui->listWidget->takeItem(row);
    });
    // QCheckBox* checkBox = new QCheckBox(w);
    //  layout->addWidget(checkBox);
    QWidget* spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(spacer);
    layout->addWidget(pushButton);
    w->setLayout(layout);

    ui->listWidget->setItemWidget(pItem, w);

    ui->listWidget->setCurrentRow(count);
  }
}

void dlgTodo::on_listWidget_itemClicked(QListWidgetItem* item) {
  Q_UNUSED(item);
}

void dlgTodo::on_listWidget_itemDoubleClicked(QListWidgetItem* item) {
  Q_UNUSED(item);
  // ui->listWidget->openPersistentEditor(item);
  // editItem = item;
}

void dlgTodo::on_listWidget_currentRowChanged(int currentRow) {
  if (editItem != NULL) ui->listWidget->closePersistentEditor(editItem);
}
