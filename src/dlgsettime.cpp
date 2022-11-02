#include "src/dlgsettime.h"

#include <QKeyEvent>

#include "mainwindow.h"
#include "ui_dlgsettime.h"
#include "ui_mainwindow.h"
extern MainWindow* mw_one;
extern QTabWidget* tabData;
extern QString iniFile, iniDir;
extern QRegularExpression regxNumber;
extern bool isBreak, isImport;

dlgList* m_List;

dlgSetTime::dlgSetTime(QWidget* parent)
    : QDialog(parent), ui(new Ui::dlgSetTime) {
  ui->setupUi(this);

  mw_one->set_btnStyle(this);

  m_List = new dlgList(this);
  frameList = NULL;

  this->installEventFilter(this);
  ui->editDesc->installEventFilter(this);
  ui->editDesc->viewport()->installEventFilter(this);

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

  font.setPointSize(21);
  ui->editDesc->setFont(font);

  QValidator* validator =
      new QRegularExpressionValidator(regxNumber, ui->editAmount);
  ui->editAmount->setValidator(validator);
  ui->editAmount->setAttribute(Qt::WA_InputMethodEnabled, false);
  ui->editAmount->setReadOnly(true);

  ui->editDesc->setPlaceholderText(tr("Please enter a category"));

  ui->btnClearAmount->setStyleSheet("border:none");
  ui->btnClearDesc->setStyleSheet("border:none");
  ui->lblCategory->setStyleSheet(ui->lblTitle->styleSheet());
  ui->lblAmount->setStyleSheet(ui->lblTitle->styleSheet());

  ui->hsM->setStyleSheet(ui->hsH->styleSheet());
}

void dlgSetTime::init() {
  mw_one->mydlgMainNotes->m_SetEditText->close();
  mw_one->mydlgMainNotes->m_SetEditText = new dlgSetEditText(this);

  setModal(true);
  setGeometry(mw_one->geometry().x(), mw_one->geometry().y(), mw_one->width(),
              mw_one->height());

  if (mw_one->isAdd) {
    ui->editDesc->setPlainText("");
    ui->editAmount->setText("");
  }
  show();
}

dlgSetTime::~dlgSetTime() { delete ui; }

void dlgSetTime::on_btnBack_clicked() { close(); }

void dlgSetTime::keyReleaseEvent(QKeyEvent* event) { Q_UNUSED(event); }

void dlgSetTime::on_btnOk_clicked() {
  if (!mw_one->isAdd) {
    mw_one->addUndo(tr("Modify Item") + " ( " + mw_one->getTabText() + " ) ");

    mw_one->set_Time();
  } else {
    if (mw_one->isTesting) {
      for (int i = 0; i < 500; i++)
        mw_one->add_Data(mw_one->get_tw(mw_one->ui->tabWidget->currentIndex()),
                         ui->lblTime->text(), ui->editAmount->text().trimmed(),
                         ui->editDesc->toPlainText().trimmed());
    } else
      mw_one->addUndo(tr("Add Item") + " ( " + mw_one->getTabText() + " ) ");

    mw_one->add_Data(mw_one->get_tw(mw_one->ui->tabWidget->currentIndex()),
                     ui->lblTime->text(), ui->editAmount->text().trimmed(),
                     ui->editDesc->toPlainText().trimmed());
  }

  // Save Desc Text
  QString str = ui->editDesc->toPlainText().trimmed();
  int count = m_List->ui->listWidget->count();
  for (int i = 0; i < count; i++) {
    QString str1 = m_List->ui->listWidget->item(i)->text().trimmed();
    if (str == str1) {
      m_List->ui->listWidget->takeItem(i);
      break;
    }
  }

  if (str.length() > 0) {
    QListWidgetItem* item = new QListWidgetItem(str);
    // QFontMetrics fm(this->font());
    // int w = fm.width(str, -1);
    // item->setSizeHint(QSize(w, 35));
    m_List->ui->listWidget->insertItem(0, item);
  }

  close();
}

void dlgSetTime::on_btn7_clicked() { set_Amount("7"); }

void dlgSetTime::on_btn8_clicked() { set_Amount("8"); }

void dlgSetTime::on_btn9_clicked() { set_Amount("9"); }

void dlgSetTime::on_btn4_clicked() { set_Amount("4"); }

void dlgSetTime::on_btn5_clicked() { set_Amount("5"); }

void dlgSetTime::on_btn6_clicked() { set_Amount("6"); }

void dlgSetTime::on_btn1_clicked() { set_Amount("1"); }

void dlgSetTime::on_btn2_clicked() { set_Amount("2"); }

void dlgSetTime::on_btn3_clicked() { set_Amount("3"); }

void dlgSetTime::on_btn0_clicked() { set_Amount("0"); }

void dlgSetTime::on_btnDot_clicked() { set_Amount("."); }

void dlgSetTime::on_btnDel_clicked() {
  QString str = ui->editAmount->text().trimmed();
  str = str.mid(0, str.length() - 1);
  ui->editAmount->setText(str);
}

