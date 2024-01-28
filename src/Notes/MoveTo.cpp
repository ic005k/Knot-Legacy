#include "MoveTo.h"

#include "src/MainWindow.h"
#include "src/Method.h"
#include "ui_MoveTo.h"

extern MainWindow* mw_one;
extern Method* m_Method;

MoveTo::MoveTo(QWidget* parent) : QDialog(parent), ui(new Ui::MoveTo) {
  ui->setupUi(this);
  setModal(true);

  setWindowFlag(Qt::FramelessWindowHint);
  QString style = "QDialog{border-radius:0px;border:0px solid darkred;}";
  this->setStyleSheet(style);
  ui->lblItem->adjustSize();
  ui->lblItem->setWordWrap(true);

  mw_one->set_ToolButtonStyle(this);

  QTreeWidgetItem* item = NULL;
  if (!mw_one->m_NotesList->ui->frame0->isHidden())
    item = mw_one->m_NotesList->tw->currentItem();
  if (!mw_one->m_NotesList->ui->frame1->isHidden())
    item = mw_one->m_NotesList->twrb->currentItem();
  if (item == NULL) close();
  ui->lblItem->setText(item->text(0));
  if (item->text(1).isEmpty())
    initTopNoteBook();
  else
    initAllNoteBook();

  showDialog();
}

MoveTo::~MoveTo() { delete ui; }

void MoveTo::showDialog() {
  int x, y, w, h;
  w = mw_one->width() - 20;
  h = this->height();
  x = mw_one->geometry().x() + (mw_one->width() - w) / 2;
  y = 150;
  setGeometry(x, y, w, h);

  m_widget = new QWidget(mw_one->m_NotesList);
  m_widget->resize(mw_one->width(), mw_one->height());
  m_widget->move(0, 0);
  m_widget->setStyleSheet("background-color:rgba(0, 0, 0,35%);");
  m_widget->show();

  show();
  while (!isHidden()) QCoreApplication::processEvents();
}

void MoveTo::closeEvent(QCloseEvent* event) {
  Q_UNUSED(event)
  m_widget->close();
}

void MoveTo::on_btnCancel_clicked() {
  isOk = false;
  strCurrentItem = "";
  currentItem = NULL;
  close();
}

void MoveTo::on_btnOk_clicked() {
  isOk = true;
  strCurrentItem = ui->cboxMoveTo->currentText();
  currentItem = listItems.at(ui->cboxMoveTo->currentIndex() - 1);
  close();
}

void MoveTo::initTopNoteBook() {
  ui->cboxMoveTo->clear();
  listItems.clear();
  QStringList itemList;
  itemList.append(tr("Main Root"));
  int count = mw_one->m_NotesList->tw->topLevelItemCount();
  for (int i = 0; i < count; i++) {
    QTreeWidgetItem* topItem = mw_one->m_NotesList->tw->topLevelItem(i);
    QString strTop = topItem->text(0);
    itemList.append(strTop);
    listItems.append(topItem);
  }

  ui->cboxMoveTo->addItems(itemList);
}

void MoveTo::initAllNoteBook() {
  ui->cboxMoveTo->clear();
  listItems.clear();
  QStringList itemList;
  itemList.append(tr("Main Root"));
  int count = mw_one->m_NotesList->tw->topLevelItemCount();
  for (int i = 0; i < count; i++) {
    QTreeWidgetItem* topItem = mw_one->m_NotesList->tw->topLevelItem(i);
    QString strTop = topItem->text(0);
    itemList.append(strTop);
    listItems.append(topItem);
    int childCount = topItem->childCount();
    for (int j = 0; j < childCount; j++) {
      QTreeWidgetItem* childItem = topItem->child(j);
      if (childItem->text(1).isEmpty()) {
        itemList.append(childItem->text(0));
        listItems.append(childItem);
      }
    }
  }

  ui->cboxMoveTo->addItems(itemList);
}
