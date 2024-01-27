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

  mw_one->set_ToolButtonStyle(this);

  QTreeWidgetItem* item = mw_one->m_NotesList->tw->currentItem();
  if (item == NULL) close();
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

  m_Method->showGrayWindows();
  show();
  while (!isHidden()) QCoreApplication::processEvents();
}

void MoveTo::closeEvent(QCloseEvent* event) {
  Q_UNUSED(event)
  m_Method->closeGrayWindows();
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
