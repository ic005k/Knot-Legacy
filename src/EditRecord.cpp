#include "src/EditRecord.h"

#include <QKeyEvent>

#include "MainWindow.h"
#include "ui_EditRecord.h"
#include "ui_MainWindow.h"
extern MainWindow *mw_one;
extern QTabWidget *tabData;
extern QString iniFile, iniDir;
extern QRegularExpression regxNumber;
extern bool isBreak;
extern int fontSize;

bool del = false;

QStringList c_list;

CategoryList *m_CategoryList;

EditRecord::EditRecord(QWidget *parent)
    : QDialog(parent), ui(new Ui::EditRecord) {
  ui->setupUi(this);

  mw_one->set_btnStyle(this);

  m_CategoryList = new CategoryList(this);

  this->installEventFilter(this);
  mw_one->ui->editCategory->installEventFilter(this);
  mw_one->ui->editDetails->viewport()->installEventFilter(mw_one);

  QFont font;
  font.setPointSize(23);
  font.setBold(true);

  mw_one->ui->editAmount->setFont(font);
  mw_one->ui->btn0->setFont(font);
  mw_one->ui->btn1->setFont(font);
  mw_one->ui->btn2->setFont(font);
  mw_one->ui->btn3->setFont(font);
  mw_one->ui->btn4->setFont(font);
  mw_one->ui->btn5->setFont(font);
  mw_one->ui->btn6->setFont(font);
  mw_one->ui->btn7->setFont(font);
  mw_one->ui->btn8->setFont(font);
  mw_one->ui->btn9->setFont(font);
  mw_one->ui->btn0->setFont(font);
  mw_one->ui->btnDot->setFont(font);
  mw_one->ui->btnDel_Number->setFont(font);

  font.setPointSize(fontSize);
  font.setBold(true);
  mw_one->ui->lblTitleEditRecord->setFont(font);

  QValidator *validator =
      new QRegularExpressionValidator(regxNumber, mw_one->ui->editAmount);
  mw_one->ui->editAmount->setValidator(validator);
  mw_one->ui->editAmount->setAttribute(Qt::WA_InputMethodEnabled, false);
  mw_one->ui->editAmount->setReadOnly(true);

  mw_one->ui->editCategory->setPlaceholderText(tr("Please enter a category"));
  mw_one->ui->editCategory->setFixedHeight(mw_one->m_Method->getFontHeight() +
                                           8);

  lblStyle = mw_one->ui->lblCategory->styleSheet();

  mw_one->ui->hsM->setStyleSheet(mw_one->ui->hsH->styleSheet());

  mw_one->setLineEditQss(mw_one->ui->editAmount, 0, 1, "#4169E1", "#4169E1");
  mw_one->setLineEditQss(mw_one->ui->editCategory, 0, 1, "#4169E1", "#4169E1");

  mw_one->ui->editDetails->setStyleSheet(
      "border-radius:0px;border: 1px groove #4169E1;");

  QScroller::grabGesture(mw_one->ui->editDetails,
                         QScroller::LeftMouseButtonGesture);
  mw_one->m_Method->setSCrollPro(mw_one->ui->editDetails);
}

void EditRecord::init() {
  mw_one->m_Notes->m_TextSelector->close();
  mw_one->m_Notes->m_TextSelector = new TextSelector(this);

  setModal(true);
  setGeometry(mw_one->geometry().x(), mw_one->geometry().y(), mw_one->width(),
              mw_one->height());

  if (mw_one->isAdd) {
    mw_one->ui->editCategory->setText("");
    mw_one->ui->editAmount->setText("");
  }

  show();
}

EditRecord::~EditRecord() { delete ui; }

void EditRecord::on_btnBack_clicked() { close(); }

void EditRecord::keyReleaseEvent(QKeyEvent *event) { Q_UNUSED(event); }

