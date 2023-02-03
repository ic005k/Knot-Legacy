#include "Report.h"

#include "mainwindow.h"
#include "ui_Report.h"
#include "ui_mainwindow.h"

extern int fontSize;
extern MainWindow* mw_one;
extern QString iniFile, iniDir, btnYText, btnMText, btnDText;
extern QTabWidget *tabData, *tabChart;
extern bool isImport, isEBook, isReport;

QString btnYearText, btnMonthText;
QStringList listCategory;

QTreeWidget* twOut2Img;
QLabel *lblTotal, *lblDetails;
QToolButton *btnCategory, *btnMonth, *btnYear;
int twTotalRow = 0;

void setTableNoItemFlags(QTableWidget* t, int row);

dlgReport::dlgReport(QWidget* parent) : QDialog(parent), ui(new Ui::dlgReport) {
  ui->setupUi(this);

  this->installEventFilter(this);
  this->setModal(true);

  twOut2Img = new QTreeWidget;
  twOut2Img->setColumnCount(3);

  twOut2Img->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
  twOut2Img->header()->setDefaultAlignment(Qt::AlignCenter);
  twOut2Img->headerItem()->setTextAlignment(2, Qt::AlignRight);
  twOut2Img->setAlternatingRowColors(true);
  twOut2Img->setStyleSheet(mw_one->treeStyle);
  twOut2Img->setUniformRowHeights(true);

  lblTotal = mw_one->ui->lblTotal;
  lblDetails = mw_one->ui->lblDetails;
  btnCategory = mw_one->ui->btnCategory;
  btnMonth = mw_one->ui->btnMonth;
  btnYear = mw_one->ui->btnYear;

  QFont font = mw_one->ui->lblTotal->font();
  font.setBold(true);
  mw_one->ui->lblTotal->setFont(font);
  mw_one->ui->lblDetails->setFont(font);
  mw_one->ui->lblTitle_Report->setFont(font);
  mw_one->ui->lblDetails->setWordWrap(true);
  mw_one->ui->lblDetails->adjustSize();

  mw_one->set_btnStyle(this);
}

void dlgReport::init() {
  mw_one->ui->frameReport->setGeometry(mw_one->geometry().x(),
                                       mw_one->geometry().y(), mw_one->width(),
                                       mw_one->height());
  mw_one->ui->qwReportSub->setMinimumHeight(mw_one->height() / 3);
  mw_one->ui->frameMain->hide();
  mw_one->ui->frameReport->show();
  mw_one->ui->lblTitle_Report->setText(
      mw_one->ui->tabWidget->tabText(mw_one->ui->tabWidget->currentIndex()));
}

dlgReport::~dlgReport() { delete ui; }

void dlgReport::keyReleaseEvent(QKeyEvent* event) { Q_UNUSED(event) }

bool dlgReport::eventFilter(QObject* watch, QEvent* evn) {
  if (evn->type() == QEvent::KeyPress) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      if (!mw_one->mydlgReport->isHidden()) {
        on_btnBack_clicked();
        return true;
      }
    }
  }

  return QWidget::eventFilter(watch, evn);
}

void dlgReport::on_btnBack_clicked() {
  saveYMD();

  listCategory.clear();

  mw_one->ui->frameReport->hide();
  mw_one->ui->frameMain->show();
}

void dlgReport::closeEvent(QCloseEvent* event) {
  Q_UNUSED(event);
  mw_one->closeGrayWindows();
  mw_one->m_widget = new QWidget(mw_one);
}

