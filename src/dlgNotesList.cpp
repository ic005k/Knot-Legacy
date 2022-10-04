#include "dlgNotesList.h"

#include "src/mainwindow.h"
#include "ui_dlgNotesList.h"
#include "ui_mainwindow.h"

extern MainWindow* mw_one;

dlgNotesList::dlgNotesList(QWidget* parent)
    : QDialog(parent), ui(new Ui::dlgNotesList) {
  ui->setupUi(this);
  setModal(true);
  this->layout()->setSpacing(5);

  QScrollBar* SB = ui->treeWidget->verticalScrollBar();
  SB->setStyleSheet(mw_one->vsbarStyleSmall);
  ui->treeWidget->setStyleSheet(mw_one->treeStyle);
  ui->treeWidget->setVerticalScrollMode(QTreeWidget::ScrollPerPixel);
  QScroller::grabGesture(ui->treeWidget, QScroller::LeftMouseButtonGesture);
  mw_one->setSCrollPro(ui->treeWidget);

  ui->treeWidget->headerItem()->setText(0, tr("Notebook"));
  if (ui->treeWidget->topLevelItemCount() == 0) {
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0, tr("Default"));

    QTreeWidgetItem* item1 = new QTreeWidgetItem(item);
    item1->setText(0, tr("My Notes"));

    ui->treeWidget->addTopLevelItem(item);
  }

  ui->treeWidget->setFocus();
  ui->treeWidget->expandAll();
}

dlgNotesList::~dlgNotesList() { delete ui; }

bool dlgNotesList::eventFilter(QObject* watch, QEvent* evn) {
  if (evn->type() == QEvent::KeyPress) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      close();
      return true;
    }
  }

  return QWidget::eventFilter(watch, evn);
}

void dlgNotesList::on_btnClose_clicked() { close(); }
