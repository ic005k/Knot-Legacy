#include "NotesList.h"

#include "src/mainwindow.h"
#include "ui_NotesList.h"
#include "ui_mainwindow.h"

extern MainWindow* mw_one;
extern QString iniDir, privateDir;
extern bool isImport, isAndroid;

dlgNotesList::dlgNotesList(QWidget* parent)
    : QDialog(parent), ui(new Ui::dlgNotesList) {
  ui->setupUi(this);

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
  setModal(true);
  this->layout()->setSpacing(5);
  this->layout()->setContentsMargins(2, 2, 2, 2);
  ui->frame1->hide();

  tw->verticalScrollBar()->setStyleSheet(mw_one->vsbarStyleSmall);
  tw->setStyleSheet(mw_one->treeStyle);
  tw->setVerticalScrollMode(QTreeWidget::ScrollPerPixel);
  QScroller::grabGesture(tw, QScroller::LeftMouseButtonGesture);
  mw_one->setSCrollPro(tw);

  twrb->verticalScrollBar()->setStyleSheet(mw_one->vsbarStyleSmall);
  twrb->setStyleSheet(mw_one->treeStyle);
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
  font.setPointSize(10);
  font.setBold(true);
  ui->lblCount->setFont(font);

  initNotesList();
  initRecycle();
}

dlgNotesList::~dlgNotesList() { delete ui; }

bool dlgNotesList::eventFilter(QObject* watch, QEvent* evn) {
  if (evn->type() == QEvent::KeyPress) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      on_btnClose_clicked();
      return true;
    }
  }

  return QWidget::eventFilter(watch, evn);
}

void dlgNotesList::on_btnClose_clicked() { this->close(); }

void dlgNotesList::on_btnNewNoteBook_clicked() {
  QTreeWidgetItem* item = new QTreeWidgetItem();
  item->setText(0, ui->editBook->text().trimmed());
  ui->treeWidget->addTopLevelItem(item);
  ui->treeWidget->setCurrentItem(item);
  on_btnNewNote_clicked();
  isSave = true;
}

void dlgNotesList::on_btnNewNote_clicked() {
  if (ui->treeWidget->topLevelItemCount() == 0) return;

  int rand = QRandomGenerator::global()->generate();

  QString noteFile = "memo/" + mw_one->mydlgMainNotes->getDateTimeStr() + "_" +
                     QString::number(rand) + ".md";
  QTreeWidgetItem* topitem = ui->treeWidget->currentItem();
  if (topitem->parent() != NULL) topitem = topitem->parent();

  QTreeWidgetItem* item1 = new QTreeWidgetItem(topitem);
  item1->setText(0, ui->editNote->text().trimmed());
  item1->setText(1, noteFile);
  QTextEdit* edit = new QTextEdit();
  mw_one->TextEditToFile(edit, noteFile);

  ui->treeWidget->setCurrentItem(topitem->child(topitem->childCount() - 1));
  on_treeWidget_itemClicked(item1, 0);

  isSave = true;
}

