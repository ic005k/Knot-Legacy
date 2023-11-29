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

 protected:
  bool eventFilter(QObject *watch, QEvent *evn) override;

 private slots:
  void on_btnBack_clicked();

 private:
};

#endif  // SEARCHDIALOG_H
