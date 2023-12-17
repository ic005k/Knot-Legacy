#include "NotesList.h"

#include "src/MainWindow.h"
#include "ui_MainWindow.h"
#include "ui_NotesList.h"

extern MainWindow *mw_one;
extern QString iniDir, privateDir;
extern bool isAndroid;

dlgNotesList::dlgNotesList(QWidget *parent)
    : QDialog(parent), ui(new Ui::dlgNotesList) {
  ui->setupUi(this);
  this->installEventFilter(this);
  mw_one->set_btnStyle(this);

  connect(pAndroidKeyboard, &QInputMethod::visibleChanged, this,
          &dlgNotesList::on_KVChanged);

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
  mw_one->setSCrollPro(tw);

  twrb->verticalScrollBar()->setStyleSheet(mw_one->vsbarStyleSmall);
  twrb->setVerticalScrollMode(QTreeWidget::ScrollPerPixel);
  QScroller::grabGesture(twrb, QScroller::LeftMouseButtonGesture);
  mw_one->setSCrollPro(twrb);

  ui->treeWidget->headerItem()->setText(0, tr("Notebook"));
  ui->treeWidget->setColumnHidden(1, true);
  twrb->header()->hide();
  twrb->setColumnHidden(1, true);
  twrb->setColumnWidth(0, 180);

  QString path = iniDir + "memo/";
  QDir dir(path);
  if (!dir.exists()) dir.mkdir(path);

  ui->editBook->setStyleSheet(
      mw_one->myEditRecord->ui->editAmount->styleSheet());
  ui->editName->setStyleSheet(
      mw_one->myEditRecord->ui->editAmount->styleSheet());
  ui->editNote->setStyleSheet(
      mw_one->myEditRecord->ui->editAmount->styleSheet());
  ui->editFind->setStyleSheet(
      mw_one->myEditRecord->ui->editAmount->styleSheet());
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

dlgNotesList::~dlgNotesList() { delete ui; }

bool dlgNotesList::eventFilter(QObject *watch, QEvent *evn) {
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

void dlgNotesList::on_btnClose_clicked() { this->close(); }

void dlgNotesList::on_btnNewNoteBook_clicked() {
  QTreeWidgetItem *item = new QTreeWidgetItem();
  item->setText(0, ui->editBook->text().trimmed());
  item->setForeground(0, Qt::red);
  ui->treeWidget->addTopLevelItem(item);
  ui->treeWidget->setCurrentItem(item);
  on_btnNewNote_clicked();
  isSave = true;
}

void dlgNotesList::on_btnNewNote_clicked() {
  if (ui->treeWidget->topLevelItemCount() == 0) return;

  int rand = QRandomGenerator::global()->generate();
  if (rand < 0) rand = 0 - rand;

  QString noteFile = "memo/" + mw_one->mydlgMainNotes->getDateTimeStr() + "_" +
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

  isSave = true;
}

void dlgNotesList::on_treeWidget_itemClicked(QTreeWidgetItem *item,
                                             int column) {
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
    mw_one->mydlgMainNotes->MD2Html(currentMDFile);
    mw_one->mydlgMainNotes->loadMemoQML();

    setNoteName(item->text(0));
  }

  ui->editName->setText(item->text(0));

  qDebug() << "currentMDFile " << currentMDFile;
}

QString dlgNotesList::getCurrentMDFile() {
  QSettings Reg(iniDir + "curmd.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif

  QString curmd =
      Reg.value("/MainNotes/currentItem", iniDir + "memo/xxx.md").toString();
  mw_one->ui->lblNoteName->setText(
      Reg.value("/MainNotes/NoteName", tr("Note Name")).toString());

  return curmd;
}

void dlgNotesList::on_btnRename_clicked() {
  if (ui->treeWidget->topLevelItemCount() == 0) return;

  QTreeWidgetItem *item = ui->treeWidget->currentItem();
  item->setText(0, ui->editName->text().trimmed());
  if (item->parent() != NULL) setNoteName(item->text(0));

  isSave = true;
}

void dlgNotesList::setNoteName(QString name) {
  // QFontMetrics fontWidth(mw_one->ui->lblNoteName->font());
  // QString elideNote = fontWidth.elidedText(name, Qt::ElideRight, 240);
  mw_one->ui->lblNoteName->adjustSize();
  mw_one->ui->lblNoteName->setWordWrap(true);
  mw_one->ui->lblNoteName->setText(name);
  mw_one->ui->lblNoteName->setToolTip(name);
}

void dlgNotesList::on_btnDel_clicked() {
  QTreeWidgetItem *item = ui->treeWidget->currentItem();

  if (item->parent() == NULL) {
    if (tw->currentIndex().row() == 0) return;
  } else {
    if (tw->currentIndex().row() == 0) {
      if (tw->currentIndex().parent().row() == 0) return;
    }
  }

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
  isSave = true;
}

void dlgNotesList::addItem(QTreeWidget *tw, QTreeWidgetItem *item) {
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

void dlgNotesList::delFile(QString file) {
  QFile _file(file);
  if (_file.exists()) _file.remove();
  _file.close();
}

bool dlgNotesList::on_btnImport_clicked() {
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
  QString fileAndroid = mw_one->mydlgReader->getUriRealPath(fileName);
  isMD = fileAndroid.contains(".md");

  QStringList list = fileAndroid.split("/");
  QString str = list.at(list.count() - 1);
  if (str.toInt() > 0) isMD = true;
  strInfo = fileAndroid;

#endif

  if (!isMD) {
    QMessageBox box;
    box.setText(tr("Invalid Markdown file.") + "\n\n" + strInfo);
    box.exec();
    return false;
  }

  if (QFile(fileName).exists()) {
    QTreeWidgetItem *item = ui->treeWidget->currentItem();

    QTreeWidgetItem *item1;
    if (item->parent() == NULL) {
      item1 = new QTreeWidgetItem(item);
      item1->setText(0, tr("Notes Imported"));
    } else {
      item1 = new QTreeWidgetItem(item->parent());
      item1->setText(0, tr("Notes Imported"));
    }
    tw->setCurrentItem(item1);
    QString a = "memo/" + mw_one->mydlgMainNotes->getDateTimeStr() + ".md";
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

  isSave = true;
  return true;
}

void dlgNotesList::on_btnExport_clicked() {
  if (ui->treeWidget->topLevelItemCount() == 0) return;

  QTreeWidgetItem *item = tw->currentItem();
  if (item->parent() == NULL) return;

  QString fileName;
  QFileDialog fd;
  fileName = fd.getSaveFileName(this, "Knot.md", "", tr("MD File(*.*)"));

  if (fileName == "") return;

  QString mdfile = iniDir + item->text(1);

  QString str = mw_one->loadText(mdfile);
  QTextEdit *edit = new QTextEdit();
  edit->setAcceptRichText(false);
  edit->setPlainText(str);

  mw_one->TextEditToFile(edit, fileName);
}

void dlgNotesList::closeEvent(QCloseEvent *event) {
  Q_UNUSED(event);

  if (isSave) {
    saveNotesList();
    saveRecycle();
  }

  mw_one->ui->btnBackNotes->show();
  mw_one->ui->btnEdit->show();
  mw_one->ui->btnNotesList->show();
  mw_one->ui->btnSetKey->show();
  mw_one->ui->btnPDF->show();
}

void dlgNotesList::saveNotesList() {
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
}

void dlgNotesList::saveRecycle() {
  mw_one->isSelf = true;

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
}

void dlgNotesList::initNotesList() {
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

  /* QSettings RegNotes(iniDir + "curmd.ini", QSettings::IniFormat);
 #if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
   RegNotes.setIniCodec("utf-8");
 #endif
   QString curmd = "memo/xxx.md";
   curmd = RegNotes.value("/MainNotes/currentItem", "memo/xxx.md").toString();
   QString cm = iniDir + curmd;
   if (QFile(cm).exists()) {
     currentMDFile = cm;
   } else {
     if (tw->topLevelItemCount() > 0) {
       QTreeWidgetItem *topItem = tw->topLevelItem(0);
       QTreeWidgetItem *childItem = topItem->child(0);
       tw->setCurrentItem(childItem);
       curmd = childItem->text(1);
       currentMDFile = iniDir + curmd;
     }
   }
   qDebug() << "init notes list" << currentMDFile;*/

  if (ui->treeWidget->topLevelItemCount() == 0) {
    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, tr("Default Notebook"));

    QTreeWidgetItem *item1 = new QTreeWidgetItem(item);
    item1->setText(0, tr("My Notes"));
    QString a = "memo/memo.md";
    QString mdfile = iniDir + a;
    currentMDFile = mdfile;
    item1->setText(1, a);
    ui->treeWidget->addTopLevelItem(item);

    ui->treeWidget->setFocus();
    ui->treeWidget->expandAll();
    ui->treeWidget->setCurrentItem(item->child(0));

    on_treeWidget_itemClicked(ui->treeWidget->currentItem(), 0);

    saveNotesList();
  }
}

void dlgNotesList::initRecycle() {
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

void dlgNotesList::on_btnRecycle_clicked() {
  ui->frame0->hide();
  ui->frame1->show();
  setWinPos();
  twrb->setFocus();
  twrb->setCurrentItem(twrb->topLevelItem(0));
}

void dlgNotesList::on_btnBack_clicked() {
  ui->frame1->hide();
  ui->frame0->show();
  setWinPos();
  tw->setFocus();
}

void dlgNotesList::on_btnRestore_clicked() {
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

    mw_one->mySearchDialog->addItemBakList(mw_one->ui->qwNoteList, str0, "", "",
                                           str1, 0);

    curItem->parent()->removeChild(curItem);
  }

  isSave = true;
}

void dlgNotesList::on_btnDel_2_clicked() {
  QTreeWidgetItem *curItem = twrb->currentItem();
  if (curItem->parent() == NULL) {
    return;
  } else {
    QString md = iniDir + curItem->text(1);
    delFile(md);
    curItem->parent()->removeChild(curItem);
  }

  isSave = true;
}

void dlgNotesList::setWinPos() {
  int w = mw_one->width();
  int x = mw_one->geometry().x();
  this->setGeometry(x, mw_one->geometry().y(), w, mw_one->height());
  mw_one->ui->btnBackNotes->hide();
  mw_one->ui->btnEdit->hide();
  mw_one->ui->btnNotesList->hide();
  mw_one->ui->btnSetKey->hide();
  mw_one->ui->btnPDF->hide();
}

void dlgNotesList::clearFiles() {
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

void dlgNotesList::clearMD_Pic(QTreeWidget *tw) {
  for (int i = 0; i < tw->topLevelItemCount(); i++) {
    QTreeWidgetItem *topItem = tw->topLevelItem(i);
    int childCount = topItem->childCount();
    for (int j = 0; j < childCount; j++) {
      QString str = topItem->child(j)->text(1);
      removePicFromMD(iniDir + str);
    }
  }
}

void dlgNotesList::removePicFromMD(QString mdfile) {
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

void dlgNotesList::getAllFiles(const QString &foldPath, QStringList &folds,
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

void dlgNotesList::on_btnFind_clicked() {
  QString strFind = ui->editFind->text().trimmed().toLower();
  if (strFind == "") {
    ui->btnPrev->hide();
    ui->btnNext->hide();
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
    findCount = 0;
    ui->lblCount->setText(QString::number(findCount + 1) + "->" +
                          QString::number(findResultList.count()));

    ui->btnPrev->show();
    ui->btnNext->show();
  } else {
    ui->btnPrev->hide();
    ui->btnNext->hide();
    ui->lblCount->setText("0");
  }
}

void dlgNotesList::on_btnPrev_clicked() {
  findCount--;
  if (findCount < 0) findCount = findResultList.count() - 1;  // findCount = 0;
  tw->setCurrentItem(findResultList.at(findCount));
  tw->scrollToItem(tw->currentItem());
  ui->lblCount->setText(QString::number(findCount + 1) + "->" +
                        QString::number(findResultList.count()));

  if (isAndroid) {
    if (pAndroidKeyboard->isVisible()) pAndroidKeyboard->setVisible(false);
  }
}

void dlgNotesList::on_btnNext_clicked() {
  if (ui->btnNext->isHidden()) return;

  findCount++;
  if (findCount >= findResultList.count())
    findCount = 0;  // findResultList.count() - 1;
  tw->setCurrentItem(findResultList.at(findCount));
  tw->scrollToItem(tw->currentItem());
  ui->lblCount->setText(QString::number(findCount + 1) + "->" +
                        QString::number(findResultList.count()));

  if (isAndroid) {
    if (pAndroidKeyboard->isVisible()) pAndroidKeyboard->setVisible(false);
  }
}

void dlgNotesList::on_editFind_textChanged(const QString &arg1) {
  if (arg1.trimmed() == "") ui->lblCount->setText("0");
  on_btnFind_clicked();
}

void dlgNotesList::on_editFind_returnPressed() {
  if (ui->btnNext->isEnabled()) on_btnNext_clicked();
}

void dlgNotesList::on_KVChanged() {
  if (!pAndroidKeyboard->isVisible()) {
  } else {
  }
}

void dlgNotesList::moveBy(int ud) {
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
        isSave = true;
      }
    }
    if (ud == 1) {
      if (index + 1 <= parentItem->childCount() - 1) {
        parentItem->removeChild(item);
        parentItem->insertChild(index + 1, item);
        tw->setCurrentItem(item);
        tw->scrollToItem(item);
        isSave = true;
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
        isSave = true;
      }
    }
    if (ud == 1) {
      if (index + 1 <= tw->topLevelItemCount() - 1) {
        tw->takeTopLevelItem(index);
        tw->insertTopLevelItem(index + 1, item);
        tw->setCurrentItem(item);
        tw->scrollToItem(item);
        tw->expandAll();
        isSave = true;
      }
    }
  }
}

void dlgNotesList::on_btnUp_clicked() { moveBy(-1); }

void dlgNotesList::on_btnDown_clicked() { moveBy(1); }

void dlgNotesList::on_actionAdd_NoteBook_triggered() {
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
    mw_one->mySearchDialog->addItemBakList(mw_one->ui->qwNoteBook, text, "", "",
                                           "", 0);

    int count = mw_one->mySearchDialog->getCountBakList(mw_one->ui->qwNoteBook);
    mw_one->mySearchDialog->setCurrentIndexBakList(mw_one->ui->qwNoteBook,
                                                   count - 1);
    mw_one->mySearchDialog->clickNoteBook();
  }
}

void dlgNotesList::on_actionDel_NoteBook_triggered() {
  if (getNoteBookCount() == 0) return;

  int index = getNoteBookCurrentIndex();
  if (!mw_one->showMsgBox(
          "Knot",
          tr("Whether to remove") + "  " +
              mw_one->mySearchDialog->getText0(mw_one->ui->qwNoteBook, index) +
              " ? ",
          "", 2))
    return;

  tw->setCurrentItem(tw->topLevelItem(index));
  on_btnDel_clicked();

  mw_one->mySearchDialog->delItemBakList(mw_one->ui->qwNoteBook, index);
}

void dlgNotesList::on_actionRename_NoteBook_triggered() {
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
  idlg->setTextValue(mw_one->mySearchDialog->getText0(
      mw_one->ui->qwNoteBook, getNoteBookCurrentIndex()));
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
    int index = getNoteBookCurrentIndex();
    tw->setCurrentItem(tw->topLevelItem(index));
    ui->editName->setText(text);
    on_btnRename_clicked();

    mw_one->mySearchDialog->modifyItemText0(mw_one->ui->qwNoteBook, index,
                                            text);
  }
}

int dlgNotesList::getNoteBookCount() {
  int count = mw_one->mySearchDialog->getCountBakList(mw_one->ui->qwNoteBook);
  return count;
}

int dlgNotesList::getNotesListCount() {
  int count = mw_one->mySearchDialog->getCountBakList(mw_one->ui->qwNoteList);
  return count;
}

int dlgNotesList::getNoteBookCurrentIndex() {
  int index =
      mw_one->mySearchDialog->getCurrentIndexBakList(mw_one->ui->qwNoteBook);
  return index;
}

int dlgNotesList::getNotesListCurrentIndex() {
  int index =
      mw_one->mySearchDialog->getCurrentIndexBakList(mw_one->ui->qwNoteList);
  return index;
}

void dlgNotesList::setNoteBookCurrentIndex(int index) {
  mw_one->mySearchDialog->setCurrentIndexBakList(mw_one->ui->qwNoteBook, index);
}

void dlgNotesList::setNotesListCurrentIndex(int index) {
  mw_one->mySearchDialog->setCurrentIndexBakList(mw_one->ui->qwNoteList, index);
}

void dlgNotesList::on_actionMoveUp_NoteBook_triggered() {
  int index = getNoteBookCurrentIndex();
  if (index == 0) return;

  QString text0 =
      mw_one->mySearchDialog->getText0(mw_one->ui->qwNoteBook, index);
  int oldIndex = index;
  tw->setCurrentItem(tw->topLevelItem(index));
  on_btnUp_clicked();

  mw_one->mySearchDialog->insertItem(mw_one->ui->qwNoteBook, text0, "", "", "",
                                     index - 1);
  mw_one->mySearchDialog->delItemBakList(mw_one->ui->qwNoteBook, oldIndex + 1);
  setNoteBookCurrentIndex(oldIndex - 1);
}

void dlgNotesList::on_actionMoveDown_NoteBook_triggered() {
  int index = getNoteBookCurrentIndex();
  if (index == getNoteBookCount() - 1) return;

  int oldIndex = index;
  tw->setCurrentItem(tw->topLevelItem(index));
  on_btnDown_clicked();

  loadAllNoteBook();

  setNoteBookCurrentIndex(oldIndex + 1);
}

void dlgNotesList::loadAllNoteBook() {
  mw_one->mySearchDialog->clearAllBakList(mw_one->ui->qwNoteBook);
  int count = mw_one->m_NotesList->tw->topLevelItemCount();
  for (int i = 0; i < count; i++) {
    QString str = mw_one->m_NotesList->tw->topLevelItem(i)->text(0);
    mw_one->mySearchDialog->addItemBakList(mw_one->ui->qwNoteBook, str, "", "",
                                           "", 0);
  }
}

void dlgNotesList::init_NoteBookMenu(QMenu *mainMenu) {
  QAction *actNew = new QAction(tr("New NoteBook"));
  QAction *actDel = new QAction(tr("Del NoteBook"));
  QAction *actRename = new QAction(tr("Rename NoteBook"));
  QAction *actMoveUp = new QAction(tr("Move Up"));
  QAction *actMoveDown = new QAction(tr("Move Down"));

  connect(actNew, &QAction::triggered, this,
          &dlgNotesList::on_actionAdd_NoteBook_triggered);
  connect(actDel, &QAction::triggered, this,
          &dlgNotesList::on_actionDel_NoteBook_triggered);
  connect(actRename, &QAction::triggered, this,
          &dlgNotesList::on_actionRename_NoteBook_triggered);

  connect(actMoveUp, &QAction::triggered, this,
          &dlgNotesList::on_actionMoveUp_NoteBook_triggered);
  connect(actMoveDown, &QAction::triggered, this,
          &dlgNotesList::on_actionMoveDown_NoteBook_triggered);

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

void dlgNotesList::on_actionAdd_Note_triggered() {
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
    int notebookIndex = getNoteBookCurrentIndex();
    tw->setCurrentItem(tw->topLevelItem(notebookIndex));
    ui->editNote->setText(text);
    on_btnNewNote_clicked();

    mw_one->mySearchDialog->addItemBakList(mw_one->ui->qwNoteList, text, "", "",
                                           "", 0);

    int count = mw_one->mySearchDialog->getCountBakList(mw_one->ui->qwNoteList);
    mw_one->mySearchDialog->setCurrentIndexBakList(mw_one->ui->qwNoteList,
                                                   count - 1);
    mw_one->mySearchDialog->clickNoteList();
  }
}

void dlgNotesList::on_actionDel_Note_triggered() {
  if (getNotesListCount() == 0) return;

  int notebookIndex = getNoteBookCurrentIndex();
  int notelistIndex = getNotesListCurrentIndex();
  if (!mw_one->showMsgBox("Knot",
                          tr("Whether to remove") + "  " +
                              mw_one->mySearchDialog->getText0(
                                  mw_one->ui->qwNoteList, notelistIndex) +
                              " ? ",
                          "", 2))
    return;

  tw->setCurrentItem(tw->topLevelItem(notebookIndex)->child(notelistIndex));
  on_btnDel_clicked();

  mw_one->mySearchDialog->delItemBakList(mw_one->ui->qwNoteList, notelistIndex);
  if (getNotesListCount() > 0) {
    setNotesListCurrentIndex(notelistIndex - 1);
    mw_one->mySearchDialog->clickNoteList();
  }
}

void dlgNotesList::on_actionRename_Note_triggered() {
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
  idlg->setTextValue(mw_one->mySearchDialog->getText0(
      mw_one->ui->qwNoteList, getNotesListCurrentIndex()));
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
    int notebookIndex = getNoteBookCurrentIndex();
    tw->setCurrentItem(
        tw->topLevelItem(notebookIndex)->child(getNotesListCurrentIndex()));
    ui->editName->setText(text);
    on_btnRename_clicked();

    mw_one->mySearchDialog->modifyItemText0(mw_one->ui->qwNoteList,
                                            getNotesListCurrentIndex(), text);
  }
}

