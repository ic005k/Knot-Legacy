#include "dlgreport.h"

#include "mainwindow.h"
#include "ui_dlgreport.h"
#include "ui_mainwindow.h"
extern int fontSize;
extern MainWindow* mw_one;
extern QString iniFile, iniDir, btnYText, btnMText, btnDText;
QString btnYearText, btnMonthText;

dlgReport::dlgReport(QWidget* parent) : QDialog(parent), ui(new Ui::dlgReport) {
  ui->setupUi(this);
  this->installEventFilter(this);
  ui->tableCategory->hide();

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
  ui->tableReport->setVerticalScrollMode(QTableWidget::ScrollPerPixel);
  QScroller::grabGesture(ui->tableReport, QScroller::LeftMouseButtonGesture);
  ui->tableReport->verticalScrollBar()->setStyleSheet(mw_one->vsbarStyleSmall);

  ui->tableDetails->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui->tableDetails->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->tableDetails->setSelectionMode(QAbstractItemView::SingleSelection);
  ui->tableDetails->setVerticalScrollMode(QTableWidget::ScrollPerPixel);
  QScroller::grabGesture(ui->tableDetails, QScroller::LeftMouseButtonGesture);
  ui->tableDetails->verticalScrollBar()->setStyleSheet(mw_one->vsbarStyleSmall);
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
  close();
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
    sel_Year();
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
  QTreeWidget* tw = mw_one->get_tw(mw_one->ui->tabWidget->currentIndex());
  ui->tableReport->setRowCount(0);
  ui->tableDetails->setRowCount(0);
  int freq = 0;
  double amount = 0;
  int j = 0;
  for (int i = 0; i < tw->topLevelItemCount(); i++) {
    QString strYear = mw_one->get_Year(tw->topLevelItem(i)->text(0));
    if (strYear == ui->btnYear->text()) {
      ui->tableReport->setRowCount(ui->tableReport->rowCount() + 1);

      QTableWidgetItem* tableItem =
          new QTableWidgetItem(tw->topLevelItem(i)->text(0));
      ui->tableReport->setItem(j, 0, tableItem);

      QString txt1 = tw->topLevelItem(i)->text(1);
      freq = freq + txt1.toInt();
      tableItem = new QTableWidgetItem(txt1);
      tableItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
      ui->tableReport->setItem(j, 1, tableItem);

      QString txt2 = tw->topLevelItem(i)->text(2);
      amount = amount + txt2.toDouble();
      tableItem = new QTableWidgetItem(txt2);
      ui->tableReport->setItem(j, 2, tableItem);

      ui->tableReport->setColumnWidth(0, 10);
      ui->tableReport->setRowHeight(j, 30);

      ui->tableReport->item(j, 0)->setFlags(Qt::NoItemFlags);
      ui->tableReport->item(j, 1)->setFlags(Qt::NoItemFlags);
      ui->tableReport->item(j, 2)->setFlags(Qt::NoItemFlags);

      j++;
    }
  }

  int count = ui->tableReport->rowCount();
  if (count > 0) {
    ui->tableReport->setRowCount(count + 1);
    QTableWidgetItem* tableItem = new QTableWidgetItem(tr("Total"));
    ui->tableReport->setItem(count, 0, tableItem);

    tableItem = new QTableWidgetItem(QString::number(freq));
    tableItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tableReport->setItem(count, 1, tableItem);

    QString strAmount = QString("%1").arg(amount, 0, 'f', 2);
    tableItem = new QTableWidgetItem(strAmount);
    ui->tableReport->setItem(count, 2, tableItem);

    ui->tableReport->setColumnWidth(0, 10);
    ui->tableReport->setRowHeight(count, 30);

    ui->tableReport->item(count, 0)->setFlags(Qt::NoItemFlags);
    ui->tableReport->item(count, 1)->setFlags(Qt::NoItemFlags);
    ui->tableReport->item(count, 2)->setFlags(Qt::NoItemFlags);

    on_tableReport_cellClicked(0, 0);
  }

  ui->btnCategory->setText(tr("Category"));
  ui->tableCategory->hide();
  ui->tableDetails->show();
}

