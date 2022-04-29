#include "dlgtodo.h"

#include "mainwindow.h"
#include "smoothscrollbar.h"
#include "ui_dlgtodo.h"

QString highLblStyle = "background-color: rgb(255, 239, 219);color:black";
int highCount;
QString orgLblStyle;
QListWidget* mylist;
extern MainWindow* mw_one;
extern QString iniFile, iniDir;
extern bool loading, isBreak, isImport;
extern int fontSize;
dlgTodo::dlgTodo(QWidget* parent) : QDialog(parent), ui(new Ui::dlgTodo) {
  ui->setupUi(this);
  this->installEventFilter(this);

  mylist = new QListWidget;
  mylist = ui->listWidget;

  ui->listWidget->setVerticalScrollMode(QListWidget::ScrollPerPixel);
  QScroller::grabGesture(ui->listWidget, QScroller::LeftMouseButtonGesture);
  ui->listWidget->horizontalScrollBar()->setHidden(true);
}

dlgTodo::~dlgTodo() { delete ui; }

void dlgTodo::keyReleaseEvent(QKeyEvent* event) {
  Q_UNUSED(event);
  // event->accept();
}

void dlgTodo::on_btnBack_clicked() { close(); }

void dlgTodo::saveTodo() {
  highCount = 0;
  QSettings Reg(iniDir + "todo.ini", QSettings::IniFormat);
  int count = mylist->count();
  Reg.setValue("/Todo/Count", count);
  for (int i = 0; i < count; i++) {
    if (isBreak) break;
    QListWidgetItem* item = mylist->item(i);
    QWidget* w = mylist->itemWidget(item);
    QLabel* lbl = (QLabel*)w->children().at(2)->children().at(2);
    QString str = lbl->text();
    QLabel* lblTime = (QLabel*)w->children().at(2)->children().at(1);
    QString strTime = lblTime->text();
    Reg.setValue("/Todo/Item" + QString::number(i), str);
    Reg.setValue("/Todo/Time" + QString::number(i), strTime);

    QLabel* lblSn = (QLabel*)w->children().at(2);
    if (orgLblStyle != lblSn->styleSheet()) highCount++;
  }

  Reg.setValue("/Todo/HighCount", highCount);
}

