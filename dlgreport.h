#ifndef DLGREPORT_H
#define DLGREPORT_H

#include <QDialog>
#include <QKeyEvent>
#include <QListWidget>
#include <QScroller>
#include <QStandardItemModel>
#include <QTableWidgetItem>

namespace Ui {
class dlgReport;
}

class dlgReport : public QDialog {
  Q_OBJECT

 public:
  explicit dlgReport(QWidget *parent = nullptr);
  ~dlgReport();
  Ui::dlgReport *ui;

  void markColor(int row);
 public slots:
  void on_tableReport_cellClicked(int row, int column);

 protected:
  void keyReleaseEvent(QKeyEvent *event) override;
 private slots:
  void on_btnBack_clicked();

  void on_btnYear_clicked();

  void on_btnMonth_clicked();

  void on_tableReport_itemClicked(QTableWidgetItem *item);

 private:
};

#endif  // DLGREPORT_H
