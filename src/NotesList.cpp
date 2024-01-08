#include "NotesList.h"

#include "src/MainWindow.h"
#include "ui_MainWindow.h"
#include "ui_NotesList.h"

extern MainWindow *mw_one;
extern QString iniDir, privateDir, currentMDFile;
extern bool isAndroid;
extern int fontSize;

NotesList::NotesList(QWidget *parent) : QDialog(parent), ui(new Ui::NotesList) {
  ui->setupUi(this);
  this->installEventFilter(this);
  mw_one->set_btnStyle(this);

  connect(pAndroidKeyboard, &QInputMethod::visibleChanged, this,
          &NotesList::on_KVChanged);

  setWindowFlag(Qt::FramelessWindowHint);
  setAttribute(Qt::WA_TranslucentBackground);
  ui->frame0->setStyleSheet(
      "#frame0{background-color: rgb(236, 236, 236);border-radius:10px; "
      "border:1px solid gray;}");
  ui->frame1->setStyleSheet(
      "#frame1{background-color: rgb(236, 236, 236);border-radius:10px; "
      "border:1px solid gray;}");
  tw = ui->treeWidget;
  twrb = ui->treeWidgetRecycle;

#ifdef Q_OS_ANDROID
  ui->treeWidget->setStyleSheet("selection-background-color: lightblue");
#else
  ui->treeWidget->setStyleSheet(mw_one->treeStyle);
#endif

  ui->treeWidgetRecycle->setStyleSheet(ui->treeWidget->styleSheet());

  setModal(true);
  this->layout()->setSpacing(5);
  this->layout()->setContentsMargins(2, 2, 2, 2);
  ui->frame1->hide();

  tw->verticalScrollBar()->setStyleSheet(mw_one->vsbarStyleSmall);
  tw->setVerticalScrollMode(QTreeWidget::ScrollPerPixel);
  QScroller::grabGesture(tw, QScroller::LeftMouseButtonGesture);
  mw_one->m_Method->setSCrollPro(tw);

  twrb->verticalScrollBar()->setStyleSheet(mw_one->vsbarStyleSmall);
  twrb->setVerticalScrollMode(QTreeWidget::ScrollPerPixel);
  QScroller::grabGesture(twrb, QScroller::LeftMouseButtonGesture);
  mw_one->m_Method->setSCrollPro(twrb);

  ui->treeWidget->headerItem()->setText(0, tr("Notebook"));
  ui->treeWidget->setColumnHidden(1, true);
  twrb->header()->hide();
  twrb->setColumnHidden(1, true);
  twrb->setColumnWidth(0, 180);

  QString path = iniDir + "memo/";
  QDir dir(path);
  if (!dir.exists()) dir.mkdir(path);

  ui->btnPrev->setEnabled(false);
  ui->btnNext->setEnabled(false);
  QFont font = this->font();
  font.setPointSize(font.pointSize() - 1);
  font.setBold(true);
  ui->lblCount->setFont(font);
  ui->btnPrev->hide();
  ui->btnNext->hide();

  initNotesList();
  initRecycle();
}

NotesList::~NotesList() { delete ui; }

bool NotesList::eventFilter(QObject *watch, QEvent *evn) {
  if (evn->type() == QEvent::KeyRelease) {
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      if (!ui->frame1->isHidden()) {
        on_btnBack_clicked();
        return true;
      }

      if (!ui->frame0->isHidden()) {
        on_btnClose_clicked();
        return true;
      }
    }

    if (keyEvent->key() == Qt::Key_Return) {
      QTreeWidgetItem *item = tw->currentItem();
      on_treeWidget_itemClicked(item, 0);
      return true;
    }
  }

  return QWidget::eventFilter(watch, evn);
}

void NotesList::on_btnClose_clicked() { this->close(); }

void NotesList::on_btnNewNoteBook_clicked() {
  QTreeWidgetItem *item = new QTreeWidgetItem();
  item->setText(0, ui->editBook->text().trimmed());
  item->setForeground(0, Qt::red);
  ui->treeWidget->addTopLevelItem(item);
  ui->treeWidget->setCurrentItem(item);

  isNeedSave = true;
}

void NotesList::on_btnNewNote_clicked() {
  if (ui->treeWidget->topLevelItemCount() == 0) return;

  int rand = QRandomGenerator::global()->generate();
  if (rand < 0) rand = 0 - rand;

  QString noteFile = "memo/" + mw_one->m_Notes->getDateTimeStr() + "_" +
                     QString::number(rand) + ".md";
  QTreeWidgetItem *topitem = ui->treeWidget->currentItem();
  if (topitem->parent() != NULL) topitem = topitem->parent();

  QTreeWidgetItem *item1 = new QTreeWidgetItem(topitem);
  item1->setText(0, ui->editNote->text().trimmed());
  item1->setText(1, noteFile);

  QTextEdit *edit = new QTextEdit();
  mw_one->TextEditToFile(edit, iniDir + noteFile);

  ui->treeWidget->setCurrentItem(topitem->child(topitem->childCount() - 1));
  on_treeWidget_itemClicked(item1, 0);

  isNeedSave = true;
}

