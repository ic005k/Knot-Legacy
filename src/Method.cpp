#include "Method.h"

#include <QKeyEvent>

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "ui_Method.h"

extern MainWindow* mw_one;
extern QTabWidget* tabData;
extern QString iniDir, searchStr;
extern dlgList* m_List;

QStringList resultsList;

Method::Method(QWidget* parent) : QDialog(parent), ui(new Ui::Method) {
  ui->setupUi(this);
  mw_one->set_btnStyle(this);
  setModal(true);
  this->installEventFilter(this);

  setWindowTitle(tr("Search"));
}

void Method::init() {
  int w = mw_one->geometry().width();
  int h = mw_one->geometry().height();
  setFixedWidth(w);
  setFixedHeight(h);
  setGeometry(mw_one->geometry().x(), mw_one->geometry().y(), w, h);

  show();
}

Method::~Method() { delete ui; }

void Method::addItem(QString text0, QString text1, QString text2, QString text3,
                     int itemH) {
  QQuickItem* root = mw_one->ui->qwSearch->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "addItem", Q_ARG(QVariant, text0),
                            Q_ARG(QVariant, text1), Q_ARG(QVariant, text2),
                            Q_ARG(QVariant, text3), Q_ARG(QVariant, itemH));
}

void Method::delItem(int index) {
  QQuickItem* root = mw_one->ui->qwSearch->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "delItem", Q_ARG(QVariant, index));
}

int Method::getCount() {
  QQuickItem* root = mw_one->ui->qwSearch->rootObject();
  QVariant itemCount;
  QMetaObject::invokeMethod((QObject*)root, "getItemCount",
                            Q_RETURN_ARG(QVariant, itemCount));
  return itemCount.toInt();
}

void Method::clearAll() {
  int count = getCount();
  for (int i = 0; i < count; i++) {
    delItem(0);
  }
}

void Method::setCurrentIndex(int index) {
  QQuickItem* root = mw_one->ui->qwSearch->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "setCurrentItem",
                            Q_ARG(QVariant, index));
}

void Method::addItemBakList(QQuickWidget* qw, QString text0, QString text1,
                            QString text2, QString text3, int itemH) {
  QQuickItem* root = qw->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "addItem", Q_ARG(QVariant, text0),
                            Q_ARG(QVariant, text1), Q_ARG(QVariant, text2),
                            Q_ARG(QVariant, text3), Q_ARG(QVariant, itemH));
}

void Method::insertItem(QQuickWidget* qw, QString text0, QString text1,
                        QString text2, QString text3, int curIndex) {
  QQuickItem* root = qw->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "insertItem",
                            Q_ARG(QVariant, text0), Q_ARG(QVariant, text1),
                            Q_ARG(QVariant, text2), Q_ARG(QVariant, text3),
                            Q_ARG(QVariant, curIndex));
}

void Method::delItemBakList(QQuickWidget* qw, int index) {
  QQuickItem* root = qw->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "delItem", Q_ARG(QVariant, index));
}

int Method::getCountBakList(QQuickWidget* qw) {
  QQuickItem* root = qw->rootObject();
  QVariant itemCount;
  QMetaObject::invokeMethod((QObject*)root, "getItemCount",
                            Q_RETURN_ARG(QVariant, itemCount));
  return itemCount.toInt();
}

void Method::clearAllBakList(QQuickWidget* qw) {
  int count = getCountBakList(qw);
  for (int i = 0; i < count; i++) {
    delItemBakList(qw, 0);
  }
}

void Method::gotoEnd(QQuickWidget* qw) {
  QQuickItem* root = qw->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "gotoEnd");
}

void Method::setCurrentIndexBakList(QQuickWidget* qw, int index) {
  QQuickItem* root = qw->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "setCurrentItem",
                            Q_ARG(QVariant, index));
}

int Method::getCurrentIndexBakList(QQuickWidget* qw) {
  QQuickItem* root = qw->rootObject();
  QVariant itemIndex;
  QMetaObject::invokeMethod((QObject*)root, "getCurrentIndex",
                            Q_RETURN_ARG(QVariant, itemIndex));
  return itemIndex.toInt();
}

