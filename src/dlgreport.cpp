#include "dlgreport.h"

#include "mainwindow.h"
#include "ui_dlgreport.h"
#include "ui_mainwindow.h"

extern int fontSize;
extern MainWindow* mw_one;
extern QString iniFile, iniDir, btnYText, btnMText, btnDText;
extern QTabWidget *tabData, *tabChart;
extern bool isImport, isEBook, isReport, isBreakReport, isReportWindowsShow,
    isRunCategory;
QString btnYearText, btnMonthText;
QTableWidget *tableReport, *tableReport0, *tableDetails, *tableCategory;
QLabel *lblTotal, *lblDetails;
QToolButton *btnCategory, *btnMonth, *btnYear;

void setTableNoItemFlags(QTableWidget* t, int row);

dlgReport::dlgReport(QWidget* parent) : QDialog(parent), ui(new Ui::dlgReport) {
  ui->setupUi(this);
  this->installEventFilter(this);
  this->setModal(true);
  tableReport = ui->tableReport;
  tableDetails = ui->tableDetails;
  tableCategory = ui->tableCategory;
  lblTotal = ui->lblTotal;
  lblDetails = ui->lblDetails;
  btnCategory = ui->btnCategory;
  btnMonth = ui->btnMonth;
  btnYear = ui->btnYear;
  ui->tableCategory->hide();
  printer = new QPrinter(QPrinter::HighResolution);
  preview = new QPrintPreviewDialog(printer, this);
  ui->btnPrint->hide();

  QFont font = ui->lblTotal->font();
  font.setBold(true);
  ui->lblTotal->setFont(font);
  ui->lblDetails->setFont(font);
  ui->lblTotal->setStyleSheet("background-color: rgb(25, 0, 250);color:white");
  ui->lblDetails->setStyleSheet(
      "background-color: rgb(250, 0, 25);color:white");
  ui->lblDetails->setWordWrap(true);
  ui->lblDetails->adjustSize();

  for (int y = 0; y < ui->tableReport->columnCount(); y++) {
    ui->tableReport->horizontalHeader()->setSectionResizeMode(
        y, QHeaderView::ResizeToContents);
  }
  for (int y = 0; y < ui->tableDetails->columnCount(); y++) {
    ui->tableDetails->horizontalHeader()->setSectionResizeMode(
        y, QHeaderView::ResizeToContents);
  }
  for (int y = 0; y < ui->tableCategory->columnCount(); y++) {
    ui->tableCategory->horizontalHeader()->setSectionResizeMode(
        y, QHeaderView::ResizeToContents);
  }

  ui->tableReport->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui->tableReport->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->tableReport->setSelectionMode(QAbstractItemView::SingleSelection);
  ui->tableReport->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->tableReport->setVerticalScrollMode(QTableWidget::ScrollPerPixel);
  QScroller::grabGesture(ui->tableReport, QScroller::LeftMouseButtonGesture);
  ui->tableReport->verticalScrollBar()->setStyleSheet(mw_one->vsbarStyleSmall);
  mw_one->setSCrollPro(ui->tableReport);

  ui->tableDetails->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui->tableDetails->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->tableDetails->setSelectionMode(QAbstractItemView::SingleSelection);
  ui->tableDetails->setVerticalScrollMode(QTableWidget::ScrollPerPixel);
  QScroller::grabGesture(ui->tableDetails, QScroller::LeftMouseButtonGesture);
  ui->tableDetails->verticalScrollBar()->setStyleSheet(mw_one->vsbarStyleSmall);
  mw_one->setSCrollPro(ui->tableDetails);
  //自动换行，数据量大时，效率非常低，不推荐
  // connect(ui->tableDetails, SIGNAL(itemChanged(QTableWidgetItem*)),
  //         ui->tableDetails, SLOT(resizeRowsToContents()));

  ui->tableCategory->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui->tableCategory->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->tableCategory->setSelectionMode(QAbstractItemView::SingleSelection);
  ui->tableCategory->setVerticalScrollMode(QTableWidget::ScrollPerPixel);
  QScroller::grabGesture(ui->tableCategory, QScroller::LeftMouseButtonGesture);
  ui->tableCategory->verticalScrollBar()->setStyleSheet(
      mw_one->vsbarStyleSmall);
  mw_one->setSCrollPro(ui->tableCategory);

  font = ui->tableReport->horizontalHeader()->font();
  font.setBold(true);
  ui->tableReport->horizontalHeader()->setFont(font);
  ui->tableDetails->horizontalHeader()->setFont(font);
}