void dlgReport::on_btnYear_clicked() {
  mw_one->ui->lblDetails->setText(tr("Details"));
  int w = mw_one->ui->btnYear->width() + 1;
  QListWidget* list = new QListWidget(mw_one);
  list->setStyleSheet(mw_one->listStyle);
  QFont font;
  font.setPointSize(fontSize);
  list->setFont(font);
  int year = 2022;
  QStringList strList;
  for (int i = 0; i < 10; i++) {
    strList.append(QString::number(year + i));
  }

  for (int i = 0; i < strList.count(); i++) {
    QListWidgetItem* item = new QListWidgetItem;
    item->setSizeHint(QSize(w - 10, 30));  // item->sizeHint().width()
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    item->setText(strList.at(i));
    list->addItem(item);
  }
  connect(list, &QListWidget::itemClicked, [=]() {
    mw_one->ui->btnYear->setText(list->currentItem()->text());
    btnYearText = mw_one->ui->btnYear->text();
    list->close();

    listCategory.clear();
    mw_one->startInitReport();
  });

  int h = 30 * list->count() + 4;
  int y = mw_one->ui->btnYear->y();

  list->setGeometry(mw_one->ui->btnYear->x(), y, w, h);

  list->show();

  QString str = mw_one->ui->btnYear->text();
  for (int i = 0; i < list->count(); i++) {
    if (str == list->item(i)->text()) {
      list->setCurrentRow(i);
      break;
    }
  }
}

void dlgReport::updateTable() {
  freq = 0;
  t_amount = 0;
  clearAll();
  clearAll_xx();
  mw_one->ui->lblTotal->setText(tr("Total") + " : " + tr("Freq") + " 0    " +
                                tr("Amount") + " 0");
  mw_one->ui->lblDetails->setText(tr("Details"));
  mw_one->ui->lblDetails->setStyleSheet(
      mw_one->myEditRecord->ui->lblTitle->styleSheet());

  for (int i = 0; i < twOut2Img->topLevelItemCount(); i++) {
    QTreeWidgetItem* topItem = twOut2Img->topLevelItem(i);
    QString text0 = topItem->text(0);
    QString text1 = topItem->text(1);
    QString text2 = topItem->text(2);
    freq = freq + text1.toInt();
    if (text2.length() > 0) t_amount = t_amount + text2.toDouble();
    appendSteps(text0, text1, text2);
  }

  mw_one->ui->lblTotal->setText(tr("Total") + " : " + tr("Freq") +
                                QString::number(freq) + "    " + tr("Amount") +
                                " " + QString("%1").arg(t_amount, 0, 'f', 2));

  mw_one->ui->btnCategory->setText(tr("View Category"));

  setScrollBarPos(0);
}

void dlgReport::getMonthData() {
  QTreeWidget* tw = mw_one->get_tw(tabData->currentIndex());

  twOut2Img->clear();
  twTotalRow = 0;
  listCategory.clear();

  for (int i = 0; i < tw->topLevelItemCount(); i++) {
    QString strYear;
    strYear = tw->topLevelItem(i)->text(3);
    QString strMonth =
        mw_one->get_Month(tw->topLevelItem(i)->text(0) + " " + strYear);

    if (btnMonthText == tr("Year-Round")) {
      if (strYear == btnYearText) {
        twTotalRow = twTotalRow + 1;
        QTreeWidgetItem* item;
        item = tw->topLevelItem(i)->clone();

        setTWImgData(item);
      }
    } else {
      if (strYear == btnYearText && strMonth == btnMonthText) {
        twTotalRow = twTotalRow + 1;
        QTreeWidgetItem* item;
        item = tw->topLevelItem(i)->clone();

        setTWImgData(item);
      }
    }
  }
}

void dlgReport::setTWImgData(QTreeWidgetItem* item) {
  QTreeWidgetItem* newtop = new QTreeWidgetItem;
  QFont f = newtop->font(0);
  f.setBold(true);
  newtop->setFont(0, f);
  newtop->setFont(1, f);
  newtop->setFont(2, f);
  newtop->setTextAlignment(2, Qt::AlignRight | Qt::AlignVCenter);
  newtop->setText(0, item->text(0));
  newtop->setText(1, item->text(1));
  newtop->setText(2, item->text(2));
  twOut2Img->addTopLevelItem(newtop);
  QBrush brush(Qt::lightGray);
  newtop->setBackground(0, brush);
  newtop->setBackground(1, brush);
  newtop->setBackground(2, brush);

  for (int z = 0; z < item->childCount(); z++) {
    QTreeWidgetItem* newchild = new QTreeWidgetItem(newtop);
    newchild->setTextAlignment(1, Qt::AlignRight | Qt::AlignVCenter);
    newchild->setTextAlignment(2, Qt::AlignRight | Qt::AlignVCenter);
    QString strClass = item->child(z)->text(2);
    newchild->setText(0, item->child(z)->text(0));
    newchild->setText(1, item->child(z)->text(1));
    newchild->setText(2, strClass);

    listCategory.removeOne(strClass);
    listCategory.append(strClass);

    QString strDes = item->child(z)->text(3);
    if (strDes.trimmed().length() > 0) {
      QTreeWidgetItem* des = new QTreeWidgetItem(newtop);
      des->setText(0, tr("Details") + ":" + strDes);
    }
  }

  twTotalRow = twTotalRow + newtop->childCount();
}

