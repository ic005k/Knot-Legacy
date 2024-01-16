#include "Method.h"

#include <QKeyEvent>

#include "MainWindow.h"
#include "ui_MainWindow.h"

extern MainWindow* mw_one;
extern QTabWidget* tabData;
extern QString iniDir, searchStr, currentMDFile;
extern CategoryList* m_CategoryList;
extern bool isEpub, isText, isPDF, loading, isDark;
extern int iPage, sPos, totallines, baseLines, htmlIndex, s_y1, s_m1, s_d1,
    s_y2, s_m2, s_d2, fontSize;
extern QStringList readTextList, htmlFiles, listCategory;

QStringList resultsList;

Method::Method(QWidget* parent) : QDialog(parent) {
  mw_one->set_btnStyle(this);

  this->installEventFilter(this);

  m_widget = new QWidget(mw_one);
  m_widget->close();
}

void Method::init() {
  int w = mw_one->geometry().width();
  int h = mw_one->geometry().height();
  setFixedWidth(w);
  setFixedHeight(h);
  setGeometry(mw_one->geometry().x(), mw_one->geometry().y(), w, h);

  show();
}

Method::~Method() {}

QString Method::getRealPathFile(QString strFile) {
  strFile = mw_one->m_Reader->getUriRealPath(strFile);
  QString realFile = strFile;

  int aver = mw_one->m_Remarks->getAndroidVer();
  // Android7.0及以上
  if (aver >= 24) {
  } else {
    // Android6.0
    QStringList list = strFile.split(":");
    int count = list.count();
    QString r_file = list.at(count - 1);
    QString driver;
    for (int i = 0; i < 5; i++) {
      driver = "/storage/emulated/" + QString::number(i) + "/";
      realFile = driver + r_file;
      if (QFile(realFile).exists()) break;
    }
  }

  return realFile;
}

void Method::showGrayWindows() {
  m_widget->resize(mw_one->width(), mw_one->height());
  m_widget->move(0, 0);
  m_widget->setStyleSheet("background-color:rgba(0, 0, 0,35%);");

  /*QPropertyAnimation *m_pAnimation = new QPropertyAnimation();
  m_pAnimation->setTargetObject(m_widget);
  m_pAnimation->setDuration(50);
  QGraphicsOpacityEffect *m_pOpacity = new QGraphicsOpacityEffect();
  m_widget->setGraphicsEffect(m_pOpacity);
  m_pOpacity->setOpacity(1);
  m_pAnimation->setTargetObject(m_pOpacity);
  m_pAnimation->setPropertyName("opacity");
  m_pAnimation->setStartValue(0);
  m_pAnimation->setEndValue(1);
  m_pAnimation->start();*/

  m_widget->show();
}

void Method::closeGrayWindows() { m_widget->close(); }

QInputDialog* Method::inputDialog(QString windowsTitle, QString lblEdit,
                                  QString defaultValue) {
  QInputDialog* idlg = new QInputDialog(this);
  idlg->hide();
  idlg->setWindowFlag(Qt::FramelessWindowHint);
  QString style1 = "QDialog{border-radius:px;border:2px solid darkred;}";
  idlg->setStyleSheet(style1);
  idlg->setOkButtonText(tr("Ok"));
  idlg->setCancelButtonText(tr("Cancel"));
  idlg->setContentsMargins(10, 10, 10, 10);
  idlg->setWindowTitle(windowsTitle);
  idlg->setTextValue(defaultValue);
  idlg->setLabelText(lblEdit);
  showGrayWindows();
  idlg->show();
  idlg->setGeometry(
      mw_one->geometry().x() + (mw_one->geometry().width() - idlg->width()) / 2,
      150, idlg->width(), idlg->height());

  connect(idlg, &QDialog::rejected, [=]() mutable { closeGrayWindows(); });
  connect(idlg, &QDialog::accepted, [=]() mutable { closeGrayWindows(); });

  return idlg;
}

void Method::closeKeyboard() {
  if (mw_one->pAndroidKeyboard->isVisible()) {
    mw_one->pAndroidKeyboard->hide();
  }

  if (!mw_one->m_Notes->m_TextSelector->isHidden()) {
    mw_one->m_Notes->m_TextSelector->on_btnClose_clicked();
  }
}