void dlgReport::init() {
  this->setGeometry(mw_one->geometry().x(), mw_one->geometry().y(),
                    mw_one->width(), mw_one->height());
  show();
}

dlgReport::~dlgReport() { delete ui; }

void dlgReport::keyReleaseEvent(QKeyEvent* event) { Q_UNUSED(event) }

bool dlgReport::eventFilter(QObject* watch, QEvent* evn) {
  if (evn->type() == QEvent::KeyPress) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      on_btnBack_clicked();
      return true;
    }
  }

  return QWidget::eventFilter(watch, evn);
}

void dlgReport::on_btnBack_clicked() {
  mw_one->startSave("ymd");
  if (!preview->isHidden()) {
    preview->close();
    return;
  }
  close();

  isReportWindowsShow = false;
  listCategory.clear();

  delete mw_one->mydlgFloatFun;
  mw_one->mydlgFloatFun = new dlgFloatFun(this);
  mw_one->mydlgFloatFun->init();
}

void dlgReport::on_btnYear_clicked() {
  ui->lblDetails->setText(tr("Details"));
  int w = ui->btnYear->width();
  QListWidget* list = new QListWidget(this);
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
    ui->btnYear->setText(list->currentItem()->text());
    btnYearText = ui->btnYear->text();
    list->close();

    listCategory.clear();
    mw_one->on_actionReport_triggered();
  });

  int h = 30 * list->count() + 4;
  int y = ui->btnYear->y();

  list->setGeometry(ui->btnYear->x(), y, w, h);

  list->show();

  QString str = ui->btnYear->text();
  for (int i = 0; i < list->count(); i++) {
    if (str == list->item(i)->text()) {
      list->setCurrentRow(i);
      break;
    }
  }
}

void dlgReport::sel_Year() {
  QTreeWidget* tw = mw_one->get_tw(tabData->currentIndex());
  tableReport->setRowCount(0);
  tableDetails->setRowCount(0);
  int freq = 0;
  double amount = 0;
  int j = 0;
  for (int i = 0; i < tw->topLevelItemCount(); i++) {
    if (isBreakReport) {
      break;
      return;
    }

    QString strYear = mw_one->get_Year(tw->topLevelItem(i)->text(0));
    if (strYear == btnYearText) {
      tableReport->setRowCount(tableReport->rowCount() + 1);

      QTableWidgetItem* tableItem =
          new QTableWidgetItem(tw->topLevelItem(i)->text(0));
      tableReport->setItem(j, 0, tableItem);

      QString txt1 = tw->topLevelItem(i)->text(1);
      freq = freq + txt1.toInt();
      tableItem = new QTableWidgetItem(txt1);
      tableItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
      tableReport->setItem(j, 1, tableItem);

      QString txt2 = tw->topLevelItem(i)->text(2);
      amount = amount + txt2.toDouble();
      tableItem = new QTableWidgetItem(txt2);
      tableReport->setItem(j, 2, tableItem);

      tableReport->setColumnWidth(0, 10);
      tableReport->setRowHeight(j, 30);

      setTableNoItemFlags(tableReport, j);

      j++;
    }
  }

  lblTotal->setText(tr("Total") + " : " + tr("Freq") + " 0    " + tr("Amount") +
                    " 0");
  int count = tableReport->rowCount();
  if (count > 0) {
    tableReport->setRowCount(count + 1);
    QTableWidgetItem* tableItem = new QTableWidgetItem(tr("Total"));
    tableReport->setItem(count, 0, tableItem);

    tableItem = new QTableWidgetItem(QString::number(freq));
    tableItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    tableReport->setItem(count, 1, tableItem);

    QString strAmount = QString("%1").arg(amount, 0, 'f', 2);
    tableItem = new QTableWidgetItem(strAmount);
    tableReport->setItem(count, 2, tableItem);

    lblTotal->setText(tr("Total") + " : " + tr("Freq") + " " +
                      QString::number(freq) + "    " + tr("Amount") + " " +
                      strAmount);

    tableReport->setColumnWidth(0, 10);
    tableReport->setRowHeight(count, 30);

    setTableNoItemFlags(tableReport, count);

    on_tableReport_cellClicked(0, 0);
  }

  btnCategory->setText(tr("Category"));
  tableCategory->hide();
  tableDetails->show();
}