void dlgNotesList::on_actionMoveUp_Note_triggered() {
  int indexBook = getNoteBookCurrentIndex();
  int indexNote = getNotesListCurrentIndex();
  if (indexNote == 0) return;

  tw->setCurrentItem(tw->topLevelItem(indexBook)->child(indexNote));
  on_btnUp_clicked();

  mw_one->mySearchDialog->clickNoteBook();
  setNotesListCurrentIndex(indexNote - 1);
  mw_one->mySearchDialog->saveCurNoteIndex();
}

void dlgNotesList::on_actionMoveDown_Note_triggered() {
  int indexBook = getNoteBookCurrentIndex();
  int indexNote = getNotesListCurrentIndex();
  if (indexNote + 1 == getNotesListCount()) return;

  tw->setCurrentItem(tw->topLevelItem(indexBook)->child(indexNote));
  on_btnDown_clicked();

  mw_one->mySearchDialog->clickNoteBook();
  setNotesListCurrentIndex(indexNote + 1);
  mw_one->mySearchDialog->saveCurNoteIndex();
}

void dlgNotesList::on_actionImport_Note_triggered() {
  int indexBook = getNoteBookCurrentIndex();
  int indexNote = getNotesListCurrentIndex();

  tw->setCurrentItem(tw->topLevelItem(indexBook)->child(indexNote));
  bool isOk = on_btnImport_clicked();

  if (isOk) {
    mw_one->mySearchDialog->clickNoteBook();
    setNotesListCurrentIndex(getNotesListCount() - 1);
    mw_one->mySearchDialog->clickNoteList();
    mw_one->mySearchDialog->saveCurNoteIndex();
  }
}

