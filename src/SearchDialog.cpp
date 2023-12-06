#include "SearchDialog.h"

#include <QKeyEvent>

#include "mainwindow.h"
#include "ui_SearchDialog.h"
extern MainWindow* mw_one;
extern QTabWidget* tabData;

QStringList resultsList;
QString searchStr;

SearchDialog::SearchDialog(QWidget* parent)
    : QDialog(parent), ui(new Ui::SearchDialog) {
  ui->setupUi(this);
  mw_one->set_btnStyle(this);
  setModal(true);
  this->installEventFilter(this);
  ui->editSearchText->installEventFilter(this);

  setWindowTitle(tr("Search"));

  ui->btnClearText->setStyleSheet("border:none");
  mw_one->setLineEditQss(ui->editSearchText, 10, 1, "#4169E1", "#4169E1");

  ui->tableSearch->setColumnCount(1);

  ui->tableSearch->setHorizontalHeaderItem(0,
                                           new QTableWidgetItem(tr("Results")));

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
  // ui->tableSearch->setFont(font);
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
          str0 = tabStr + "\n\n" + strYear + " " + day + " " + weeks + " " +
                 childItem->text(0).split(".").at(1).trimmed();
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

  int count = resultsList.count();
  ui->tableSearch->setHorizontalHeaderItem(
      0, new QTableWidgetItem(tr("Results") + " : " + QString::number(count)));
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

    QString a0("<span style=\"color: white;background: red;\">");
    QString a1("</span>");

    if (str1.contains(searchStr)) {
      str1 = str1.replace(searchStr, a0 + searchStr + a1);
    }

    if (str2.contains(searchStr)) {
      str2 = str2.replace(searchStr, a0 + searchStr + a1);
    }

    if (str3.contains(searchStr)) {
      str3 = str3.replace(searchStr, a0 + searchStr + a1);
    }

    lbl0->setText(str0);
    lbl1->setText(tr("Amount") + " : " + str1);
    lbl2->setText(tr("Category") + " : " + str2);
    lbl3->setText(tr("Details") + " : " + str3);

    QWidget* widget = new QWidget();
    QVBoxLayout* vbox = new QVBoxLayout;
    widget->setLayout(vbox);
    vbox->addWidget(lbl0);
    if (str1.trimmed().length() > 0) vbox->addWidget(lbl1);
    if (str2.trimmed().length() > 0) vbox->addWidget(lbl2);
    if (str3.trimmed().length() > 0) vbox->addWidget(lbl3);

    table->setCellWidget(i, 0, widget);
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