void dlgReport::sel_Month() {
  if (btnMonth->text() == tr("Year-Round")) {
    sel_Year();
    return;
  }

  QTreeWidget* tw = mw_one->get_tw(tabData->currentIndex());
  tableReport->setRowCount(0);
  tableDetails->setRowCount(0);
  int freq = 0;
  double amount = 0;
  int j = 0;
  for (int i = 0; i < tw->topLevelItemCount(); i++) {
    if (isBreakReport) {
      break;
      return;
    }

    QString strYear = mw_one->get_Year(tw->topLevelItem(i)->text(0));
    QString strMonth = mw_one->get_Month(tw->topLevelItem(i)->text(0));
    if (strYear == btnYearText && strMonth == btnMonthText) {
      tableReport->setRowCount(tableReport->rowCount() + 1);

      QTableWidgetItem* tableItem =
          new QTableWidgetItem(tw->topLevelItem(i)->text(0));
      tableReport->setItem(j, 0, tableItem);

      QString txt1 = tw->topLevelItem(i)->text(1);
      freq = freq + txt1.toInt();
      tableItem = new QTableWidgetItem(txt1);
      tableItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
      tableReport->setItem(j, 1, tableItem);

      QString txt2 = tw->topLevelItem(i)->text(2);
      amount = amount + txt2.toDouble();
      tableItem = new QTableWidgetItem(txt2);
      tableReport->setItem(j, 2, tableItem);

      tableReport->setColumnWidth(0, 10);
      tableReport->setRowHeight(j, 30);

      tableReport->item(j, 0)->setFlags(Qt::NoItemFlags);
      tableReport->item(j, 1)->setFlags(Qt::NoItemFlags);
      tableReport->item(j, 2)->setFlags(Qt::NoItemFlags);

      j++;
    }
  }

  lblTotal->setText(tr("Total") + " : " + tr("Freq") + " 0    " + tr("Amount") +
                    " 0");

  int count = tableReport->rowCount();
  if (count > 0) {
    tableReport->setRowCount(count + 1);
    QTableWidgetItem* tableItem = new QTableWidgetItem(tr("Total"));
    tableReport->setItem(count, 0, tableItem);

    tableItem = new QTableWidgetItem(QString::number(freq));
    tableItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    tableReport->setItem(count, 1, tableItem);

    QString strAmount = QString("%1").arg(amount, 0, 'f', 2);
    tableItem = new QTableWidgetItem(strAmount);
    tableReport->setItem(count, 2, tableItem);

    lblTotal->setText(tr("Total") + " : " + tr("Freq") + " " +
                      QString::number(freq) + "    " + tr("Amount") + " " +
                      strAmount);

    tableReport->setColumnWidth(0, 10);
    tableReport->setRowHeight(count, 30);

    tableReport->item(count, 0)->setFlags(Qt::NoItemFlags);
    tableReport->item(count, 1)->setFlags(Qt::NoItemFlags);
    tableReport->item(count, 2)->setFlags(Qt::NoItemFlags);

    on_tableReport_cellClicked(0, 0);
  }

  btnCategory->setText(tr("Category"));
  tableCategory->hide();
  tableDetails->show();
}

