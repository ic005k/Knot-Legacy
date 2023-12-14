#include "Report.h"

#include <qdebug.h>

#include "MainWindow.h"
#include "ui_DateSelector.h"
#include "ui_MainWindow.h"
#include "ui_Report.h"

extern int fontSize;
extern MainWindow* mw_one;
extern QString iniFile, iniDir, btnYText, btnMText, btnDText;
extern QTabWidget *tabData, *tabChart;
extern bool isEBook, isReport;

QString btnYearText, btnMonthText;
QStringList listCategory;

QTreeWidget* twOut2Img;
QLabel *lblTotal, *lblDetails;
QToolButton *btnCategory, *btnMonth, *btnYear;
int twTotalRow = 0;
bool isWholeMonth = true;
bool isDateSection = false;

int s_y1 = 0;
int s_m1 = 0;
int s_d1 = 0;
int s_y2 = 0;
int s_m2 = 0;
int s_d2 = 0;

void setTableNoItemFlags(QTableWidget* t, int row);

dlgReport::dlgReport(QWidget* parent) : QDialog(parent), ui(new Ui::dlgReport) {
  ui->setupUi(this);

  this->installEventFilter(this);
  this->setModal(true);

  myDateSelector = new DateSelector(this);

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

  mw_one->ui->tableDetails->setColumnCount(1);
  mw_one->ui->tableDetails->setHorizontalHeaderItem(
      0, new QTableWidgetItem(tr("Details")));
  mw_one->ui->tableDetails->verticalHeader()->setSectionResizeMode(
      QHeaderView::ResizeToContents);
  mw_one->ui->tableDetails->setStyleSheet(
      "selection-background-color: lightblue");
  mw_one->ui->tableDetails->horizontalHeader()->setStretchLastSection(true);
  mw_one->ui->tableDetails->setAlternatingRowColors(true);
  mw_one->ui->tableDetails->setSelectionBehavior(QTableWidget::SelectRows);
  mw_one->ui->tableDetails->setSelectionMode(
      QAbstractItemView::SingleSelection);
  mw_one->ui->tableDetails->setEditTriggers(QTableWidget::NoEditTriggers);
  mw_one->ui->tableDetails->setHorizontalScrollBarPolicy(
      Qt::ScrollBarAlwaysOff);
  mw_one->ui->tableDetails->verticalScrollBar()->setStyleSheet(
      mw_one->vsbarStyleSmall);
  mw_one->ui->tableDetails->setVerticalScrollMode(QListWidget::ScrollPerPixel);
  QScroller::grabGesture(mw_one->ui->tableDetails,
                         QScroller::LeftMouseButtonGesture);
  mw_one->setSCrollPro(mw_one->ui->tableDetails);
  mw_one->ui->tableDetails->hide();

  mw_one->set_btnStyle(this);
}

void dlgReport::init() {
  mw_one->ui->frameReport->setGeometry(
      mw_one->geometry().x(), mw_one->geometry().y(),
      mw_one->geometry().width(), mw_one->geometry().height());
  mw_one->ui->qwReport->setFixedHeight(mw_one->height() / 3 - 15);
  mw_one->ui->frameMain->hide();
  mw_one->ui->frameReport->show();
  if (isWholeMonth)
    mw_one->ui->lblTitle_Report->setText(
        mw_one->ui->tabWidget->tabText(mw_one->ui->tabWidget->currentIndex()) +
        "(" + mw_one->ui->btnYear->text() + "-" + mw_one->ui->btnMonth->text() +
        ")");

  if (isDateSection) {
    QStringList listStart = mw_one->ui->btnStartDate->text().split("  ");
    QStringList listEnd = mw_one->ui->btnEndDate->text().split("  ");
    mw_one->ui->lblTitle_Report->setText(
        mw_one->ui->tabWidget->tabText(mw_one->ui->tabWidget->currentIndex()) +
        "(" + listStart.at(0) + "-" + listStart.at(1) + "-" + listStart.at(2) +
        "~" + listEnd.at(0) + "-" + listEnd.at(1) + "-" + listEnd.at(2) + ")");
  }
}

dlgReport::~dlgReport() { delete ui; }

