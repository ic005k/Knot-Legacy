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

  initNotesList();
  if (ui->treeWidget->topLevelItemCount() == 0) {
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0, tr("Default"));
    QTreeWidgetItem* item1 = new QTreeWidgetItem(item);
    item1->setText(0, tr("My Notes"));
    item1->setText(1, iniDir + "memo/memo.md");
    ui->treeWidget->addTopLevelItem(item);
    ui->treeWidget->setCurrentItem(item->child(item->childCount() - 1));

    ui->treeWidget->setFocus();
    ui->treeWidget->expandAll();
    on_treeWidget_itemClicked(ui->treeWidget->currentItem(), 0);

    saveNotesList();
  }

  ui->editBook->setStyleSheet(
      mw_one->mydlgSetTime->ui->editAmount->styleSheet());
  ui->editName->setStyleSheet(
      mw_one->mydlgSetTime->ui->editAmount->styleSheet());
  ui->editNote->setStyleSheet(
      mw_one->mydlgSetTime->ui->editAmount->styleSheet());
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

  ui->editName->setText(item->text(0));
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
      QFileDialog::getOpenFileName(this, tr("Knot"), "", tr("MD File (*.*)"));

  if (!fileName.contains(".md")) {
    QMessageBox box;
    box.setText(tr("Invalid Markdown file."));
    box.exec();
    return;
  }

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
    QString str = mw_one->loadText(fileName);
    QTextEdit* edit = new QTextEdit();
    edit->setAcceptRichText(false);
    edit->setPlainText(str);
    mw_one->TextEditToFile(edit, currentMDFile);

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
  fileName = fd.getSaveFileName(this, tr("Knot"), "", tr("MD File(*.*)"));

  if (fileName == "") return;

  QString mdfile = item->text(1);

  QString str = mw_one->loadText(mdfile);
  QTextEdit* edit = new QTextEdit();
  edit->setAcceptRichText(false);
  edit->setPlainText(str);

  if (fileName.contains(".md")) fileName.replace(".md", "");
  if (QFile(fileName + ".md").exists()) {
    if (!mw_one->showMsgBox(
            "Knot", tr("The file already exists, is it overwritten?"), "", 2))
      return;
  }

  mw_one->TextEditToFile(edit, fileName + ".md");
}

void dlgNotesList::closeEvent(QCloseEvent* event) {
  Q_UNUSED(event);
  saveNotesList();
}

void dlgNotesList::saveNotesList() {
  QSettings Reg(iniDir + "mainnotes.ini", QSettings::IniFormat);
  Reg.setValue("/MainNotes/currentItem", currentMDFile);

  int count = tw->topLevelItemCount();
  Reg.setValue("/MainNotes/topItemCount", count);
  for (int i = 0; i < count; i++) {
    QTreeWidgetItem* topItem = tw->topLevelItem(i);
    QString strtop = topItem->text(0);
    Reg.setValue("/MainNotes/strTopItem" + QString::number(i), strtop);

    int childCount = topItem->childCount();
    Reg.setValue("/MainNotes/childCount" + QString::number(i), childCount);

    for (int j = 0; j < childCount; j++) {
      QTreeWidgetItem* childItem = tw->topLevelItem(i)->child(j);
      QString strChild0 = childItem->text(0);
      QString strChild1 = childItem->text(1);

      Reg.setValue(
          "/MainNotes/childItem0" + QString::number(i) + QString::number(j),
          strChild0);
      Reg.setValue(
          "/MainNotes/childItem1" + QString::number(i) + QString::number(j),
          strChild1);
    }
  }
}

void dlgNotesList::initNotesList() {
  tw->clear();
  QSettings Reg(iniDir + "mainnotes.ini", QSettings::IniFormat);
  int topCount = Reg.value("/MainNotes/topItemCount").toInt();
  for (int i = 0; i < topCount; i++) {
    QString strTop =
        Reg.value("/MainNotes/strTopItem" + QString::number(i)).toString();
    QTreeWidgetItem* topItem = new QTreeWidgetItem;
    topItem->setText(0, strTop);

    int childCount =
        Reg.value("/MainNotes/childCount" + QString::number(i)).toInt();
    for (int j = 0; j < childCount; j++) {
      QString str0, str1;
      str0 = Reg.value("/MainNotes/childItem0" + QString::number(i) +
                       QString::number(j))
                 .toString();
      str1 = Reg.value("/MainNotes/childItem1" + QString::number(i) +
                       QString::number(j))
                 .toString();
      QTreeWidgetItem* childItem = new QTreeWidgetItem(topItem);
      childItem->setText(0, str0);
      childItem->setText(1, str1);
    }
    tw->addTopLevelItem(topItem);
  }

  tw->expandAll();
  currentMDFile = Reg.value("/MainNotes/currentItem").toString();
  bool stop = false;
  for (int i = 0; i < tw->topLevelItemCount(); i++) {
    if (stop) break;
    QTreeWidgetItem* topItem = tw->topLevelItem(i);
    int childCount = topItem->childCount();

    for (int j = 0; j < childCount; j++) {
      QTreeWidgetItem* childItem = topItem->child(j);
      QString strChild1 = childItem->text(1);
      if (strChild1 == currentMDFile) {
        tw->setCurrentItem(childItem);
        on_treeWidget_itemClicked(childItem, 0);
        stop = true;
        break;
      }
    }
  }
}