void dlgReport::updateTable() {
  tableReport->setRowCount(0);
  tableDetails->setRowCount(0);

  int freq = 0;
  double amount = 0;

  for (int i = 0; i < tableReport0->rowCount(); i++) {
    tableReport->setRowCount(tableReport->rowCount() + 1);

    QTableWidgetItem* tableItem =
        new QTableWidgetItem(tableReport0->item(i, 0)->text());
    tableReport->setItem(i, 0, tableItem);

    QString txt1 = tableReport0->item(i, 1)->text();
    freq = freq + txt1.toInt();
    tableItem = new QTableWidgetItem(txt1);
    tableItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    tableReport->setItem(i, 1, tableItem);

    QString txt2 = tableReport0->item(i, 2)->text();
    amount = amount + txt2.toDouble();
    tableItem = new QTableWidgetItem(txt2);
    tableReport->setItem(i, 2, tableItem);

    tableReport->setColumnWidth(0, 10);
    tableReport->setRowHeight(i, 30);

    tableReport->item(i, 0)->setFlags(Qt::NoItemFlags);
    tableReport->item(i, 1)->setFlags(Qt::NoItemFlags);
    tableReport->item(i, 2)->setFlags(Qt::NoItemFlags);
  }

  lblTotal->setText(tr("Total") + " : " + tr("Freq") + " 0    " + tr("Amount") +
                    " 0");

  int count = tableReport->rowCount();
  if (count > 0) {
    tableReport->setRowCount(count + 1);
    QTableWidgetItem* tableItem = new QTableWidgetItem(tr("Total"));
    tableReport->setItem(count, 0, tableItem);

    tableItem = new QTableWidgetItem(QString::number(freq));
    tableItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    tableReport->setItem(count, 1, tableItem);

    QString strAmount = QString("%1").arg(amount, 0, 'f', 2);
    tableItem = new QTableWidgetItem(strAmount);
    tableReport->setItem(count, 2, tableItem);

    lblTotal->setText(tr("Total") + " : " + tr("Freq") + " " +
                      QString::number(freq) + "    " + tr("Amount") + " " +
                      strAmount);

    tableReport->setColumnWidth(0, 10);
    tableReport->setRowHeight(count, 30);

    tableReport->item(count, 0)->setFlags(Qt::NoItemFlags);
    tableReport->item(count, 1)->setFlags(Qt::NoItemFlags);
    tableReport->item(count, 2)->setFlags(Qt::NoItemFlags);

    on_tableReport_cellClicked(0, 0);
  }

  btnCategory->setText(tr("Category"));
  tableCategory->hide();
  tableDetails->show();
}

void dlgReport::getMonthData() {
  tableReport0 = new QTableWidget;
  tableReport0->setColumnCount(3);
  tableReport0->setRowCount(0);
  QTreeWidget* tw = mw_one->get_tw(tabData->currentIndex());
  int freq = 0;
  double amount = 0;
  int j = 0;

  for (int i = 0; i < tw->topLevelItemCount(); i++) {
    if (isBreakReport) {
      break;
      return;
    }

    QString strYear = mw_one->get_Year(tw->topLevelItem(i)->text(0));
    QString strMonth = mw_one->get_Month(tw->topLevelItem(i)->text(0));

    if (btnMonthText == tr("Year-Round")) {
      if (strYear == btnYearText) {
        tableReport0->setRowCount(tableReport0->rowCount() + 1);

        QString txt0 = tw->topLevelItem(i)->text(0);
        QTableWidgetItem* tableItem = new QTableWidgetItem;
        tableItem->setText(txt0);
        tableReport0->setItem(j, 0, tableItem);

        QString txt1 = tw->topLevelItem(i)->text(1);
        freq = freq + txt1.toInt();
        tableItem = new QTableWidgetItem(txt1);
        tableItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        tableReport0->setItem(j, 1, tableItem);

        QString txt2 = tw->topLevelItem(i)->text(2);
        amount = amount + txt2.toDouble();
        tableItem = new QTableWidgetItem(txt2);
        tableReport0->setItem(j, 2, tableItem);

        j++;
      }
    } else {
      if (strYear == btnYearText && strMonth == btnMonthText) {
        tableReport0->setRowCount(tableReport0->rowCount() + 1);

        QTableWidgetItem* tableItem =
            new QTableWidgetItem(tw->topLevelItem(i)->text(0));
        tableReport0->setItem(j, 0, tableItem);

        QString txt1 = tw->topLevelItem(i)->text(1);
        freq = freq + txt1.toInt();
        tableItem = new QTableWidgetItem(txt1);
        tableItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        tableReport0->setItem(j, 1, tableItem);

        QString txt2 = tw->topLevelItem(i)->text(2);
        amount = amount + txt2.toDouble();
        tableItem = new QTableWidgetItem(txt2);
        tableReport0->setItem(j, 2, tableItem);

        j++;
      }
    }
  }
}

