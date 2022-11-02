#include "dlglist.h"

#include "mainwindow.h"
#include "ui_dlglist.h"
#include "ui_mainwindow.h"
extern MainWindow* mw_one;
extern int fontSize, red;
extern QTabWidget *tabData, *tabChart;

dlgList::dlgList(QWidget* parent) : QDialog(parent), ui(new Ui::dlgList) {
  ui->setupUi(this);
  mw_one->set_btnStyle(this);
  setWindowFlag(Qt::FramelessWindowHint);
  // setAttribute(Qt::WA_TranslucentBackground);
  this->layout()->setContentsMargins(5, 5, 5, 5);
  ui->frame->setStyleSheet(
      "QFrame{background-color: rgb(255, 255, 255);border-radius:10px; "
      "border:0px solid gray;}");
  setModal(true);
  this->installEventFilter(this);

  ui->listWidget->verticalScrollBar()->setStyleSheet(mw_one->vsbarStyleSmall);
  ui->listWidget->setVerticalScrollMode(QListWidget::ScrollPerPixel);
  QScroller::grabGesture(ui->listWidget, QScroller::LeftMouseButtonGesture);
  ui->listWidget->horizontalScrollBar()->setHidden(true);
  ui->listWidget->setViewMode(QListView::IconMode);
  ui->listWidget->setMovement(QListView::Static);
  ui->listWidget->setStyleSheet(mw_one->listStyleMain);
  ui->listWidget->setSpacing(12);
  mw_one->setSCrollPro(ui->listWidget);
  QFont font;
  font.setPointSize(fontSize + 1);
  ui->listWidget->setFont(font);
  mw_one->setLineEditQss(ui->editRename, 4, 1, "#4169E1", "#4169E1");
  ui->btnRename->setFixedHeight(ui->editRename->height() + 2);
}

dlgList::~dlgList() { delete ui; }

void dlgList::keyReleaseEvent(QKeyEvent* event) { Q_UNUSED(event) }

void dlgList::closeEvent(QCloseEvent* event) {
  Q_UNUSED(event);
  mw_one->closeGrayWindows();
  mw_one->m_widget = new QWidget(mw_one);
}

bool dlgList::eventFilter(QObject* watch, QEvent* evn) {
  if (evn->type() == QEvent::KeyPress) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      close();
      return true;
    }
  }

  return QWidget::eventFilter(watch, evn);
}

void dlgList::on_listWidget_itemClicked(QListWidgetItem* item) {
  Q_UNUSED(item);
  QString txt = item->text();
  ui->editRename->setText(txt);
}

void dlgList::on_btnDel_clicked() {
  int row = ui->listWidget->currentRow();
  if (row >= 0) ui->listWidget->takeItem(row);
  mw_one->mydlgSetTime->saveCustomDesc();
  if (ui->listWidget->count() > 0)
    on_listWidget_itemClicked(ui->listWidget->currentItem());
  else
    ui->editRename->clear();
}

void dlgList::on_btnBack_clicked() { setCategoryText(); }

void dlgList::setCategoryText() {
  int row = ui->listWidget->currentRow();
  if (row >= 0) {
    mw_one->mydlgSetTime->ui->editDesc->setPlainText(
        ui->listWidget->currentItem()->text());
  }

  close();
}

void dlgList::on_btnChange_clicked() {
  if (ui->listWidget->count() == 0) return;
  QString str = ui->listWidget->currentItem()->text().trimmed();
  bool ok;

  QInputDialog* idlg = new QInputDialog(this);
  QString style = "QDialog{border-radius:0px;border:2px solid red;}";

  idlg->setStyleSheet(style);
  idlg->setOkButtonText(tr("Ok"));
  idlg->setCancelButtonText(tr("Cancel"));
  idlg->setWindowTitle(tr("Please enter a new name : "));
  idlg->setTextValue(str);
  idlg->setLabelText(tr("New Name : "));
  idlg->setContentsMargins(6, 6, 6, 6);

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
    // item->setSizeHint(QSize(ui->listWidget->width() - 20, 35));
    ui->listWidget->insertItem(index, item);

    QStringList list;
    for (int i = 0; i < ui->listWidget->count(); i++) {
      list.append(ui->listWidget->item(i)->text().trimmed());
    }
    mw_one->mydlgSetTime->removeDuplicates(&list);
    ui->listWidget->clear();
    for (int i = 0; i < list.count(); i++) {
      QListWidgetItem* item = new QListWidgetItem(list.at(i));
      // item->setSizeHint(QSize(ui->listWidget->width() - 20, 35));
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

void dlgList::on_listWidget_itemDoubleClicked(QListWidgetItem* item) {
  Q_UNUSED(item);
  setCategoryText();
}

void dlgList::on_btnRename_clicked() {
  if (ui->listWidget->count() == 0) return;

  QString text = ui->editRename->text().trimmed();
  QString str = ui->listWidget->currentItem()->text();
  if (!text.isEmpty() && text != str) {
    int index = ui->listWidget->currentRow();
    ui->listWidget->takeItem(index);
    QListWidgetItem* item = new QListWidgetItem(text);
    // item->setSizeHint(QSize(ui->listWidget->width() - 20, 35));
    ui->listWidget->insertItem(index, item);

    QStringList list;
    for (int i = 0; i < ui->listWidget->count(); i++) {
      list.append(ui->listWidget->item(i)->text().trimmed());
    }
    mw_one->mydlgSetTime->removeDuplicates(&list);
    ui->listWidget->clear();
    for (int i = 0; i < list.count(); i++) {
      QListWidgetItem* item = new QListWidgetItem(list.at(i));
      // item->setSizeHint(QSize(ui->listWidget->width() - 20, 35));
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

    mw_one->mydlgSetTime->ui->editDesc->setPlainText(
        ui->editRename->text().trimmed());
  }
}
