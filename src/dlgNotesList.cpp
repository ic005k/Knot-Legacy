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
  ui->treeWidget->setColumnHidden(1, true);
  if (ui->treeWidget->topLevelItemCount() == 0) {
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0, tr("Default"));
    QTreeWidgetItem* item1 = new QTreeWidgetItem(item);
    item1->setText(0, tr("My Notes"));
    ui->treeWidget->addTopLevelItem(item);
    ui->treeWidget->setCurrentItem(item->child(item->childCount() - 1));
  }

  ui->treeWidget->setFocus();
  ui->treeWidget->expandAll();
}

dlgNotesList::~dlgNotesList() { delete ui; }

bool dlgNotesList::eventFilter(QObject* watch, QEvent* evn) {
  if (evn->type() == QEvent::KeyPress) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      on_btnClose_clicked();
      return true;
    }
  }

  return QWidget::eventFilter(watch, evn);
}

void dlgNotesList::on_btnClose_clicked() { close(); }

void dlgNotesList::on_btnNewNoteBook_clicked() {
  QTreeWidgetItem* item = new QTreeWidgetItem();
  item->setText(0, ui->editBook->text().trimmed());
  QTreeWidgetItem* item1 = new QTreeWidgetItem(item);
  item1->setText(0, ui->editNote->text().trimmed());
  ui->treeWidget->addTopLevelItem(item);

  ui->treeWidget->setCurrentItem(item->child(item->childCount() - 1));
}

void dlgNotesList::on_btnNewNote_clicked() {
  QTreeWidgetItem* topitem = ui->treeWidget->currentItem();
  if (topitem->childCount() == 0) topitem = topitem->parent();

  QTreeWidgetItem* item1 = new QTreeWidgetItem(topitem);
  item1->setText(0, ui->editNote->text().trimmed());

  ui->treeWidget->setCurrentItem(topitem->child(topitem->childCount() - 1));
}

void dlgNotesList::on_treeWidget_itemClicked(QTreeWidgetItem* item,
                                             int column) {
  ui->editName->setText(item->text(column));
}

void dlgNotesList::on_btnRename_clicked() {
  QTreeWidgetItem* item = ui->treeWidget->currentItem();
  item->setText(0, ui->editName->text().trimmed());
}