void dlgReport::on_btnMonth_clicked() {
  ui->lblDetails->setText(tr("Details"));
  int w = ui->btnYear->width();
  QListWidget* list = new QListWidget(this);
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
    ui->btnMonth->setText(list->currentItem()->text());
    btnMonthText = ui->btnMonth->text();
    list->close();

    listCategory.clear();
    mw_one->on_actionReport_triggered();
  });

  int h = 30 * list->count() + 4;
  int y = ui->btnMonth->y();

  list->setGeometry(ui->btnMonth->x(), y, w, h);

  list->show();

  QString str = ui->btnMonth->text();
  for (int i = 0; i < list->count(); i++) {
    if (str == list->item(i)->text()) {
      list->setCurrentRow(i);
      break;
    }
  }
}

void dlgReport::on_tableReport_cellClicked(int row, int column) {
  Q_UNUSED(column);
  bool isSetDetailsText = true;
  if (btnCategory->text() != tr("None") &&
      btnCategory->text() != tr("Category")) {
    isSetDetailsText = false;
  }

  if (isSetDetailsText) lblDetails->setText(tr("Details"));
  markColor(row);
  tableDetails->setRowCount(0);
  QString str = tableReport->item(row, 0)->text();
  QTreeWidget* tw = mw_one->get_tw(tabData->currentIndex());

  for (int i = 0; i < tw->topLevelItemCount(); i++) {
    QTreeWidgetItem* topItem = tw->topLevelItem(i);
    if (str == topItem->text(0)) {
      if (isSetDetailsText) lblDetails->setText(tr("Details") + " : " + str);
      int childCount = topItem->childCount();
      tableDetails->setRowCount(childCount);
      for (int m = 0; m < childCount; m++) {
        tableDetails->setColumnWidth(0, 10);
        tableDetails->setRowHeight(m, 30);

        QTableWidgetItem* tableItem;
        QString str = topItem->child(m)->text(0);
        QStringList list = str.split(".");
        if (list.count() == 2)
          tableItem = new QTableWidgetItem(list.at(1));
        else
          tableItem = new QTableWidgetItem(str);
        tableDetails->setItem(m, 0, tableItem);
        tableItem = new QTableWidgetItem(topItem->child(m)->text(1));
        tableDetails->setItem(m, 1, tableItem);
        tableItem = new QTableWidgetItem(topItem->child(m)->text(2));
        tableDetails->setItem(m, 2, tableItem);

        tableDetails->item(m, 0)->setFlags(Qt::NoItemFlags);
        tableDetails->item(m, 1)->setFlags(Qt::NoItemFlags);
        tableDetails->item(m, 2)->setFlags(Qt::NoItemFlags);

        //数量特别大的时候，影响性能
        // tableDetails->scrollToBottom();
      }
      break;
    }
  }
}

