#ifndef DLGNOTESLIST_H
#define DLGNOTESLIST_H

#include <QDialog>
#include <QKeyEvent>
#include <QTreeWidgetItem>

namespace Ui {
class dlgNotesList;
}

class dlgNotesList : public QDialog {
  Q_OBJECT

 public:
  explicit dlgNotesList(QWidget *parent = nullptr);
  ~dlgNotesList();
  Ui::dlgNotesList *ui;

 protected:
  bool eventFilter(QObject *watch, QEvent *evn) override;

 private slots:
  void on_btnClose_clicked();

 private:
};

#endif  // DLGNOTESLIST_H
