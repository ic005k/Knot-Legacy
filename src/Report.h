#ifndef REPORT_H
#define REPORT_H

#include <QDialog>
#include <QKeyEvent>
#include <QListWidget>
#include <QPainter>
//#include <QPrintPreviewDialog>
//#include <QPrinter>
#include <QScroller>
#include <QStandardItemModel>
#include <QTableWidgetItem>
#include <QTreeWidget>

namespace Ui {
class dlgReport;
}

class dlgReport : public QDialog {
  Q_OBJECT

 public:
  explicit dlgReport(QWidget *parent = nullptr);
  ~dlgReport();
  Ui::dlgReport *ui;

  static void markColor(int row);
  static void sel_Year();
  static void sel_Month();
  static void saveYMD();

  static void getCategoryData();
  static void getMonthData();
  void updateTable();
  void init();
  void updateCategoryTable();
  static void getCategoryText();
 public slots:
  static void on_tableReport_cellClicked(int row, int column);
  void on_btnYear_clicked();
  void on_btnCategory_clicked();
  void on_btnMonth_clicked();

  static void on_tableReport0_cellClicked(int row, int column);

 protected:
  void keyReleaseEvent(QKeyEvent *event) override;
  bool eventFilter(QObject *watch, QEvent *evn) override;
  void closeEvent(QCloseEvent *event) override;
 private slots:

  void on_btnOut2Img_clicked();

  void on_btnBack_clicked();

 private:
  QFrame *frameCategory;
};

#endif  // REPORT_H