void dlgReport::markColor(int row) {
  QIcon icon;
  int size = 10;
  icon.addFile(":/res/sel.png", QSize(size, size));
  QIcon icon1;
  icon1.addFile(":/res/nosel.png", QSize(size, size));
  QTableWidgetItem* id1;

  QBrush brush1 = QBrush(QColor(253, 185, 176));
  for (int i = 0; i < tableReport->rowCount(); i++) {
    if (i == row) {
      id1 = new QTableWidgetItem(icon, QString::number(i + 1) + "  ");
      id1->setBackground(brush1);

    } else {
      id1 = new QTableWidgetItem(icon1, QString::number(i + 1) + "  ");
    }
    tableReport->setVerticalHeaderItem(i, id1);
  }
}

void dlgReport::saveYMD() {
  QSettings Reg(iniDir + "ymd.ini", QSettings::IniFormat);
  Reg.setValue("/YMD/btnYearText", btnYearText);
  Reg.setValue("/YMD/btnMonthText", btnMonthText);
  Reg.setValue("/YMD/btnYText", btnYText);
  Reg.setValue("/YMD/btnMText", btnMText);
  Reg.setValue("/YMD/btnDText", btnDText);
}

void dlgReport::on_btnCategory_clicked() {
  int count = tableReport->rowCount();
  if (count == 0) {
    btnCategory->setText(tr("Category"));
    return;
  }
  QListWidget* list = new QListWidget(mw_one->mydlgReport);
  list->setStyleSheet(mw_one->listStyle);
  list->verticalScrollBar()->setStyleSheet(mw_one->vsbarStyleSmall);
  list->setVerticalScrollMode(QListWidget::ScrollPerPixel);
  list->horizontalScrollBar()->hide();
  QScroller::grabGesture(list, QScroller::LeftMouseButtonGesture);
  QFont font;
  font.setPointSize(fontSize);
  list->setFont(font);

  if (listCategory.count() == 0) {
    QStringList listType;
    listType.append(tr("None"));
    QListWidgetItem* pItem = new QListWidgetItem();
    pItem->setSizeHint(QSize(btnCategory->width() - 20, 30));
    pItem->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    pItem->setText(tr("None"));
    list->addItem(pItem);

    listCategory.append(tr("None"));

    for (int i = 0; i < count - 1; i++) {
      on_tableReport_cellClicked(i, 0);
      for (int j = 0; j < tableDetails->rowCount(); j++) {
        QString str = tableDetails->item(j, 2)->text().trimmed();
        if (str != "") {
          if (!listType.removeOne(str)) {
            listType.append(str);
            QListWidgetItem* pItem = new QListWidgetItem();
            pItem->setSizeHint(QSize(btnCategory->width() - 20, 30));
            pItem->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
            pItem->setText(str);
            list->addItem(pItem);

            listCategory.append(str);
          } else
            listType.append(str);
        }
      }
    }
  } else {
    for (int i = 0; i < listCategory.count(); i++) {
      QListWidgetItem* pItem = new QListWidgetItem();
      pItem->setSizeHint(QSize(btnCategory->width() - 20, 30));
      pItem->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
      pItem->setText(listCategory.at(i));
      list->addItem(pItem);
    }
  }

  int h = mw_one->height() / 2;
  if (list->count() * 30 < h) h = list->count() * 30 + 4;
  list->setGeometry(btnCategory->x(), btnCategory->y() - h / 2,
                    btnCategory->width(), h);
  if (list->count() > 1) list->show();

  connect(list, &QListWidget::itemClicked, [=]() {
    btnCategory->setText(list->currentItem()->text());
    if (list->currentRow() == 0) {
      lblDetails->setText(tr("Details"));
      tableCategory->hide();
      tableDetails->show();
      list->close();
      return;
    }

    mw_one->on_RunCategory();

    list->close();
  });
}