void dlgNotesList::on_actionExport_Note_triggered() {
  int indexBook = getNoteBookCurrentIndex();
  int indexNote = getNotesListCurrentIndex();
  tw->setCurrentItem(tw->topLevelItem(indexBook)->child(indexNote));
  on_btnExport_clicked();
}

void dlgNotesList::init_NotesListMenu(QMenu *mainMenu) {
  QAction *actNew = new QAction(tr("New Note"));
  QAction *actDel = new QAction(tr("Del Note"));
  QAction *actRename = new QAction(tr("Rename Note"));
  QAction *actMoveUp = new QAction(tr("Move Up"));
  QAction *actMoveDown = new QAction(tr("Move Down"));
  QAction *actImport = new QAction(tr("Import"));
  QAction *actExport = new QAction(tr("Export"));

  connect(actNew, &QAction::triggered, this,
          &dlgNotesList::on_actionAdd_Note_triggered);
  connect(actDel, &QAction::triggered, this,
          &dlgNotesList::on_actionDel_Note_triggered);
  connect(actRename, &QAction::triggered, this,
          &dlgNotesList::on_actionRename_Note_triggered);

  connect(actMoveUp, &QAction::triggered, this,
          &dlgNotesList::on_actionMoveUp_Note_triggered);
  connect(actMoveDown, &QAction::triggered, this,
          &dlgNotesList::on_actionMoveDown_Note_triggered);

  connect(actImport, &QAction::triggered, this,
          &dlgNotesList::on_actionImport_Note_triggered);
  connect(actExport, &QAction::triggered, this,
          &dlgNotesList::on_actionExport_Note_triggered);

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