void dlgReport::sel_Month() {
  if (ui->btnMonth->text() == tr("Year-Round")) {
    sel_Year();
    return;
  }

  QTreeWidget* tw = mw_one->get_tw(mw_one->ui->tabWidget->currentIndex());
  ui->tableReport->setRowCount(0);
  ui->tableDetails->setRowCount(0);
  int freq = 0;
  double amount = 0;
  int j = 0;
  for (int i = 0; i < tw->topLevelItemCount(); i++) {
    QString strYear = mw_one->get_Year(tw->topLevelItem(i)->text(0));
    QString strMonth = mw_one->get_Month(tw->topLevelItem(i)->text(0));
    if (strYear == ui->btnYear->text() && strMonth == ui->btnMonth->text()) {
      ui->tableReport->setRowCount(ui->tableReport->rowCount() + 1);

      QTableWidgetItem* tableItem =
          new QTableWidgetItem(tw->topLevelItem(i)->text(0));
      ui->tableReport->setItem(j, 0, tableItem);

      QString txt1 = tw->topLevelItem(i)->text(1);
      freq = freq + txt1.toInt();
      tableItem = new QTableWidgetItem(txt1);
      tableItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
      ui->tableReport->setItem(j, 1, tableItem);

      QString txt2 = tw->topLevelItem(i)->text(2);
      amount = amount + txt2.toDouble();
      tableItem = new QTableWidgetItem(txt2);
      ui->tableReport->setItem(j, 2, tableItem);

      ui->tableReport->setColumnWidth(0, 10);
      ui->tableReport->setRowHeight(j, 30);

      ui->tableReport->item(j, 0)->setFlags(Qt::NoItemFlags);
      ui->tableReport->item(j, 1)->setFlags(Qt::NoItemFlags);
      ui->tableReport->item(j, 2)->setFlags(Qt::NoItemFlags);

      j++;
    }
  }

  int count = ui->tableReport->rowCount();
  if (count > 0) {
    ui->tableReport->setRowCount(count + 1);
    QTableWidgetItem* tableItem = new QTableWidgetItem(tr("Total"));
    ui->tableReport->setItem(count, 0, tableItem);

    tableItem = new QTableWidgetItem(QString::number(freq));
    tableItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tableReport->setItem(count, 1, tableItem);

    QString strAmount = QString("%1").arg(amount, 0, 'f', 2);
    tableItem = new QTableWidgetItem(strAmount);
    ui->tableReport->setItem(count, 2, tableItem);

    ui->tableReport->setColumnWidth(0, 10);
    ui->tableReport->setRowHeight(count, 30);

    ui->tableReport->item(count, 0)->setFlags(Qt::NoItemFlags);
    ui->tableReport->item(count, 1)->setFlags(Qt::NoItemFlags);
    ui->tableReport->item(count, 2)->setFlags(Qt::NoItemFlags);

    on_tableReport_cellClicked(0, 0);
  }

  ui->btnCategory->setText(tr("Category"));
  ui->tableCategory->hide();
  ui->tableDetails->show();
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

    sel_Month();
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
  ui->lblDetails->setText(tr("Details"));
  markColor(row);
  ui->tableDetails->setRowCount(0);
  QString str = ui->tableReport->item(row, 0)->text();
  QTreeWidget* tw = mw_one->get_tw(mw_one->ui->tabWidget->currentIndex());

  for (int i = 0; i < tw->topLevelItemCount(); i++) {
    QTreeWidgetItem* topItem = tw->topLevelItem(i);
    if (str == topItem->text(0)) {
      ui->lblDetails->setText(tr("Details") + " : " + str);
      int childCount = topItem->childCount();
      ui->tableDetails->setRowCount(childCount);
      for (int m = 0; m < childCount; m++) {
        ui->tableDetails->setColumnWidth(0, 10);
        ui->tableDetails->setRowHeight(m, 30);

        QTableWidgetItem* tableItem;
        QString str = topItem->child(m)->text(0);
        QStringList list = str.split(".");
        if (list.count() == 2)
          tableItem = new QTableWidgetItem(list.at(1));
        else
          tableItem = new QTableWidgetItem(str);
        ui->tableDetails->setItem(m, 0, tableItem);
        tableItem = new QTableWidgetItem(topItem->child(m)->text(1));
        ui->tableDetails->setItem(m, 1, tableItem);
        tableItem = new QTableWidgetItem(topItem->child(m)->text(2));
        ui->tableDetails->setItem(m, 2, tableItem);

        ui->tableDetails->item(m, 0)->setFlags(Qt::NoItemFlags);
        ui->tableDetails->item(m, 1)->setFlags(Qt::NoItemFlags);
        ui->tableDetails->item(m, 2)->setFlags(Qt::NoItemFlags);

        //数量特别大的时候，影响性能
        // ui->tableDetails->scrollToBottom();
      }
      break;
    }
  }
}