int Method::getFontHeight() {
  QFontMetrics fontMetrics(font());
  int nFontHeight = fontMetrics.height();
  return nFontHeight;
}

void Method::addItem(QString text_tab, QString text0, QString text1,
                     QString text2, QString text3, int itemH) {
  QQuickItem* root = mw_one->ui->qwSearch->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "addItem",
                            Q_ARG(QVariant, text_tab), Q_ARG(QVariant, text0),
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

void Method::addItemToQW(QQuickWidget* qw, QString text0, QString text1,
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

int Method::getCountFromQW(QQuickWidget* qw) {
  QQuickItem* root = qw->rootObject();
  QVariant itemCount;
  QMetaObject::invokeMethod((QObject*)root, "getItemCount",
                            Q_RETURN_ARG(QVariant, itemCount));
  return itemCount.toInt();
}

void Method::clearAllBakList(QQuickWidget* qw) {
  int count = getCountFromQW(qw);
  for (int i = 0; i < count; i++) {
    delItemBakList(qw, 0);
  }
}

void Method::gotoEnd(QQuickWidget* qw) {
  QQuickItem* root = qw->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "gotoEnd");
}

void Method::setCurrentIndexFromQW(QQuickWidget* qw, int index) {
  QQuickItem* root = qw->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "setCurrentItem",
                            Q_ARG(QVariant, index));
}