void Method::modifyItemText0(QQuickWidget* qw, int index, QString strText) {
  QQuickItem* root = qw->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "modifyItemText0",
                            Q_ARG(QVariant, index), Q_ARG(QVariant, strText));
}

void Method::modifyItemText2(QQuickWidget* qw, int index, QString strText) {
  QQuickItem* root = qw->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "modifyItemText2",
                            Q_ARG(QVariant, index), Q_ARG(QVariant, strText));
}

QString Method::getText0(QQuickWidget* qw, int index) {
  QQuickItem* root = qw->rootObject();
  QVariant item;
  QMetaObject::invokeMethod((QObject*)root, "getText0",
                            Q_RETURN_ARG(QVariant, item),
                            Q_ARG(QVariant, index));
  return item.toString();
}

QString Method::getText3(QQuickWidget* qw, int index) {
  QQuickItem* root = qw->rootObject();
  QVariant item;
  QMetaObject::invokeMethod((QObject*)root, "getText3",
                            Q_RETURN_ARG(QVariant, item),
                            Q_ARG(QVariant, index));
  return item.toString();
}

bool Method::eventFilter(QObject* watchDlgSearch, QEvent* evn) {
  if (evn->type() == QEvent::KeyRelease) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      close();
      return true;
    }
  }

  return QWidget::eventFilter(watchDlgSearch, evn);
}

void Method::startSearch() {
  resultsList.clear();
  int tabCount = tabData->count();
  for (int j = 0; j < tabCount; j++) {
    QTreeWidget* tw = mw_one->get_tw(j);
    QString tabStr = tabData->tabText(j);

    for (int i = 0; i < tw->topLevelItemCount(); i++) {
      QString strYear, strMonthDay;
      strYear = tw->topLevelItem(i)->text(3);
      strMonthDay = tw->topLevelItem(i)->text(0);
      QString weeks = strMonthDay.split(" ").at(0);
      QString day =
          strMonthDay.split(" ").at(1) + " " + strMonthDay.split(" ").at(2);

      QTreeWidgetItem* topItem;
      topItem = tw->topLevelItem(i);
      int childCount = topItem->childCount();
      for (int j = 0; j < childCount; j++) {
        QString txt1, txt2, txt3;
        QTreeWidgetItem* childItem = topItem->child(j);
        txt1 = childItem->text(1);
        txt2 = childItem->text(2);
        txt3 = childItem->text(3);
        if (txt1.contains(searchStr) || txt2.contains(searchStr) ||
            txt3.contains(searchStr)) {
          QString str0, str1, str2, str3;
          str0 = tabStr + "\n\n" + strYear + " " + day + " " + weeks + " " +
                 childItem->text(0).split(".").at(1).trimmed();
          str1 = childItem->text(1);
          str2 = childItem->text(2);
          str3 = childItem->text(3);

          resultsList.append(str0 + "=|=" + str1 + "=|=" + str2 + "=|=" + str3);
        }
      }
    }
  }
}

void Method::initSearchResults() {
  // qDebug() << resultsList;

  clearAll();
  int count = resultsList.count();

  mw_one->ui->lblSearchResult->setText(tr("Results") + " : " +
                                       QString::number(count));
  if (count == 0) return;

  generateData(count);

  setCurrentIndex(0);
}

void Method::generateData(int count) {
  QFontMetrics fontMetrics(font());
  int nFontHeight = fontMetrics.height();
  int line_count;

  for (int i = 0; i < count; i++) {
    QStringList list = resultsList.at(i).split("=|=");
    QString str0, str1, str2, str3;
    str0 = list.at(0);
    str1 = list.at(1);
    str2 = list.at(2);
    str3 = list.at(3);

    // QString a0("<span style=\"color: white;background: red;\">");
    // QString a1("</span>");

    QString a0 = "<font color=\" red \"><b>";
    QString a1 = "</b></font>";

    if (str1.contains(searchStr)) {
      str1 = str1.replace(searchStr, a0 + searchStr + a1);
    }

    if (str2.contains(searchStr)) {
      str2 = str2.replace(searchStr, a0 + searchStr + a1);
    }

    if (str3.contains(searchStr)) {
      str3 = str3.replace(searchStr, a0 + searchStr + a1);
    }

    line_count = 4;
    QString text1, text2, text3;
    if (str1.trimmed().length() > 0) {
      text1 = tr("Amount") + " : " + str1;
      line_count++;
    }
    if (str2.trimmed().length() > 0) {
      text2 = tr("Category") + " : " + str2;
      line_count++;
    }
    if (str3.trimmed().length() > 0) {
      text3 = tr("Details") + " : " + str3;
      line_count++;
    }

    addItem(str0, text1, text2, text3, nFontHeight * (0));
  }
}

