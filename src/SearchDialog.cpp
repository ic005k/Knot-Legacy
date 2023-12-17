#include "SearchDialog.h"

#include <QKeyEvent>

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "ui_SearchDialog.h"

extern MainWindow* mw_one;
extern QTabWidget* tabData;
extern QString iniDir, searchStr;

QStringList resultsList;

SearchDialog::SearchDialog(QWidget* parent)
    : QDialog(parent), ui(new Ui::SearchDialog) {
  ui->setupUi(this);
  mw_one->set_btnStyle(this);
  setModal(true);
  this->installEventFilter(this);

  setWindowTitle(tr("Search"));
}

void SearchDialog::init() {
  int w = mw_one->geometry().width();
  int h = mw_one->geometry().height();
  setFixedWidth(w);
  setFixedHeight(h);
  setGeometry(mw_one->geometry().x(), mw_one->geometry().y(), w, h);

  show();
}

SearchDialog::~SearchDialog() { delete ui; }

void SearchDialog::addItem(QString text0, QString text1, QString text2,
                           QString text3, int itemH) {
  QQuickItem* root = mw_one->ui->qwSearch->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "addItem", Q_ARG(QVariant, text0),
                            Q_ARG(QVariant, text1), Q_ARG(QVariant, text2),
                            Q_ARG(QVariant, text3), Q_ARG(QVariant, itemH));
}

void SearchDialog::delItem(int index) {
  QQuickItem* root = mw_one->ui->qwSearch->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "delItem", Q_ARG(QVariant, index));
}

int SearchDialog::getCount() {
  QQuickItem* root = mw_one->ui->qwSearch->rootObject();
  QVariant itemCount;
  QMetaObject::invokeMethod((QObject*)root, "getItemCount",
                            Q_RETURN_ARG(QVariant, itemCount));
  return itemCount.toInt();
}

void SearchDialog::clearAll() {
  int count = getCount();
  for (int i = 0; i < count; i++) {
    delItem(0);
  }
}

void SearchDialog::setCurrentIndex(int index) {
  QQuickItem* root = mw_one->ui->qwSearch->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "setCurrentItem",
                            Q_ARG(QVariant, index));
}

void SearchDialog::addItemBakList(QQuickWidget* qw, QString text0,
                                  QString text1, QString text2, QString text3,
                                  int itemH) {
  QQuickItem* root = qw->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "addItem", Q_ARG(QVariant, text0),
                            Q_ARG(QVariant, text1), Q_ARG(QVariant, text2),
                            Q_ARG(QVariant, text3), Q_ARG(QVariant, itemH));
}

void SearchDialog::insertItem(QQuickWidget* qw, QString text0, QString text1,
                              QString text2, QString text3, int curIndex) {
  QQuickItem* root = qw->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "insertItem",
                            Q_ARG(QVariant, text0), Q_ARG(QVariant, text1),
                            Q_ARG(QVariant, text2), Q_ARG(QVariant, text3),
                            Q_ARG(QVariant, curIndex));
}

void SearchDialog::delItemBakList(QQuickWidget* qw, int index) {
  QQuickItem* root = qw->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "delItem", Q_ARG(QVariant, index));
}

int SearchDialog::getCountBakList(QQuickWidget* qw) {
  QQuickItem* root = qw->rootObject();
  QVariant itemCount;
  QMetaObject::invokeMethod((QObject*)root, "getItemCount",
                            Q_RETURN_ARG(QVariant, itemCount));
  return itemCount.toInt();
}

void SearchDialog::clearAllBakList(QQuickWidget* qw) {
  int count = getCountBakList(qw);
  for (int i = 0; i < count; i++) {
    delItemBakList(qw, 0);
  }
}

void SearchDialog::setCurrentIndexBakList(QQuickWidget* qw, int index) {
  QQuickItem* root = qw->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "setCurrentItem",
                            Q_ARG(QVariant, index));
}

int SearchDialog::getCurrentIndexBakList(QQuickWidget* qw) {
  QQuickItem* root = qw->rootObject();
  QVariant itemIndex;
  QMetaObject::invokeMethod((QObject*)root, "getCurrentIndex",
                            Q_RETURN_ARG(QVariant, itemIndex));
  return itemIndex.toInt();
}