int Method::getCurrentIndexFromQW(QQuickWidget* qw) {
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

void Method::modifyItemText3(QQuickWidget* qw, int index, QString strText) {
  QQuickItem* root = qw->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "modifyItemText3",
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

  // QString a0("<span style=\"color: white;background: red;\">");
  // QString a1("</span>");

  QString a0 = "<font color=\" red \"><b>";
  QString a1 = "</b></font>";

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
        QString txt0, txt1, txt2, txt3;
        QTreeWidgetItem* childItem = topItem->child(j);

        QString strTime = childItem->text(0);
        if (strTime.split(".").count() == 2) {
          txt0 = strYear + " " + day + " " + weeks + " " +
                 strTime.split(".").at(1).trimmed();
        }

        txt1 = childItem->text(1);
        txt2 = childItem->text(2);
        txt3 = childItem->text(3);

        QStringList list;
        bool isYes = false;
        if (searchStr.contains("&")) {
          list = searchStr.split("&");
          bool is0, is1, is2, is3;
          is0 = false;
          is1 = false;
          is2 = false;
          is3 = false;
          for (int n = 0; n < list.count(); n++) {
            QString str = list.at(n);
            str = str.trimmed();

            if (str.length() > 0) {
              if (strYear.contains(str) || day.contains(str) ||
                  weeks.contains(str)) {
                is0 = true;
                txt0 = txt0.replace(str, a0 + str + a1);
              }

              if (txt1.contains(str)) {
                is1 = true;
                txt1 = txt1.replace(str, a0 + str + a1);
              }
              if (txt2.contains(str)) {
                is2 = true;
                txt2 = txt2.replace(str, a0 + str + a1);
              }
              if (txt3.contains(str)) {
                is3 = true;
                txt3 = txt3.replace(str, a0 + str + a1);
              }
            }
          }

          if (list.count() == 2) {
            if (is0 && is1) isYes = true;
            if (is0 && is2) isYes = true;
            if (is0 && is3) isYes = true;
            if (is1 && is2) isYes = true;
            if (is1 && is3) isYes = true;
            if (is2 && is3) isYes = true;
          }

          if (list.count() == 3) {
            if (is0 && is1 && is2) isYes = true;
            if (is0 && is1 && is3) isYes = true;
            if (is0 && is2 && is3) isYes = true;
            if (is1 && is2 && is3) isYes = true;
          }

          if (list.count() >= 4) {
            if (is0 && is1 && is2 && is3) isYes = true;
          }

          QString s_total = txt0 + txt1 + txt2 + txt3;
          int n_count = 0;
          for (int x = 0; x < list.count(); x++) {
            QString str = list.at(x);
            if (str.length() > 0) {
              if (s_total.contains(str)) {
                n_count++;
              }
            }
          }

          if (isYes) {
            if (n_count < list.count()) isYes = false;
          }

        } else {
          if (txt1.contains(searchStr) || txt2.contains(searchStr) ||
              txt3.contains(searchStr)) {
            isYes = true;

            if (txt1.contains(searchStr)) {
              txt1 = txt1.replace(searchStr, a0 + searchStr + a1);
            }

            if (txt2.contains(searchStr)) {
              txt2 = txt2.replace(searchStr, a0 + searchStr + a1);
            }

            if (txt3.contains(searchStr)) {
              txt3 = txt3.replace(searchStr, a0 + searchStr + a1);
            }
          }
        }

        if (isYes) {
          resultsList.append(tabStr + "=|=" + txt0 + "=|=" + txt1 +
                             "=|=" + txt2 + "=|=" + txt3);
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
    QString str_tab, str0, str1, str2, str3;
    str_tab = list.at(0);
    str0 = list.at(1);
    str1 = list.at(2);
    str2 = list.at(3);
    str3 = list.at(4);

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

    addItem(str_tab, str0, text1, text2, text3, nFontHeight * (0));
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
  int index = getCurrentIndexFromQW(mw_one->ui->qwNoteBook);
  QTreeWidgetItem* topItem = mw_one->m_NotesList->tw->topLevelItem(index);
  int child_count = topItem->childCount();
  for (int i = 0; i < child_count; i++) {
    QString text0 = topItem->child(i)->text(0);
    QString text3 = topItem->child(i)->text(1);
    addItemToQW(mw_one->ui->qwNoteList, text0, "", "", text3, 0);
  }

  mw_one->m_NotesList->setNotesListCurrentIndex(-1);
  mw_one->m_NotesList->setNoteLabel();
}

void Method::clickNoteList() {
  mw_one->m_Notes->saveQMLVPos();

  int index = getCurrentIndexFromQW(mw_one->ui->qwNoteList);
  currentMDFile = iniDir + getText3(mw_one->ui->qwNoteList, index);
  QString noteName = getText0(mw_one->ui->qwNoteList, index);
  mw_one->m_Notes->MD2Html(currentMDFile);
  mw_one->m_Notes->loadMemoQML();
  mw_one->ui->lblNoteName->setText(noteName);

  saveCurNoteIndex();
}

void Method::clickMainDate() {
  bool isAniEffects;
  if (mw_one->isDelItem || mw_one->isEditItem)
    isAniEffects = false;
  else
    isAniEffects = mw_one->m_Preferences->ui->chkAniEffects->isChecked();
  mw_one->isDelItem = false;
  mw_one->isEditItem = false;

  mw_one->ui->qwMainEvent->rootContext()->setContextProperty("isAniEffects",
                                                             isAniEffects);

  mw_one->ui->qwMainEvent->rootContext()->setContextProperty(
      "maineventWidth", mw_one->ui->qwMainEvent->width());

  QTreeWidget* tw = mw_one->get_tw(mw_one->ui->tabWidget->currentIndex());
  int maindateIndex = getCurrentIndexFromQW(mw_one->ui->qwMainDate);
  int maindateCount = getCountFromQW(mw_one->ui->qwMainDate);
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
      // text1 = tr("Amount") + " : " + text1;
      nullrows++;
    }

    if (text2.length() > 0) {
      // text2 = tr("Category") + " : " + text2;
      nullrows++;
    }

    if (text3.length() > 0) {
      // text3 = tr("Details") + " : " + text3;
      nullrows++;
    }

    addItemToQW(mw_one->ui->qwMainEvent, text0, text1, text2, text3, 0);
  }

  gotoEnd(mw_one->ui->qwMainEvent);
  int count = getCountFromQW(mw_one->ui->qwMainEvent);
  setCurrentIndexFromQW(mw_one->ui->qwMainEvent, count - 1);

  setMainTabCurrentIndex();
}

void Method::setMainTabCurrentIndex() {
  int tabIndex = tabData->currentIndex();
  if (tabIndex > 0) {
    mw_one->setCurrentIndex(tabIndex - 1);
    mw_one->setCurrentIndex(tabIndex);
  }
  if (tabIndex == 0 && mw_one->getCount() > 1) {
    mw_one->setCurrentIndex(tabIndex + 1);
    mw_one->setCurrentIndex(tabIndex);
  }
}