void Method::setCellText(int row, int column, QString str,
                         QTableWidget* table) {
  QString a0("<span style=\"color: white;background: red;\">");
  QString a1("</span>");

  if (str.contains(searchStr)) {
    str = str.replace(searchStr, a0 + searchStr + a1);

    QLabel* lbl = new QLabel();
    lbl->adjustSize();
    lbl->setWordWrap(true);
    lbl->setText(str);
    table->setCellWidget(row, column, lbl);
  } else
    table->setItem(row, column, new QTableWidgetItem(str));
}

void Method::clickNoteBook() {
  clearAllBakList(mw_one->ui->qwNoteList);
  int index = getCurrentIndexBakList(mw_one->ui->qwNoteBook);
  QTreeWidgetItem* topItem = mw_one->m_NotesList->tw->topLevelItem(index);
  int child_count = topItem->childCount();
  for (int i = 0; i < child_count; i++) {
    QString text0 = topItem->child(i)->text(0);
    QString text3 = topItem->child(i)->text(1);
    addItemBakList(mw_one->ui->qwNoteList, text0, "", "", text3, 0);
  }

  mw_one->m_NotesList->setNotesListCurrentIndex(-1);
  mw_one->m_NotesList->setNoteLabel();
}

void Method::clickNoteList() {
  mw_one->m_Notes->saveQMLVPos();

  int index = getCurrentIndexBakList(mw_one->ui->qwNoteList);
  mw_one->m_NotesList->currentMDFile =
      iniDir + getText3(mw_one->ui->qwNoteList, index);
  QString noteName = getText0(mw_one->ui->qwNoteList, index);
  mw_one->m_Notes->MD2Html(mw_one->m_NotesList->currentMDFile);
  mw_one->m_Notes->loadMemoQML();
  mw_one->ui->lblNoteName->setText(noteName);

  saveCurNoteIndex();
}

void Method::clickMainDate() {
  QTreeWidget* tw = mw_one->get_tw(mw_one->ui->tabWidget->currentIndex());
  int maindateIndex = getCurrentIndexBakList(mw_one->ui->qwMainDate);
  int maindateCount = getCountBakList(mw_one->ui->qwMainDate);
  int topIndex = tw->topLevelItemCount() - maindateCount + maindateIndex;

  if (topIndex < 0) return;

  clearAllBakList(mw_one->ui->qwMainEvent);
  QTreeWidgetItem* topItem = tw->topLevelItem(topIndex);
  int childCount = topItem->childCount();
  QString text0, text1, text2, text3;
  int nullrows;
  for (int j = 0; j < childCount; j++) {
    QTreeWidgetItem* childItem = topItem->child(j);
    text0 = childItem->text(0);
    text1 = childItem->text(1);
    text2 = childItem->text(2);
    text3 = childItem->text(3);

    nullrows = 1;

    if (text1.length() > 0) {
      text1 = tr("Amount") + " : " + text1;
      nullrows++;
    }

    if (text2.length() > 0) {
      text2 = tr("Category") + " : " + text2;
      nullrows++;
    }

    if (text3.length() > 0) {
      text3 = tr("Details") + " : " + text3;
      nullrows++;
    }

    addItemBakList(mw_one->ui->qwMainEvent, text0, text1, text2, text3, 0);
  }

  gotoEnd(mw_one->ui->qwMainEvent);
  int count = getCountBakList(mw_one->ui->qwMainEvent);
  setCurrentIndexBakList(mw_one->ui->qwMainEvent, count - 1);
}