void dlgNotesList::on_treeWidget_itemClicked(QTreeWidgetItem* item,
                                             int column) {
  Q_UNUSED(column);

  if (ui->treeWidget->topLevelItemCount() == 0) return;

  if (item->parent() != NULL) {
    if (tw->currentIndex().row() == 0) {
      if (tw->currentIndex().parent().row() == 0) {
        item->setText(1, "memo/memo.md");
      }
    }

    QSettings Reg(privateDir + "notes.ini", QSettings::IniFormat);
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

void dlgNotesList::on_btnRename_clicked() {
  if (ui->treeWidget->topLevelItemCount() == 0) return;

  QTreeWidgetItem* item = ui->treeWidget->currentItem();
  item->setText(0, ui->editName->text().trimmed());
  setNoteName(item->text(0));

  isSave = true;
}

void dlgNotesList::setNoteName(QString name) {
  QFontMetrics fontWidth(mw_one->ui->lblNoteName->font());
  QString elideNote = fontWidth.elidedText(name, Qt::ElideRight, 240);

  mw_one->ui->lblNoteName->setText(elideNote);
  mw_one->ui->lblNoteName->setToolTip(name);
}

void dlgNotesList::on_btnDel_clicked() {
  QTreeWidgetItem* item = ui->treeWidget->currentItem();

  if (item->parent() == NULL) {
    if (tw->currentIndex().row() == 0) return;
  } else {
    if (tw->currentIndex().row() == 0) {
      if (tw->currentIndex().parent().row() == 0) return;
    }
  }

  if (mw_one->showMsgBox(
          "Kont", tr("Delete?") + "\n\n" + tw->currentItem()->text(0) + "\n",
          "", 2)) {
    if (item->parent() == NULL) {
      int count = item->childCount();
      for (int i = 0; i < count; i++) {
        QTreeWidgetItem* childItem = new QTreeWidgetItem;
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
  }

  tw->setFocus();
  isSave = true;
}

void dlgNotesList::addItem(QTreeWidget* tw, QTreeWidgetItem* item) {
  tw->setFocus();
  QTreeWidgetItem* curItem = tw->currentItem();
  if (curItem->parent() == NULL) {
    curItem->addChild(item);
  } else {
    curItem->parent()->addChild(item);
  }

  tw->expandAll();
}

void dlgNotesList::delFile(QString file) {
  QFile _file(file);
  if (_file.exists()) _file.remove();
  _file.close();
}

void dlgNotesList::on_btnImport_clicked() {
  if (ui->treeWidget->topLevelItemCount() == 0) return;

  QString fileName;
  fileName =
      QFileDialog::getOpenFileName(this, tr("Knot"), "", tr("MD File (*.*)"));

  if (fileName.isNull()) return;

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
    return;
  }

  if (QFile(fileName).exists()) {
    QTreeWidgetItem* item = ui->treeWidget->currentItem();

    QTreeWidgetItem* item1;
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
    QTextEdit* edit = new QTextEdit();
    edit->setAcceptRichText(false);
    edit->setPlainText(str);
    mw_one->TextEditToFile(edit, currentMDFile);

    item1->setText(1, a);

    on_treeWidget_itemClicked(item1, 0);
  }

  isSave = true;
}

void dlgNotesList::on_btnExport_clicked() {
  if (ui->treeWidget->topLevelItemCount() == 0) return;

  QTreeWidgetItem* item = tw->currentItem();
  if (item->parent() == NULL) return;

  QString fileName;
  QFileDialog fd;
  fileName = fd.getSaveFileName(this, "Knot.md", "", tr("MD File(*.*)"));

  if (fileName == "") return;

  QString mdfile = iniDir + item->text(1);

  QString str = mw_one->loadText(mdfile);
  QTextEdit* edit = new QTextEdit();
  edit->setAcceptRichText(false);
  edit->setPlainText(str);

  mw_one->TextEditToFile(edit, fileName);
}

void dlgNotesList::closeEvent(QCloseEvent* event) {
  Q_UNUSED(event);

  if (isSave) {
    saveNotesList();
    saveRecycle();
  }
}

void dlgNotesList::saveNotesList() {
  mw_one->isSelf = true;

  QSettings Reg(iniDir + "mainnotes.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif

  int count = tw->topLevelItemCount();
  Reg.setValue("/MainNotes/topItemCount", count);
  for (int i = 0; i < count; i++) {
    QTreeWidgetItem* topItem = tw->topLevelItem(i);
    QString strtop = topItem->text(0);
    Reg.setValue("/MainNotes/strTopItem" + QString::number(i), strtop);

    int childCount = topItem->childCount();
    Reg.setValue("/MainNotes/childCount" + QString::number(i), childCount);

    for (int j = 0; j < childCount; j++) {
      QTreeWidgetItem* childItem = tw->topLevelItem(i)->child(j);
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
    QTreeWidgetItem* topItem = twrb->topLevelItem(i);
    QString strtop = topItem->text(0);
    Reg.setValue("/MainNotes/rbstrTopItem" + QString::number(i), strtop);

    int childCount = topItem->childCount();
    Reg.setValue("/MainNotes/rbchildCount" + QString::number(i), childCount);

    for (int j = 0; j < childCount; j++) {
      QTreeWidgetItem* childItem = twrb->topLevelItem(i)->child(j);
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
  for (int i = 0; i < topCount; i++) {
    QString strTop =
        Reg.value("/MainNotes/strTopItem" + QString::number(i)).toString();
    QTreeWidgetItem* topItem = new QTreeWidgetItem;
    topItem->setText(0, strTop);
    QFont font = this->font();
    font.setBold(true);
    topItem->setFont(0, font);

    int childCount =
        Reg.value("/MainNotes/childCount" + QString::number(i)).toInt();
    for (int j = 0; j < childCount; j++) {
      QString str0, str1;
      str0 = Reg.value("/MainNotes/childItem0" + QString::number(i) +
                       QString::number(j))
                 .toString();
      str1 = Reg.value("/MainNotes/childItem1" + QString::number(i) +
                       QString::number(j))
                 .toString();

      QTreeWidgetItem* childItem = new QTreeWidgetItem(topItem);
      childItem->setText(0, str0);
      childItem->setText(1, str1);
    }
    tw->addTopLevelItem(topItem);
  }

  tw->expandAll();

  QSettings RegNotes(privateDir + "notes.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  RegNotes.setIniCodec("utf-8");
#endif
  QString curmd =
      RegNotes.value("/MainNotes/currentItem", "memo/xxx.md").toString();
  QString cm = iniDir + curmd;

  if (QFile(cm).exists()) {
    currentMDFile = cm;
  } else {
    if (tw->topLevelItemCount() > 0) {
      QTreeWidgetItem* topItem = tw->topLevelItem(0);
      QTreeWidgetItem* childItem = topItem->child(0);
      tw->setCurrentItem(childItem);
      currentMDFile = iniDir + childItem->text(1);
    }
  }
  qDebug() << currentMDFile;

  bool stop = false;
  for (int i = 0; i < tw->topLevelItemCount(); i++) {
    QTreeWidgetItem* topItem = tw->topLevelItem(i);
    int childCount = topItem->childCount();

    for (int j = 0; j < childCount; j++) {
      QTreeWidgetItem* childItem = topItem->child(j);
      QString strChild1 = childItem->text(1);
      if (strChild1 == curmd) {
        stop = true;
        tw->setCurrentItem(childItem);
        ui->editName->setText(childItem->text(0));
        if (mw_one->initMain || isImport)
          on_treeWidget_itemClicked(childItem, 0);

        break;
      }
    }
    if (stop) break;
  }

  if (ui->treeWidget->topLevelItemCount() == 0) {
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0, tr("Default Notebook"));

    QTreeWidgetItem* item1 = new QTreeWidgetItem(item);
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
    QTreeWidgetItem* topItem = new QTreeWidgetItem;
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

      QTreeWidgetItem* childItem = new QTreeWidgetItem(topItem);
      childItem->setText(0, str0);
      childItem->setText(1, str1);
    }
    twrb->addTopLevelItem(topItem);
  }

  twrb->expandAll();

  if (twrb->topLevelItemCount() == 0) {
    QTreeWidgetItem* topItem = new QTreeWidgetItem;
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
}

void dlgNotesList::on_btnBack_clicked() {
  ui->frame1->hide();
  ui->frame0->show();
  setWinPos();
  tw->setFocus();
}

void dlgNotesList::on_btnRestore_clicked() {
  QTreeWidgetItem* curItem = twrb->currentItem();
  if (curItem->parent() == NULL) {
    return;
  } else {
    QTreeWidgetItem* item = new QTreeWidgetItem;
    item->setText(0, curItem->text(0));
    item->setText(1, curItem->text(1));
    addItem(tw, item);
    curItem->parent()->removeChild(curItem);
  }

  isSave = true;
}

void dlgNotesList::on_btnDel_2_clicked() {
  QTreeWidgetItem* curItem = twrb->currentItem();
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
  int w = mw_one->width() * 3 / 4;
  int x = mw_one->geometry().x() + mw_one->width() - w - 2;
  this->setGeometry(
      x, mw_one->geometry().y() + mw_one->ui->lblNoteName->height() * 1.2, w,
      mw_one->ui->qwNotes->height());
}

void dlgNotesList::clearFiles() {
  QString tempDir = iniDir + "memo/";
  files.clear();
  QStringList fmt = QString("md;html;jpg;bmp;png").split(';');
  getAllFiles(tempDir, files, fmt);

  clearMD_Pic(tw);
  clearMD_Pic(twrb);

  for (int i = 0; i < files.count(); i++) {
    QString a = files.at(i);
    QFile file(a);
    if (a != iniDir + "memo/memo.md") file.remove();
  }
}

void dlgNotesList::clearMD_Pic(QTreeWidget* tw) {
  for (int i = 0; i < tw->topLevelItemCount(); i++) {
    QTreeWidgetItem* topItem = tw->topLevelItem(i);
    int childCount = topItem->childCount();
    for (int j = 0; j < childCount; j++) {
      QString str = topItem->child(j)->text(1);
      removeFromFiles(str);
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

void dlgNotesList::removeFromFiles(QString str) {
  for (int i = 0; i < files.count(); i++) {
    QString str0 = files.at(i);
    if (str0.contains(str)) {
      files.removeAt(i);
      break;
    }
  }
}

void dlgNotesList::getAllFiles(const QString& foldPath, QStringList& folds,
                               const QStringList& formats) {
  QDirIterator it(foldPath, QDir::Files | QDir::NoDotAndDotDot,
                  QDirIterator::Subdirectories);
  while (it.hasNext()) {
    it.next();
    QFileInfo fileInfo = it.fileInfo();
    if (formats.contains(fileInfo.suffix())) {  //检测格式，按需保存
      folds << fileInfo.absoluteFilePath();
    }
  }
}

void dlgNotesList::on_btnFind_clicked() {
  QString strFind = ui->editFind->text().trimmed().toLower();
  if (strFind == "") return;
  findResultList.clear();
  int count = tw->topLevelItemCount();
  for (int i = 0; i < count; i++) {
    QTreeWidgetItem* topItem = tw->topLevelItem(i);
    if (topItem->text(0).toLower().contains(strFind))
      findResultList.append(topItem);
    int childCount = topItem->childCount();
    for (int j = 0; j < childCount; j++) {
      QTreeWidgetItem* childItem = topItem->child(j);
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
    ui->lblCount->setText(QString::number(findCount + 1) + "\n" +
                          QString::number(findResultList.count()));
  }
}

void dlgNotesList::on_btnPrev_clicked() {
  findCount--;
  if (findCount < 0) findCount = 0;
  tw->setCurrentItem(findResultList.at(findCount));
  tw->scrollToItem(tw->currentItem());
  ui->lblCount->setText(QString::number(findCount + 1) + "\n" +
                        QString::number(findResultList.count()));

  if (isAndroid) {
    if (pAndroidKeyboard->isVisible()) pAndroidKeyboard->setVisible(false);
  }
}

void dlgNotesList::on_btnNext_clicked() {
  findCount++;
  if (findCount >= findResultList.count())
    findCount = findResultList.count() - 1;
  tw->setCurrentItem(findResultList.at(findCount));
  tw->scrollToItem(tw->currentItem());
  ui->lblCount->setText(QString::number(findCount + 1) + "\n" +
                        QString::number(findResultList.count()));

  if (isAndroid) {
    if (pAndroidKeyboard->isVisible()) pAndroidKeyboard->setVisible(false);
  }
}

void dlgNotesList::on_editFind_textChanged(const QString& arg1) {
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