void dlgReport::markColor(int row) {
  QIcon icon;
  int size = 5;
  icon.addFile(":/src/sel.png", QSize(size, size));
  QIcon icon1;
  icon1.addFile(":/src/nosel.png", QSize(size, size));
  QTableWidgetItem* id1;

  for (int i = 0; i < ui->tableReport->rowCount(); i++) {
    if (i == row)
      id1 = new QTableWidgetItem(icon, QString::number(i + 1) + "  ");
    else
      id1 = new QTableWidgetItem(icon1, QString::number(i + 1) + "  ");
    ui->tableReport->setVerticalHeaderItem(i, id1);
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
  int count = ui->tableReport->rowCount();
  if (count == 0) {
    ui->btnCategory->setText(tr("Category"));
    return;
  }
  QListWidget* list = new QListWidget(this);
  list->setStyleSheet(mw_one->listStyle);
  list->verticalScrollBar()->setStyleSheet(mw_one->vsbarStyleSmall);
  list->setVerticalScrollMode(QListWidget::ScrollPerPixel);
  list->horizontalScrollBar()->hide();
  QScroller::grabGesture(list, QScroller::LeftMouseButtonGesture);
  QFont font;
  font.setPointSize(fontSize);
  list->setFont(font);

  QStringList listType;
  listType.append(tr("None"));
  QListWidgetItem* pItem = new QListWidgetItem();
  pItem->setSizeHint(QSize(ui->btnCategory->width() - 4, 30));
  pItem->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
  pItem->setText(tr("None"));
  list->addItem(pItem);

  for (int i = 0; i < count - 1; i++) {
    on_tableReport_cellClicked(i, 0);
    for (int j = 0; j < ui->tableDetails->rowCount(); j++) {
      QString str = ui->tableDetails->item(j, 2)->text().trimmed();
      if (str != "") {
        if (!listType.removeOne(str)) {
          listType.append(str);
          QListWidgetItem* pItem = new QListWidgetItem();
          pItem->setSizeHint(QSize(ui->btnCategory->width() - 4, 30));
          pItem->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
          pItem->setText(str);
          list->addItem(pItem);
        } else
          listType.append(str);
      }
    }
  }
  int h = this->height() / 2;
  if (list->count() * 30 < h) h = list->count() * 30 + 4;
  list->setGeometry(ui->btnCategory->x(), ui->btnCategory->y() - h / 2,
                    ui->btnCategory->width(), h);
  if (list->count() > 1) list->show();

  connect(list, &QListWidget::itemClicked, [=]() {
    ui->btnCategory->setText(list->currentItem()->text());
    if (list->currentRow() == 0) {
      ui->tableCategory->hide();
      ui->tableDetails->show();
      list->close();
      return;
    }

    ui->tableCategory->setRowCount(0);
    double abc = 0;
    for (int i = 0; i < ui->tableReport->rowCount(); i++) {
      on_tableReport_cellClicked(i, 0);

      for (int j = 0; j < ui->tableDetails->rowCount(); j++) {
        if (ui->tableDetails->item(j, 2)->text() == ui->btnCategory->text()) {
          int count = ui->tableCategory->rowCount();
          ui->tableCategory->setRowCount(count + 1);
          QString str0 = ui->tableReport->item(i, 0)->text();
          QStringList list0 = str0.split(" ");
          if (list0.count() == 4) {
            str0 = list0.at(0) + " " + list0.at(1) + " " + list0.at(2);
          }
          ui->tableCategory->setItem(count, 0, new QTableWidgetItem(str0));
          ui->tableCategory->setItem(
              count, 1,
              new QTableWidgetItem(ui->tableDetails->item(j, 0)->text()));
          ui->tableCategory->setItem(
              count, 2,
              new QTableWidgetItem(ui->tableDetails->item(j, 1)->text()));
          abc = abc + ui->tableDetails->item(j, 1)->text().toDouble();

          ui->tableCategory->item(count, 0)->setFlags(Qt::NoItemFlags);
          ui->tableCategory->item(count, 1)->setFlags(Qt::NoItemFlags);
          ui->tableCategory->item(count, 2)->setFlags(Qt::NoItemFlags);
        }
      }
    }
    int t = ui->tableCategory->rowCount();
    ui->tableCategory->setRowCount(t + 1);
    ui->tableCategory->setItem(t, 0, new QTableWidgetItem(tr("Total")));
    QTableWidgetItem* item = new QTableWidgetItem(QString::number(t));
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tableCategory->setItem(t, 1, item);
    QString strAmount = QString("%1").arg(abc, 0, 'f', 2);
    ui->tableCategory->setItem(t, 2, new QTableWidgetItem(strAmount));
    ui->tableCategory->item(t, 0)->setFlags(Qt::NoItemFlags);
    ui->tableCategory->item(t, 1)->setFlags(Qt::NoItemFlags);
    ui->tableCategory->item(t, 2)->setFlags(Qt::NoItemFlags);
    ui->tableDetails->hide();
    ui->tableCategory->show();
    list->close();
  });
}
