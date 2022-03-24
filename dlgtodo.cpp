#include "dlgtodo.h"

#include "mainwindow.h"
#include "ui_dlgtodo.h"
extern QString iniFile;
extern bool loading;
extern int fontSize;
dlgTodo::dlgTodo(QWidget* parent) : QDialog(parent), ui(new Ui::dlgTodo) {
  ui->setupUi(this);

  ui->listWidget->setVerticalScrollMode(QListWidget::ScrollPerPixel);
  QScroller::grabGesture(ui->listWidget, QScroller::LeftMouseButtonGesture);
  ui->listWidget->horizontalScrollBar()->setHidden(true);
  ui->btnModi->setHidden(true);
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
    QLabel* lbl = (QLabel*)w->children().at(1);
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
    QListWidgetItem* item = ui->listWidget->item(i);
    QWidget* w = ui->listWidget->itemWidget(item);
    // (QHBoxLayout(0x915ca030), QLabel(0x90885060), QToolButton(0x8fdf4200))
    QLabel* lbl = (QLabel*)w->children().at(1);
    if (lbl->text() == str) {
      ui->listWidget->setCurrentRow(i);
      return;
    }
  }
  add_Item(str, true);
  ui->lineEdit->setText("");
  ui->listWidget->verticalScrollBar()->setSliderPosition(0);
}

void dlgTodo::add_Item(QString str, bool insert) {
  if (str == "") return;
  QListWidgetItem* pItem = new QListWidgetItem;
  // pItem->setSizeHint(QSize(this->width() - 15, 45));
  // pItem->setCheckState(Qt::Unchecked);
  pItem->setText("");
  if (insert)
    ui->listWidget->insertItem(0, pItem);
  else
    ui->listWidget->addItem(pItem);

  QWidget* w = new QWidget;
  QHBoxLayout* layout = new QHBoxLayout;
  layout->setMargin(0);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  QToolButton* btn = new QToolButton(this);
  btn->setStyleSheet("border:none");
  btn->setIconSize(QSize(30, 30));
  btn->setIcon(QIcon(":/src/done.png"));
  connect(btn, &QToolButton::clicked, [=]() {
    ui->listWidget->setCurrentItem(pItem);
    int row = ui->listWidget->currentRow();
    ui->listWidget->takeItem(row);
    int index = ui->listWidget->currentRow();
    add_ItemSn(index);
  });

  QLabel* label = new QLabel(this);
  QFont font;
  font.setPixelSize(fontSize);
  label->setFont(font);
  label->installEventFilter(this);
  //让label自适应text大小
  label->adjustSize();
  //设置label换行
  label->setWordWrap(true);
  label->setText(str);

  QTextEdit* edit = new QTextEdit(this);
  QScroller::grabGesture(edit, QScroller::TouchGesture);
  edit->setPlainText(str);
  edit->setHidden(true);
  connect(edit, &QTextEdit::textChanged,
          [=]() { label->setText(edit->toPlainText().trimmed()); });

  layout->addWidget(label);
  layout->addWidget(edit);
  layout->addWidget(btn);
  w->setLayout(layout);

  ui->listWidget->setItemWidget(pItem, w);

  add_ItemSn(0);
}

void dlgTodo::add_ItemSn(int index) {
  for (int i = 0; i < ui->listWidget->count(); i++) {
    QListWidgetItem* item = ui->listWidget->item(i);
    QWidget* w = ui->listWidget->itemWidget(item);
    QLabel* lbl = (QLabel*)w->children().at(1);
    QString txt = lbl->text();
    QStringList list0 = txt.split(".");
    if (list0.count() == 2) {
      txt = list0.at(1);
    }
    lbl->setText(QString::number(i + 1) + ". " + txt.trimmed());
  }
  ui->listWidget->setCurrentRow(index);
}

void dlgTodo::on_listWidget_itemClicked(QListWidgetItem* item) {
  QWidget* w = ui->listWidget->itemWidget(item);
  qDebug() << w->children();

  if (isModi) {
    lblModi->setHidden(false);
    editModi->setHidden(true);
    ui->btnModify->setText(tr("Modify"));
    isModi = false;
  }
}

void dlgTodo::on_listWidget_itemDoubleClicked(QListWidgetItem* item) {
  Q_UNUSED(item);
  on_btnModify_clicked();
}

void dlgTodo::on_listWidget_currentRowChanged(int currentRow) {
  Q_UNUSED(currentRow);
  // if (editItem != NULL) ui->listWidget->closePersistentEditor(editItem);
}

void dlgTodo::closeEvent(QCloseEvent* event) {
  Q_UNUSED(event);
  saveTodo();
}

bool dlgTodo::eventFilter(QObject* watch, QEvent* evn) {
  if (loading) return QWidget::eventFilter(watch, evn);

  // if (watch == spacer) {
  if (evn->type() == QEvent::MouseButtonDblClick) {
    qDebug() << "mouse dbclick";
    // return true;
  }
  // return false;
  //}
  return QWidget::eventFilter(watch, evn);
}

void dlgTodo::on_btnModi_clicked() {
  if (ui->listWidget->count() <= 0 || ui->lineEdit->text().trimmed() == "")
    return;
  QListWidgetItem* item = ui->listWidget->currentItem();
  // ui->listWidget->openPersistentEditor(item);
  // editItem = item;
  QWidget* w = ui->listWidget->itemWidget(item);
  // (QHBoxLayout(0x915ca030), QLabel(0x90885060), QToolButton(0x8fdf4200))
  QLabel* lbl = (QLabel*)w->children().at(1);
  lbl->setText(ui->lineEdit->text().trimmed());
  ui->lineEdit->setText("");
}

void dlgTodo::on_btnModify_clicked() {
  if (ui->btnModify->text() == tr("Modify")) {
    QListWidgetItem* item = ui->listWidget->currentItem();
    QWidget* w = ui->listWidget->itemWidget(item);
    QLabel* lbl = (QLabel*)w->children().at(1);
    QTextEdit* edit = (QTextEdit*)w->children().at(2);
    edit->setPlainText(lbl->text());
    lbl->setHidden(true);
    edit->setHidden(false);
    lblModi = lbl;
    editModi = edit;
    ui->btnModify->setText(tr("Finish Editing"));
    isModi = true;
  } else if (ui->btnModify->text() == tr("Finish Editing")) {
    lblModi->setHidden(false);
    editModi->setHidden(true);
    ui->btnModify->setText(tr("Modify"));
    isModi = false;
  }
}