void dlgReport::keyReleaseEvent(QKeyEvent* event) { Q_UNUSED(event) }

bool dlgReport::eventFilter(QObject* watch, QEvent* evn) {
  if (evn->type() == QEvent::KeyRelease) {
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
  indexCategory = 0;

  mw_one->ui->frameReport->hide();
  mw_one->ui->frameMain->show();
}

void dlgReport::closeEvent(QCloseEvent* event) {
  Q_UNUSED(event);
  mw_one->closeGrayWindows();
  mw_one->m_widget = new QWidget(mw_one);
}

void dlgReport::on_btnYear_clicked() {
  myDateSelector->dateFlag = 1;
  mw_one->ui->lblDetails->setText(tr("Details"));

  if (myDateSelector->nWidgetType == 1)
    myDateSelector->rboxYear->setValue(mw_one->ui->btnYear->text().toInt());
  if (myDateSelector->nWidgetType == 2)
    myDateSelector->wheelYear->setValue(mw_one->ui->btnYear->text().toInt());

  myDateSelector->init();
}

void dlgReport::startReport1() {
  btnYearText = mw_one->ui->btnYear->text();
  btnMonthText = mw_one->ui->btnMonth->text();

  isWholeMonth = true;
  isDateSection = false;
  mw_one->ui->lblTitle_Report->setText(
      mw_one->ui->tabWidget->tabText(mw_one->ui->tabWidget->currentIndex()) +
      "(" + mw_one->ui->btnYear->text() + "-" + mw_one->ui->btnMonth->text() +
      ")");
  listCategory.clear();
  mw_one->startInitReport();
}

void dlgReport::startReport2() {
  isWholeMonth = false;
  isDateSection = true;
  listCategory.clear();

  QString start = mw_one->ui->btnStartDate->text();
  QString end = mw_one->ui->btnEndDate->text();
  QStringList listStart = start.split("  ");
  QStringList listEnd = end.split("  ");

  s_y1 = listStart.at(0).toInt();
  s_m1 = listStart.at(1).toInt();
  s_d1 = listStart.at(2).toInt();
  s_y2 = listEnd.at(0).toInt();
  s_m2 = listEnd.at(1).toInt();
  s_d2 = listEnd.at(2).toInt();

  mw_one->ui->lblTitle_Report->setText(
      mw_one->ui->tabWidget->tabText(mw_one->ui->tabWidget->currentIndex()) +
      "(" + listStart.at(0) + "-" + listStart.at(1) + "-" + listStart.at(2) +
      "~" + listEnd.at(0) + "-" + listEnd.at(1) + "-" + listEnd.at(2) + ")");

  mw_one->startInitReport();
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

    bool isDetails = false;
    for (int j = 0; j < topItem->childCount(); j++) {
      QTreeWidgetItem* child = topItem->child(j);
      if (child->text(0).contains(tr("Details"))) {
        isDetails = true;
        break;
      }
    }

    if (isDetails) text0 = "*" + text0;

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
    QString strYear, strMonth;
    strYear = tw->topLevelItem(i)->text(3);
    strMonth = mw_one->get_Month(tw->topLevelItem(i)->text(0) + " " + strYear);
    int iDay = mw_one->get_Day(tw->topLevelItem(i)->text(0) + " " + strYear);

    if (isWholeMonth) {
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

    if (isDateSection) {
      int sy, sm, sd;
      sy = strYear.toInt();
      sm = strMonth.toInt();
      sd = iDay;
      QDateTime currentDateTime = QDateTime(QDate(sy, sm, sd), QTime(0, 0));
      QDateTime startDateTime = QDateTime(QDate(s_y1, s_m1, s_d1), QTime(0, 0));
      QDateTime endDateTime = QDateTime(QDate(s_y2, s_m2, s_d2), QTime(0, 0));
      int secondsDiff1 = startDateTime.secsTo(currentDateTime);
      int secondsDiff2 = currentDateTime.secsTo(endDateTime);

      if (secondsDiff1 >= 0 && secondsDiff2 >= 0) {
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
  newtop->setText(3, item->text(3));
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

    if (strClass.trimmed() != "") {
      listCategory.removeOne(strClass);
      listCategory.append(strClass);
    }

    QString strDes = item->child(z)->text(3);
    if (strDes.trimmed().length() > 0) {
      QTreeWidgetItem* des = new QTreeWidgetItem(newtop);
      des->setText(0, tr("Details") + " : " + strDes);
    }
  }

  twTotalRow = twTotalRow + newtop->childCount();
}

void dlgReport::on_btnMonth_clicked() {
  myDateSelector->dateFlag = 2;
  mw_one->ui->lblDetails->setText(tr("Details"));

  myDateSelector->rboxYear->setValue(mw_one->ui->btnYear->text().toInt());
  if (mw_one->ui->btnMonth->text().trimmed() == tr("Year-Round"))
    myDateSelector->rboxMonth->setValue(13);
  else
    myDateSelector->rboxMonth->setValue(mw_one->ui->btnMonth->text().toInt());

  myDateSelector->init();
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

  QStringList list1, list2;
  list1 = mw_one->ui->btnStartDate->text().split("  ");
  list2 = mw_one->ui->btnEndDate->text().split("  ");
  Reg.setValue("/YMD/Y1", list1.at(0));
  Reg.setValue("/YMD/Y2", list2.at(0));
  Reg.setValue("/YMD/M1", list1.at(1));
  Reg.setValue("/YMD/M2", list2.at(1));
  Reg.setValue("/YMD/D1", list1.at(2));
  Reg.setValue("/YMD/D2", list2.at(2));

  Reg.setValue("/YMD/isWholeMonth", isWholeMonth);
  Reg.setValue("/YMD/isDateSection", isDateSection);
}

int dlgReport::cmp(const void* a, const void* b) { return *(int*)a < *(int*)b; }

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
      ".QFrame{background-color: rgb(255, 255, 255);border-radius:2px; "
      "border:1px solid gray;}");

  QVBoxLayout* vbox = new QVBoxLayout;
  frame->setLayout(vbox);

  QLabel* lblTitle = new QLabel;
  QLabel* lblTotal = new QLabel;
  QFont ft = lblTitle->font();
  ft.setBold(true);
  lblTitle->setFont(ft);
  lblTitle->setText(mw_one->ui->lblTitle_Report->text());
  lblTotal->setText(mw_one->ui->lblTotal->text());
  vbox->addWidget(lblTitle);
  vbox->addWidget(lblTotal);

  QTableWidget* table = new QTableWidget;
  table->setColumnCount(2);
  table->setColumnHidden(1, true);

  table->setHorizontalHeaderItem(0, new QTableWidgetItem(tr("View Category")));

  table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

  table->horizontalHeader()->setStretchLastSection(true);
  table->setAlternatingRowColors(true);
  table->setSelectionBehavior(QTableWidget::SelectRows);
  table->setSelectionMode(QAbstractItemView::SingleSelection);
  table->setEditTriggers(QTableWidget::NoEditTriggers);
  table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  table->verticalScrollBar()->setStyleSheet(mw_one->vsbarStyleSmall);
  table->setVerticalScrollMode(QTableWidget::ScrollPerPixel);
  QScroller::grabGesture(table, QScroller::LeftMouseButtonGesture);
  mw_one->setSCrollPro(table);

  table->setStyleSheet("selection-background-color: lightblue");

  vbox->addWidget(table);

  QToolButton* btnOk = new QToolButton();
  btnOk->setText(tr("Ok"));
  btnOk->setFixedHeight(35);
  btnOk->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  btnOk->setStyleSheet(mw_one->ui->btnYear->styleSheet());
  vbox->addWidget(btnOk);

  if (listCategory.count() > 0) {
    listCategorySort.clear();
    listD.clear();
    for (int i = 0; i < listCategory.count(); i++) {
      getCategoryData(listCategory.at(i), false);
    }

    QList<double> listE = listD;
    std::sort(listE.begin(), listE.end());

    for (int j = 0; j < listE.count(); j++) {
      for (int i = 0; i < listCategorySort.count(); i++) {
        QString str1 = listCategorySort.at(i);
        QStringList l1 = str1.split("-=-");
        if (l1.count() == 2 && l1.at(0).split("|").at(0).trimmed() != "") {
          if (QString::number(listE.at(j)) == l1.at(1)) {
            QString str2 =
                l1.at(0) +
                "===" + QString("%1").arg(listE.at(j) * 100, 0, 'f', 2) + " %";

            table->insertRow(0);

            QString item0 = str2.split("|").at(0);

            QString pre = str2.split("===").at(1);

            QString item1 = str2.split("===").at(0).split("|").at(1);

            QLabel* lbl0 = new QLabel();
            lbl0->adjustSize();
            lbl0->setWordWrap(true);
            lbl0->setText(tr("Category") + " : " + item0);
            QFont font = this->font();
            font.setBold(true);
            lbl0->setFont(font);

            QLabel* lblPre = new QLabel();
            lblPre->adjustSize();
            lblPre->setWordWrap(true);
            lblPre->setText(tr("Percent") + " : " + pre);

            QLabel* lbl1 = new QLabel();
            lbl1->adjustSize();
            lbl1->setWordWrap(true);
            lbl1->setText(tr("Amount") + " : " + item1);

            QWidget* itemW = new QWidget();
            QVBoxLayout* vbox = new QVBoxLayout();
            vbox->addWidget(lbl0);
            vbox->addWidget(lblPre);
            vbox->addWidget(lbl1);
            itemW->setLayout(vbox);

            table->setCellWidget(0, 0, itemW);

            QString item = tr("Category") + " : " + item0 + "\n" +
                           tr("Percent") + " : " + pre + "\n" + tr("Amount") +
                           " : " + item1;
            table->setItem(0, 1, new QTableWidgetItem(item));

            listCategorySort.removeOne(str1);

            break;
          }
        }
      }
    }

    if (table->rowCount() > 0)
      table->setHorizontalHeaderItem(
          0, new QTableWidgetItem(tr("View Category") + "  " +
                                  QString::number(table->rowCount())));

    // qDebug() << "listCategorySort=" << listCategorySort.count()
    //        << listCategorySort;
    // qDebug() << "listE=" << listE.count() << listE;
  }

  int h = mw_one->geometry().height() - 0;
  int w = mw_one->width() - 0;
  int x = mw_one->geometry().x() + (mw_one->width() - w) / 2;
  int y = mw_one->geometry().y() + (mw_one->height() - h) / 2;
  dlg->setGeometry(x, y, w, h);
  if (table->rowCount() > 0) {
    mw_one->showGrayWindows();
    dlg->show();
    table->setFocus();
    if (table->rowCount() - 1 < indexCategory) indexCategory = 0;
    table->setCurrentCell(indexCategory, 0);
  }

  connect(table, &QTableWidget::itemClicked, [=]() {
    QString str0 = table->item(table->currentRow(), 0)->text();
    str0 = str0.split("\n").at(0);
    str0 = str0.replace(tr("Category") + " : ", "").trimmed();
    getCategoryData(str0, true);
    indexCategory = table->currentRow();
    dlg->close();
    mw_one->closeGrayWindows();
  });

  connect(btnOk, &QToolButton::clicked, [=]() {
    QString str0 = table->item(table->currentRow(), 1)->text();
    str0 = str0.split("\n").at(0);
    str0 = str0.replace(tr("Category") + " : ", "").trimmed();
    getCategoryData(str0, true);
    indexCategory = table->currentRow();
    dlg->close();
    mw_one->closeGrayWindows();
  });

  connect(dlg, &QDialog::rejected, [=]() {
    dlg->close();
    mw_one->closeGrayWindows();
  });
}

void dlgReport::getCategoryData(QString strCategory, bool appendTable) {
  if (appendTable) {
    clearAll_xx();
    setCurrentHeader(2);

    mw_one->ui->tableDetails->setRowCount(0);
  }

  int freq = 0;
  double d_amount = 0;
  QTreeWidget* tw = twOut2Img;
  for (int i = 0; i < tw->topLevelItemCount(); i++) {
    QTreeWidgetItem* topItem = tw->topLevelItem(i);

    for (int j = 0; j < topItem->childCount(); j++) {
      QTreeWidgetItem* childItem = topItem->child(j);
      QString strClass = childItem->text(2);
      if (strClass == strCategory && strClass.trimmed() != "") {
        freq++;
        QString date, time, details;
        if (appendTable) {
          date = topItem->text(3) + "-" + topItem->text(0).split(" ").at(1) +
                 "-" + topItem->text(0).split(" ").at(2);
          time = childItem->text(0).split(".").at(1);

          if (j + 1 < topItem->childCount()) {
            QTreeWidgetItem* nextChild = topItem->child(j + 1);

            if (nextChild->text(0).contains(tr("Details"))) {
              details = nextChild->text(0);
            }
          }
        }
        QString amount = childItem->text(1);
        if (appendTable) {  // appendSteps_xx(date, time, amount);
          mw_one->ui->tableDetails->setRowCount(freq);

          QString str;
          if (details.trimmed().length() > 0)
            str = details;
          else
            str = "";

          QTableWidgetItem* item = new QTableWidgetItem(str);
          mw_one->ui->tableDetails->setItem(freq - 1, 0, item);

          mw_one->mySearchDialog->addItemBakList(
              mw_one->ui->qwReportSub, tr("Date") + " : " + date + "  " + time,
              tr("Amount") + " : " + amount, str, "", 0);
        }

        if (amount.length() > 0) {
          d_amount = d_amount + amount.toDouble();
        }
      }
    }
  }

  double bfb;
  if (t_amount > 0) bfb = d_amount / t_amount;

  QString ta = QString("%1").arg(d_amount, 0, 'f', 2);
  if (appendTable) {
    // appendSteps_xx(tr("Total"), QString::number(freq), ta);

    mw_one->ui->tableDetails->setHorizontalHeaderItem(
        0, new QTableWidgetItem(strCategory + "\n" + tr("Freq") + " : " +
                                QString::number(freq) + "  " + tr("Amount") +
                                " : " + ta));

    mw_one->ui->lblDetails->setStyleSheet(
        mw_one->myEditRecord->ui->lblTitle->styleSheet());
    mw_one->ui->lblDetails->setText(strCategory + "\n" + tr("Freq") + " : " +
                                    QString::number(freq) + "  " +
                                    tr("Amount") + " : " + ta);

    setScrollBarPos_xx(0);
  } else {
    listCategorySort.append(strCategory + "|" + ta + "-=-" +
                            QString::number(bfb));
    listD.append(bfb);
  }

  if (mw_one->ui->tableDetails->rowCount() > 0)
    mw_one->ui->tableDetails->setCurrentCell(0, 0);
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
    QTreeWidgetItem* item1 = new QTreeWidgetItem;
    QString st = mw_one->ui->lblTitle_Report->text();
    QStringList st_list = st.split("(");
    QString st1 = st_list.at(0);
    QString st2 = "(" + st_list.at(1);
    item0->setText(0, st2);
    twOut2Img->insertTopLevelItem(0, item0);
    item1->setText(0, st1);
    twOut2Img->insertTopLevelItem(0, item1);

    QTreeWidgetItem* item = new QTreeWidgetItem;
    item->setText(0, tr("Total") + " : ");
    item->setText(1, tr("Freq") + " " + QString::number(freq));
    item->setText(2,
                  tr("Amount") + " " + QString("%1").arg(t_amount, 0, 'f', 2));
    twOut2Img->addTopLevelItem(item);

    twTotalRow = twTotalRow + 4;
    qreal h = twTotalRow * 28;
    twOut2Img->setGeometry(0, 0, this->width(), h);

    // The column merge is implemented
    QTreeWidget* m_t = new QTreeWidget(this);
    QFont f = this->font();
    f.setPointSize(f.pointSize() - 2);
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

    // Method1
    QPixmap pixmap(m_t->size());
    m_t->render(&pixmap);

    // Method2
    // QPixmap pixmap = QPixmap::grabWidget(m_t);

    QString strFile;
    strFile = mw_one->ui->lblTitle_Report->text() + ".png";

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
    QString fileName;
    fileName = QFileDialog::getSaveFileName(this, tr("Save Config"),
                                            QDir::homePath() + "/" + strFile,
                                            tr("PNG files(*.png)"));

    if (!fileName.isNull()) {
      pixmap.save(fileName, "PNG");
    }

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
  QTreeWidget* tw = mw_one->get_tw(tabData->currentIndex());
  mw_one->ui->tableDetails->setRowCount(0);
  mw_one->ui->btnCategory->setText(tr("View Category"));

  int row = getCurrentIndex();
  QString date = getDate(row);
  date.replace("*", "");
  date = date.trimmed();

  for (int i = 0; i < tw->topLevelItemCount(); i++) {
    QTreeWidgetItem* topItem = tw->topLevelItem(i);
    if (topItem->text(0) == date) {
      mw_one->ui->tableDetails->setHorizontalHeaderItem(
          0, new QTableWidgetItem(tr("Details") + "    " + date + "    " +
                                  topItem->text(3)));

      int childCount = topItem->childCount();
      mw_one->ui->tableDetails->setRowCount(childCount);
      for (int j = 0; j < childCount; j++) {
        QTreeWidgetItem* childItem = topItem->child(j);

        QString text0 = childItem->text(0);
        QStringList list = text0.split(".");
        if (list.count() == 2) text0 = list.at(1).trimmed();

        QString text1 = childItem->text(1);
        QString text2 = childItem->text(2);
        QString text3 = childItem->text(3);

        QLabel* lbl0 = new QLabel();
        QLabel* lbl1 = new QLabel();
        QLabel* lbl2 = new QLabel();
        QLabel* lbl3 = new QLabel();

        lbl0->adjustSize();
        lbl0->setWordWrap(true);
        lbl1->adjustSize();
        lbl1->setWordWrap(true);
        lbl2->adjustSize();
        lbl2->setWordWrap(true);
        lbl3->adjustSize();
        lbl3->setWordWrap(true);

        lbl0->setText(tr("Time") + " : " + text0);
        lbl1->setText(tr("Amount") + " : " + text1);
        lbl2->setText(tr("Category") + " : " + text2);
        lbl3->setText(tr("Details") + " : " + text3);

        QWidget* widget = new QWidget();
        QVBoxLayout* vbox = new QVBoxLayout;
        widget->setLayout(vbox);
        vbox->addWidget(lbl0);
        if (text1.trimmed().length() > 0) vbox->addWidget(lbl1);
        if (text2.trimmed().length() > 0) vbox->addWidget(lbl2);
        if (text3.trimmed().length() > 0) vbox->addWidget(lbl3);

        mw_one->ui->tableDetails->setCellWidget(j, 0, widget);
      }
    }
  }

  if (mw_one->ui->tableDetails->rowCount() > 0)
    mw_one->ui->tableDetails->setCurrentCell(0, 0);
}

void dlgReport::loadDetailsQml() {
  btnCategory->setText(tr("View Category"));
  mw_one->ui->lblDetails->setText(tr("Details"));
  clearAll_xx();
  setCurrentHeader(1);
  str_xx.clear();

  int row = getCurrentIndex();
  QString date = getDate(row);
  date.replace("*", "");
  date = date.trimmed();

  QTreeWidget* tw = mw_one->get_tw(tabData->currentIndex());
  for (int i = 0; i < tw->topLevelItemCount(); i++) {
    QTreeWidgetItem* topItem = tw->topLevelItem(i);
    if (topItem->text(0) == date) {
      mw_one->ui->lblDetails->setText(tr("Details") + "    " + date + "    " +
                                      topItem->text(3));

      int childCount = topItem->childCount();

      for (int j = 0; j < childCount; j++) {
        QTreeWidgetItem* childItem = topItem->child(j);

        QString text0 = childItem->text(0);
        QStringList list = text0.split(".");
        if (list.count() == 2) text0 = list.at(1).trimmed();
        text0 = tr("Time") + " : " + text0;

        QString text1 = childItem->text(1);
        QString text2 = childItem->text(2);
        QString text3 = childItem->text(3);

        QString str1, str2, str3;
        if (text1.trimmed().length() > 0) str1 = tr("Amount") + " : " + text1;
        if (text2.trimmed().length() > 0) str2 = tr("Category") + " : " + text2;
        if (text3.trimmed().length() > 0) str3 = tr("Details") + " : " + text3;

        // appendSteps_xx(text0, text1, text2);
        mw_one->mySearchDialog->addItemBakList(mw_one->ui->qwReportSub, text0,
                                               str1, str2, str3, 0);
      }
    }
  }
}