void dlgReport::on_btnMonth_clicked() {
  mw_one->ui->lblDetails->setText(tr("Details"));
  int w = mw_one->ui->btnMonth->width() + 1;

  QListWidget* list = new QListWidget(mw_one);
  list->setStyleSheet(mw_one->listStyle);
  QFont font;
  font.setPointSize(fontSize);
  list->setFont(font);

  QStringList strList = mw_one->listMonth;

  for (int i = 0; i < strList.count(); i++) {
    QListWidgetItem* item = new QListWidgetItem;
    item->setSizeHint(QSize(w - 10, 30));  // item->sizeHint().width()
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    item->setText(strList.at(i));
    list->addItem(item);
  }
  QListWidgetItem* item = new QListWidgetItem;
  item->setSizeHint(QSize(w - 10, 30));
  item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  item->setText(tr("Year-Round"));
  list->addItem(item);

  connect(list, &QListWidget::itemClicked, [=]() {
    mw_one->ui->btnMonth->setText(list->currentItem()->text());
    btnMonthText = mw_one->ui->btnMonth->text();
    list->close();

    listCategory.clear();
    mw_one->startInitReport();
  });

  int h = 30 * list->count() + 4;
  int y = mw_one->ui->btnMonth->y();

  list->setGeometry(mw_one->ui->btnMonth->x(), y, w, h);

  list->show();

  QString str = mw_one->ui->btnMonth->text();
  for (int i = 0; i < list->count(); i++) {
    if (str == list->item(i)->text()) {
      list->setCurrentRow(i);
      break;
    }
  }
}