void Method::clickMainDateData() {
  QTreeWidget* tw = mw_one->get_tw(mw_one->ui->tabWidget->currentIndex());
  int maindateIndex = getCurrentIndexBakList(mw_one->ui->qwMainDate);
  int maindateCount = getCountBakList(mw_one->ui->qwMainDate);
  int topIndex = tw->topLevelItemCount() - maindateCount + maindateIndex;

  if (topIndex < 0) return;

  tw->setCurrentItem(tw->topLevelItem(topIndex));

  mw_one->on_twItemClicked();
}

void Method::clickMainEventData() {
  QTreeWidget* tw = mw_one->get_tw(mw_one->ui->tabWidget->currentIndex());
  int maindateIndex = getCurrentIndexBakList(mw_one->ui->qwMainDate);
  int maindateCount = getCountBakList(mw_one->ui->qwMainDate);
  int topIndex = tw->topLevelItemCount() - maindateCount + maindateIndex;
  int childIndex = getCurrentIndexBakList(mw_one->ui->qwMainEvent);
  tw->setCurrentItem(tw->topLevelItem(topIndex)->child(childIndex));

  if (topIndex < 0) return;
  if (childIndex < 0) return;

  mw_one->on_twItemClicked();
}

void Method::reeditMainEventData() {
  QTreeWidget* tw = mw_one->get_tw(mw_one->ui->tabWidget->currentIndex());
  int maindateIndex = getCurrentIndexBakList(mw_one->ui->qwMainDate);
  int maindateCount = getCountBakList(mw_one->ui->qwMainDate);
  int topIndex = tw->topLevelItemCount() - maindateCount + maindateIndex;
  int childIndex = getCurrentIndexBakList(mw_one->ui->qwMainEvent);

  if (topIndex < 0) return;
  if (childIndex < 0) return;

  tw->setCurrentItem(tw->topLevelItem(topIndex)->child(childIndex));
  mw_one->on_twItemDoubleClicked();
}

void Method::saveCurNoteIndex() {
  QSettings Reg(iniDir + "curmd.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif

  Reg.setValue("/MainNotes/notebookIndex",
               getCurrentIndexBakList(mw_one->ui->qwNoteBook));
  Reg.setValue("/MainNotes/noteIndex",
               getCurrentIndexBakList(mw_one->ui->qwNoteList));
  Reg.setValue("/MainNotes/currentItem", mw_one->m_NotesList->currentMDFile);
  Reg.setValue("/MainNotes/NoteName", mw_one->ui->lblNoteName->text());
}

QList<int> Method::getCurNoteIndex() {
  QList<int> indexList;
  QSettings Reg(iniDir + "curmd.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif

  int notebookIndex = Reg.value("/MainNotes/notebookIndex", 0).toInt();
  int noteIndex = Reg.value("/MainNotes/noteIndex", 0).toInt();

  indexList.append(notebookIndex);
  indexList.append(noteIndex);

  return indexList;
}

void Method::showNoteBookMenu(int x, int y) {
  QMenu* mainMenu = new QMenu(this);
  mw_one->m_NotesList->init_NoteBookMenu(mainMenu);

  QPoint pos(mw_one->geometry().x() + x, mw_one->geometry().y() + y);
  mainMenu->exec(pos);
}

void Method::showNotsListMenu(int x, int y) {
  QMenu* mainMenu = new QMenu(this);
  mw_one->m_NotesList->init_NotesListMenu(mainMenu);

  QPoint pos(mw_one->geometry().x() + x, mw_one->geometry().y() + y);
  mainMenu->exec(pos);
}

void Method::setTypeRenameText() {
  int index = getCurrentIndexBakList(mw_one->ui->qwCategory);
  QString str = getText0(mw_one->ui->qwCategory, index);
  mw_one->ui->editRenameType->setText(str);
}

void Method::okType() {
  int index = getCurrentIndexBakList(mw_one->ui->qwCategory);
  m_List->ui->listWidget->setCurrentRow(index);
  QListWidgetItem* item = m_List->ui->listWidget->currentItem();
  m_List->on_listWidget_itemDoubleClicked(item);
  m_List->on_btnCancel_clicked();
}
