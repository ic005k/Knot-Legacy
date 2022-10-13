#include "dlgNotesList.h"

#include "src/mainwindow.h"
#include "ui_dlgNotesList.h"
#include "ui_mainwindow.h"

extern MainWindow* mw_one;
extern QString iniDir;

dlgNotesList::dlgNotesList(QWidget* parent)
    : QDialog(parent), ui(new Ui::dlgNotesList) {
  ui->setupUi(this);
  tw = ui->treeWidget;
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
    item1->setText(1, iniDir + "memo/memo.md");
    ui->treeWidget->addTopLevelItem(item);
    ui->treeWidget->setCurrentItem(item->child(item->childCount() - 1));
  }

  ui->treeWidget->setFocus();
  ui->treeWidget->expandAll();
  on_treeWidget_itemClicked(ui->treeWidget->currentItem(), 0);
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
  // QTreeWidgetItem* item1 = new QTreeWidgetItem(item);
  // item1->setText(0, ui->editNote->text().trimmed());
  // item1->setText(
  //     1, iniDir + "memo/" + mw_one->mydlgMainNotes->getDateTimeStr() +
  //     ".md");
  ui->treeWidget->addTopLevelItem(item);

  ui->treeWidget->setCurrentItem(item);
  on_btnNewNote_clicked();
}

void dlgNotesList::on_btnNewNote_clicked() {
  if (ui->treeWidget->topLevelItemCount() == 0) return;

  QString noteFile =
      iniDir + "memo/" + mw_one->mydlgMainNotes->getDateTimeStr() + ".md";
  QTreeWidgetItem* topitem = ui->treeWidget->currentItem();
  if (topitem->parent() != NULL) topitem = topitem->parent();

  QTreeWidgetItem* item1 = new QTreeWidgetItem(topitem);
  item1->setText(0, ui->editNote->text().trimmed());
  item1->setText(1, noteFile);
  QTextEdit* edit = new QTextEdit();
  mw_one->TextEditToFile(edit, noteFile);

  ui->treeWidget->setCurrentItem(topitem->child(topitem->childCount() - 1));
  on_treeWidget_itemClicked(item1, 0);
}

void dlgNotesList::on_treeWidget_itemClicked(QTreeWidgetItem* item,
                                             int column) {
  if (ui->treeWidget->topLevelItemCount() == 0) return;
  QString mdfile;
  if (item->parent() != NULL) {
    mdfile = item->text(1);
    mw_one->mydlgMainNotes->MD2Html(mdfile);
    mw_one->mydlgMainNotes->loadMemoQML();
    currentMDFile = mdfile;
    if (!mw_one->initMain) mw_one->mydlgMainNotes->setVPos();
  }

  ui->editName->setText(item->text(column));
}

void dlgNotesList::on_btnRename_clicked() {
  if (ui->treeWidget->topLevelItemCount() == 0) return;

  QTreeWidgetItem* item = ui->treeWidget->currentItem();
  item->setText(0, ui->editName->text().trimmed());
}

void dlgNotesList::on_btnDel_clicked() {
  QTreeWidgetItem* item = ui->treeWidget->currentItem();
  int count = ui->treeWidget->topLevelItemCount();

  if (item->parent() == NULL) {
    if (ui->treeWidget->currentIndex().row() == 0) return;
  } else {
    if (ui->treeWidget->currentIndex().row() == 0) {
      if (count == 1) return;
    }
  }

  if (mw_one->showMsgBox("Kont", tr("Delete?"), "", 2)) {
    if (item->parent() == NULL) {
      int count = item->childCount();
      for (int i = 0; i < count; i++) {
        QString file = item->child(i)->text(1);
        delFile(file);
      }
      ui->treeWidget->takeTopLevelItem(ui->treeWidget->currentIndex().row());

    } else {
      QString file = item->text(1);
      delFile(file);
      item->parent()->removeChild(item);
    }
  }
}

void dlgNotesList::delFile(QString file) {
  QFile _file(file);
  bool yes;
  if (_file.exists()) yes = _file.remove();
  _file.close();
}

void dlgNotesList::on_btnImport_clicked() {
  if (ui->treeWidget->topLevelItemCount() == 0) return;

  QString fileName;
  fileName =
      QFileDialog::getOpenFileName(this, tr("Knot"), "", tr("MD File (*.md)"));
  if (QFile(fileName).exists()) {
    QTreeWidgetItem* item = ui->treeWidget->currentItem();

    QTreeWidgetItem* item1;
    if (item->parent() == NULL) {
      item1 = new QTreeWidgetItem(item);
      item1->setText(0, tr("Notes Imported"));
    } else {
      item1 = new QTreeWidgetItem(item->parent());
      item1->setText(0, tr("Notes Imported"));
    }
    tw->setCurrentItem(item1);
    currentMDFile =
        iniDir + "memo/" + mw_one->mydlgMainNotes->getDateTimeStr() + ".md";
    QFile::copy(fileName, currentMDFile);
    item1->setText(1, currentMDFile);

    on_treeWidget_itemClicked(item1, 1);
  }
}

void dlgNotesList::on_btnExport_clicked() {
  if (ui->treeWidget->topLevelItemCount() == 0) return;

  QTreeWidgetItem* item = tw->currentItem();
  if (item->parent() == NULL) return;

  QString fileName;
  QFileDialog fd;
  fileName = fd.getSaveFileName(this, tr("Knot"), "", tr("MD File(*.md)"));
  if (!fileName.contains(".md")) fileName = fileName + ".md";

  QString mdfile = item->text(1);

  QString str = mw_one->loadText(mdfile);
  QTextEdit* edit = new QTextEdit();
  edit->setAcceptRichText(false);
  edit->setPlainText(str);
  mw_one->TextEditToFile(edit, fileName);
}