void EditRecord::on_btnOk_clicked() {
  mw_one->on_btnBackEditRecord_clicked();

  if (!mw_one->isAdd) {
    mw_one->set_Time();

    mw_one->isNeedAutoBackup = true;
    mw_one->strLatestModify =
        tr("Modify Item") + " ( " + mw_one->getTabText() + " ) ";

  } else {
    if (mw_one->isTesting) {
      for (int i = 0; i < 500; i++)
        mw_one->add_Data(mw_one->get_tw(mw_one->ui->tabWidget->currentIndex()),
                         mw_one->ui->lblTime->text(),
                         mw_one->ui->editAmount->text().trimmed(),
                         mw_one->ui->editCategory->text().trimmed());
    } else

      mw_one->add_Data(mw_one->get_tw(mw_one->ui->tabWidget->currentIndex()),
                       mw_one->ui->lblTime->text(),
                       mw_one->ui->editAmount->text().trimmed(),
                       mw_one->ui->editCategory->text().trimmed());

    mw_one->isNeedAutoBackup = true;
    mw_one->strLatestModify =
        tr("Add Item") + " ( " + mw_one->getTabText() + " ) ";
  }

  mw_one->clickData();

  // Save Category Text
  QString str = mw_one->ui->editCategory->text().trimmed();
  int count = m_CategoryList->ui->listWidget->count();
  for (int i = 0; i < count; i++) {
    QString str1 = m_CategoryList->ui->listWidget->item(i)->text().trimmed();
    if (str == str1) {
      m_CategoryList->ui->listWidget->takeItem(i);
      break;
    }
  }

  if (str.length() > 0) {
    QListWidgetItem *item = new QListWidgetItem(str);
    m_CategoryList->ui->listWidget->insertItem(0, item);
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
  QString str = mw_one->ui->editAmount->text().trimmed();
  str = str.mid(0, str.length() - 1);
  mw_one->ui->editAmount->setText(str);
}

void EditRecord::set_Amount(QString Number) {
  QString str = mw_one->ui->editAmount->text().trimmed();
  if (str == "0.00") mw_one->ui->editAmount->setText("");
  if (str.split(".").count() == 2 && str != "0.00") {
    QString str0 = str.split(".").at(1);
    if (str0.length() == 2) return;
  }
  mw_one->ui->editAmount->setText(str + Number);
}

void EditRecord::on_btnCustom_clicked() {
  this->hide();
  mw_one->ui->frameEditRecord->hide();
  mw_one->ui->frameCategory->show();
  init_Desc();
  m_CategoryList->ui->listWidget->setCurrentRow(0);
  mw_one->m_Method->setCurrentIndexFromQW(mw_one->ui->qwCategory, 0);
  mw_one->m_Method->setTypeRenameText();

  int count = mw_one->m_Method->getCountFromQW(mw_one->ui->qwCategory);
  mw_one->ui->lblTypeInfo->setText(tr("Total") + " : " +
                                   QString::number(count));

  return;

  m_CategoryList->close();
  m_CategoryList = new CategoryList(mw_one->m_EditRecord);

  int h = mw_one->height();
  int w = mw_one->width();
  m_CategoryList->setGeometry(mw_one->geometry().x(), mw_one->geometry().y(), w,
                              h);

  init_Desc();
  m_CategoryList->ui->listWidget->setFocus();
  if (m_CategoryList->ui->listWidget->count() > 0)
    m_CategoryList->ui->listWidget->setCurrentRow(0);
  m_CategoryList->ui->editRename->clear();

  m_CategoryList->show();
}

void EditRecord::saveCustomDesc() {
  QSettings Reg(iniDir + "desc.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif
  int count = m_CategoryList->ui->listWidget->count();

  c_list.clear();
  for (int i = 0; i < count; i++) {
    if (isBreak) break;
    c_list.append(m_CategoryList->ui->listWidget->item(i)->text().trimmed());
  }
  // list = QSet<QString>(list.begin(), list.end()).values(); //IOS无法编译通过
  removeDuplicates(&c_list);

  for (int i = 0; i < c_list.count(); i++) {
    if (isBreak) break;
    QString str = c_list.at(i);
    if (str.length() > 0)
      Reg.setValue("/CustomDesc/Item" + QString::number(i), str);
  }
  Reg.setValue("/CustomDesc/Count", c_list.count());
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
    if (setSize == seen.size())  // unchanged size => was already seen
      continue;
    ++setSize;

    // //将不重复项与重复项交换（新，IOS无法编译通过）
    // #if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    if (j != i) that->swapItemsAt(i, j);
    // #else
    //     if (j != i) that->swap(i, j);
    // #endif

    ++j;
  }
  if (n != j) that->erase(that->begin() + j, that->end());
  return n - j;
}

void EditRecord::init_Desc() {
  // Custom Desc
  QString ini_file;

  ini_file = iniDir + "desc.ini";
  QSettings RegDesc(ini_file, QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  RegDesc.setIniCodec("utf-8");
#endif

  mw_one->m_Method->clearAllBakList(mw_one->ui->qwCategory);

  c_list.clear();
  m_CategoryList->ui->listWidget->clear();
  m_CategoryList->ui->listWidget->setViewMode(QListView::IconMode);
  int descCount = RegDesc.value("/CustomDesc/Count").toInt();
  for (int i = 0; i < descCount; i++) {
    QString str =
        RegDesc.value("/CustomDesc/Item" + QString::number(i)).toString();
    QListWidgetItem *item = new QListWidgetItem(str);

    m_CategoryList->ui->listWidget->addItem(item);
    c_list.append(str);

    mw_one->m_Method->addItemToQW(mw_one->ui->qwCategory, str, "", "", "", 0);
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
  mw_one->ui->lblTime->setText(strh + ":" + strm + ":" + strs);
}

bool EditRecord::eventFilter(QObject *watch, QEvent *evn) {
  if (evn->type() == QEvent::KeyRelease) {
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      if (!mw_one->m_Notes->m_TextSelector->isHidden()) {
        mw_one->m_Notes->m_TextSelector->close();
        return true;
      } else if (!m_CategoryList->isHidden()) {
        m_CategoryList->close();
        mw_one->closeGrayWindows();

        return true;
      } else if (!mw_one->m_EditRecord->isHidden()) {
        on_btnBack_clicked();
        return true;
      }
    }

    if (keyEvent->key() == Qt::Key_Return) {
      return false;
    }
  }

  return QWidget::eventFilter(watch, evn);
}

void EditRecord::on_btnClearAmount_clicked() {
  mw_one->ui->editAmount->clear();
}

void EditRecord::on_btnClearDesc_clicked() {
  mw_one->ui->editCategory->clear();
}

void EditRecord::on_editAmount_textChanged(const QString &arg1) {
  int count = 0;
  for (int i = 0; i < arg1.count(); i++) {
    if (arg1.mid(i, 1) == ".") count++;
    if (count == 2) {
      QString str0 = arg1;
      QString str = str0.mid(0, str0.length() - 1);
      mw_one->ui->editAmount->setText(str);
      break;
    }
  }

  if (arg1.length() > 0)
    mw_one->ui->lblAmount->setStyleSheet(lblStyleHighLight);
  else
    mw_one->ui->lblAmount->setStyleSheet(lblStyle);
}

void EditRecord::on_hsH_valueChanged(int value) {
  getTime(value, mw_one->ui->hsM->value());
}

void EditRecord::on_hsM_valueChanged(int value) {
  getTime(mw_one->ui->hsH->value(), value);
}

void EditRecord::on_btnClearDetails_clicked() {
  mw_one->ui->editDetails->clear();
}

void EditRecord::on_editCategory_textChanged(const QString &arg1) {
  if (arg1.length() > 0)
    mw_one->ui->lblCategory->setStyleSheet(lblStyleHighLight);
  else
    mw_one->ui->lblCategory->setStyleSheet(lblStyle);

  QCompleter *completer = new QCompleter(c_list);
  completer->setFilterMode(Qt::MatchContains);
  mw_one->ui->editCategory->setCompleter(completer);
}

void EditRecord::on_editDetails_textChanged() {
  QString arg1 = mw_one->ui->editDetails->toPlainText();
  if (arg1.length() > 0)
    mw_one->ui->lblDetailsType->setStyleSheet(lblStyleHighLight);
  else
    mw_one->ui->lblDetailsType->setStyleSheet(lblStyle);
}

void EditRecord::saveOne() {
  QTreeWidget *tw = (QTreeWidget *)tabData->currentWidget();

  QString name = tw->objectName();

  QString ini_file = iniDir + name + ".ini";
  QSettings Reg(ini_file, QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif

  QString flag;
  QString group = Reg.childGroups().at(0);
  if (group.trimmed().length() == 0)
    flag = "/" + name + "/";
  else
    flag = "/" + group + "/";

  int count = tw->topLevelItemCount();
  Reg.setValue(flag + "TopCount", count);

  if (count == 0) return;

  QTreeWidgetItem *item = tw->currentItem();
  int i = 0;
  if (item->parent() == NULL)
    i = tw->indexOfTopLevelItem(item);
  else
    i = tw->indexOfTopLevelItem(item->parent());

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
        if (isBreak) return;
        Reg.setValue(
            flag + QString::number(i + 1) + "-childTime" + QString::number(j),
            tw->topLevelItem(i)->child(j)->text(0));
        Reg.setValue(
            flag + QString::number(i + 1) + "-childAmount" + QString::number(j),
            tw->topLevelItem(i)->child(j)->text(1));
        Reg.setValue(
            flag + QString::number(i + 1) + "-childDesc" + QString::number(j),
            tw->topLevelItem(i)->child(j)->text(2));
        Reg.setValue(flag + QString::number(i + 1) + "-childDetails" +
                         QString::number(j),
                     tw->topLevelItem(i)->child(j)->text(3));
      }
    }
  }

  Reg.setValue(flag + QString::number(i + 1) + "-childCount", childCount);
}
