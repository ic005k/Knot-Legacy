#include "NotesList.h"

#include "src/MainWindow.h"
#include "ui_MainWindow.h"
#include "ui_NewNoteBook.h"
#include "ui_NotesList.h"

extern MainWindow *mw_one;
extern Method *m_Method;
extern QString iniDir, privateDir, currentMDFile;
extern bool isAndroid, isDark;
extern int fontSize;

NotesList::NotesList(QWidget *parent) : QDialog(parent), ui(new Ui::NotesList) {
  ui->setupUi(this);
  this->installEventFilter(this);
  mw_one->set_ToolButtonStyle(this);

  connect(pAndroidKeyboard, &QInputMethod::visibleChanged, this,
          &NotesList::on_KVChanged);

  tw = ui->treeWidget;
  twrb = ui->treeWidgetRecycle;

  if (!isDark) {
#ifdef Q_OS_ANDROID
    // tw->setStyleSheet("selection-background-color: lightblue");
#else
    // tw->setStyleSheet(mw_one->treeStyle);
#endif
    // twrb->setStyleSheet(tw->styleSheet());
    ui->editName->verticalScrollBar()->setStyleSheet(m_Method->vsbarStyleSmall);
    // tw->verticalScrollBar()->setStyleSheet(m_Method->vsbarStyleSmall);
    // twrb->verticalScrollBar()->setStyleSheet(m_Method->vsbarStyleSmall);
  }

  setModal(true);
  this->layout()->setSpacing(5);
  this->layout()->setContentsMargins(2, 2, 2, 2);
  ui->frame1->hide();

  tw->setVerticalScrollMode(QTreeWidget::ScrollPerPixel);
  QScroller::grabGesture(tw, QScroller::LeftMouseButtonGesture);
  m_Method->setSCrollPro(tw);

  twrb->setVerticalScrollMode(QTreeWidget::ScrollPerPixel);
  QScroller::grabGesture(twrb, QScroller::LeftMouseButtonGesture);
  m_Method->setSCrollPro(twrb);

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
  font.setPointSize(fontSize - 1);
  font.setBold(true);
  ui->lblCount->setFont(font);

  ui->btnPrev->hide();
  ui->btnNext->hide();
  ui->btnFind->hide();
  ui->editFind->hide();
  ui->lblCount->hide();
  ui->btnNewNote->hide();
  ui->btnNewNoteBook->hide();
  ui->editBook->hide();
  ui->editNote->hide();
  ui->btnImport->hide();
  ui->btnExport->hide();
  ui->editName->hide();
  ui->btnRename->hide();

  mw_one->ui->btnNoteRecycle->hide();

  QScroller::grabGesture(ui->editName, QScroller::LeftMouseButtonGesture);
  m_Method->setSCrollPro(ui->editName);

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
  item->setIcon(0, QIcon(":/res/nb.png"));

  if (rootIndex == 0) {
    ui->treeWidget->addTopLevelItem(item);
    ui->treeWidget->setCurrentItem(item);
  } else {
    QTreeWidgetItem *topItem = tw->topLevelItem(rootIndex - 1);
    tw->setCurrentItem(topItem);
    topItem->addChild(item);
    tw->setCurrentItem(item);
  }

  isNeedSave = true;
}

void NotesList::on_btnNewNote_clicked() {
  if (ui->treeWidget->topLevelItemCount() == 0) return;

  int rand = QRandomGenerator::global()->generate();
  if (rand < 0) rand = 0 - rand;

  QString noteFile = "memo/" + mw_one->m_Notes->getDateTimeStr() + "_" +
                     QString::number(rand) + ".md";
  QTreeWidgetItem *parentitem = ui->treeWidget->currentItem();

  QTreeWidgetItem *item1 = new QTreeWidgetItem(parentitem);
  item1->setText(0, ui->editNote->text().trimmed());
  item1->setText(1, noteFile);
  item1->setIcon(0, QIcon(":/res/n.png"));

  QTextEdit *edit = new QTextEdit();
  mw_one->TextEditToFile(edit, iniDir + noteFile);

  ui->treeWidget->setCurrentItem(item1);
  on_treeWidget_itemClicked(item1, 0);

  pNoteItems.clear();
  int count = parentitem->childCount();
  for (int i = 0; i < count; i++) {
    pNoteItems.append(parentitem->child(i));
  }

  isNeedSave = true;
}

