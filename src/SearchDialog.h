#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QDialog>
#include <QTableWidget>

namespace Ui {
class SearchDialog;
}

class SearchDialog : public QDialog {
  Q_OBJECT

 public:
  explicit SearchDialog(QWidget *parent = nullptr);
  ~SearchDialog();
  Ui::SearchDialog *ui;

  void startSearch();
  void initSearchResults();

  void init();

 protected:
  bool eventFilter(QObject *watchDlgSearch, QEvent *evn) override;

 public slots:
  void on_btnBack_clicked();

 private slots:

  void on_btnSearch_clicked();

  void on_btnClearText_clicked();

 private:
  void setCellText(int row, int column, QString str, QTableWidget *table);
  void generateData(int count, QTableWidget *table);
};

#endif  // SEARCHDIALOG_H