void Method::clickMainDateData() {
  QTreeWidget* tw = mw_one->get_tw(mw_one->ui->tabWidget->currentIndex());
  int maindateIndex = getCurrentIndexFromQW(mw_one->ui->qwMainDate);
  int maindateCount = getCountFromQW(mw_one->ui->qwMainDate);
  int topIndex = tw->topLevelItemCount() - maindateCount + maindateIndex;

  if (topIndex < 0) return;

  tw->setCurrentItem(tw->topLevelItem(topIndex));

  mw_one->on_twItemClicked();
}

void Method::clickMainEventData() {
  QTreeWidget* tw = mw_one->get_tw(mw_one->ui->tabWidget->currentIndex());
  int maindateIndex = getCurrentIndexFromQW(mw_one->ui->qwMainDate);
  int maindateCount = getCountFromQW(mw_one->ui->qwMainDate);
  int topIndex = tw->topLevelItemCount() - maindateCount + maindateIndex;
  int childIndex = getCurrentIndexFromQW(mw_one->ui->qwMainEvent);
  tw->setCurrentItem(tw->topLevelItem(topIndex)->child(childIndex));

  if (topIndex < 0) return;
  if (childIndex < 0) return;

  mw_one->on_twItemClicked();

  setMainTabCurrentIndex();
}

void Method::reeditMainEventData() {
  QTreeWidget* tw = mw_one->get_tw(mw_one->ui->tabWidget->currentIndex());
  int maindateIndex = getCurrentIndexFromQW(mw_one->ui->qwMainDate);
  int maindateCount = getCountFromQW(mw_one->ui->qwMainDate);
  int topIndex = tw->topLevelItemCount() - maindateCount + maindateIndex;
  int childIndex = getCurrentIndexFromQW(mw_one->ui->qwMainEvent);

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

  QString str = currentMDFile;
  QString iniName = str.replace(iniDir, "");

  Reg.setValue("/MainNotes/notebookIndex",
               getCurrentIndexFromQW(mw_one->ui->qwNoteBook));
  Reg.setValue("/MainNotes/noteIndex",
               getCurrentIndexFromQW(mw_one->ui->qwNoteList));
  Reg.setValue("/MainNotes/currentItem", iniName);
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
  int index = getCurrentIndexFromQW(mw_one->ui->qwCategory);
  QString str = getText0(mw_one->ui->qwCategory, index);
  mw_one->ui->editRenameType->setText(str);
}

void Method::okType() {
  int index = getCurrentIndexFromQW(mw_one->ui->qwCategory);
  m_CategoryList->ui->listWidget->setCurrentRow(index);
  QListWidgetItem* item = m_CategoryList->ui->listWidget->currentItem();
  m_CategoryList->on_listWidget_itemDoubleClicked(item);
  m_CategoryList->on_btnCancel_clicked();
}

void Method::setSCrollPro(QObject* obj) {
  QScrollerProperties sp;
  sp.setScrollMetric(QScrollerProperties::DragStartDistance, 0.001);
  sp.setScrollMetric(QScrollerProperties::ScrollingCurve,
                     QEasingCurve::OutQuad);
  sp.setScrollMetric(QScrollerProperties::DragVelocitySmoothingFactor, 0.001);
  sp.setScrollMetric(QScrollerProperties::FrameRate,
                     QScrollerProperties::FrameRates::Fps60);
  QScroller* qs = QScroller::scroller(obj);
  qs->setScrollerProperties(sp);
}

void Method::init_all_notes() {
  mw_one->m_NotesList->initNotesList();
  mw_one->m_NotesList->initRecycle();

  // load note
  currentMDFile = mw_one->m_NotesList->getCurrentMDFile();
  mw_one->m_Notes->MD2Html(currentMDFile);
  mw_one->m_Notes->loadMemoQML();
  mw_one->m_Notes->setVPos();
}