void dlgSetTime::set_Amount(QString Number) {
  QString str = ui->editAmount->text().trimmed();
  if (str == "0.00") ui->editAmount->setText("");
  if (str.split(".").count() == 2 && str != "0.00") {
    QString str0 = str.split(".").at(1);
    if (str0.length() == 2) return;
  }
  ui->editAmount->setText(str + Number);
}

void dlgSetTime::on_btnCustom_clicked() {
  mw_one->m_widget = new QWidget(this);

  frameList = new QFrame(this);
  frameList->setStyleSheet(
      "QFrame{background-color: rgb(255, 255, "
      "255);border-radius:10px;border:0px solid gray;}");
  QVBoxLayout* vbox = new QVBoxLayout;
  frameList->setLayout(vbox);
  vbox->addWidget(m_List->ui->frame);

  int h = mw_one->height() - 60;
  int w = mw_one->width() - 40;
  frameList->setGeometry(20, (mw_one->height() - h) / 2, w, h);

  init_Desc();
  m_List->ui->listWidget->setFocus();
  m_List->ui->editRename->clear();

  mw_one->showGrayWindows();

  frameList->show();
}

void dlgSetTime::saveCustomDesc() {
  QSettings Reg(iniDir + "desc.ini", QSettings::IniFormat);
  int count = m_List->ui->listWidget->count();

  QStringList list;
  for (int i = 0; i < count; i++) {
    if (isBreak) break;
    list.append(m_List->ui->listWidget->item(i)->text().trimmed());
  }
  // list = QSet<QString>(list.begin(), list.end()).values(); //IOS无法编译通过
  removeDuplicates(&list);
  for (int i = 0; i < list.count(); i++) {
    if (isBreak) break;
    QString str = list.at(i);
    if (str.length() > 0)
      Reg.setValue("/CustomDesc/Item" + QString::number(i), str);
  }
  Reg.setValue("/CustomDesc/Count", list.count());
}

int dlgSetTime::removeDuplicates(QStringList* that) {
  int n = that->size();
  int j = 0;
  QSet<QString> seen;
  seen.reserve(n);
  int setSize = 0;
  for (int i = 0; i < n; ++i) {
    const QString& s = that->at(i);
    seen.insert(s);
    if (setSize == seen.size())  // unchanged size => was already seen
      continue;
    ++setSize;

    // //将不重复项与重复项交换（新，IOS无法编译通过）
    //#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    if (j != i) that->swapItemsAt(i, j);
    //#else
    //    if (j != i) that->swap(i, j);
    //#endif

    ++j;
  }
  if (n != j) that->erase(that->begin() + j, that->end());
  return n - j;
}

void dlgSetTime::init_Desc() {
  // Custom Desc
  QString ini_file;
  if (isImport)
    ini_file = iniFile;
  else
    ini_file = iniDir + "desc.ini";
  QSettings RegDesc(ini_file, QSettings::IniFormat);

  m_List->ui->listWidget->clear();
  m_List->ui->listWidget->setViewMode(QListView::IconMode);
  int descCount = RegDesc.value("/CustomDesc/Count").toInt();
  for (int i = 0; i < descCount; i++) {
    QString str =
        RegDesc.value("/CustomDesc/Item" + QString::number(i)).toString();
    QListWidgetItem* item = new QListWidgetItem(str);
    // item->setSizeHint(
    //    QSize(mw_one->mydlgList->ui->listWidget->width() - 20, 35));
    m_List->ui->listWidget->addItem(item);
  }
}

void dlgSetTime::getTime(int h, int m) {
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

bool dlgSetTime::eventFilter(QObject* watch, QEvent* evn) {
  if (watch == ui->editDesc->viewport()) {
    // mw_one->mydlgMainNotes->getEditPanel(ui->editDesc, evn);
  }

  if (evn->type() == QEvent::KeyPress) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      if (frameList != NULL) {
        frameList->close();
        mw_one->closeGrayWindows();
        frameList = NULL;
        return true;
      } else if (!mw_one->mydlgSetTime->isHidden()) {
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

void dlgSetTime::on_btnClearAmount_clicked() { ui->editAmount->clear(); }

void dlgSetTime::on_btnClearDesc_clicked() { ui->editDesc->clear(); }

void dlgSetTime::on_editAmount_textChanged(const QString& arg1) {
  int count = 0;
  for (int i = 0; i < arg1.count(); i++) {
    if (arg1.mid(i, 1) == ".") count++;
    if (count == 2) {
      QString str0 = arg1;
      QString str = str0.mid(0, str0.length() - 1);
      ui->editAmount->setText(str);
      break;
    }
  }
}

void dlgSetTime::on_hsH_valueChanged(int value) {
  getTime(value, ui->hsM->value());
}

void dlgSetTime::on_hsM_valueChanged(int value) {
  getTime(ui->hsH->value(), value);
}
