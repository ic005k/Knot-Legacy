#include "src/EditRecord.h"

#include <QKeyEvent>

#include "mainwindow.h"
#include "ui_EditRecord.h"
#include "ui_mainwindow.h"
extern MainWindow *mw_one;
extern QTabWidget *tabData;
extern QString iniFile, iniDir;
extern QRegularExpression regxNumber;
extern bool isBreak, isImport;

bool del = false;

QStringList c_list;

dlgList *m_List;

EditRecord::EditRecord(QWidget *parent)
    : QDialog(parent), ui(new Ui::EditRecord) {
  ui->setupUi(this);

  mw_one->set_btnStyle(this);

  m_List = new dlgList(this);

  this->installEventFilter(this);
  ui->editCategory->installEventFilter(this);

  QFont font;
  font.setPointSize(23);
  font.setBold(true);

  ui->editAmount->setFont(font);
  ui->btn0->setFont(font);
  ui->btn1->setFont(font);
  ui->btn2->setFont(font);
  ui->btn3->setFont(font);
  ui->btn4->setFont(font);
  ui->btn5->setFont(font);
  ui->btn6->setFont(font);
  ui->btn7->setFont(font);
  ui->btn8->setFont(font);
  ui->btn9->setFont(font);
  ui->btn0->setFont(font);
  ui->btnDot->setFont(font);
  ui->btnDel->setFont(font);

  QValidator *validator =
      new QRegularExpressionValidator(regxNumber, ui->editAmount);
  ui->editAmount->setValidator(validator);
  ui->editAmount->setAttribute(Qt::WA_InputMethodEnabled, false);
  ui->editAmount->setReadOnly(true);

  ui->editCategory->setPlaceholderText(tr("Please enter a category"));

  ui->btnClearAmount->setStyleSheet("border:none");
  ui->btnClearDesc->setStyleSheet("border:none");
  ui->btnClearDetails->setStyleSheet("border:none");
  lblStyle = ui->lblCategory->styleSheet();

  ui->hsM->setStyleSheet(ui->hsH->styleSheet());

  mw_one->setLineEditQss(ui->editAmount, 10, 1, "#4169E1", "#4169E1");
  mw_one->setLineEditQss(ui->editCategory, 10, 1, "#4169E1", "#4169E1");
  mw_one->setLineEditQss(ui->editDetails, 10, 1, "#4169E1", "#4169E1");
}

void EditRecord::init() {
  mw_one->mydlgMainNotes->m_SetEditText->close();
  mw_one->mydlgMainNotes->m_SetEditText = new dlgSetEditText(this);

  setModal(true);
  setGeometry(mw_one->geometry().x(), mw_one->geometry().y(), mw_one->width(),
              mw_one->height());

  if (mw_one->isAdd) {
    ui->editCategory->setText("");
    ui->editAmount->setText("");
  }
  show();
}

EditRecord::~EditRecord() { delete ui; }

void EditRecord::on_btnBack_clicked() { close(); }

void EditRecord::keyReleaseEvent(QKeyEvent *event) { Q_UNUSED(event); }

void EditRecord::on_btnOk_clicked() {
  close();

  if (!mw_one->isAdd) {
    mw_one->addUndo(tr("Modify Item") + " ( " + mw_one->getTabText() + " ) ");

    mw_one->set_Time();
  } else {
    if (mw_one->isTesting) {
      for (int i = 0; i < 500; i++)
        mw_one->add_Data(mw_one->get_tw(mw_one->ui->tabWidget->currentIndex()),
                         ui->lblTime->text(), ui->editAmount->text().trimmed(),
                         ui->editCategory->text().trimmed());
    } else

      mw_one->addUndo(tr("Add Item") + " ( " + mw_one->getTabText() + " ) ");

    mw_one->add_Data(mw_one->get_tw(mw_one->ui->tabWidget->currentIndex()),
                     ui->lblTime->text(), ui->editAmount->text().trimmed(),
                     ui->editCategory->text().trimmed());
  }

  // Save Desc Text
  QString str = ui->editCategory->text().trimmed();
  int count = m_List->ui->listWidget->count();
  for (int i = 0; i < count; i++) {
    QString str1 = m_List->ui->listWidget->item(i)->text().trimmed();
    if (str == str1) {
      m_List->ui->listWidget->takeItem(i);
      break;
    }
  }

  if (str.length() > 0) {
    QListWidgetItem *item = new QListWidgetItem(str);
    m_List->ui->listWidget->insertItem(0, item);
  }

  // Save Details
  QString strDetails = ui->editDetails->text().trimmed();
  if (strDetails != "") {
    if (c_list.count() > 50) {
      c_list.removeAt(c_list.count() - 1);
    }

    c_list.removeOne(strDetails);
    c_list.insert(0, strDetails);
  }

  del = false;
  mw_one->startSave("tab");
}