void dlgTodo::init_Items() {
  ui->listWidget->clear();
  QString ini_file;
  if (isImport)
    ini_file = iniFile;
  else
    ini_file = iniDir + "todo.ini";
  QSettings Reg(ini_file, QSettings::IniFormat);
  int count = Reg.value("/Todo/Count").toInt();
  for (int i = 0; i < count; i++) {
    QString str = Reg.value("/Todo/Item" + QString::number(i)).toString();
    QString strTime = Reg.value("/Todo/Time" + QString::number(i)).toString();
    add_Item(str, strTime, false);
  }

  highCount = Reg.value("/Todo/HighCount").toInt();
  for (int i = 0; i < highCount; i++) {
    QListWidgetItem* item = ui->listWidget->item(i);
    QWidget* w = ui->listWidget->itemWidget(item);
    QLabel* lbl = (QLabel*)w->children().at(2);
    lbl->setStyleSheet(highLblStyle);
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
  add_Item(str,
           QDate::currentDate().toString("ddd MM dd yyyy") + "  " +
               QTime::currentTime().toString(),
           true);
  ui->lineEdit->setText("");
  ui->listWidget->verticalScrollBar()->setSliderPosition(0);
}

void dlgTodo::add_Item(QString str, QString time, bool insert) {
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
  w->setContentsMargins(0, 0, 0, 0);

  QHBoxLayout* layout = new QHBoxLayout;
  layout->setMargin(0);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  QToolButton* btn = new QToolButton(this);
  btn->setStyleSheet("border:none");
  // QPixmap pixmap = QPixmap(":/src/done.png");
  // btn->setIcon(pixmap);
  // btn->setIconSize(QSize(20, 20));
  // QPainter p(btn);
  // p.setRenderHint(QPainter::Antialiasing);

  btn->setIconSize(QSize(25, 25));
  btn->setIcon(QIcon(":/src/done.png"));

  connect(btn, &QToolButton::clicked, [=]() {
    btn->setIcon(QIcon(":/src/done1.png"));

    mw_one->Sleep(1000);

    ui->listWidget->setCurrentItem(pItem);
    int row = ui->listWidget->currentRow();
    ui->listWidget->takeItem(row);
    int index = ui->listWidget->currentRow();
    add_ItemSn(index);
  });

  QFont font;
  font.setPointSize(fontSize);
  QLabel* lblSn = new QLabel(this);
  lblSn->setFont(font);
  lblSn->setFixedWidth(fontSize * 1.5);

  QLabel* label = new QLabel(this);
  label->setFont(font);
  label->installEventFilter(this);
  //让label自适应text大小
  label->adjustSize();
  //设置label换行
  label->setWordWrap(true);
  label->setText(str);
  orgLblStyle = label->styleSheet();

  QTextEdit* edit = new QTextEdit(this);
  QScroller::grabGesture(edit, QScroller::LeftMouseButtonGesture);
  edit->setPlainText(str);
  edit->setHidden(true);
  connect(edit, &QTextEdit::textChanged,
          [=]() { label->setText(edit->toPlainText().trimmed()); });

  QFrame* frame = new QFrame(this);
  QHBoxLayout* hbox = new QHBoxLayout();
  hbox->addWidget(label);
  hbox->addWidget(edit);
  QVBoxLayout* vbox = new QVBoxLayout();
  QLabel* lblTime = new QLabel(this);
  QFont f;
  f.setPointSize(10);
  lblTime->setFont(f);
  lblTime->setText(time);
  vbox->addWidget(lblTime);
  vbox->addLayout(hbox);
  frame->setLayout(vbox);
  frame->layout()->setMargin(0);
  frame->layout()->setContentsMargins(0, 0, 0, 0);

  layout->addWidget(lblSn);
  layout->addWidget(frame);
  layout->addWidget(btn);
  w->setLayout(layout);

  // qDebug() << w->children();
  // qDebug() << w->children().at(2)->children().at(2);  // lblText

  ui->listWidget->setItemWidget(pItem, w);

  add_ItemSn(0);
}

void dlgTodo::add_ItemSn(int index) {
  for (int i = 0; i < ui->listWidget->count(); i++) {
    QListWidgetItem* item = ui->listWidget->item(i);
    QWidget* w = ui->listWidget->itemWidget(item);
    QLabel* lbl = (QLabel*)w->children().at(1);

    lbl->setText(QString::number(i + 1) + ". ");
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

  mw_one->startSave("todo");
}

bool dlgTodo::eventFilter(QObject* watch, QEvent* evn) {
  if (loading) return QWidget::eventFilter(watch, evn);

  if (evn->type() == QEvent::KeyPress) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      on_btnBack_clicked();
      qDebug() << "back";
      return true;
    }
  }

  return QWidget::eventFilter(watch, evn);
}

void dlgTodo::on_btnModify_clicked() {
  int row = ui->listWidget->currentRow();
  if (row < 0) return;

  if (ui->btnModify->text() == tr("Modify")) {
    QListWidgetItem* item = ui->listWidget->currentItem();
    QWidget* w = ui->listWidget->itemWidget(item);
    QLabel* lbl = (QLabel*)w->children().at(2)->children().at(2);
    QTextEdit* edit = (QTextEdit*)w->children().at(2)->children().at(3);
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

void dlgTodo::on_btnHigh_clicked() {
  int row = ui->listWidget->currentRow();
  if (row < 0) return;
  QListWidgetItem* item = ui->listWidget->currentItem();
  QWidget* w = ui->listWidget->itemWidget(item);
  QLabel* lbl = (QLabel*)w->children().at(2)->children().at(2);
  ui->listWidget->takeItem(row);
  QString str = lbl->text();
  QLabel* lblTime = (QLabel*)w->children().at(2)->children().at(1);
  add_Item(str, lblTime->text(), true);
  QListWidgetItem* item1 = ui->listWidget->currentItem();
  QWidget* w1 = ui->listWidget->itemWidget(item1);
  QFrame* frame = (QFrame*)w1->children().at(2);
  frame->setStyleSheet(highLblStyle);
  ui->listWidget->scrollToTop();
}

void dlgTodo::on_btnLow_clicked() {
  int row = ui->listWidget->currentRow();
  if (row < 0) return;
  QListWidgetItem* item = ui->listWidget->currentItem();
  QWidget* w = ui->listWidget->itemWidget(item);
  QLabel* lbl = (QLabel*)w->children().at(2)->children().at(2);
  ui->listWidget->takeItem(row);
  QString str = lbl->text();
  QLabel* lblTime = (QLabel*)w->children().at(2)->children().at(1);
  add_Item(str, lblTime->text(), false);
  ui->listWidget->setCurrentRow(ui->listWidget->count() - 1);
  ui->listWidget->scrollToBottom();
}