void NotesList::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column) {
  Q_UNUSED(column);

  if (ui->treeWidget->topLevelItemCount() == 0) return;

  ui->editName->setText(item->text(0));

  return;

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
  QTreeWidgetItem *item = ui->treeWidget->currentItem();
  if (item == NULL) return;

  QDialog *dlg = new QDialog(this);
  QVBoxLayout *vbox0 = new QVBoxLayout;
  dlg->setLayout(vbox0);
  vbox0->setContentsMargins(5, 5, 5, 5);
  dlg->setModal(true);
  dlg->setWindowFlag(Qt::FramelessWindowHint);

  QFrame *frame = new QFrame(this);
  vbox0->addWidget(frame);

  QVBoxLayout *vbox = new QVBoxLayout;

  frame->setLayout(vbox);
  vbox->setContentsMargins(6, 6, 6, 10);
  vbox->setSpacing(3);

  QLabel *lblTitle = new QLabel(this);
  lblTitle->adjustSize();
  lblTitle->setWordWrap(true);
  lblTitle->setText(tr("Editor"));
  vbox->addWidget(lblTitle);
  lblTitle->hide();

  QFrame *hframe = new QFrame(this);
  hframe->setFrameShape(QFrame::HLine);
  hframe->setStyleSheet("QFrame{background:red;min-height:2px}");
  vbox->addWidget(hframe);
  hframe->hide();

  QTextEdit *edit = new QTextEdit(this);
  vbox->addWidget(edit);
  edit->setPlainText(item->text(0));
  QScroller::grabGesture(edit, QScroller::LeftMouseButtonGesture);
  edit->horizontalScrollBar()->setHidden(true);
  edit->verticalScrollBar()->setStyleSheet(
      mw_one->ui->editDetails->verticalScrollBar()->styleSheet());
  m_Method->setSCrollPro(edit);

  QToolButton *btnCancel = new QToolButton(this);
  QToolButton *btnCopy = new QToolButton(this);
  QToolButton *btnOk = new QToolButton(this);
  btnCancel->setText(tr("Cancel"));
  btnCopy->setText(tr("Copy"));
  btnOk->setText(tr("OK"));

  btnOk->setFixedHeight(35);
  btnCancel->setFixedHeight(35);
  btnCopy->setFixedHeight(35);

  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->addWidget(btnCancel);
  hbox->addWidget(btnCopy);
  hbox->addWidget(btnOk);
  btnCancel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  btnCopy->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  btnOk->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

  QSpacerItem *sparcer_item =
      new QSpacerItem(0, 60, QSizePolicy::Fixed, QSizePolicy::Expanding);
  vbox->addItem(sparcer_item);

  vbox->addLayout(hbox, 0);

  connect(btnCancel, &QToolButton::clicked, [=]() mutable { dlg->close(); });
  connect(dlg, &QDialog::rejected,
          [=]() mutable { m_Method->closeGrayWindows(); });
  connect(dlg, &QDialog::accepted,
          [=]() mutable { m_Method->closeGrayWindows(); });
  connect(btnCopy, &QToolButton::clicked, [=]() mutable {
    edit->selectAll();
    edit->copy();
    dlg->close();
  });
  connect(btnOk, &QToolButton::clicked, [=]() mutable {
    int index0 = getNoteBookCurrentIndex();
    int index1 = getNotesListCurrentIndex();

    item->setText(0, edit->toPlainText().trimmed());
    if (item->parent() != NULL) setNoteName(item->text(0));

    loadAllNoteBook();

    setNoteBookCurrentIndex(index0);
    m_Method->clickNoteBook();
    setNotesListCurrentIndex(index1);

    isNeedSave = true;
    dlg->close();
  });

  int x, y, w, h;
  w = mw_one->width() - 20;
  x = mw_one->geometry().x() + (mw_one->width() - w) / 2;
  h = mw_one->height() / 3;
  y = geometry().y() + (height() - h) / 4;
  dlg->setGeometry(x, y, w, h);
  dlg->setModal(true);
  mw_one->set_ToolButtonStyle(dlg);

  m_Method->m_widget = new QWidget(mw_one);
  m_Method->showGrayWindows();

  dlg->show();
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

  if (item == NULL) return;

  m_Method->m_widget = new QWidget(this);
  ShowMessage *m_ShowMsg = new ShowMessage(this);
  if (!m_ShowMsg->showMsg(
          "Knot", tr("Move to the recycle bin?") + "\n\n" + item->text(0), 2)) {
    return;
  }

  QString str0, str1;
  if (item->parent() == NULL) {
    int count = item->childCount();

    for (int i = 0; i < count; i++) {
      QTreeWidgetItem *childItem = new QTreeWidgetItem;

      str0 = item->child(i)->text(0);
      str1 = item->child(i)->text(1);

      if (!str1.isEmpty()) {
        childItem->setText(0, str0);
        childItem->setText(1, str1);
        addItem(twrb, childItem);
      } else {
        int count = item->child(i)->childCount();
        for (int j = 0; j < count; j++) {
          str0 = item->child(i)->child(j)->text(0);
          str1 = item->child(i)->child(j)->text(1);
          QTreeWidgetItem *childItem = new QTreeWidgetItem;
          childItem->setText(0, str0);
          childItem->setText(1, str1);
          addItem(twrb, childItem);
        }
      }
    }
    tw->takeTopLevelItem(tw->currentIndex().row());

  } else {
    if (!item->text(1).isEmpty()) {
      str0 = item->text(0);
      str1 = item->text(1);
      QTreeWidgetItem *childItem = new QTreeWidgetItem;
      childItem->setText(0, str0);
      childItem->setText(1, str1);
      addItem(twrb, childItem);

    } else {
      int count = item->childCount();
      for (int n = 0; n < count; n++) {
        str0 = item->child(n)->text(0);
        str1 = item->child(n)->text(1);
        QTreeWidgetItem *childItem = new QTreeWidgetItem;
        childItem->setText(0, str0);
        childItem->setText(1, str1);
        addItem(twrb, childItem);
      }
    }

    item->parent()->removeChild(item);
  }

  on_treeWidget_itemClicked(tw->currentItem(), 0);

  if (tw->currentItem()->childCount() == 0) {
    loadEmptyNote();
  }

  tw->setFocus();
  isNeedSave = true;
  saveNotesList();
}