QDialog* Method::getProgBar() {
  QDialog* dlg;
  dlg = new QDialog(this);
  dlg->setModal(true);

  QFrame* frame = new QFrame(this);

  dlg->setWindowFlag(Qt::FramelessWindowHint);
  dlg->setAttribute(Qt::WA_TranslucentBackground);

  if (isDark)
    frame->setStyleSheet(
        "QFrame{background-color: #455364; border-radius:10px; "
        "border:0px solid red;}");
  else
    frame->setStyleSheet(
        "QFrame{background-color: rgb(255, 255, 255);border-radius:10px; "
        "border:0px solid red;}");

  QGridLayout* grid = new QGridLayout();
  dlg->setLayout(grid);
  grid->addWidget(frame);

  dlg->setFixedHeight(200);
  dlg->setFixedWidth(mw_one->geometry().width() - 50);
  QVBoxLayout* vbox = new QVBoxLayout;
  frame->setLayout(vbox);
  dlg->setGeometry(
      mw_one->geometry().x() + (mw_one->width() - dlg->width()) / 2,
      mw_one->geometry().y() + (mw_one->height() - dlg->height()) / 2,
      dlg->width(), dlg->height());

  QLabel* lbl = new QLabel(dlg);
  if (isDark)
    lbl->setStyleSheet("color:#ffffff;");
  else
    lbl->setStyleSheet("color:#000000;");
  QFont font = this->font();
  int size = font.pointSize();
  font.setBold(true);
  font.setPointSize(size + 2);
  lbl->setFont(font);
  lbl->setText(tr("Reading, please wait..."));
  lbl->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  vbox->addWidget(lbl);

  if (nProgressBarType == 1) {
    QProgressBar* prog = new QProgressBar(this);
    prog->setStyleSheet(
        "QProgressBar{border:0px solid #FFFFFF;"
        "height:25;"
        "background:rgb(25,255,25);"
        "text-align:right;"
        "color:rgb(255,255,255);"
        "border-radius:0px;}"

        "QProgressBar:chunk{"
        "border-radius:0px;"
        "background-color:rgba(18,150,219,255);"
        "}");
    prog->setMaximum(0);
    prog->setMinimum(0);
    vbox->addWidget(prog);
  }

  if (nProgressBarType == 2) {
    QtMaterialCircularProgress* qmProgress =
        new QtMaterialCircularProgress(this);
    vbox->addWidget(qmProgress);
  }

  return dlg;
}

