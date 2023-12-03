#ifndef REPORT_H
#define REPORT_H

#include <QDialog>
#include <QKeyEvent>
#include <QListWidget>
#include <QPainter>
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

  static void saveYMD();
  QString str_xx;
  void getCategoryData(QString strCategory, bool appendTable);
  QStringList listCategorySort;
  QList<double> listD;
  static void getMonthData();
  void updateTable();
  void init();

  static void setTWImgData(QTreeWidgetItem *item);
  void clearAll();
  void delItem(int index);
  int getCount();
  void appendSteps(QString date, QString steps, QString km);
  void appendSteps_xx(QString date, QString steps, QString km);
  int getCount_xx();
  void delItem_xx(int index);
  void clearAll_xx();
  QString getDate(int row);
  int getCurrentIndex();
  void setCurrentHeader(int sn);
  void setScrollBarPos(double pos);

  void on_btnOut2Img_clicked();
  void on_btnBack_clicked();
  void on_btnYear_clicked();
  void on_btnCategory_clicked();
  void on_btnMonth_clicked();

  void setScrollBarPos_xx(double pos);

  static int cmp(const void *a, const void *b);

 protected:
  void keyReleaseEvent(QKeyEvent *event) override;
  bool eventFilter(QObject *watch, QEvent *evn) override;
  void closeEvent(QCloseEvent *event) override;
 public slots:

  void loadDetails();

  void loadDetailsQml();

 private:
  double t_amount = 0;
  int freq = 0;
  int indexCategory = 0;
};

#endif  // REPORT_H
