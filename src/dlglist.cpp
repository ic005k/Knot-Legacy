#include "dlglist.h"

#include "mainwindow.h"
#include "ui_dlglist.h"
#include "ui_mainwindow.h"
extern MainWindow* mw_one;
extern int fontSize, red;
extern QTabWidget *tabData, *tabChart;
dlgList::dlgList(QWidget* parent) : QDialog(parent), ui(new Ui::dlgList) {
  ui->setupUi(this);
  this->installEventFilter(this);
  ui->listWidget->verticalScrollBar()->setStyleSheet(mw_one->vsbarStyleSmall);
  ui->listWidget->setVerticalScrollMode(QListWidget::ScrollPerPixel);
  QScroller::grabGesture(ui->listWidget, QScroller::LeftMouseButtonGesture);
  ui->listWidget->horizontalScrollBar()->setHidden(true);
  ui->listWidget->setStyleSheet(mw_one->listStyle);
}

dlgList::~dlgList() { delete ui; }

void dlgList::keyReleaseEvent(QKeyEvent* event) { Q_UNUSED(event) }

bool dlgList::eventFilter(QObject* watch, QEvent* evn) {
  if (evn->type() == QEvent::KeyPress) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      on_btnBack_clicked();
      return true;
    }
  }

  return QWidget::eventFilter(watch, evn);
}

void dlgList::on_listWidget_itemClicked(QListWidgetItem* item) {
  Q_UNUSED(item);
}

void dlgList::on_btnClear_clicked() {
  int row = ui->listWidget->currentRow();
  if (row >= 0) ui->listWidget->takeItem(row);
  mw_one->mydlgSetTime->saveCustomDesc();
}

void dlgList::on_btnBack_clicked() {
  int row = ui->listWidget->currentRow();
  if (row >= 0) {
    mw_one->mydlgSetTime->ui->editDesc->setText(
        ui->listWidget->item(row)->text());
  }
  close();
}

void dlgList::on_btnChange_clicked() {
  if (ui->listWidget->count() == 0) return;
  QString str = ui->listWidget->currentItem()->text().trimmed();
  bool ok;
  QInputDialog* idlg = new QInputDialog(this);
  QString style =
      "QDialog{background: "
      "rgb(244,237,241);border-radius:0px;border:2px solid gray;}";
  idlg->setStyleSheet(style);
  idlg->setOkButtonText(tr("Ok"));
  idlg->setCancelButtonText(tr("Cancel"));
  idlg->setWindowTitle(tr("Please enter a new name : "));
  idlg->setTextValue(str);
  idlg->setLabelText(tr("New Name : "));
  QLineEdit::EchoMode echoMode = QLineEdit::Normal;
  idlg->setTextEchoMode(echoMode);
  QString text;

  if (QDialog::Accepted == idlg->exec()) {
    ok = true;
    text = idlg->textValue().trimmed();
  } else
    ok = false;

  if (ok && !text.isEmpty() && text != str) {
    int index = ui->listWidget->currentRow();
    ui->listWidget->takeItem(index);
    QListWidgetItem* item = new QListWidgetItem(text);
    item->setSizeHint(QSize(ui->listWidget->width() - 20, 35));
    ui->listWidget->insertItem(index, item);

    QStringList list;
    for (int i = 0; i < ui->listWidget->count(); i++) {
      list.append(ui->listWidget->item(i)->text().trimmed());
    }
    mw_one->mydlgSetTime->removeDuplicates(&list);
    ui->listWidget->clear();
    for (int i = 0; i < list.count(); i++) {
      QListWidgetItem* item = new QListWidgetItem(list.at(i));
      item->setSizeHint(QSize(ui->listWidget->width() - 20, 35));
      ui->listWidget->addItem(item);
    }
    if (index >= 0) ui->listWidget->setCurrentRow(index);
    mw_one->mydlgSetTime->saveCustomDesc();

    for (int i = 0; i < tabData->tabBar()->count(); i++) {
      QTreeWidget* tw = (QTreeWidget*)tabData->widget(i);
      for (int m = 0; m < tw->topLevelItemCount(); m++) {
        QTreeWidgetItem* topItem = tw->topLevelItem(m);
        for (int n = 0; n < topItem->childCount(); n++) {
          if (str == topItem->child(n)->text(2)) {
            topItem->child(n)->setText(2, text);
          }
        }
      }
    }
    mw_one->startSave("alltab");
  }
}