bool Method::eventFilterReader(QObject* watch, QEvent* evn) {
  QMouseEvent* event = static_cast<QMouseEvent*>(evn);
  if (watch == mw_one->ui->qwReader) {
    if (event->type() == QEvent::MouseButtonDblClick) {
      mw_one->on_SetReaderFunVisible();
    }

    static int press_x;
    static int press_y;
    static int relea_x;
    static int relea_y;
    int length = 75;

    if (mw_one->ui->textBrowser->isHidden()) {
      if (event->type() == QEvent::MouseButtonPress) {
        mw_one->isMousePress = true;
        mw_one->isMouseMove = false;
        if (!mw_one->isMouseMove) mw_one->timerMousePress->start(1300);
      }

      if (event->type() == QEvent::MouseButtonRelease) {
        mw_one->isMousePress = false;
        mw_one->isMouseMove = false;
      }

      if (event->type() == QEvent::MouseMove) {
        mw_one->isMouseMove = true;

        if (mw_one->isMousePress) {
          if ((relea_x - press_x) > length && qAbs(relea_y - press_y) < 35) {
            // qDebug() << "book right...";
            int cn = mw_one->ui->btnPages->text().split("\n").at(1).toInt();
            if (cn != 1) {
              mw_one->m_PageIndicator->setPicRight();
            }
          } else if ((press_x - relea_x) > length &&
                     qAbs(relea_y - press_y) < 35) {
            // qDebug() << "book left...";
            int cn = mw_one->ui->btnPages->text().split("\n").at(1).toInt();
            int tn = mw_one->ui->btnPages->text().split("\n").at(2).toInt();
            if (cn != tn) {
              mw_one->m_PageIndicator->setPicLeft();
            }
          } else
            mw_one->m_PageIndicator->close();
        }
      }
    }

    if (event->type() == QEvent::MouseButtonPress) {
      mw_one->isMousePress = true;

      press_x = event->globalX();
      press_y = event->globalY();
      x = 0;
      y = 0;
      w = mw_one->ui->qwReader->width();
      h = mw_one->ui->qwReader->height();
    }

    if (event->type() == QEvent::MouseButtonRelease) {
      relea_x = event->globalX();
      relea_y = event->globalY();
      mw_one->ui->lblTitle->hide();
      QQuickItem* root = mw_one->ui->qwReader->rootObject();

      mw_one->isTurnThePage = false;
      mw_one->isMousePress = false;
      mw_one->isMouseMove = false;

      // Right Slide
      if ((relea_x - press_x) > length && qAbs(relea_y - press_y) < 35) {
        if (isText) {
          if (iPage - baseLines <= 0) {
            if (isText || isEpub) {
              QMetaObject::invokeMethod((QObject*)root, "setX",
                                        Q_ARG(QVariant, 0));
              return QWidget::eventFilter(watch, evn);
            }
          }
        } else if (isEpub) {
          if (htmlIndex <= 0) {
            if (isText || isEpub) {
              QMetaObject::invokeMethod((QObject*)root, "setX",
                                        Q_ARG(QVariant, 0));
              return QWidget::eventFilter(watch, evn);
            }
          }
        }
        mw_one->isTurnThePage = true;

        mw_one->on_btnPageUp_clicked();
        mw_one->m_PageIndicator->close();
      }

      // Left Slide
      if ((press_x - relea_x) > length && qAbs(relea_y - press_y) < 35) {
        if (isText) {
          if (iPage + baseLines > totallines) {
            if (isText || isEpub) {
              QMetaObject::invokeMethod((QObject*)root, "setX",
                                        Q_ARG(QVariant, 0));
              return QWidget::eventFilter(watch, evn);
            }
          }
        } else if (isEpub) {
          if (htmlIndex + 1 >= htmlFiles.count()) {
            if (isText || isEpub) {
              QMetaObject::invokeMethod((QObject*)root, "setX",
                                        Q_ARG(QVariant, 0));
              return QWidget::eventFilter(watch, evn);
            }
          }
        }
        mw_one->isTurnThePage = true;

        mw_one->on_btnPageNext_clicked();
        mw_one->m_PageIndicator->close();
      }

      if (isText || isEpub)
        QMetaObject::invokeMethod((QObject*)root, "setX", Q_ARG(QVariant, 0));

      mw_one->curx = 0;
    }

    if (event->type() == QEvent::MouseMove) {
      relea_x = event->globalX();
      relea_y = event->globalY();
      if (mw_one->isMousePress && qAbs(relea_x - press_x) > 20 &&
          qAbs(relea_y - press_y) < 20) {
        mw_one->isMouseMove = true;
      }
    }
  }
  return QWidget::eventFilter(watch, evn);
}

