#include "dlgreport.h"

#include "mainwindow.h"
#include "ui_dlgreport.h"
#include "ui_mainwindow.h"
extern int fontSize;
extern MainWindow* mw_one;
dlgReport::dlgReport(QWidget* parent) : QDialog(parent), ui(new Ui::dlgReport) {
  ui->setupUi(this);
  ui->tableReport->setStyleSheet(tableStyle);
  for (int y = 0; y < ui->tableReport->columnCount(); y++) {
    ui->tableReport->horizontalHeader()->setSectionResizeMode(
        y, QHeaderView::ResizeToContents);
  }
  ui->tableReport->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui->tableReport->setSelectionBehavior(QAbstractItemView::SelectItems);
  ui->tableReport->setVerticalScrollMode(QTableWidget::ScrollPerPixel);
  QScroller::grabGesture(ui->tableReport, QScroller::LeftMouseButtonGesture);
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
    for (int i = 0; i < tw->topLevelItemCount(); i++) {
      QString strYear = mw_one->get_Year(tw->topLevelItem(i)->text(0));
      if (strYear == ui->btnYear->text()) {
        ui->tableReport->setRowCount(ui->tableReport->rowCount() + 1);
        ui->tableReport->setColumnWidth(0, ui->tableReport->columnWidth(0));
        ui->tableReport->setRowHeight(i, 25);
        QTableWidgetItem* tableItem =
            new QTableWidgetItem(tw->topLevelItem(i)->text(0));
        ui->tableReport->setItem(i, 0, tableItem);
        tableItem = new QTableWidgetItem(tw->topLevelItem(i)->text(1));
        tableItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableReport->setItem(i, 1, tableItem);
        tableItem = new QTableWidgetItem(tw->topLevelItem(i)->text(2));
        ui->tableReport->setItem(i, 2, tableItem);
      }
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
    for (int i = 0; i < tw->topLevelItemCount(); i++) {
      QString strYear = mw_one->get_Year(tw->topLevelItem(i)->text(0));
      QString strMonth = mw_one->get_Month(tw->topLevelItem(i)->text(0));
      if (strYear == ui->btnYear->text() && strMonth == ui->btnMonth->text()) {
        ui->tableReport->setRowCount(ui->tableReport->rowCount() + 1);
        ui->tableReport->setColumnWidth(0, ui->tableReport->columnWidth(0));
        ui->tableReport->setRowHeight(i, 25);
        QTableWidgetItem* tableItem =
            new QTableWidgetItem(tw->topLevelItem(i)->text(0));
        ui->tableReport->setItem(i, 0, tableItem);
        tableItem = new QTableWidgetItem(tw->topLevelItem(i)->text(1));
        tableItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableReport->setItem(i, 1, tableItem);
        tableItem = new QTableWidgetItem(tw->topLevelItem(i)->text(2));
        ui->tableReport->setItem(i, 2, tableItem);
      }
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