void dlgReport::saveYMD() {
  QSettings Reg(iniDir + "ymd.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif
  Reg.setValue("/YMD/btnYearText", btnYearText);
  Reg.setValue("/YMD/btnMonthText", btnMonthText);
  Reg.setValue("/YMD/btnYText", btnYText);
  Reg.setValue("/YMD/btnMText", btnMText);
  Reg.setValue("/YMD/btnDText", btnDText);

  Reg.setValue("/YMD/M1", mw_one->ui->cboxM1->currentIndex());
  Reg.setValue("/YMD/M2", mw_one->ui->cboxM2->currentIndex());
  Reg.setValue("/YMD/D1", mw_one->ui->cboxD1->currentIndex());
  Reg.setValue("/YMD/D2", mw_one->ui->cboxD2->currentIndex());
}

void dlgReport::on_btnCategory_clicked() {
  int count = getCount();
  if (count == 0) {
    btnCategory->setText(tr("View Category"));

    return;
  }

  mw_one->m_widget = new QWidget(this);

  QDialog* dlg = new QDialog(this);
  QVBoxLayout* vbox0 = new QVBoxLayout;
  dlg->setLayout(vbox0);
  vbox0->setContentsMargins(5, 5, 5, 5);
  dlg->setModal(true);
  dlg->setWindowFlag(Qt::FramelessWindowHint);
  dlg->setAttribute(Qt::WA_TranslucentBackground);

  QFrame* frame = new QFrame(dlg);
  vbox0->addWidget(frame);

  frame->setStyleSheet(
      ".QFrame{background-color: rgb(255, 255, 255);border-radius:10px; "
      "border:1px solid gray;}");

  QVBoxLayout* vbox = new QVBoxLayout;
  frame->setLayout(vbox);
  QListWidget* list = new QListWidget(mw_one->mydlgReport);
  list->setStyleSheet("QListWidget{border:0px solid gray;}");
  vbox->addWidget(list);

  list->setSpacing(12);
  list->setViewMode(QListView::IconMode);
  list->setMovement(QListView::Static);
  list->setStyleSheet(mw_one->listStyleMain);
  list->verticalScrollBar()->setStyleSheet(mw_one->vsbarStyleSmall);
  list->setVerticalScrollMode(QListWidget::ScrollPerPixel);
  list->horizontalScrollBar()->hide();
  QScroller::grabGesture(list, QScroller::LeftMouseButtonGesture);
  mw_one->setSCrollPro(list);
  QFont font;
  font.setPointSize(fontSize + 1);
  list->setFont(font);

  if (listCategory.count() == 0) {
  } else {
    for (int i = 0; i < listCategory.count(); i++) {
      QListWidgetItem* pItem = new QListWidgetItem();
      // pItem->setSizeHint(QSize(btnCategory->width() - 20, 30));
      pItem->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
      pItem->setText(listCategory.at(i));
      list->addItem(pItem);
    }
  }

  int h = mw_one->height() / 2;
  if (list->count() * 30 < h) h = list->count() * 30 + 4;
  int w = mw_one->width() - 40;
  int x = mw_one->geometry().x() + (mw_one->width() - w) / 2;
  dlg->setGeometry(x, btnCategory->y() - h / 2, w, h);
  if (list->count() > 1) {
    mw_one->showGrayWindows();
    dlg->show();
  }

  connect(list, &QListWidget::itemClicked, [=]() {
    btnCategory->setText(list->currentItem()->text());

    getCategoryData();

    dlg->close();
    mw_one->closeGrayWindows();
  });

  connect(dlg, &QDialog::rejected, [=]() {
    dlg->close();
    mw_one->closeGrayWindows();
  });
}

void dlgReport::getCategoryData() {
  clearAll_xx();
  setCurrentHeader(2);
  int freq = 0;
  double d_amount = 0;
  for (int i = 0; i < twOut2Img->topLevelItemCount(); i++) {
    QTreeWidgetItem* topItem = twOut2Img->topLevelItem(i);
    for (int j = 0; j < topItem->childCount(); j++) {
      QTreeWidgetItem* childItem = topItem->child(j);
      QString strClass = childItem->text(2);
      if (strClass == mw_one->ui->btnCategory->text()) {
        QString date = topItem->text(0);
        QString time = childItem->text(0).split(".").at(1);
        QString amount = childItem->text(1);
        appendSteps_xx(date, time, amount);

        freq++;
        if (amount.length() > 0) {
          d_amount = d_amount + amount.toDouble();
        }
      }
    }
  }

  QString ta = QString("%1").arg(d_amount, 0, 'f', 2);
  appendSteps_xx(tr("Total"), QString::number(freq), ta);
  double bfb = d_amount / t_amount;
  mw_one->ui->lblDetails->setStyleSheet(
      mw_one->myEditRecord->ui->lblTitle->styleSheet());
  mw_one->ui->lblDetails->setText(
      tr("Details") + " : " + tr("Amount") + " " + ta + "    " +
      QString("%1").arg(bfb * 100, 0, 'f', 2) + " %");

  setScrollBarPos_xx(0);
}

void setTableNoItemFlags(QTableWidget* t, int row) {
  for (int z = 0; z < t->columnCount(); z++) {
    t->item(row, z)->setFlags(Qt::NoItemFlags);
  }
}

void dlgReport::on_btnOut2Img_clicked() {
  if (twOut2Img->topLevelItemCount() == 0) return;

  if (twOut2Img->topLevelItem(0)->text(0) != mw_one->ui->btnYear->text()) {
    twOut2Img->expandAll();

    QTreeWidgetItem* item0 = new QTreeWidgetItem;
    item0->setText(
        0, mw_one->ui->btnYear->text() + " . " + mw_one->ui->btnMonth->text());
    twOut2Img->insertTopLevelItem(0, item0);

    QTreeWidgetItem* item = new QTreeWidgetItem;
    item->setText(0, tr("Total") + " : ");
    item->setText(1, tr("Freq") + " " + QString::number(freq));
    item->setText(2,
                  tr("Amount") + " " + QString("%1").arg(t_amount, 0, 'f', 2));
    twOut2Img->addTopLevelItem(item);

    twTotalRow = twTotalRow + 3;
    qreal h = twTotalRow * 28;
    twOut2Img->setGeometry(0, 0, this->width(), h);

    // The column merge is implemented
    QTreeWidget* m_t = new QTreeWidget(this);
    QFont f = this->font();
    f.setPointSize(f.pointSize() - 1);
    m_t->setFont(f);
    m_t->setColumnCount(3);
    m_t->headerItem()->setText(0, "  " + tr("Date") + "  ");
    m_t->headerItem()->setText(1, tr("Freq"));
    m_t->headerItem()->setText(2, tr("Amount"));
    m_t->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_t->header()->setDefaultAlignment(Qt::AlignCenter);
    m_t->headerItem()->setTextAlignment(1, Qt::AlignLeft);
    m_t->headerItem()->setTextAlignment(2, Qt::AlignRight);
    m_t->setAlternatingRowColors(true);
    m_t->setStyleSheet(mw_one->treeStyle);
    m_t->setUniformRowHeights(true);
    for (int i = 0; i < twOut2Img->topLevelItemCount(); i++) {
      QTreeWidgetItem* top = twOut2Img->topLevelItem(i)->clone();
      m_t->addTopLevelItem(top);
    }

    for (int i = 0; i < m_t->topLevelItemCount(); i++) {
      QTreeWidgetItem* top = m_t->topLevelItem(i);
      for (int j = 0; j < top->childCount(); j++) {
        if (top->child(j)->text(0).contains(tr("Details")))
          top->child(j)->setFirstColumnSpanned(true);
      }
    }
    m_t->expandAll();
    m_t->setGeometry(0, 0, this->width(), h + m_t->header()->height());

    // 方法1
    QPixmap pixmap(m_t->size());
    m_t->render(&pixmap);

    // 方法2
    // QPixmap pixmap = QPixmap::grabWidget(m_t);

    QString strFile = mw_one->ui->lblTitle->text() + "-" +
                      mw_one->ui->btnYear->text() + "-" +
                      mw_one->ui->btnMonth->text() + ".png";

#ifdef Q_OS_ANDROID
    QDir* folder = new QDir;
    QString path = "/storage/emulated/0/KnotBak/";
    folder->mkdir(path);
    QString filename = path + strFile;
    pixmap.save(filename, "PNG");
    QMessageBox box;
    if (!QFile(filename).exists()) {
      box.setText(tr("Please turn on the storage permission of the app."));
      box.exec();
    } else {
      box.setText(tr("Picture output successful!") + "\n\n" + filename);
      box.exec();
    }
#else
    QString filename = iniDir + strFile;
    pixmap.save(filename, "PNG");
#endif
  }
}

void dlgReport::appendSteps(QString date, QString steps, QString km) {
  QQuickItem* root = mw_one->ui->qwReport->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "appendTableRow",
                            Q_ARG(QVariant, date), Q_ARG(QVariant, steps),
                            Q_ARG(QVariant, km));
}