void SearchDialog::modifyItemText0(QQuickWidget* qw, int index,
                                   QString strText) {
  QQuickItem* root = qw->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "modifyItemText0",
                            Q_ARG(QVariant, index), Q_ARG(QVariant, strText));
}

void SearchDialog::modifyItemText2(QQuickWidget* qw, int index,
                                   QString strText) {
  QQuickItem* root = qw->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "modifyItemText2",
                            Q_ARG(QVariant, index), Q_ARG(QVariant, strText));
}

QString SearchDialog::getText0(QQuickWidget* qw, int index) {
  QQuickItem* root = qw->rootObject();
  QVariant item;
  QMetaObject::invokeMethod((QObject*)root, "getText0",
                            Q_RETURN_ARG(QVariant, item),
                            Q_ARG(QVariant, index));
  return item.toString();
}

QString SearchDialog::getText3(QQuickWidget* qw, int index) {
  QQuickItem* root = qw->rootObject();
  QVariant item;
  QMetaObject::invokeMethod((QObject*)root, "getText3",
                            Q_RETURN_ARG(QVariant, item),
                            Q_ARG(QVariant, index));
  return item.toString();
}

bool SearchDialog::eventFilter(QObject* watchDlgSearch, QEvent* evn) {
  if (evn->type() == QEvent::KeyRelease) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      close();
      return true;
    }
  }

  return QWidget::eventFilter(watchDlgSearch, evn);
}

void SearchDialog::startSearch() {
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

void SearchDialog::initSearchResults() {
  // qDebug() << resultsList;

  clearAll();
  int count = resultsList.count();

  mw_one->ui->lblSearchResult->setText(tr("Results") + " : " +
                                       QString::number(count));
  if (count == 0) return;

  generateData(count);

  setCurrentIndex(0);
}

void SearchDialog::generateData(int count) {
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

void SearchDialog::setCellText(int row, int column, QString str,
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

void SearchDialog::clickNoteBook() {
  clearAllBakList(mw_one->ui->qwNoteList);
  int index = getCurrentIndexBakList(mw_one->ui->qwNoteBook);
  QTreeWidgetItem* topItem = mw_one->m_NotesList->tw->topLevelItem(index);
  int child_count = topItem->childCount();
  for (int i = 0; i < child_count; i++) {
    QString text0 = topItem->child(i)->text(0);
    QString text3 = topItem->child(i)->text(1);
    addItemBakList(mw_one->ui->qwNoteList, text0, "", "", text3, 0);
  }

  mw_one->m_NotesList->setNoteLabel();
}

void SearchDialog::clickNoteList() {
  int index = getCurrentIndexBakList(mw_one->ui->qwNoteList);
  QString noteFile = iniDir + getText3(mw_one->ui->qwNoteList, index);
  QString noteName = getText0(mw_one->ui->qwNoteList, index);
  mw_one->mydlgMainNotes->MD2Html(noteFile);
  mw_one->mydlgMainNotes->loadMemoQML();
  mw_one->ui->lblNoteName->setText(noteName);

  mw_one->m_NotesList->currentMDFile = noteFile;

  saveCurNoteIndex();
}

void SearchDialog::saveCurNoteIndex() {
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

QList<int> SearchDialog::getCurNoteIndex() {
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

void SearchDialog::showNoteBookMenu(int x, int y) {
  QMenu* mainMenu = new QMenu(this);
  mw_one->m_NotesList->init_NoteBookMenu(mainMenu);

  QPoint pos(mw_one->geometry().x() + x, mw_one->geometry().y() + y);
  mainMenu->exec(pos);
}

void SearchDialog::showNotsListMenu(int x, int y) {
  QMenu* mainMenu = new QMenu(this);
  mw_one->m_NotesList->init_NotesListMenu(mainMenu);

  QPoint pos(mw_one->geometry().x() + x, mw_one->geometry().y() + y);
  mainMenu->exec(pos);
}