void NotesList::addItem(QTreeWidget *tw, QTreeWidgetItem *item) {
  item->setIcon(0, QIcon(":/res/n.png"));
  tw->setFocus();
  if (tw == twrb) {
    tw->setCurrentItem(tw->topLevelItem(0));
    QTreeWidgetItem *curItem = tw->currentItem();
    curItem->addChild(item);
  } else {
    QTreeWidgetItem *curItem = tw->currentItem();
    curItem->addChild(item);
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
    m_Method->m_widget = new QWidget(this);
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

    item1 = new QTreeWidgetItem(item);
    item1->setText(0, name);

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

  if (!ui->frame1->isHidden()) {
    on_btnBack_clicked();
    event->ignore();
    return;
  }

  bool save = isNeedSave;
  saveNotesList();
  isNeedSave = save;
  saveRecycle();

  loadAllNoteBook();
  int index = 0;
  QTreeWidgetItem *item = tw->currentItem();
  QTreeWidgetItem *pNoteBook = NULL;
  if (item == NULL)
    index = 0;
  else {
    // NoteBook
    if (item->text(1).isEmpty()) {
      if (item->parent() == NULL) {
        index = tw->indexOfTopLevelItem(item);
        pNoteBook = item;
      } else {
        index = tw->indexOfTopLevelItem(item->parent());
        pNoteBook = item;
      }
    }

    // Notes
    if (!item->text(1).isEmpty()) {
      if (item->parent()->parent() == NULL) {
        index = tw->indexOfTopLevelItem(item->parent());
        pNoteBook = item->parent();
      }

      else {
        if (item->parent()->parent()->parent() == NULL) {
          index = tw->indexOfTopLevelItem(item->parent()->parent());
          pNoteBook = item->parent();
        }
      }
    }
  }

  int count = m_Method->getCountFromQW(mw_one->ui->qwNoteBook);

  for (int i = 0; i < count; i++) {
    if (pNoteBookItems.at(i) == pNoteBook) {
      index = i;
      break;
    }
  }

  setNoteBookCurrentIndex(index);
  m_Method->clickNoteBook();

  mw_one->ui->btnBackNotes->show();
  mw_one->ui->btnEdit->show();
  mw_one->ui->btnNotesList->show();
  mw_one->ui->btnSetKey->show();
  mw_one->ui->btnPDF->show();
}

void NotesList::saveNotesList() {
  if (!isNeedSave) return;

  QSettings *iniNotes =
      new QSettings(iniDir + "mainnotes.ini", QSettings::IniFormat, NULL);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  iniNotes->setIniCodec("utf-8");
#endif

  mw_one->isSelf = true;

  mw_one->isNeedAutoBackup = true;
  mw_one->strLatestModify = tr("Modi Notes List");

  int count = tw->topLevelItemCount();
  iniNotes->setValue("/MainNotes/topItemCount", count);
  for (int i = 0; i < count; i++) {
    QTreeWidgetItem *topItem = tw->topLevelItem(i);
    QString strtop = topItem->text(0);
    iniNotes->setValue("/MainNotes/strTopItem" + QString::number(i), strtop);

    int childCount = topItem->childCount();
    iniNotes->setValue("/MainNotes/childCount" + QString::number(i),
                       childCount);

    for (int j = 0; j < childCount; j++) {
      QTreeWidgetItem *childItem = topItem->child(j);
      QString strChild0 = childItem->text(0);
      QString strChild1 = childItem->text(1);

      if (!strChild1.isEmpty()) {
        iniNotes->setValue(
            "/MainNotes/childItem0" + QString::number(i) + QString::number(j),
            strChild0);
        iniNotes->setValue(
            "/MainNotes/childItem1" + QString::number(i) + QString::number(j),
            strChild1);
      } else {
        int count = childItem->childCount();
        iniNotes->setValue(
            "/MainNotes/childCount" + QString::number(i) + QString::number(j),
            count);
        iniNotes->setValue(
            "/MainNotes/childItem0" + QString::number(i) + QString::number(j),
            strChild0);
        iniNotes->setValue(
            "/MainNotes/childItem1" + QString::number(i) + QString::number(j),
            "");
        for (int n = 0; n < count; n++) {
          QString strChild00 = childItem->child(n)->text(0);
          QString strChild11 = childItem->child(n)->text(1);

          iniNotes->setValue("/MainNotes/childItem0" + QString::number(i) +
                                 QString::number(j) + QString::number(n),
                             strChild00);
          iniNotes->setValue("/MainNotes/childItem1" + QString::number(i) +
                                 QString::number(j) + QString::number(n),
                             strChild11);
        }
      }
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

  QSettings *iniNotes =
      new QSettings(iniDir + "mainnotes.ini", QSettings::IniFormat, NULL);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  iniNotes->setIniCodec("utf-8");
#endif

  mw_one->isSelf = true;

  mw_one->isNeedAutoBackup = true;
  mw_one->strLatestModify = tr("Modi Notes Recycle");

  int count = twrb->topLevelItemCount();
  iniNotes->setValue("/MainNotes/rbtopItemCount", count);
  for (int i = 0; i < count; i++) {
    QTreeWidgetItem *topItem = twrb->topLevelItem(i);
    QString strtop = topItem->text(0);
    iniNotes->setValue("/MainNotes/rbstrTopItem" + QString::number(i), strtop);

    int childCount = topItem->childCount();
    iniNotes->setValue("/MainNotes/rbchildCount" + QString::number(i),
                       childCount);

    for (int j = 0; j < childCount; j++) {
      QTreeWidgetItem *childItem = twrb->topLevelItem(i)->child(j);
      QString strChild0 = childItem->text(0);
      QString strChild1 = childItem->text(1);

      iniNotes->setValue(
          "/MainNotes/rbchildItem0" + QString::number(i) + QString::number(j),
          strChild0);
      iniNotes->setValue(
          "/MainNotes/rbchildItem1" + QString::number(i) + QString::number(j),
          strChild1);
    }
  }

  isNeedSave = false;
}

void NotesList::initNotesList() {
  mw_one->isSelf = true;
  tw->clear();

  QSettings *iniNotes =
      new QSettings(iniDir + "mainnotes.ini", QSettings::IniFormat, NULL);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  iniNotes->setIniCodec("utf-8");
#endif

  int topCount = iniNotes->value("/MainNotes/topItemCount").toInt();
  int nNoteBook = topCount;

  int notesTotal = 0;
  QString str0, str1;
  for (int i = 0; i < topCount; i++) {
    QString strTop =
        iniNotes->value("/MainNotes/strTopItem" + QString::number(i))
            .toString();
    int childCount =
        iniNotes->value("/MainNotes/childCount" + QString::number(i)).toInt();
    notesTotal = notesTotal + childCount;

    QTreeWidgetItem *topItem = new QTreeWidgetItem;
    topItem->setText(0, strTop);
    topItem->setForeground(0, Qt::red);
    QFont font = this->font();
    font.setBold(true);
    topItem->setFont(0, font);
    topItem->setIcon(0, QIcon(":/res/nb.png"));

    for (int j = 0; j < childCount; j++) {
      str0 = iniNotes
                 ->value("/MainNotes/childItem0" + QString::number(i) +
                         QString::number(j))
                 .toString();
      str1 = iniNotes
                 ->value("/MainNotes/childItem1" + QString::number(i) +
                         QString::number(j))
                 .toString();

      if (!str1.isEmpty()) {
        QTreeWidgetItem *childItem = new QTreeWidgetItem(topItem);
        childItem->setText(0, str0);
        childItem->setText(1, str1);
        childItem->setIcon(0, QIcon(":/res/n.png"));
      } else {
        QTreeWidgetItem *childItem = new QTreeWidgetItem(topItem);
        childItem->setText(0, str0);
        childItem->setText(1, "");
        childItem->setForeground(0, Qt::red);
        childItem->setFont(0, font);
        childItem->setIcon(0, QIcon(":/res/nb.png"));

        nNoteBook++;
        notesTotal--;

        int count = iniNotes
                        ->value("/MainNotes/childCount" + QString::number(i) +
                                QString::number(j))
                        .toInt();
        for (int n = 0; n < count; n++) {
          QString str00, str11;
          str00 = iniNotes
                      ->value("/MainNotes/childItem0" + QString::number(i) +
                              QString::number(j) + QString::number(n))
                      .toString();
          str11 = iniNotes
                      ->value("/MainNotes/childItem1" + QString::number(i) +
                              QString::number(j) + QString::number(n))
                      .toString();
          QTreeWidgetItem *item = new QTreeWidgetItem(childItem);
          item->setText(0, str00);
          item->setText(1, str11);
          item->setIcon(0, QIcon(":/res/n.png"));

          notesTotal++;
        }
      }
    }
    tw->addTopLevelItem(topItem);
  }

  tw->headerItem()->setText(
      0, tr("Notebook") + " : " + QString::number(nNoteBook) + "  " +
             tr("Notes") + " : " + QString::number(notesTotal));
  tw->expandAll();
}

void NotesList::initRecycle() {
  mw_one->isSelf = true;
  twrb->clear();

  QSettings *iniNotes =
      new QSettings(iniDir + "mainnotes.ini", QSettings::IniFormat, NULL);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  iniNotes->setIniCodec("utf-8");
#endif

  int topCount = iniNotes->value("/MainNotes/rbtopItemCount").toInt();
  for (int i = 0; i < topCount; i++) {
    QString strTop =
        iniNotes->value("/MainNotes/rbstrTopItem" + QString::number(i))
            .toString();
    QTreeWidgetItem *topItem = new QTreeWidgetItem;
    topItem->setText(0, strTop);

    int childCount =
        iniNotes->value("/MainNotes/rbchildCount" + QString::number(i)).toInt();
    for (int j = 0; j < childCount; j++) {
      QString str0, str1;
      str0 = iniNotes
                 ->value("/MainNotes/rbchildItem0" + QString::number(i) +
                         QString::number(j))
                 .toString();
      str1 = iniNotes
                 ->value("/MainNotes/rbchildItem1" + QString::number(i) +
                         QString::number(j))
                 .toString();

      QTreeWidgetItem *childItem = new QTreeWidgetItem(topItem);
      childItem->setText(0, str0);
      childItem->setText(1, str1);
      childItem->setIcon(0, QIcon(":/res/n.png"));
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
  if (curItem->parent() == NULL) return;

  if (moveItem(twrb))
    on_btnBack_clicked();
  else
    return;

  saveRecycle();
  isNeedSave = true;
  saveNotesList();

  return;

  QTreeWidgetItem *item = new QTreeWidgetItem;
  QString str0 = curItem->text(0);
  QString str1 = curItem->text(1);

  item->setText(0, str0);
  item->setText(1, str1);
  addItem(tw, item);

  m_Method->addItemToQW(mw_one->ui->qwNoteList, str0, "", "", str1, 0);

  curItem->parent()->removeChild(curItem);
  isNeedSave = true;
  saveRecycle();

  isNeedSave = true;
  saveNotesList();
}

void NotesList::on_btnDel_Recycle_clicked() {
  QTreeWidgetItem *curItem = twrb->currentItem();
  if (curItem->parent() == NULL) {
    return;
  } else {
    m_Method->m_widget = new QWidget(this);
    ShowMessage *m_ShowMsg = new ShowMessage(this);
    if (!m_ShowMsg->showMsg(
            "Knot", tr("Whether to remove") + "  " + curItem->text(0) + " ? ",
            2)) {
      return;
    }

    QString md = iniDir + curItem->text(1);
    delFile(md);
    curItem->parent()->removeChild(curItem);
  }

  isNeedSave = true;
  saveRecycle();
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

void NotesList::startFind(QString strFind) {
  strFind = strFind.trimmed();
  strFind = strFind.toLower();
  findResult.clear();
  if (strFind.length() == 0) return;

  int count = m_Method->getCountFromQW(mw_one->ui->qwNoteBook);
  for (int i = 0; i < count; i++) {
    setNoteBookCurrentIndex(i);
    QString str0 = m_Method->getText0(mw_one->ui->qwNoteBook, i);
    if (str0.toLower().contains(strFind)) {
      findResult.append(QString::number(i) + "===" + "NoteBook");
    }

    m_Method->clickNoteBook();
    int count1 = m_Method->getCountFromQW(mw_one->ui->qwNoteList);
    for (int j = 0; j < count1; j++) {
      QString str0 = m_Method->getText0(mw_one->ui->qwNoteList, j);
      if (str0.toLower().contains(strFind)) {
        findResult.append(QString::number(i) + "===" + QString::number(j));
      }
    }
  }

  if (findResult.count() > 0) {
    mw_one->ui->btnFindNextNote->setEnabled(true);
    mw_one->ui->btnFindPreviousNote->setEnabled(true);
  }

  goFindResult(0);
}

void NotesList::goPrevious() {
  findCount = findCount - 1;
  if (findCount < 0) findCount = findResult.count() - 1;

  goFindResult(findCount);

  if (pAndroidKeyboard->isVisible()) pAndroidKeyboard->hide();
}

void NotesList::goNext() {
  findCount = findCount + 1;
  if (findCount > findResult.count() - 1) findCount = 0;

  goFindResult(findCount);

  if (pAndroidKeyboard->isVisible()) pAndroidKeyboard->hide();
}

void NotesList::goFindResult(int index) {
  if (findResult.count() == 0) return;

  findCount = index;
  QString str = findResult.at(index);
  QStringList list = str.split("===");
  if (list.at(1) == "NoteBook") {
    int indexNoteBook = list.at(0).toInt();
    setNoteBookCurrentIndex(indexNoteBook);
    m_Method->clickNoteBook();
    setNotesListCurrentIndex(-1);
  } else {
    int index0, index1;
    index0 = list.at(0).toInt();
    index1 = list.at(1).toInt();
    setNoteBookCurrentIndex(index0);
    m_Method->clickNoteBook();
    setNotesListCurrentIndex(index1);
  }

  mw_one->ui->lblFindNoteCount->setText(QString::number(findCount + 1) +
                                        " -> " +
                                        QString::number(findResult.count()));
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
    if (topItem->text(0).toLower().contains(strFind)) {
      findResultList.append(topItem);
      qDebug() << topItem->text(0);
    }
    int childCount = topItem->childCount();
    for (int j = 0; j < childCount; j++) {
      QTreeWidgetItem *childItem = topItem->child(j);
      if (childItem->text(0).toLower().contains(strFind)) {
        findResultList.append(childItem);
        qDebug() << childItem->text(0);
      }

      QString str1 = childItem->text(1);
      if (str1.isEmpty()) {
        int count = childItem->childCount();
        for (int n = 0; n < count; n++) {
          QTreeWidgetItem *item = childItem->child(n);
          if (item->text(0).toLower().contains(strFind)) {
            findResultList.append(item);
            qDebug() << item->text(0);
          }
        }
      }
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
  // NoteBook

  if (item->childCount() > 0) {
    if (item->parent() == NULL) {
      int topIndex = tw->indexOfTopLevelItem(item);
      setNoteBookCurrentIndex(topIndex);
    } else {
      int index = tw->indexOfTopLevelItem(item->parent());

      int childCount = item->parent()->childCount();
      int newRow = 0;
      for (int i = 0; i < childCount; i++) {
        QTreeWidgetItem *item1 = item->parent()->child(i);
        if (item1 == item) break;

        if (item1->text(1).isEmpty()) newRow++;
      }

      setNoteBookCurrentIndex(index + newRow + 1);
    }

    m_Method->clickNoteBook();
    setNotesListCurrentIndex(-1);

    // Notes
  } else {
    QTreeWidgetItem *top_item = item->parent()->parent();
    if (top_item == NULL) {
      int topIndex = tw->indexOfTopLevelItem(item->parent());
      int childIndex = tw->currentIndex().row();
      setNoteBookCurrentIndex(topIndex);
      m_Method->clickNoteBook();
      setNotesListCurrentIndex(childIndex);
    } else {
      int index = tw->indexOfTopLevelItem(top_item);

      int childCount = top_item->childCount();
      int newRow = 0;
      for (int i = 0; i < childCount; i++) {
        QTreeWidgetItem *item1 = top_item->child(i);
        if (item1 == item->parent()) break;

        if (item1->text(1).isEmpty()) newRow++;
      }

      setNoteBookCurrentIndex(index + newRow + 1);

      m_Method->clickNoteBook();
      setNotesListCurrentIndex(item->parent()->indexOfChild(item));
    }
  }
}

QString NotesList::getNoteBookText0(int index) {
  return m_Method->getText0(mw_one->ui->qwNoteBook, index);
}

QString NotesList::getNotesListText0(int index) {
  return m_Method->getText0(mw_one->ui->qwNoteList, index);
}

void NotesList::modifyNoteBookText0(QString text0, int index) {
  m_Method->modifyItemText0(mw_one->ui->qwNoteBook, index, text0);
}

void NotesList::modifyNotesListText0(QString text0, int index) {
  m_Method->modifyItemText0(mw_one->ui->qwNoteList, index, text0);
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

  if (item == NULL) return;

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

void NotesList::setNoteBookCurrentItem() {
  int index = getNoteBookCurrentIndex();
  if (index < 0) return;

  QString text1 = m_Method->getText1(mw_one->ui->qwNoteBook, index);
  QString text2 = m_Method->getText2(mw_one->ui->qwNoteBook, index);
  if (text2.isEmpty()) {
    tw->setCurrentItem(tw->topLevelItem(text1.toInt()));

  } else {
    QStringList list = text1.split("===");
    int indexMain, indexChild;
    if (list.count() == 2) {
      indexMain = list.at(0).toInt();
      indexChild = list.at(1).toInt();

      QTreeWidgetItem *topItem = tw->topLevelItem(indexMain);
      QTreeWidgetItem *childItem = topItem->child(indexChild);
      tw->setCurrentItem(childItem);
    }
  }
}

void NotesList::on_actionAdd_NoteBook_triggered() {
  QString text;

  NewNoteBook *m_new = new NewNoteBook(mw_one);
  text = m_new->notebookName;
  if (m_new->isOk && !text.isEmpty()) {
    rootIndex = m_new->rootIndex;
    ui->editBook->setText(text);
    on_btnNewNoteBook_clicked();

    loadAllNoteBook();

    int count = m_Method->getCountFromQW(mw_one->ui->qwNoteBook);
    int index = 0;
    for (int i = 0; i < count; i++) {
      if (pNoteBookItems.at(i) == tw->currentItem()) {
        index = i;
        break;
      }
    }
    setNoteBookCurrentIndex(index);
    m_Method->clickNoteBook();
  }
}

void NotesList::on_actionDel_NoteBook_triggered() {
  int index = getNoteBookCurrentIndex();
  if (index < 0) return;

  tw->setCurrentItem(pNoteBookItems.at(index));
  on_btnDel_clicked();

  loadAllNoteBook();

  int count = getNoteBookCount();
  for (int i = 0; i < count; i++) {
    if (tw->currentItem() == pNoteBookItems.at(i)) {
      index = i;
      break;
    }
  }
  setNoteBookCurrentIndex(index);

  setNoteLabel();

  bool save = isNeedSave;
  saveRecycle();
  isNeedSave = save;
  saveNotesList();

  if (count == 0) {
    loadEmptyNote();
  }
}

void NotesList::on_actionRename_NoteBook_triggered() {
  int index = getNoteBookCurrentIndex();
  if (index < 0) return;

  on_btnRename_clicked();
}

int NotesList::getNoteBookCount() {
  int count = m_Method->getCountFromQW(mw_one->ui->qwNoteBook);
  return count;
}

int NotesList::getNotesListCount() {
  int count = m_Method->getCountFromQW(mw_one->ui->qwNoteList);
  return count;
}

int NotesList::getNoteBookCurrentIndex() {
  int index = m_Method->getCurrentIndexFromQW(mw_one->ui->qwNoteBook);
  return index;
}

int NotesList::getNotesListCurrentIndex() {
  int index = m_Method->getCurrentIndexFromQW(mw_one->ui->qwNoteList);
  return index;
}

void NotesList::setNoteBookCurrentIndex(int index) {
  m_Method->setCurrentIndexFromQW(mw_one->ui->qwNoteBook, index);
}

void NotesList::setNotesListCurrentIndex(int index) {
  m_Method->setCurrentIndexFromQW(mw_one->ui->qwNoteList, index);
}

void NotesList::on_actionMoveUp_NoteBook_triggered() {
  int index = getNoteBookCurrentIndex();
  if (index <= 0) return;

  setNoteBookCurrentItem();
  on_btnUp_clicked();

  int newIndex = getNoteBookIndex_twToqml();

  loadAllNoteBook();
  setNoteBookCurrentIndex(newIndex);
  m_Method->clickNoteBook();
}

void NotesList::on_actionMoveDown_NoteBook_triggered() {
  int index = getNoteBookCurrentIndex();

  if (index < 0) return;

  if (index == getNoteBookCount() - 1) return;

  setNoteBookCurrentItem();
  on_btnDown_clicked();

  int newIndex = getNoteBookIndex_twToqml();

  loadAllNoteBook();
  setNoteBookCurrentIndex(newIndex);
  m_Method->clickNoteBook();
}

int NotesList::getNoteBookIndex_twToqml() {
  QTreeWidgetItem *item = tw->currentItem();
  int index = 0;
  if (item->parent() == NULL) {
    index = tw->indexOfTopLevelItem(item);
  } else {
    int index0 = tw->indexOfTopLevelItem(item->parent());
    int index1 = 0;
    int count = item->parent()->childCount();
    for (int i = 0; i < count; i++) {
      QTreeWidgetItem *item0 = item->parent()->child(i);
      if (item0 == item) break;

      if (item0->text(1).isEmpty()) index1++;
    }

    index = index0 + index1 + 1;
  }

  return index;
}

void NotesList::loadAllNoteBook() {
  pNoteBookItems.clear();

  m_Method->clearAllBakList(mw_one->ui->qwNoteBook);
  m_Method->clearAllBakList(mw_one->ui->qwNoteList);
  int count = tw->topLevelItemCount();
  for (int i = 0; i < count; i++) {
    QTreeWidgetItem *topItem = tw->topLevelItem(i);
    QString str = topItem->text(0);

    int sum = topItem->childCount();
    int child_count = sum;
    for (int n = 0; n < child_count; n++) {
      if (topItem->child(n)->text(1).isEmpty()) sum--;
    }

    QString strSum = QString::number(sum);
    m_Method->addItemToQW(mw_one->ui->qwNoteBook, str, QString::number(i), "",
                          strSum, fontSize);
    pNoteBookItems.append(topItem);

    int childCount = topItem->childCount();
    for (int j = 0; j < childCount; j++) {
      QTreeWidgetItem *childItem = topItem->child(j);
      if (childItem->text(1).isEmpty()) {
        m_Method->addItemToQW(
            mw_one->ui->qwNoteBook, childItem->text(0),
            QString::number(i) + "===" + QString::number(j), "isNoteBook",
            QString::number(childItem->childCount()), fontSize - 1);

        pNoteBookItems.append(childItem);
      }
    }
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

  mainMenu->addAction(actNew);
  mainMenu->addAction(actRename);
  mainMenu->addAction(actDel);

  mainMenu->addAction(actMoveUp);
  mainMenu->addAction(actMoveDown);

  actRename->setVisible(true);
  actDel->setVisible(false);
  actMoveUp->setVisible(false);
  actMoveDown->setVisible(false);

  mainMenu->setStyleSheet(m_Method->qssMenu);
}

void NotesList::on_actionAdd_Note_triggered() {
  int notebookIndex = getNoteBookCurrentIndex();
  if (notebookIndex < 0) return;

  bool ok = false;
  QString text;

  QInputDialog *idlg =
      m_Method->inputDialog(tr("New Note"), tr("New Note Name"), "");

  if (QDialog::Accepted == idlg->exec()) {
    ok = true;
    text = idlg->textValue().trimmed();
    idlg->close();
  } else {
    idlg->close();
    return;
  }

  if (ok && !text.isEmpty()) {
    tw->setCurrentItem(pNoteBookItems.at(notebookIndex));
    ui->editNote->setText(text);
    on_btnNewNote_clicked();

    QTreeWidgetItem *childItem = tw->currentItem();
    QString text3 = childItem->text(1);
    m_Method->addItemToQW(mw_one->ui->qwNoteList, text, "", "", text3, 0);

    int count = getNotesListCount();
    setNotesListCurrentIndex(count - 1);
    m_Method->clickNoteList();
  }

  setNoteLabel();
}

void NotesList::on_actionDel_Note_triggered() {
  if (getNotesListCount() == 0) return;

  int notebookIndex = getNoteBookCurrentIndex();
  int notelistIndex = getNotesListCurrentIndex();

  if (notebookIndex < 0) return;
  if (notelistIndex < 0) return;

  on_btnDel_clicked();

  int count = getNotesListCount();

  setNoteLabel();

  bool save = isNeedSave;
  saveRecycle();
  isNeedSave = save;
  saveNotesList();

  if (count == 0) {
    loadEmptyNote();
  }
}

void NotesList::loadEmptyNote() {
  currentMDFile = "";
  mw_one->m_Notes->MD2Html(currentMDFile);
  mw_one->m_Notes->loadMemoQML();
  mw_one->ui->lblNoteName->setText("");

  m_Method->saveCurNoteIndex();
}

void NotesList::on_actionRename_Note_triggered() {
  int notebookIndex = getNoteBookCurrentIndex();
  int noteIndex = getNotesListCurrentIndex();
  if (notebookIndex < 0) return;
  if (noteIndex < 0) return;

  on_btnRename_clicked();
}

void NotesList::on_actionMoveUp_Note_triggered() {
  int indexBook = getNoteBookCurrentIndex();
  int indexNote = getNotesListCurrentIndex();
  if (indexBook < 0) return;
  if (indexNote <= 0) return;

  setNoteBookCurrentItem();
  tw->setCurrentItem(tw->currentItem()->child(indexNote));
  on_btnUp_clicked();

  m_Method->clickNoteBook();
  setNotesListCurrentIndex(indexNote - 1);
  m_Method->saveCurNoteIndex();
}

void NotesList::on_actionMoveDown_Note_triggered() {
  int indexBook = getNoteBookCurrentIndex();
  int indexNote = getNotesListCurrentIndex();
  if (indexBook < 0) return;
  if (indexNote < 0) return;
  if (indexNote + 1 == getNotesListCount()) return;

  setNoteBookCurrentItem();
  tw->setCurrentItem(tw->currentItem()->child(indexNote));
  on_btnDown_clicked();

  m_Method->clickNoteBook();
  setNotesListCurrentIndex(indexNote + 1);
  m_Method->saveCurNoteIndex();
}

void NotesList::on_actionImport_Note_triggered() {
  int indexBook = getNoteBookCurrentIndex();
  if (indexBook < 0) return;

  setNoteBookCurrentItem();
  bool isOk = on_btnImport_clicked();

  if (isOk) {
    m_Method->clickNoteBook();
    setNotesListCurrentIndex(getNotesListCount() - 1);
    m_Method->clickNoteList();
    m_Method->saveCurNoteIndex();
  }
}

void NotesList::on_actionExport_Note_triggered() {
  int indexBook = getNoteBookCurrentIndex();
  int indexNote = getNotesListCurrentIndex();

  if (indexBook < 0) return;
  if (indexNote < 0) return;

  setNoteBookCurrentItem();
  tw->setCurrentItem(tw->currentItem()->child(indexNote));

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

  mainMenu->addAction(actNew);
  mainMenu->addAction(actRename);
  mainMenu->addAction(actDel);

  mainMenu->addAction(actImport);
  mainMenu->addAction(actExport);

  mainMenu->addAction(actMoveUp);
  mainMenu->addAction(actMoveDown);

  actRename->setVisible(true);
  actDel->setVisible(false);
  actMoveUp->setVisible(false);
  actMoveDown->setVisible(false);

  mainMenu->setStyleSheet(m_Method->qssMenu);
}

void NotesList::setNoteLabel() {
  mw_one->ui->lblNoteBook->setText(tr("Note Book") + " : " +
                                   QString::number(getNoteBookCount()));
  QString notesSum = QString::number(getNotesListCount());
  mw_one->ui->lblNoteList->setText(tr("Note List") + " : " + notesSum);
  int index = getNoteBookCurrentIndex();
  m_Method->modifyItemText3(mw_one->ui->qwNoteBook, index, notesSum);
}

void NotesList::on_btnMoveTo_clicked() {
  QTreeWidgetItem *item = tw->currentItem();
  if (item == NULL) return;

  int count = item->childCount();
  for (int i = 0; i < count; i++) {
    if (item->child(i)->text(1).isEmpty()) return;
  }

  moveItem(tw);
}

bool NotesList::moveItem(QTreeWidget *twMain) {
  QTreeWidgetItem *item = twMain->currentItem();
  if (item == NULL) return false;

  MoveTo *m_MoveTo = new MoveTo(this);
  if (!m_MoveTo->isOk) return false;

  // NoteBook
  if (item->text(1).isEmpty()) {
    QTreeWidgetItem *new_item = item;

    if (m_MoveTo->strCurrentItem == tr("Main Root")) {
      if (item->parent() == NULL) return false;

      item->parent()->removeChild(item);
      tw->addTopLevelItem(new_item);
      tw->setCurrentItem(new_item);
    }

    if (m_MoveTo->strCurrentItem != tr("Main Root")) {
      if (m_MoveTo->currentItem != item) {
        if (item->parent() == NULL)
          tw->takeTopLevelItem(tw->currentIndex().row());
        else
          item->parent()->removeChild(item);

        m_MoveTo->currentItem->addChild(new_item);
        tw->setCurrentItem(new_item);
      }
    }
  }

  // Notes
  if (!item->text(1).isEmpty()) {
    if (m_MoveTo->strCurrentItem == tr("Main Root")) return false;

    QTreeWidgetItem *new_item = item;
    item->parent()->removeChild(item);
    m_MoveTo->currentItem->addChild(new_item);
    tw->setCurrentItem(new_item);
  }

  isNeedSave = true;

  return true;
}

void NotesList::loadAllRecycle() {
  m_Method->clearAllBakList(mw_one->ui->qwNoteRecycle);
  int childCount = twrb->topLevelItem(0)->childCount();
  for (int i = 0; i < childCount; i++) {
    QTreeWidgetItem *childItem = twrb->topLevelItem(0)->child(i);
    QString text0 = childItem->text(0);
    QString text3 = childItem->text(1);

    m_Method->addItemToQW(mw_one->ui->qwNoteRecycle, text0, "", "", text3, 0);
  }
}