int dlgReport::getCount() {
  QQuickItem* root = mw_one->ui->qwReport->rootObject();
  QVariant itemCount;
  QMetaObject::invokeMethod((QObject*)root, "getItemCount",
                            Q_RETURN_ARG(QVariant, itemCount));
  return itemCount.toInt();
}

void dlgReport::delItem(int index) {
  QQuickItem* root = mw_one->ui->qwReport->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "delItem", Q_ARG(QVariant, index));
}

void dlgReport::clearAll() {
  int count = getCount();
  for (int i = 0; i < count; i++) delItem(0);
}

void dlgReport::appendSteps_xx(QString date, QString steps, QString km) {
  QQuickItem* root = mw_one->ui->qwReportSub->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "appendTableRow",
                            Q_ARG(QVariant, date), Q_ARG(QVariant, steps),
                            Q_ARG(QVariant, km));
}

int dlgReport::getCount_xx() {
  QQuickItem* root = mw_one->ui->qwReportSub->rootObject();
  QVariant itemCount;
  QMetaObject::invokeMethod((QObject*)root, "getItemCount",
                            Q_RETURN_ARG(QVariant, itemCount));
  return itemCount.toInt();
}

void dlgReport::delItem_xx(int index) {
  QQuickItem* root = mw_one->ui->qwReportSub->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "delItem", Q_ARG(QVariant, index));
}