void EditRecord::on_btn7_clicked() { set_Amount("7"); }

void EditRecord::on_btn8_clicked() { set_Amount("8"); }

void EditRecord::on_btn9_clicked() { set_Amount("9"); }

void EditRecord::on_btn4_clicked() { set_Amount("4"); }

void EditRecord::on_btn5_clicked() { set_Amount("5"); }

void EditRecord::on_btn6_clicked() { set_Amount("6"); }

void EditRecord::on_btn1_clicked() { set_Amount("1"); }

void EditRecord::on_btn2_clicked() { set_Amount("2"); }

void EditRecord::on_btn3_clicked() { set_Amount("3"); }

void EditRecord::on_btn0_clicked() { set_Amount("0"); }

void EditRecord::on_btnDot_clicked() { set_Amount("."); }

void EditRecord::on_btnDel_clicked() {
  QString str = ui->editAmount->text().trimmed();
  str = str.mid(0, str.length() - 1);
  ui->editAmount->setText(str);
}

void EditRecord::set_Amount(QString Number) {
  QString str = ui->editAmount->text().trimmed();
  if (str == "0.00")
    ui->editAmount->setText("");
  if (str.split(".").count() == 2 && str != "0.00") {
    QString str0 = str.split(".").at(1);
    if (str0.length() == 2)
      return;
  }
  ui->editAmount->setText(str + Number);
}

void EditRecord::on_btnCustom_clicked() {
  m_List->close();
  m_List = new dlgList(mw_one->myEditRecord);

  int h = mw_one->height();
  int w = mw_one->width();
  m_List->setGeometry(mw_one->geometry().x(), mw_one->geometry().y(), w, h);

  init_Desc();
  m_List->ui->listWidget->setFocus();
  m_List->ui->editRename->clear();

  m_List->show();
}

