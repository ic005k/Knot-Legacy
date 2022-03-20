#include "dlgtodo.h"

#include "mainwindow.h"
#include "ui_dlgtodo.h"
extern QString iniFile;
extern bool loading;
dlgTodo::dlgTodo(QWidget* parent) : QDialog(parent), ui(new Ui::dlgTodo) {
  ui->setupUi(this);

  QScroller::grabGesture(ui->listWidget, QScroller::TouchGesture);
  ui->listWidget->horizontalScrollBar()->setHidden(true);
}

dlgTodo::~dlgTodo() { delete ui; }

void dlgTodo::keyReleaseEvent(QKeyEvent* event) { event->accept(); }

void dlgTodo::on_btnBack_clicked() { close(); }

void dlgTodo::saveTodo() {
  QSettings Reg(iniFile, QSettings::IniFormat);
  int count = ui->listWidget->count();
  Reg.setValue("/Todo/Count", count);
  for (int i = 0; i < count; i++) {
    QListWidgetItem* item = ui->listWidget->item(i);
    QWidget* w = ui->listWidget->itemWidget(item);
    // (QToolButton(0x92815320), QHBoxLayout(0x92811710), QLabel(0x928156e0))
    QLabel* lbl = (QLabel*)w->children().at(2);
    QString str = lbl->text();
    Reg.setValue("/Todo/Item" + QString::number(i), str);
  }
}

void dlgTodo::init_Items() {
  QSettings Reg(iniFile, QSettings::IniFormat);
  int count = Reg.value("/Todo/Count").toInt();
  for (int i = 0; i < count; i++) {
    QString str = Reg.value("/Todo/Item" + QString::number(i)).toString();
    add_Item(str, false);
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
  add_Item(str, true);
  ui->lineEdit->setText("");
}

void dlgTodo::add_Item(QString str, bool insert) {
  if (str != "") {
    int count = ui->listWidget->count();
    QListWidgetItem* pItem = new QListWidgetItem;
    // pItem->setSizeHint(QSize(this->width() - 15, 45));
    // pItem->setCheckState(Qt::Unchecked);
    pItem->setText("");
    if (insert)
      ui->listWidget->insertItem(0, pItem);
    else
      ui->listWidget->addItem(pItem);

    QWidget* w = new QWidget;
    w->installEventFilter(this);
    QHBoxLayout* layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    QToolButton* pushButton = new QToolButton(w);
    pushButton->setIconSize(QSize(22, 22));
    pushButton->setIcon(QIcon(":/src/done.png"));
    connect(pushButton, &QToolButton::clicked, [=]() {
      ui->listWidget->setCurrentItem(pItem);
      int row = ui->listWidget->currentRow();
      ui->listWidget->takeItem(row);
    });

    QWidget* spacer = new QWidget(this);
    // connect(spacer, &QListWidget::itemDoubleClicked,
    //         [=]() { qDebug() << pItem->text(); });
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    spacer->setMouseTracking(true);
    spacer->installEventFilter(this);

    QLabel* label = new QLabel;
    //让label自适应text大小
    label->adjustSize();
    //设置label换行
    label->setWordWrap(true);
    label->setText(str);
    layout->addWidget(label);
    layout->addWidget(pushButton);
    w->setLayout(layout);

    ui->listWidget->setItemWidget(pItem, w);

    ui->listWidget->setCurrentRow(count);
  }
}

void dlgTodo::on_listWidget_itemClicked(QListWidgetItem* item) {
  Q_UNUSED(item);
  QWidget* w = ui->listWidget->itemWidget(item);
  // (QToolButton(0x92815320), QHBoxLayout(0x92811710), QLabel(0x928156e0))
  QLabel* lbl = (QLabel*)w->children().at(2);
  QString str = lbl->text();
  ui->lineEdit->setText(str);
}

void dlgTodo::on_listWidget_itemDoubleClicked(QListWidgetItem* item) {
  Q_UNUSED(item);
  // ui->listWidget->openPersistentEditor(item);
  // editItem = item;
  qDebug() << "testing";
}

void dlgTodo::on_listWidget_currentRowChanged(int currentRow) {
  if (editItem != NULL) ui->listWidget->closePersistentEditor(editItem);
}

void dlgTodo::closeEvent(QCloseEvent* event) {
  Q_UNUSED(event);
  saveTodo();
}

bool dlgTodo::eventFilter(QObject* watch, QEvent* evn) {
  if (loading) return QWidget::eventFilter(watch, evn);

  // if (watch == spacer) {
  // if (evn->type() == QEvent::MouseButtonDblClick) {
  // return true;
  //}
  // return false;
  //}
  return QWidget::eventFilter(watch, evn);
}

void dlgTodo::on_btnModi_clicked() {
  if (ui->listWidget->count() <= 0) return;
  QListWidgetItem* item = ui->listWidget->currentItem();
  // ui->listWidget->openPersistentEditor(item);
  // editItem = item;
  QWidget* w = ui->listWidget->itemWidget(item);
  // (QToolButton(0x92815320), QHBoxLayout(0x92811710), QLabel(0x928156e0))
  QLabel* lbl = (QLabel*)w->children().at(2);
  lbl->setText(ui->lineEdit->text().trimmed());
  ui->lineEdit->setText("");
}
