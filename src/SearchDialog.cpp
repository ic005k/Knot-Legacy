#include "SearchDialog.h"

#include <QKeyEvent>

#include "mainwindow.h"
#include "ui_SearchDialog.h"
extern MainWindow* mw_one;
extern QTabWidget* tabData;

QStringList resultsList;
QString searchStr;
QTableWidget* table;
int maxw;

SearchDialog::SearchDialog(QWidget* parent)
    : QDialog(parent), ui(new Ui::SearchDialog) {
  ui->setupUi(this);
  table = new QTableWidget();
  table->setColumnCount(4);
  table->horizontalHeader()->setSectionResizeMode(
      2, QHeaderView::ResizeToContents);

  setModal(true);
  this->installEventFilter(this);
  ui->editSearchText->installEventFilter(this);

  setWindowTitle(tr("Search"));

  ui->btnBack->setStyleSheet(mw_one->btnStyle);
  ui->btnSearch->setStyleSheet(mw_one->btnStyle);
  ui->btnClearText->setStyleSheet("border:none");
  mw_one->setLineEditQss(ui->editSearchText, 10, 1, "#4169E1", "#4169E1");

  ui->tableSearch->setColumnCount(4);

  ui->tableSearch->setHorizontalHeaderItem(0, new QTableWidgetItem(tr("Time")));
  ui->tableSearch->setHorizontalHeaderItem(1,
                                           new QTableWidgetItem(tr("Amount")));
  ui->tableSearch->setHorizontalHeaderItem(
      2, new QTableWidgetItem(tr("Category")));
  ui->tableSearch->setHorizontalHeaderItem(3,
                                           new QTableWidgetItem(tr("Details")));

  ui->tableSearch->horizontalHeader()->setSectionResizeMode(
      0, QHeaderView::ResizeToContents);
  ui->tableSearch->horizontalHeader()->setSectionResizeMode(
      1, QHeaderView::ResizeToContents);

  ui->tableSearch->verticalHeader()->setSectionResizeMode(
      QHeaderView::ResizeToContents);

  ui->tableSearch->horizontalHeader()->setStretchLastSection(true);
  ui->tableSearch->setAlternatingRowColors(true);
  ui->tableSearch->setSelectionBehavior(QTableWidget::SelectRows);
  ui->tableSearch->setSelectionMode(QAbstractItemView::SingleSelection);
  ui->tableSearch->setEditTriggers(QTableWidget::NoEditTriggers);
  ui->tableSearch->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui->tableSearch->verticalScrollBar()->setStyleSheet(mw_one->vsbarStyleSmall);
  ui->tableSearch->setVerticalScrollMode(QListWidget::ScrollPerPixel);
  QScroller::grabGesture(ui->tableSearch, QScroller::LeftMouseButtonGesture);
  mw_one->setSCrollPro(ui->tableSearch);

#ifdef Q_OS_ANDROID
  QFont font;
  font.setPointSize(15);
  ui->tableSearch->setFont(font);
#endif
}

void SearchDialog::init() {
  int w = mw_one->geometry().width();
  int h = mw_one->geometry().height();
  setFixedWidth(w);
  setFixedHeight(h);
  setGeometry(mw_one->geometry().x(), mw_one->geometry().y(), w, h);

  show();
  ui->editSearchText->setFocus();
  maxw = this->geometry().width() / 6;
  ui->tableSearch->setColumnWidth(2, maxw);
}

SearchDialog::~SearchDialog() { delete ui; }

bool SearchDialog::eventFilter(QObject* watchDlgSearch, QEvent* evn) {
  if (evn->type() == QEvent::KeyRelease) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      on_btnBack_clicked();
      return true;
    }

    if (watchDlgSearch == ui->editSearchText &&
        keyEvent->key() == Qt::Key_Return) {
      on_btnSearch_clicked();
      return true;
    }
  }

  return QWidget::eventFilter(watchDlgSearch, evn);
}

void SearchDialog::on_btnBack_clicked() { this->close(); }

void SearchDialog::on_btnSearch_clicked() {
  searchStr = ui->editSearchText->text().trimmed();
  if (searchStr.length() == 0) return;

  mw_one->showProgress();
  ui->tableSearch->setRowCount(0);
  mw_one->mySearchThread->start();
}

void SearchDialog::startSearch() {
  resultsList.clear();
  int tabCount = tabData->count();
  for (int j = 0; j < tabCount; j++) {
    QTreeWidget* tw = mw_one->get_tw(j);
    QString tabStr = tabData->tabText(j);

    for (int i = 0; i < tw->topLevelItemCount(); i++) {
      QString strYear, strMonthDay;
      strYear = tw->topLevelItem(i)->text(3);
      strMonthDay = tw->topLevelItem(i)->text(0);
      QString weeks = strMonthDay.split(" ").at(0);
      QString day =
          strMonthDay.split(" ").at(1) + " " + strMonthDay.split(" ").at(2);

      QTreeWidgetItem* topItem;
      topItem = tw->topLevelItem(i);
      int childCount = topItem->childCount();
      for (int j = 0; j < childCount; j++) {
        QString txt1, txt2, txt3;
        QTreeWidgetItem* childItem = topItem->child(j);
        txt1 = childItem->text(1);
        txt2 = childItem->text(2);
        txt3 = childItem->text(3);
        if (txt1.contains(searchStr) || txt2.contains(searchStr) ||
            txt3.contains(searchStr)) {
          QString str0, str1, str2, str3;
          str0 = "\n" + tabStr + "\n\n" + strYear + " \n" + day + "\n" + weeks +
                 "\n" + childItem->text(0).split(".").at(1).trimmed() + "\n";
          str1 = childItem->text(1);
          str2 = childItem->text(2);
          str3 = childItem->text(3);

          resultsList.append(str0 + "=|=" + str1 + "=|=" + str2 + "=|=" + str3);
        }
      }
    }
  }
}

void SearchDialog::initSearchResults() {
  // qDebug() << resultsList;

  table->horizontalHeader()->setSectionResizeMode(
      2, QHeaderView::ResizeToContents);

  int count = resultsList.count();
  if (count == 0) return;

  generateData(count, ui->tableSearch);

  ui->tableSearch->setCurrentCell(0, 0);
}

void SearchDialog::generateData(int count, QTableWidget* table) {
  table->setRowCount(count);
  for (int i = 0; i < count; i++) {
    QStringList list = resultsList.at(i).split("=|=");
    QString str0, str1, str2, str3;
    str0 = list.at(0);
    str1 = list.at(1);
    str2 = list.at(2);
    str3 = list.at(3);

    table->setItem(i, 0, new QTableWidgetItem(str0));
    setCellText(i, 1, str1, table);
    setCellText(i, 2, str2, table);
    setCellText(i, 3, str3, table);
  }
}

void SearchDialog::setCellText(int row, int column, QString str,
                               QTableWidget* table) {
  QString a0("<span style=\"color: white;background: red;\">");
  QString a1("</span>");

  if (str.contains(searchStr)) {
    str = str.replace(searchStr, a0 + searchStr + a1);

    QLabel* lbl = new QLabel();
    lbl->adjustSize();
    lbl->setWordWrap(true);
    lbl->setText(str);
    table->setCellWidget(row, column, lbl);
  } else
    table->setItem(row, column, new QTableWidgetItem(str));
}

void SearchDialog::on_btnClearText_clicked() {
  if (ui->editSearchText->text().length() > 0) ui->editSearchText->setText("");
  ui->editSearchText->setFocus();
}