void dlgReport::clearAll_xx() {
  int count = getCount_xx();
  for (int i = 0; i < count; i++) delItem_xx(0);
}

int dlgReport::getCurrentIndex() {
  QQuickItem* root = mw_one->ui->qwReport->rootObject();
  QVariant itemIndex;
  QMetaObject::invokeMethod((QObject*)root, "getCurrentIndex",
                            Q_RETURN_ARG(QVariant, itemIndex));
  return itemIndex.toInt();
}

QString dlgReport::getDate(int row) {
  QQuickItem* root = mw_one->ui->qwReport->rootObject();
  QVariant item;
  QMetaObject::invokeMethod((QObject*)root, "getDate",
                            Q_RETURN_ARG(QVariant, item), Q_ARG(QVariant, row));
  return item.toString();
}

void dlgReport::setCurrentHeader(int sn) {
  QQuickItem* root = mw_one->ui->qwReportSub->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "setHeader", Q_ARG(QVariant, sn));
}

void dlgReport::setScrollBarPos(double pos) {
  QQuickItem* root = mw_one->ui->qwReport->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "setScrollBarPos",
                            Q_ARG(QVariant, pos));
}

void dlgReport::setScrollBarPos_xx(double pos) {
  QQuickItem* root = mw_one->ui->qwReportSub->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "setScrollBarPos",
                            Q_ARG(QVariant, pos));
}

void dlgReport::loadDetails() {
  mw_one->ui->qwReportSub->setSource(
      QUrl(QStringLiteral("qrc:/src/qmlsrc/details.qml")));
  btnCategory->setText(tr("View Category"));
  mw_one->ui->lblDetails->setText(tr("Details"));
  clearAll_xx();
  setCurrentHeader(1);
  str_xx.clear();

  int row = getCurrentIndex();
  QString date = getDate(row);

  for (int i = 0; i < twOut2Img->topLevelItemCount(); i++) {
    QTreeWidgetItem* topItem = twOut2Img->topLevelItem(i);
    if (topItem->text(0) == date) {
      int childCount = topItem->childCount();

      for (int j = 0; j < childCount; j++) {
        QTreeWidgetItem* childItem = topItem->child(j);

        QString text0 = childItem->text(0);
        QStringList list = text0.split(".");
        if (list.count() == 2) text0 = list.at(1);

        QString text1 = childItem->text(1);
        QString text2 = childItem->text(2);

        if (!text0.contains(tr("Details"))) {
          appendSteps_xx(text0, text1, text2);

        } else {
          QString t0, t1, t2;
          QTreeWidgetItem* c_item = topItem->child(j - 1);
          t0 = c_item->text(0);
          QStringList list = t0.split(".");
          if (list.count() == 2) t0 = list.at(1);

          t1 = c_item->text(1);
          t2 = c_item->text(2);

          str_xx = str_xx + t0 + "  " + t1 + "  " + t2 + "\n" + text0 + "\n\n";
        }
      }
    }
  }

  if (str_xx.length() > 0) {
    mw_one->ui->lblDetails->setStyleSheet(
        mw_one->myEditRecord->lblStyleHighLight);

  } else
    mw_one->ui->lblDetails->setStyleSheet(
        mw_one->myEditRecord->ui->lblTitle->styleSheet());
}
