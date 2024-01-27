#include "MoveTo.h"

#include "src/MainWindow.h"
#include "src/Method.h"
#include "ui_MoveTo.h"

extern MainWindow* mw_one;
extern Method* m_Method;

MoveTo::MoveTo(QWidget* parent) : QDialog(parent), ui(new Ui::MoveTo) {
  ui->setupUi(this);

  setWindowFlag(Qt::FramelessWindowHint);
  QString style = "QDialog{border-radius:0px;border:0px solid darkred;}";
  this->setStyleSheet(style);

  mw_one->set_ToolButtonStyle(this);

  ui->cboxMoveTo->clear();
  QStringList itemList;
  int count = mw_one->m_NotesList->tw->topLevelItemCount();
  for (int i = 0; i < count; i++) {
    QTreeWidgetItem* topItem = mw_one->m_NotesList->tw->topLevelItem(i);
    QString strTop = topItem->text(0);
    itemList.append(strTop);
    int childCount = topItem->childCount();
    for (int j = 0; j < childCount; j++) {
      QTreeWidgetItem* childItem = topItem->child(j);
      if (childItem->text(1).isEmpty()) itemList.append(childItem->text(0));
    }
  }

  ui->cboxMoveTo->addItems(itemList);

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

MoveTo::~MoveTo() { delete ui; }

void MoveTo::closeEvent(QCloseEvent* event) {
  Q_UNUSED(event)
  m_Method->closeGrayWindows();
}

QString MoveTo::getCurrentItem() {
  if (isOk)
    return ui->cboxMoveTo->currentText();
  else
    return "";
}

void MoveTo::on_btnCancel_clicked() {
  isOk = false;
  strCurrentItem = "";
  close();
}

void MoveTo::on_btnOk_clicked() {
  isOk = true;
  strCurrentItem = ui->cboxMoveTo->currentText();
  close();
}