void Method::saveRecycleTabName(QString keyStr, QString tabName) {
  QSettings Reg(iniDir + "del_tabname.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif

  Reg.setValue(keyStr, tabName);
}

QString Method::getRecycleTabName(QString keyStr) {
  QSettings Reg(iniDir + "del_tabname.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif

  QString tabName = Reg.value(keyStr).toString();
  if (tabName.trimmed().length() == 0) tabName = "None";
  return tabName;
}

void Method::showDelMsgBox(QString title, QString info) {
  bool isOK;

  QDialog* dlg = new QDialog(this);
  QVBoxLayout* vbox0 = new QVBoxLayout;
  dlg->setLayout(vbox0);
  dlg->setModal(true);
  dlg->setWindowFlag(Qt::FramelessWindowHint);
  dlg->setAttribute(Qt::WA_TranslucentBackground);

  QFrame* frame = new QFrame(this);
  vbox0->addWidget(frame);
  frame->setStyleSheet(
      "QFrame{background-color: rgb(255, 255, 255);border-radius:10px; "
      "border:0px solid gray;}");

  QVBoxLayout* vbox = new QVBoxLayout;
  vbox->setContentsMargins(12, 12, 12, 12);
  vbox->setSpacing(12);
  frame->setLayout(vbox);

  QLabel* lblTitle = new QLabel(this);
  lblTitle->adjustSize();
  lblTitle->setWordWrap(true);
  lblTitle->setText(title);
  vbox->addWidget(lblTitle);

  QFrame* hframe = new QFrame(this);
  hframe->setFrameShape(QFrame::HLine);
  hframe->setStyleSheet("QFrame{background:red;min-height:2px}");
  vbox->addWidget(hframe);

  QLabel* lbl = new QLabel(this);
  lbl->adjustSize();
  lbl->setWordWrap(true);
  lbl->setText(info);
  vbox->addWidget(lbl);

  QToolButton* btnCancel = new QToolButton(this);
  QToolButton* btnOk = new QToolButton(this);
  btnCancel->setText(tr("Cancel"));
  btnOk->setText(tr("Delete"));
  btnOk->setStyleSheet(
      "QToolButton {background-color: rgb(255, 0, 0);color: rgb(255, "
      "255, 255);border-radius:10px;border:0px solid gray;} "
      "QToolButton:pressed "
      "{ background-color: "
      "rgb(220,220,230);color: black}");

  btnCancel->setStyleSheet(btnStyle);
  btnOk->setFixedHeight(35);
  btnCancel->setFixedHeight(35);

  QHBoxLayout* hbox = new QHBoxLayout;
  hbox->addWidget(btnCancel);
  hbox->addWidget(btnOk);
  btnCancel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  btnOk->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

  QSpacerItem* sparcer_item =
      new QSpacerItem(0, 160, QSizePolicy::Fixed, QSizePolicy::Expanding);
  vbox->addItem(sparcer_item);

  vbox->addLayout(hbox, 0);

  isOK = false;
  connect(btnCancel, &QToolButton::clicked, [=]() mutable {
    isOK = false;
    dlg->close();
  });
  connect(dlg, &QDialog::rejected, [=]() mutable { closeGrayWindows(); });
  connect(btnOk, &QToolButton::clicked, [=]() mutable {
    isOK = true;
    dlg->close();
  });

  int x, y, w, h;
  w = mw_one->width() - 40;
  x = mw_one->geometry().x() + (mw_one->width() - w) / 2;
  h = mw_one->calcStringPixelHeight(this->font(), fontSize) * 15;
  y = geometry().y() + (height() - h) / 2;
  dlg->setGeometry(x, y, w, h);

  showGrayWindows();

  dlg->exec();
}

QFont Method::getNewFont(int maxSize) {
  QFont font0 = this->font();
  if (fontSize > maxSize)
    font0.setPointSize(maxSize);
  else
    font0.setPointSize(fontSize);

  return font0;
}

void Method::setDark(QString strDark) {
  Q_UNUSED(strDark);
#ifdef Q_OS_ANDROID

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  QAndroidJniObject javaDark = QAndroidJniObject::fromString(strDark);
  QAndroidJniObject jo = QAndroidJniObject::fromString("dark");
  jo.callMethod<void>("com.x/MyActivity", "setDark", "(Ljava/lang/String;)V",
                      javaDark.object<jstring>());

#else
  QAndroidJniObject javaDark = QAndroidJniObject::fromString(strDark);
  QJniObject jo = QJniObject::fromString("dark");
  jo.callMethod<void>("com.x/MyActivity", "setDark", "(Ljava/lang/String;)V",
                      javaDark.object<jstring>());

#endif

#endif
}

QString Method::setPushButtonQss(QToolButton* btn, int radius, int padding,
                                 const QString& normalColor,
                                 const QString& normalTextColor,
                                 const QString& hoverColor,
                                 const QString& hoverTextColor,
                                 const QString& pressedColor,
                                 const QString& pressedTextColor) {
  QStringList list;
  list.append(QString("QToolButton{border-style:none;padding:%1px;border-"
                      "radius:%2px;color:%3;background:%4;}")
                  .arg(padding)
                  .arg(radius)
                  .arg(normalTextColor)
                  .arg(normalColor));
  list.append(QString("QToolButton:hover{color:%1;background:%2;}")
                  .arg(hoverTextColor)
                  .arg(hoverColor));
  list.append(QString("QToolButton:pressed{color:%1;background:%2;}")
                  .arg(pressedTextColor)
                  .arg(pressedColor));

  QString qss = list.join("");
  btn->setStyleSheet(qss);
  return qss;
}