void NotesList::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column) {
  Q_UNUSED(column);

  return;

  if (ui->treeWidget->topLevelItemCount() == 0) return;

  if (item->parent() != NULL) {
    if (tw->currentIndex().row() == 0) {
      if (tw->currentIndex().parent().row() == 0) {
      }
    }

    QSettings Reg(iniDir + "curmd.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    Reg.setIniCodec("utf-8");
#endif

    QString curmd = item->text(1);
    Reg.setValue("/MainNotes/currentItem", curmd);

    currentMDFile = iniDir + curmd;
    mw_one->m_Notes->MD2Html(currentMDFile);
    mw_one->m_Notes->loadMemoQML();

    setNoteName(item->text(0));
  }

  ui->editName->setText(item->text(0));
}

QString NotesList::getCurrentMDFile() {
  QSettings Reg(iniDir + "curmd.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif

  QString curmd = Reg.value("/MainNotes/currentItem", "memo/xxx.md").toString();
  mw_one->ui->lblNoteName->setText(
      Reg.value("/MainNotes/NoteName", tr("Note Name")).toString());

  return iniDir + curmd;
}

void NotesList::on_btnRename_clicked() {
  if (ui->treeWidget->topLevelItemCount() == 0) return;

  QTreeWidgetItem *item = ui->treeWidget->currentItem();
  item->setText(0, ui->editName->text().trimmed());
  if (item->parent() != NULL) setNoteName(item->text(0));

  isNeedSave = true;
}

void NotesList::setNoteName(QString name) {
  mw_one->ui->lblNoteName->adjustSize();
  mw_one->ui->lblNoteName->setWordWrap(true);
  mw_one->ui->lblNoteName->setText(name);
  mw_one->ui->lblNoteName->setToolTip(name);
}

void NotesList::on_btnDel_clicked() {
  if (tw->topLevelItemCount() == 0) return;

  QTreeWidgetItem *item = ui->treeWidget->currentItem();

  if (item->parent() == NULL) {
    int count = item->childCount();
    for (int i = 0; i < count; i++) {
      QTreeWidgetItem *childItem = new QTreeWidgetItem;
      childItem->setText(0, item->child(i)->text(0));
      childItem->setText(1, item->child(i)->text(1));
      addItem(twrb, childItem);
    }
    ui->treeWidget->takeTopLevelItem(ui->treeWidget->currentIndex().row());

  } else {
    item->parent()->removeChild(item);
    addItem(twrb, item);
  }

  on_treeWidget_itemClicked(tw->currentItem(), 0);

  tw->setFocus();
  isNeedSave = true;
}

void NotesList::addItem(QTreeWidget *tw, QTreeWidgetItem *item) {
  tw->setFocus();
  if (tw == twrb) {
    tw->setCurrentItem(tw->topLevelItem(0));
    QTreeWidgetItem *curItem = tw->currentItem();
    curItem->addChild(item);
  } else {
    QTreeWidgetItem *curItem = tw->currentItem();
    if (curItem->parent() == NULL) {
      curItem->addChild(item);
    } else {
      curItem->parent()->addChild(item);
    }
  }

  tw->expandAll();
}

void NotesList::delFile(QString file) {
  QFile _file(file);
  if (_file.exists()) _file.remove();
  _file.close();
}

bool NotesList::on_btnImport_clicked() {
  if (ui->treeWidget->topLevelItemCount() == 0) return false;

  QString fileName;
  fileName =
      QFileDialog::getOpenFileName(this, tr("Knot"), "", tr("MD File (*.*)"));

  if (fileName.isNull()) return false;

  bool isMD = false;
  QString strInfo;
  isMD = fileName.contains(".md");
  strInfo = fileName;
#ifdef Q_OS_ANDROID
  QString fileAndroid = mw_one->m_Reader->getUriRealPath(fileName);
  isMD = fileAndroid.contains(".md");

  QStringList list = fileAndroid.split("/");
  QString str = list.at(list.count() - 1);
  if (str.toInt() > 0) isMD = true;
  strInfo = fileAndroid;

#endif

  if (!isMD) {
    ShowMessage *m_ShowMsg = new ShowMessage(this);
    m_ShowMsg->showMsg("Knot", tr("Invalid Markdown file.") + "\n\n" + strInfo,
                       1);

    return false;
  }

  if (QFile(fileName).exists()) {
    QTreeWidgetItem *item = ui->treeWidget->currentItem();

    QTreeWidgetItem *item1;
    QFileInfo fi(strInfo);
    QString name = fi.fileName();
    QString suffix = fi.suffix();
    name.replace("." + suffix, "");
    if (item->parent() == NULL) {
      item1 = new QTreeWidgetItem(item);
      item1->setText(0, name);
    } else {
      item1 = new QTreeWidgetItem(item->parent());
      item1->setText(0, name);
    }
    tw->setCurrentItem(item1);
    QString a = "memo/" + mw_one->m_Notes->getDateTimeStr() + ".md";
    currentMDFile = iniDir + a;
    QString str = mw_one->loadText(fileName);
    QTextEdit *edit = new QTextEdit();
    edit->setAcceptRichText(false);
    edit->setPlainText(str);
    mw_one->TextEditToFile(edit, currentMDFile);

    item1->setText(1, a);

    on_treeWidget_itemClicked(item1, 0);
  } else
    return false;

  isNeedSave = true;
  return true;
}

void NotesList::on_btnExport_clicked() {
  if (ui->treeWidget->topLevelItemCount() == 0) return;

  QTreeWidgetItem *item = tw->currentItem();
  if (item->parent() == NULL) return;

  QString name = item->text(0);
  name = name + ".md";
  QString fileName;
  QFileDialog fd;
  fileName = fd.getSaveFileName(this, name, name, tr("MD File(*.*)"));

  if (fileName == "") return;

  QString mdfile = iniDir + item->text(1);

  QString str = mw_one->loadText(mdfile);
  QTextEdit *edit = new QTextEdit();
  edit->setAcceptRichText(false);
  edit->setPlainText(str);

  mw_one->TextEditToFile(edit, fileName);
}

void NotesList::closeEvent(QCloseEvent *event) {
  Q_UNUSED(event);

  saveNotesList();
  saveRecycle();

  mw_one->ui->btnBackNotes->show();
  mw_one->ui->btnEdit->show();
  mw_one->ui->btnNotesList->show();
  mw_one->ui->btnSetKey->show();
  mw_one->ui->btnPDF->show();
}

void NotesList::saveNotesList() {
  if (!isNeedSave) return;

  mw_one->isSelf = true;

  mw_one->isNeedAutoBackup = true;
  mw_one->strLatestModify = tr("Modi Notes List");

  QSettings Reg(iniDir + "mainnotes.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif

  int count = tw->topLevelItemCount();
  Reg.setValue("/MainNotes/topItemCount", count);
  for (int i = 0; i < count; i++) {
    QTreeWidgetItem *topItem = tw->topLevelItem(i);
    QString strtop = topItem->text(0);
    Reg.setValue("/MainNotes/strTopItem" + QString::number(i), strtop);

    int childCount = topItem->childCount();
    Reg.setValue("/MainNotes/childCount" + QString::number(i), childCount);

    for (int j = 0; j < childCount; j++) {
      QTreeWidgetItem *childItem = tw->topLevelItem(i)->child(j);
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

  // Save Note Name
  QSettings Reg1(iniDir + "curmd.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg1.setIniCodec("utf-8");
#endif

  Reg1.setValue("/MainNotes/NoteName", mw_one->ui->lblNoteName->text());

  isNeedSave = false;
}

void NotesList::saveRecycle() {
  if (!isNeedSave) return;

  mw_one->isSelf = true;

  mw_one->isNeedAutoBackup = true;
  mw_one->strLatestModify = tr("Modi Notes Recycle");

  QSettings Reg(iniDir + "mainnotes.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif

  int count = twrb->topLevelItemCount();
  Reg.setValue("/MainNotes/rbtopItemCount", count);
  for (int i = 0; i < count; i++) {
    QTreeWidgetItem *topItem = twrb->topLevelItem(i);
    QString strtop = topItem->text(0);
    Reg.setValue("/MainNotes/rbstrTopItem" + QString::number(i), strtop);

    int childCount = topItem->childCount();
    Reg.setValue("/MainNotes/rbchildCount" + QString::number(i), childCount);

    for (int j = 0; j < childCount; j++) {
      QTreeWidgetItem *childItem = twrb->topLevelItem(i)->child(j);
      QString strChild0 = childItem->text(0);
      QString strChild1 = childItem->text(1);

      Reg.setValue(
          "/MainNotes/rbchildItem0" + QString::number(i) + QString::number(j),
          strChild0);
      Reg.setValue(
          "/MainNotes/rbchildItem1" + QString::number(i) + QString::number(j),
          strChild1);
    }
  }

  isNeedSave = false;
}

void NotesList::initNotesList() {
  mw_one->isSelf = true;
  tw->clear();
  QSettings Reg(iniDir + "mainnotes.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif

  int topCount = Reg.value("/MainNotes/topItemCount").toInt();

  int notesTotal = 0;
  for (int i = 0; i < topCount; i++) {
    QString strTop =
        Reg.value("/MainNotes/strTopItem" + QString::number(i)).toString();
    int childCount =
        Reg.value("/MainNotes/childCount" + QString::number(i)).toInt();
    notesTotal = notesTotal + childCount;

    QTreeWidgetItem *topItem = new QTreeWidgetItem;
    topItem->setText(0, strTop);
    topItem->setForeground(0, Qt::red);
    QFont font = this->font();
    font.setBold(true);
    topItem->setFont(0, font);

    for (int j = 0; j < childCount; j++) {
      QString str0, str1;
      str0 = Reg.value("/MainNotes/childItem0" + QString::number(i) +
                       QString::number(j))
                 .toString();
      str1 = Reg.value("/MainNotes/childItem1" + QString::number(i) +
                       QString::number(j))
                 .toString();

      QTreeWidgetItem *childItem = new QTreeWidgetItem(topItem);
      childItem->setText(0, str0);
      childItem->setText(1, str1);
    }
    tw->addTopLevelItem(topItem);
  }

  tw->headerItem()->setText(0, tr("Notebook") + "(" +
                                   QString::number(topCount) + "  " +
                                   QString::number(notesTotal) + ")");
  tw->expandAll();
}

void NotesList::initRecycle() {
  mw_one->isSelf = true;
  twrb->clear();
  QSettings Reg(iniDir + "mainnotes.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif

  int topCount = Reg.value("/MainNotes/rbtopItemCount").toInt();
  for (int i = 0; i < topCount; i++) {
    QString strTop =
        Reg.value("/MainNotes/rbstrTopItem" + QString::number(i)).toString();
    QTreeWidgetItem *topItem = new QTreeWidgetItem;
    topItem->setText(0, strTop);

    int childCount =
        Reg.value("/MainNotes/rbchildCount" + QString::number(i)).toInt();
    for (int j = 0; j < childCount; j++) {
      QString str0, str1;
      str0 = Reg.value("/MainNotes/rbchildItem0" + QString::number(i) +
                       QString::number(j))
                 .toString();
      str1 = Reg.value("/MainNotes/rbchildItem1" + QString::number(i) +
                       QString::number(j))
                 .toString();

      QTreeWidgetItem *childItem = new QTreeWidgetItem(topItem);
      childItem->setText(0, str0);
      childItem->setText(1, str1);
    }
    twrb->addTopLevelItem(topItem);
  }

  twrb->expandAll();

  if (twrb->topLevelItemCount() == 0) {
    QTreeWidgetItem *topItem = new QTreeWidgetItem;
    topItem->setText(0, tr("Notes Recycle Bin"));
    twrb->addTopLevelItem(topItem);
    twrb->setCurrentItem(topItem);
    saveRecycle();
  }
}

void NotesList::on_btnRecycle_clicked() {
  ui->frame0->hide();
  ui->frame1->show();
  setWinPos();
  twrb->setFocus();
  twrb->setCurrentItem(twrb->topLevelItem(0));
}

void NotesList::on_btnBack_clicked() {
  ui->frame1->hide();
  ui->frame0->show();
  setWinPos();
  tw->setFocus();
}

void NotesList::on_btnRestore_clicked() {
  QTreeWidgetItem *curItem = twrb->currentItem();

  if (curItem->parent() == NULL) {
    return;
  } else {
    QTreeWidgetItem *item = new QTreeWidgetItem;
    QString str0 = curItem->text(0);
    QString str1 = curItem->text(1);

    item->setText(0, str0);
    item->setText(1, str1);
    addItem(tw, item);

    mw_one->m_Method->addItemToQW(mw_one->ui->qwNoteList, str0, "", "", str1,
                                  0);

    curItem->parent()->removeChild(curItem);
  }

  isNeedSave = true;
}

void NotesList::on_btnDel_Recycle_clicked() {
  QTreeWidgetItem *curItem = twrb->currentItem();
  if (curItem->parent() == NULL) {
    return;
  } else {
    QString md = iniDir + curItem->text(1);
    delFile(md);
    curItem->parent()->removeChild(curItem);
  }

  isNeedSave = true;
}

void NotesList::setWinPos() {
  int w = mw_one->width();
  int x = mw_one->geometry().x();
  this->setGeometry(x, mw_one->geometry().y(), w, mw_one->height());
  mw_one->ui->btnBackNotes->hide();
  mw_one->ui->btnEdit->hide();
  mw_one->ui->btnNotesList->hide();
  mw_one->ui->btnSetKey->hide();
  mw_one->ui->btnPDF->hide();
}

void NotesList::clearFiles() {
  QString tempDir = iniDir;
  files.clear();
  QStringList fmt = QString("zip;md;html;jpg;bmp;png;ini").split(';');
  getAllFiles(tempDir, files, fmt);

  clearMD_Pic(tw);
  clearMD_Pic(twrb);

  int count = files.count();
  for (int i = 0; i < count; i++) {
    QString a = files.at(i);

    QFile file(a);
    if (a.contains(".sync-conflict-")) {
      file.remove();
    }
  }
}

void NotesList::clearMD_Pic(QTreeWidget *tw) {
  for (int i = 0; i < tw->topLevelItemCount(); i++) {
    QTreeWidgetItem *topItem = tw->topLevelItem(i);
    int childCount = topItem->childCount();
    for (int j = 0; j < childCount; j++) {
      QString str = topItem->child(j)->text(1);
      removePicFromMD(iniDir + str);
    }
  }
}

void NotesList::removePicFromMD(QString mdfile) {
  QString txt = mw_one->loadText(mdfile);

  for (int i = 0; i < files.count(); i++) {
    QString str0 = files.at(i);
    str0.replace(iniDir, "");

    if (txt.contains(str0)) {
      files.removeAt(i);
      break;
    }
  }
}

void NotesList::getAllFiles(const QString &foldPath, QStringList &folds,
                            const QStringList &formats) {
  QDirIterator it(foldPath, QDir::Files | QDir::NoDotAndDotDot,
                  QDirIterator::Subdirectories);
  while (it.hasNext()) {
    it.next();
    QFileInfo fileInfo = it.fileInfo();
    if (formats.contains(fileInfo.suffix())) {  // 检测格式，按需保存
      folds << fileInfo.absoluteFilePath();
    }
  }
}

void NotesList::on_btnFind_clicked() {
  QString strFind = ui->editFind->text().trimmed().toLower();
  if (strFind == "") {
    ui->btnPrev->hide();
    ui->btnNext->hide();
    mw_one->ui->btnFindNextNote->setEnabled(false);
    mw_one->ui->btnFindPreviousNote->setEnabled(false);
    return;
  }
  findResultList.clear();
  int count = tw->topLevelItemCount();
  for (int i = 0; i < count; i++) {
    QTreeWidgetItem *topItem = tw->topLevelItem(i);
    if (topItem->text(0).toLower().contains(strFind))
      findResultList.append(topItem);
    int childCount = topItem->childCount();
    for (int j = 0; j < childCount; j++) {
      QTreeWidgetItem *childItem = topItem->child(j);
      if (childItem->text(0).toLower().contains(strFind))
        findResultList.append(childItem);
    }
  }

  if (findResultList.count() > 0) {
    ui->btnPrev->setEnabled(true);
    ui->btnNext->setEnabled(true);
    tw->setCurrentItem(findResultList.at(0));
    tw->scrollToItem(tw->currentItem());

    localItem();

    findCount = 0;
    ui->lblCount->setText(QString::number(findCount + 1) + "->" +
                          QString::number(findResultList.count()));

    mw_one->ui->lblFindNoteCount->setText(ui->lblCount->text());

    ui->btnPrev->show();
    ui->btnNext->show();

    mw_one->ui->btnFindNextNote->setEnabled(true);
    mw_one->ui->btnFindPreviousNote->setEnabled(true);
  } else {
    ui->btnPrev->hide();
    ui->btnNext->hide();

    mw_one->ui->btnFindNextNote->setEnabled(false);
    mw_one->ui->btnFindPreviousNote->setEnabled(false);

    ui->lblCount->setText("0");

    mw_one->ui->lblFindNoteCount->setText(ui->lblCount->text());

    setNoteBookCurrentIndex(-1);
    setNotesListCurrentIndex(-1);
  }
}

void NotesList::localItem() {
  QTreeWidgetItem *item = tw->currentItem();
  if (item->childCount() > 0) {
    int topIndex = tw->indexOfTopLevelItem(item);
    setNoteBookCurrentIndex(topIndex);

    mw_one->m_Method->clickNoteBook();
    setNotesListCurrentIndex(-1);
  } else {
    int topIndex = tw->indexOfTopLevelItem(item->parent());
    int childIndex = tw->currentIndex().row();
    setNoteBookCurrentIndex(topIndex);
    mw_one->m_Method->clickNoteBook();
    setNotesListCurrentIndex(childIndex);
  }
}

QString NotesList::getNoteBookText0(int index) {
  return mw_one->m_Method->getText0(mw_one->ui->qwNoteBook, index);
}

QString NotesList::getNotesListText0(int index) {
  return mw_one->m_Method->getText0(mw_one->ui->qwNoteList, index);
}

void NotesList::modifyNoteBookText0(QString text0, int index) {
  mw_one->m_Method->modifyItemText0(mw_one->ui->qwNoteBook, index, text0);
}

void NotesList::modifyNotesListText0(QString text0, int index) {
  mw_one->m_Method->modifyItemText0(mw_one->ui->qwNoteList, index, text0);
}

void NotesList::on_btnPrev_clicked() {
  findCount--;
  if (findCount < 0) findCount = findResultList.count() - 1;  // findCount = 0;
  tw->setCurrentItem(findResultList.at(findCount));
  tw->scrollToItem(tw->currentItem());
  ui->lblCount->setText(QString::number(findCount + 1) + "->" +
                        QString::number(findResultList.count()));

  mw_one->ui->lblFindNoteCount->setText(ui->lblCount->text());

  if (isAndroid) {
    if (pAndroidKeyboard->isVisible()) pAndroidKeyboard->setVisible(false);
  }

  localItem();
}

void NotesList::on_btnNext_clicked() {
  if (ui->btnNext->isHidden()) return;

  findCount++;
  if (findCount >= findResultList.count())
    findCount = 0;  // findResultList.count() - 1;
  tw->setCurrentItem(findResultList.at(findCount));
  tw->scrollToItem(tw->currentItem());
  ui->lblCount->setText(QString::number(findCount + 1) + "->" +
                        QString::number(findResultList.count()));

  mw_one->ui->lblFindNoteCount->setText(ui->lblCount->text());

  if (isAndroid) {
    if (pAndroidKeyboard->isVisible()) pAndroidKeyboard->setVisible(false);
  }

  localItem();
}

void NotesList::on_editFind_textChanged(const QString &arg1) {
  if (arg1.trimmed() == "") {
    ui->lblCount->setText("0");
    mw_one->ui->lblFindNoteCount->setText("0");
  }
  on_btnFind_clicked();
}

void NotesList::on_editFind_returnPressed() {
  if (ui->btnNext->isEnabled()) on_btnNext_clicked();
}

void NotesList::on_KVChanged() {
  if (!pAndroidKeyboard->isVisible()) {
  } else {
  }
}

void NotesList::moveBy(int ud) {
  QTreeWidgetItem *item = tw->currentItem();
  if (item->parent() != NULL) {
    QTreeWidgetItem *parentItem = item->parent();
    int index = parentItem->indexOfChild(item);
    if (ud == -1) {
      if (index - 1 >= 0) {
        parentItem->removeChild(item);
        parentItem->insertChild(index - 1, item);
        tw->setCurrentItem(item);
        tw->scrollToItem(item);
        isNeedSave = true;
      }
    }
    if (ud == 1) {
      if (index + 1 <= parentItem->childCount() - 1) {
        parentItem->removeChild(item);
        parentItem->insertChild(index + 1, item);
        tw->setCurrentItem(item);
        tw->scrollToItem(item);
        isNeedSave = true;
      }
    }
  } else {
    // top item
    int index = tw->indexOfTopLevelItem(item);
    if (ud == -1) {
      if (index - 1 >= 0) {
        tw->takeTopLevelItem(index);
        tw->insertTopLevelItem(index - 1, item);
        tw->setCurrentItem(item);
        tw->scrollToItem(item);
        tw->expandAll();
        isNeedSave = true;
      }
    }
    if (ud == 1) {
      if (index + 1 <= tw->topLevelItemCount() - 1) {
        tw->takeTopLevelItem(index);
        tw->insertTopLevelItem(index + 1, item);
        tw->setCurrentItem(item);
        tw->scrollToItem(item);
        tw->expandAll();
        isNeedSave = true;
      }
    }
  }
}

void NotesList::on_btnUp_clicked() { moveBy(-1); }

void NotesList::on_btnDown_clicked() { moveBy(1); }

void NotesList::on_actionAdd_NoteBook_triggered() {
  bool ok = false;
  QString text;
  QFrame *frame = new QFrame(mw_one);
  QVBoxLayout *vbox = new QVBoxLayout;
  frame->setLayout(vbox);
  QInputDialog *idlg = new QInputDialog(mw_one);
  idlg->hide();
  vbox->addWidget(idlg);

  idlg->setWindowFlag(Qt::FramelessWindowHint);
  QString style =
      "QDialog{background: "
      "rgb(244,237,241);border-radius:10px;border:2px solid red;}";

  idlg->setStyleSheet(style);
  idlg->setOkButtonText(tr("Ok"));
  idlg->setCancelButtonText(tr("Cancel"));
  idlg->setContentsMargins(10, 10, 10, 10);

  idlg->setWindowTitle(tr("New NoteBook"));
  idlg->setTextValue("");
  idlg->setLabelText(tr("NoteBook Name"));

  frame->setGeometry(25, -100, mw_one->width() - 50, this->height());
  idlg->show();

  frame->show();

  if (QDialog::Accepted == idlg->exec()) {
    ok = true;
    text = idlg->textValue();
    frame->close();
  } else {
    frame->close();
    return;
  }

  if (ok && !text.isEmpty()) {
    ui->editBook->setText(text);
    on_btnNewNoteBook_clicked();
    mw_one->m_Method->addItemToQW(mw_one->ui->qwNoteBook, text, "", "", "",
                                  fontSize);

    int count = getNoteBookCount();
    setNoteBookCurrentIndex(count - 1);
    mw_one->m_Method->clickNoteBook();
    setNotesListCurrentIndex(0);
    mw_one->m_Method->clickNoteList();
  }
}

void NotesList::on_actionDel_NoteBook_triggered() {
  int index = getNoteBookCurrentIndex();
  if (index < 0) return;

  ShowMessage *m_ShowMsg = new ShowMessage(this);
  if (!m_ShowMsg->showMsg(
          "Knot",
          tr("Whether to remove") + "  " +
              mw_one->m_Method->getText0(mw_one->ui->qwNoteBook, index) + " ? ",
          2))
    return;

  tw->setCurrentItem(tw->topLevelItem(index));
  on_btnDel_clicked();

  loadAllNoteBook();

  if (index - 1 >= 0) {
    setNoteBookCurrentIndex(index - 1);
    mw_one->m_Method->clickNoteBook();
    if (getNotesListCount() > 0) {
      setNotesListCurrentIndex(0);
      mw_one->m_Method->clickNoteList();
    }
  }

  setNoteLabel();

  saveRecycle();
  saveNotesList();

  if (getNoteBookCount() == 0) {
    loadEmptyNote();
  }
}

void NotesList::on_actionRename_NoteBook_triggered() {
  int index = getNoteBookCurrentIndex();
  if (index < 0) return;

  bool ok = false;
  QString text;
  QFrame *frame = new QFrame(mw_one);
  QVBoxLayout *vbox = new QVBoxLayout;
  frame->setLayout(vbox);
  QInputDialog *idlg = new QInputDialog(mw_one);
  idlg->hide();
  vbox->addWidget(idlg);

  idlg->setWindowFlag(Qt::FramelessWindowHint);
  QString style =
      "QDialog{background: "
      "rgb(244,237,241);border-radius:10px;border:2px solid red;}";

  idlg->setStyleSheet(style);
  idlg->setOkButtonText(tr("Ok"));
  idlg->setCancelButtonText(tr("Cancel"));
  idlg->setContentsMargins(10, 10, 10, 10);

  idlg->setWindowTitle(tr("Rename NoteBook"));
  idlg->setTextValue(mw_one->m_Method->getText0(mw_one->ui->qwNoteBook,
                                                getNoteBookCurrentIndex()));
  idlg->setLabelText(tr("NoteBook Name"));

  frame->setGeometry(25, -100, mw_one->width() - 50, this->height());
  idlg->show();

  frame->show();

  if (QDialog::Accepted == idlg->exec()) {
    ok = true;
    text = idlg->textValue();
    frame->close();
  } else {
    frame->close();
    return;
  }

  if (ok && !text.isEmpty()) {
    tw->setCurrentItem(tw->topLevelItem(index));
    ui->editName->setText(text);
    on_btnRename_clicked();

    mw_one->m_Method->modifyItemText0(mw_one->ui->qwNoteBook, index, text);
  }
}

int NotesList::getNoteBookCount() {
  int count = mw_one->m_Method->getCountFromQW(mw_one->ui->qwNoteBook);
  return count;
}

int NotesList::getNotesListCount() {
  int count = mw_one->m_Method->getCountFromQW(mw_one->ui->qwNoteList);
  return count;
}

int NotesList::getNoteBookCurrentIndex() {
  int index = mw_one->m_Method->getCurrentIndexFromQW(mw_one->ui->qwNoteBook);
  return index;
}

int NotesList::getNotesListCurrentIndex() {
  int index = mw_one->m_Method->getCurrentIndexFromQW(mw_one->ui->qwNoteList);
  return index;
}

void NotesList::setNoteBookCurrentIndex(int index) {
  mw_one->m_Method->setCurrentIndexFromQW(mw_one->ui->qwNoteBook, index);
}

void NotesList::setNotesListCurrentIndex(int index) {
  mw_one->m_Method->setCurrentIndexFromQW(mw_one->ui->qwNoteList, index);
}

void NotesList::on_actionMoveUp_NoteBook_triggered() {
  int index = getNoteBookCurrentIndex();
  if (index <= 0) return;

  QString text0 = mw_one->m_Method->getText0(mw_one->ui->qwNoteBook, index);
  QString text3 = mw_one->m_Method->getText3(mw_one->ui->qwNoteBook, index);
  int oldIndex = index;
  tw->setCurrentItem(tw->topLevelItem(index));
  on_btnUp_clicked();

  mw_one->m_Method->insertItem(mw_one->ui->qwNoteBook, text0, "", "", text3,
                               index - 1);
  mw_one->m_Method->delItemBakList(mw_one->ui->qwNoteBook, oldIndex + 1);
  setNoteBookCurrentIndex(oldIndex - 1);
}

void NotesList::on_actionMoveDown_NoteBook_triggered() {
  int index = getNoteBookCurrentIndex();

  if (index < 0) return;

  if (index == getNoteBookCount() - 1) return;

  int oldIndex = index;
  tw->setCurrentItem(tw->topLevelItem(index));
  on_btnDown_clicked();

  loadAllNoteBook();

  setNoteBookCurrentIndex(oldIndex + 1);
  mw_one->m_Method->clickNoteBook();
}

void NotesList::loadAllNoteBook() {
  mw_one->m_Method->clearAllBakList(mw_one->ui->qwNoteBook);
  mw_one->m_Method->clearAllBakList(mw_one->ui->qwNoteList);
  int count = tw->topLevelItemCount();
  for (int i = 0; i < count; i++) {
    QString str = tw->topLevelItem(i)->text(0);
    int sum = tw->topLevelItem(i)->childCount();
    QString strSum = tr("Sum") + " : " + QString::number(sum);
    mw_one->m_Method->addItemToQW(mw_one->ui->qwNoteBook, str, "", "", strSum,
                                  fontSize);
  }
}

void NotesList::init_NoteBookMenu(QMenu *mainMenu) {
  QAction *actNew = new QAction(tr("New NoteBook"));
  QAction *actDel = new QAction(tr("Del NoteBook"));
  QAction *actRename = new QAction(tr("Rename NoteBook"));
  QAction *actMoveUp = new QAction(tr("Move Up"));
  QAction *actMoveDown = new QAction(tr("Move Down"));

  connect(actNew, &QAction::triggered, this,
          &NotesList::on_actionAdd_NoteBook_triggered);
  connect(actDel, &QAction::triggered, this,
          &NotesList::on_actionDel_NoteBook_triggered);
  connect(actRename, &QAction::triggered, this,
          &NotesList::on_actionRename_NoteBook_triggered);

  connect(actMoveUp, &QAction::triggered, this,
          &NotesList::on_actionMoveUp_NoteBook_triggered);
  connect(actMoveDown, &QAction::triggered, this,
          &NotesList::on_actionMoveDown_NoteBook_triggered);

  QString qss =
      "QMenu {"
      "border: 1px solid rgb(172, 172, 172);"
      "border-radius: 3px; }"
      "QMenu::item {"
      "border-bottom: 1px solid rgb(172, 172, 172);"
      "padding:10px 10px;"
      "margin:0px 0px; }"
      "QMenu::item:selected {"
      "background-color: rgb(62, 186, 231); }";

  mainMenu->addAction(actNew);
  mainMenu->addAction(actRename);
  mainMenu->addAction(actDel);

  mainMenu->addAction(actMoveUp);
  mainMenu->addAction(actMoveDown);

  mainMenu->setStyleSheet(qss);
}

void NotesList::on_actionAdd_Note_triggered() {
  int notebookIndex = getNoteBookCurrentIndex();
  if (notebookIndex < 0) return;

  bool ok = false;
  QString text;
  QFrame *frame = new QFrame(mw_one);
  QVBoxLayout *vbox = new QVBoxLayout;
  frame->setLayout(vbox);
  QInputDialog *idlg = new QInputDialog(mw_one);
  idlg->hide();
  vbox->addWidget(idlg);

  idlg->setWindowFlag(Qt::FramelessWindowHint);
  QString style =
      "QDialog{background: "
      "rgb(244,237,241);border-radius:10px;border:2px solid red;}";

  idlg->setStyleSheet(style);
  idlg->setOkButtonText(tr("Ok"));
  idlg->setCancelButtonText(tr("Cancel"));
  idlg->setContentsMargins(10, 10, 10, 10);

  idlg->setWindowTitle(tr("New Note"));
  idlg->setTextValue("");
  idlg->setLabelText(tr("New Note Name"));

  frame->setGeometry(25, -100, mw_one->width() - 50, this->height());
  idlg->show();

  frame->show();

  if (QDialog::Accepted == idlg->exec()) {
    ok = true;
    text = idlg->textValue();
    frame->close();
  } else {
    frame->close();
    return;
  }

  if (ok && !text.isEmpty()) {
    tw->setCurrentItem(tw->topLevelItem(notebookIndex));
    ui->editNote->setText(text);
    on_btnNewNote_clicked();

    QTreeWidgetItem *childItem = tw->currentItem();
    int childCount = childItem->parent()->childCount();
    QString text3 = childItem->parent()->child(childCount - 1)->text(1);
    mw_one->m_Method->addItemToQW(mw_one->ui->qwNoteList, text, "", "", text3,
                                  0);

    int count = getNotesListCount();
    setNotesListCurrentIndex(count - 1);
    mw_one->m_Method->clickNoteList();
  }

  setNoteLabel();
}

void NotesList::on_actionDel_Note_triggered() {
  if (getNotesListCount() == 0) return;

  int notebookIndex = getNoteBookCurrentIndex();
  int notelistIndex = getNotesListCurrentIndex();

  if (notebookIndex < 0) return;
  if (notelistIndex < 0) return;

  ShowMessage *m_ShowMsg = new ShowMessage(this);
  if (!m_ShowMsg->showMsg("Knot",
                          tr("Whether to remove") + "  " +
                              mw_one->m_Method->getText0(mw_one->ui->qwNoteList,
                                                         notelistIndex) +
                              " ? ",
                          2))
    return;

  tw->setCurrentItem(tw->topLevelItem(notebookIndex)->child(notelistIndex));
  on_btnDel_clicked();

  mw_one->m_Method->delItemBakList(mw_one->ui->qwNoteList, notelistIndex);
  if (getNotesListCount() > 0) {
    setNotesListCurrentIndex(notelistIndex - 1);
    mw_one->m_Method->clickNoteList();
  } else {
  }

  setNoteLabel();

  saveRecycle();
  saveNotesList();

  if (getNotesListCount() == 0) {
    loadEmptyNote();
  }
}

void NotesList::loadEmptyNote() {
  currentMDFile = "";
  mw_one->m_Notes->MD2Html(currentMDFile);
  mw_one->m_Notes->loadMemoQML();
  mw_one->ui->lblNoteName->setText("");

  mw_one->m_Method->saveCurNoteIndex();
}

void NotesList::on_actionRename_Note_triggered() {
  int notebookIndex = getNoteBookCurrentIndex();
  int noteIndex = getNotesListCurrentIndex();
  if (notebookIndex < 0) return;
  if (noteIndex < 0) return;

  bool ok = false;
  QString text;
  QFrame *frame = new QFrame(mw_one);
  QVBoxLayout *vbox = new QVBoxLayout;
  frame->setLayout(vbox);
  QInputDialog *idlg = new QInputDialog(mw_one);
  idlg->hide();
  vbox->addWidget(idlg);

  idlg->setWindowFlag(Qt::FramelessWindowHint);
  QString style =
      "QDialog{background: "
      "rgb(244,237,241);border-radius:10px;border:2px solid red;}";

  idlg->setStyleSheet(style);
  idlg->setOkButtonText(tr("Ok"));
  idlg->setCancelButtonText(tr("Cancel"));
  idlg->setContentsMargins(10, 10, 10, 10);

  idlg->setWindowTitle(tr("Rename Note"));
  idlg->setTextValue(mw_one->m_Method->getText0(mw_one->ui->qwNoteList,
                                                getNotesListCurrentIndex()));
  idlg->setLabelText(tr("Note Name"));

  frame->setGeometry(25, -100, mw_one->width() - 50, this->height());
  idlg->show();

  frame->show();

  if (QDialog::Accepted == idlg->exec()) {
    ok = true;
    text = idlg->textValue();
    frame->close();
  } else {
    frame->close();
    return;
  }

  if (ok && !text.isEmpty()) {
    tw->setCurrentItem(tw->topLevelItem(notebookIndex)->child(noteIndex));
    ui->editName->setText(text);
    on_btnRename_clicked();

    mw_one->m_Method->modifyItemText0(mw_one->ui->qwNoteList, noteIndex, text);
  }
}

void NotesList::on_actionMoveUp_Note_triggered() {
  int indexBook = getNoteBookCurrentIndex();
  int indexNote = getNotesListCurrentIndex();
  if (indexBook < 0) return;
  if (indexNote <= 0) return;

  tw->setCurrentItem(tw->topLevelItem(indexBook)->child(indexNote));
  on_btnUp_clicked();

  mw_one->m_Method->clickNoteBook();
  setNotesListCurrentIndex(indexNote - 1);
  mw_one->m_Method->saveCurNoteIndex();
}

void NotesList::on_actionMoveDown_Note_triggered() {
  int indexBook = getNoteBookCurrentIndex();
  int indexNote = getNotesListCurrentIndex();
  if (indexBook < 0) return;
  if (indexNote < 0) return;
  if (indexNote + 1 == getNotesListCount()) return;

  tw->setCurrentItem(tw->topLevelItem(indexBook)->child(indexNote));
  on_btnDown_clicked();

  mw_one->m_Method->clickNoteBook();
  setNotesListCurrentIndex(indexNote + 1);
  mw_one->m_Method->saveCurNoteIndex();
}

void NotesList::on_actionImport_Note_triggered() {
  int indexBook = getNoteBookCurrentIndex();

  tw->setCurrentItem(tw->topLevelItem(indexBook));
  bool isOk = on_btnImport_clicked();

  if (isOk) {
    mw_one->m_Method->clickNoteBook();
    setNotesListCurrentIndex(getNotesListCount() - 1);
    mw_one->m_Method->clickNoteList();
    mw_one->m_Method->saveCurNoteIndex();
  }
}

void NotesList::on_actionExport_Note_triggered() {
  int indexBook = getNoteBookCurrentIndex();
  int indexNote = getNotesListCurrentIndex();

  if (indexBook < 0) return;
  if (indexNote < 0) return;

  tw->setCurrentItem(tw->topLevelItem(indexBook)->child(indexNote));
  on_btnExport_clicked();
}

void NotesList::init_NotesListMenu(QMenu *mainMenu) {
  QAction *actNew = new QAction(tr("New Note"));
  QAction *actDel = new QAction(tr("Del Note"));
  QAction *actRename = new QAction(tr("Rename Note"));
  QAction *actMoveUp = new QAction(tr("Move Up"));
  QAction *actMoveDown = new QAction(tr("Move Down"));
  QAction *actImport = new QAction(tr("Import"));
  QAction *actExport = new QAction(tr("Export"));

  connect(actNew, &QAction::triggered, this,
          &NotesList::on_actionAdd_Note_triggered);
  connect(actDel, &QAction::triggered, this,
          &NotesList::on_actionDel_Note_triggered);
  connect(actRename, &QAction::triggered, this,
          &NotesList::on_actionRename_Note_triggered);

  connect(actMoveUp, &QAction::triggered, this,
          &NotesList::on_actionMoveUp_Note_triggered);
  connect(actMoveDown, &QAction::triggered, this,
          &NotesList::on_actionMoveDown_Note_triggered);

  connect(actImport, &QAction::triggered, this,
          &NotesList::on_actionImport_Note_triggered);
  connect(actExport, &QAction::triggered, this,
          &NotesList::on_actionExport_Note_triggered);

  QString qss =
      "QMenu {"
      "border: 1px solid rgb(172, 172, 172);"
      "border-radius: 3px; }"
      "QMenu::item {"
      "border-bottom: 1px solid rgb(172, 172, 172);"
      "padding:10px 10px;"
      "margin:0px 0px; }"
      "QMenu::item:selected {"
      "background-color: rgb(62, 186, 231); }";

  mainMenu->addAction(actNew);
  mainMenu->addAction(actRename);
  mainMenu->addAction(actDel);

  mainMenu->addAction(actImport);
  mainMenu->addAction(actExport);

  mainMenu->addAction(actMoveUp);
  mainMenu->addAction(actMoveDown);

  mainMenu->setStyleSheet(qss);
}

void NotesList::setNoteLabel() {
  mw_one->ui->lblNoteBook->setText(tr("Note Book") + " : " +
                                   QString::number(getNoteBookCount()));
  QString notesSum = QString::number(getNotesListCount());
  mw_one->ui->lblNoteList->setText(tr("Note List") + " : " + notesSum);
  int index = getNoteBookCurrentIndex();
  mw_one->m_Method->modifyItemText3(mw_one->ui->qwNoteBook, index,
                                    tr("Sum") + " : " + notesSum);
}
