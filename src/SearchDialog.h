#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QDialog>
#include <QKeyEvent>

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

 protected:
  bool eventFilter(QObject *watch, QEvent *evn) override;

 public slots:
  void on_btnBack_clicked();

 private slots:

  void on_btnSearch_clicked();

  void on_btnClearText_clicked();

 private:
};

#endif  // SEARCHDIALOG_H
