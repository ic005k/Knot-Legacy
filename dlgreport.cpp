#include "dlgreport.h"

#include "mainwindow.h"
#include "ui_dlgreport.h"
#include "ui_mainwindow.h"
extern int fontSize;
extern MainWindow* mw_one;
dlgReport::dlgReport(QWidget* parent) : QDialog(parent), ui(new Ui::dlgReport) {
  ui->setupUi(this);

  for (int y = 0; y < ui->tableReport->columnCount(); y++) {
    ui->tableReport->horizontalHeader()->setSectionResizeMode(
        y, QHeaderView::ResizeToContents);
  }
  for (int y = 0; y < ui->tableDetails->columnCount(); y++) {
    ui->tableDetails->horizontalHeader()->setSectionResizeMode(
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
}

dlgReport::~dlgReport() { delete ui; }

void dlgReport::keyReleaseEvent(QKeyEvent* event) { event->accept(); }

void dlgReport::on_btnBack_clicked() { close(); }

void dlgReport::on_btnYear_clicked() {
  int w = ui->btnYear->width();
  QListWidget* list = new QListWidget(this);
  QFont font;
  font.setPixelSize(fontSize);
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
    list->close();
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
        amount = freq + txt2.toDouble();
        QString strAmount = QString("%1").arg(amount, 0, 'f', 2);
        tableItem = new QTableWidgetItem(strAmount);
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

      tableItem = new QTableWidgetItem(QString::number(amount));
      ui->tableReport->setItem(count, 2, tableItem);

      ui->tableReport->setColumnWidth(0, 10);
      ui->tableReport->setRowHeight(count, 30);

      ui->tableReport->item(count, 0)->setFlags(Qt::NoItemFlags);
      ui->tableReport->item(count, 1)->setFlags(Qt::NoItemFlags);
      ui->tableReport->item(count, 2)->setFlags(Qt::NoItemFlags);

      on_tableReport_cellClicked(0, 0);
    }
  });

  int h = 30 * list->count() + 2;
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

void dlgReport::on_btnMonth_clicked() {
  int w = ui->btnYear->width();
  QListWidget* list = new QListWidget(this);
  QFont font;
  font.setPixelSize(fontSize);
  list->setFont(font);

  QStringList strList = mw_one->listMonth;

  for (int i = 0; i < strList.count(); i++) {
    QListWidgetItem* item = new QListWidgetItem;
    item->setSizeHint(QSize(w - 10, 30));  // item->sizeHint().width()
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    item->setText(strList.at(i));
    list->addItem(item);
  }
  connect(list, &QListWidget::itemClicked, [=]() {
    ui->btnMonth->setText(list->currentItem()->text());
    list->close();

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
        QString strAmount = QString("%1").arg(amount, 0, 'f', 2);
        tableItem = new QTableWidgetItem(strAmount);
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

      tableItem = new QTableWidgetItem(QString::number(amount));
      ui->tableReport->setItem(count, 2, tableItem);

      ui->tableReport->setColumnWidth(0, 10);
      ui->tableReport->setRowHeight(count, 30);

      ui->tableReport->item(count, 0)->setFlags(Qt::NoItemFlags);
      ui->tableReport->item(count, 1)->setFlags(Qt::NoItemFlags);
      ui->tableReport->item(count, 2)->setFlags(Qt::NoItemFlags);

      on_tableReport_cellClicked(0, 0);
    }
  });

  int h = 30 * list->count() + 2;
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

void dlgReport::on_tableReport_itemClicked(QTableWidgetItem* item) {
  Q_UNUSED(item);
}

void dlgReport::on_tableReport_cellClicked(int row, int column) {
  Q_UNUSED(column);
  markColor(row);
  ui->tableDetails->setRowCount(0);
  QString str = ui->tableReport->item(row, 0)->text();
  QTreeWidget* tw = mw_one->get_tw(mw_one->ui->tabWidget->currentIndex());
  QString name = tw->objectName();

  for (int i = 0; i < tw->topLevelItemCount(); i++) {
    QTreeWidgetItem* topItem = tw->topLevelItem(i);
    if (str == topItem->text(0)) {
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
