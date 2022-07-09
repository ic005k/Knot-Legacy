#ifndef DLGREPORT_H
#define DLGREPORT_H

#include <QDialog>
#include <QKeyEvent>
#include <QListWidget>
#include <QPainter>
#include <QPrintPreviewDialog>
#include <QPrinter>
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
  void sel_Year();
  void sel_Month();
  static void saveYMD();
  void setTableNoItemFlags(QTableWidget *t, int row);
public slots:
  void on_tableReport_cellClicked(int row, int column);
  void on_btnYear_clicked();

  void on_btnMonth_clicked();

 protected:
  void keyReleaseEvent(QKeyEvent *event) override;
  bool eventFilter(QObject *watch, QEvent *evn) override;
 private slots:
  void on_btnBack_clicked();

  void on_btnCategory_clicked();

  void on_btnPrint_clicked();

  void plotPic(QPrinter *);

 private:
  QPrintPreviewDialog *preview;
  QPrinter *printer;
  QBrush brush1 = QBrush(QColor(253, 185, 176));
};

#endif  // DLGREPORT_H