void EditRecord::saveCustomDesc() {
  QSettings Reg(iniDir + "desc.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif
  int count = m_List->ui->listWidget->count();

  QStringList list;
  for (int i = 0; i < count; i++) {
    if (isBreak)
      break;
    list.append(m_List->ui->listWidget->item(i)->text().trimmed());
  }
  // list = QSet<QString>(list.begin(), list.end()).values(); //IOS无法编译通过
  removeDuplicates(&list);
  for (int i = 0; i < list.count(); i++) {
    if (isBreak)
      break;
    QString str = list.at(i);
    if (str.length() > 0)
      Reg.setValue("/CustomDesc/Item" + QString::number(i), str);
  }
  Reg.setValue("/CustomDesc/Count", list.count());

  // Details
  for (int i = 0; i < c_list.count(); i++) {
    if (isBreak)
      break;
    QString str = c_list.at(i);
    Reg.setValue("/Details/Item" + QString::number(i), str);
  }
  Reg.setValue("/Details/Count", c_list.count());
}

int EditRecord::removeDuplicates(QStringList *that) {
  int n = that->size();
  int j = 0;
  QSet<QString> seen;
  seen.reserve(n);
  int setSize = 0;
  for (int i = 0; i < n; ++i) {
    const QString &s = that->at(i);
    seen.insert(s);
    if (setSize == seen.size()) // unchanged size => was already seen
      continue;
    ++setSize;

    // //将不重复项与重复项交换（新，IOS无法编译通过）
    //#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    if (j != i)
      that->swapItemsAt(i, j);
    //#else
    //    if (j != i) that->swap(i, j);
    //#endif

    ++j;
  }
  if (n != j)
    that->erase(that->begin() + j, that->end());
  return n - j;
}

void EditRecord::init_Desc() {
  // Custom Desc
  QString ini_file;
  if (isImport)
    ini_file = iniFile;
  else
    ini_file = iniDir + "desc.ini";
  QSettings RegDesc(ini_file, QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  RegDesc.setIniCodec("utf-8");
#endif

  m_List->ui->listWidget->clear();
  m_List->ui->listWidget->setViewMode(QListView::IconMode);
  int descCount = RegDesc.value("/CustomDesc/Count").toInt();
  for (int i = 0; i < descCount; i++) {
    QString str =
        RegDesc.value("/CustomDesc/Item" + QString::number(i)).toString();
    QListWidgetItem *item = new QListWidgetItem(str);
    // item->setSizeHint(
    //    QSize(mw_one->mydlgList->ui->listWidget->width() - 20, 35));
    m_List->ui->listWidget->addItem(item);
  }

  // Details
  c_list.clear();
  int DetailsCount = RegDesc.value("/Details/Count").toInt();
  for (int i = 0; i < DetailsCount; i++) {
    QString str =
        RegDesc.value("/Details/Item" + QString::number(i)).toString();
    c_list.append(str);
  }
}

void EditRecord::getTime(int h, int m) {
  QString strh, strm, strs;
  if (h < 10)
    strh = "0" + QString::number(h);
  else
    strh = QString::number(h);
  if (m < 10)
    strm = "0" + QString::number(m);
  else
    strm = QString::number(m);
  int s = QTime::currentTime().second();
  if (s < 10)
    strs = "0" + QString::number(s);
  else
    strs = QString::number(s);
  ui->lblTime->setText(strh + ":" + strm + ":" + strs);
}

bool EditRecord::eventFilter(QObject *watch, QEvent *evn) {
  if (evn->type() == QEvent::KeyPress) {
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      if (!m_List->isHidden()) {
        m_List->close();
        mw_one->closeGrayWindows();

        return true;
      } else if (!mw_one->myEditRecord->isHidden()) {
        on_btnBack_clicked();
        return true;
      }
    }

    if (keyEvent->key() == Qt::Key_Return) {
      return true;
    }
  }

  return QWidget::eventFilter(watch, evn);
}

void EditRecord::on_btnClearAmount_clicked() { ui->editAmount->clear(); }

void EditRecord::on_btnClearDesc_clicked() { ui->editCategory->clear(); }

void EditRecord::on_editAmount_textChanged(const QString &arg1) {
  int count = 0;
  for (int i = 0; i < arg1.count(); i++) {
    if (arg1.mid(i, 1) == ".")
      count++;
    if (count == 2) {
      QString str0 = arg1;
      QString str = str0.mid(0, str0.length() - 1);
      ui->editAmount->setText(str);
      break;
    }
  }

  if (arg1.length() > 0)
    ui->lblAmount->setStyleSheet(lblStyleHighLight);
  else
    ui->lblAmount->setStyleSheet(lblStyle);
}

void EditRecord::on_hsH_valueChanged(int value) {
  getTime(value, ui->hsM->value());
}

void EditRecord::on_hsM_valueChanged(int value) {
  getTime(ui->hsH->value(), value);
}

void EditRecord::on_btnClearDetails_clicked() { ui->editDetails->clear(); }

void EditRecord::on_editDesc_textChanged(const QString &arg1) {
  if (arg1.length() > 0)
    ui->lblCategory->setStyleSheet(lblStyleHighLight);
  else
    ui->lblCategory->setStyleSheet(lblStyle);
}

void EditRecord::on_editDetails_textChanged(const QString &arg1) {
  if (arg1.length() > 0)
    ui->lblDetails->setStyleSheet(lblStyleHighLight);
  else
    ui->lblDetails->setStyleSheet(lblStyle);

  QCompleter *completer = new QCompleter(c_list);
  ui->editDetails->setCompleter(completer);
}

void EditRecord::saveOne() {
  QTreeWidget *tw = (QTreeWidget *)tabData->currentWidget();
  int tabIndex = tabData->currentIndex();

  QString name = "tab" + QString::number(tabIndex + 1);
  tw->setObjectName(name);

  QString ini_file = iniDir + name + ".ini";
  QSettings Reg(ini_file, QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif

  int count = tw->topLevelItemCount();

  QTreeWidgetItem *item = tw->currentItem();
  int i = 0;
  if (item->parent() == NULL)
    i = tw->indexOfTopLevelItem(item);
  else
    i = tw->indexOfTopLevelItem(item->parent());

  QString flag = "/" + name + "/";

  Reg.setValue(flag + QString::number(i + 1) + "-topDate",
               tw->topLevelItem(i)->text(0));
  Reg.setValue(flag + QString::number(i + 1) + "-topYear",
               tw->topLevelItem(i)->text(3));
  Reg.setValue(flag + QString::number(i + 1) + "-topFreq",
               tw->topLevelItem(i)->text(1));
  Reg.setValue(flag + QString::number(i + 1) + "-topAmount",
               tw->topLevelItem(i)->text(2));

  int childCount = tw->topLevelItem(i)->childCount();

  if (childCount > 0) {
    if (!del) {
      for (int j = 0; j < childCount; j++) {
        if (isBreak)
          return;
        Reg.setValue(flag + QString::number(i + 1) + "-childTime" +
                         QString::number(j),
                     tw->topLevelItem(i)->child(j)->text(0));
        Reg.setValue(flag + QString::number(i + 1) + "-childAmount" +
                         QString::number(j),
                     tw->topLevelItem(i)->child(j)->text(1));
        Reg.setValue(flag + QString::number(i + 1) + "-childDesc" +
                         QString::number(j),
                     tw->topLevelItem(i)->child(j)->text(2));
        Reg.setValue(flag + QString::number(i + 1) + "-childDetails" +
                         QString::number(j),
                     tw->topLevelItem(i)->child(j)->text(3));
      }
    }
  }

  Reg.setValue(flag + QString::number(i + 1) + "-childCount", childCount);
  Reg.setValue("/" + name + "/TopCount", count);
}