void dlgReport::runCategory() {
  tableCategory->setRowCount(0);
  double abc = 0;
  double total =
      tableReport->item(tableReport->rowCount() - 1, 2)->text().toDouble();
  for (int i = 0; i < tableReport->rowCount(); i++) {
    on_tableReport_cellClicked(i, 0);

    for (int j = 0; j < tableDetails->rowCount(); j++) {
      if (tableDetails->item(j, 2)->text() == btnCategory->text()) {
        int count = tableCategory->rowCount();
        tableCategory->setRowCount(count + 1);
        QString str0 = tableReport->item(i, 0)->text();
        QStringList list0 = str0.split(" ");
        if (list0.count() == 4) {
          str0 = list0.at(0) + " " + list0.at(1) + " " + list0.at(2);
        }
        tableCategory->setItem(count, 0, new QTableWidgetItem(str0));
        tableCategory->setItem(
            count, 1, new QTableWidgetItem(tableDetails->item(j, 0)->text()));
        tableCategory->setItem(
            count, 2, new QTableWidgetItem(tableDetails->item(j, 1)->text()));
        abc = abc + tableDetails->item(j, 1)->text().toDouble();

        tableCategory->item(count, 0)->setFlags(Qt::NoItemFlags);
        tableCategory->item(count, 1)->setFlags(Qt::NoItemFlags);
        tableCategory->item(count, 2)->setFlags(Qt::NoItemFlags);
      }
    }
  }
  int t = tableCategory->rowCount();
  tableCategory->setRowCount(t + 1);
  tableCategory->setItem(t, 0, new QTableWidgetItem(tr("Total")));
  QTableWidgetItem* item = new QTableWidgetItem(QString::number(t));
  item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  tableCategory->setItem(t, 1, item);
  QString strAmount = QString("%1").arg(abc, 0, 'f', 2);
  tableCategory->setItem(t, 2, new QTableWidgetItem(strAmount));

  double bai = (abc / total) * 100;
  QString strBai = QString::number(bai, 'f', 2);
  if (total > 0)
    lblDetails->setText(tr("Total") + " : " + tr("Freq") + " " +
                        QString::number(t) + "    " + tr("Amount") + " " +
                        strAmount + "    " + strBai + "%");
  else
    lblDetails->setText(tr("Total") + " : " + tr("Freq") + " " +
                        QString::number(t) + "    " + tr("Amount") + " " +
                        strAmount);
  qDebug() << abc / total;

  tableCategory->item(t, 0)->setFlags(Qt::NoItemFlags);
  tableCategory->item(t, 1)->setFlags(Qt::NoItemFlags);
  tableCategory->item(t, 2)->setFlags(Qt::NoItemFlags);

  tableDetails->hide();
  tableCategory->show();
}

void setTableNoItemFlags(QTableWidget* t, int row) {
  for (int z = 0; z < t->columnCount(); z++) {
    t->item(row, z)->setFlags(Qt::NoItemFlags);
  }
}

void dlgReport::on_btnPrint_clicked() {
  preview->setFixedHeight(this->height());
  preview->setFixedWidth(this->width());

  connect(preview, SIGNAL(paintRequested(QPrinter*)), this,
          SLOT(plotPic(QPrinter*)));

  QMessageBox msgBox;
  msgBox.setText(tr("Please select the printing method"));
  msgBox.addButton(tr("Output to document"), QMessageBox::AcceptRole);
  msgBox.addButton(tr("Output to printer"), QMessageBox::RejectRole);
  if (msgBox.exec() == QMessageBox::AcceptRole)
    printer->setOutputFormat(QPrinter::PdfFormat);
  preview->show();
}

void dlgReport::plotPic(QPrinter* printer) {
  QPainter painter(printer);
  QPixmap p_w_picpath;

  p_w_picpath = p_w_picpath.grabWidget(ui->tableReport->viewport(), 0, 0,
                                       ui->tableReport->width(),
                                       ui->tableReport->height());

  QRect rect = painter.viewport();
  QSize size = p_w_picpath.size();
  size.scale(rect.size(), Qt::KeepAspectRatio);  //此处保证图片显示完整
  painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
  painter.setWindow(p_w_picpath.rect());
  painter.drawPixmap(0, 0, p_w_picpath);
}
