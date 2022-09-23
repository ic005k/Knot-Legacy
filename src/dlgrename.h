#ifndef DLGRENAME_H
#define DLGRENAME_H

#include <QDialog>
#include <QKeyEvent>

namespace Ui {
class dlgRename;
}

class dlgRename : public QDialog {
  Q_OBJECT

 public:
  explicit dlgRename(QWidget *parent = nullptr);
  ~dlgRename();
  Ui::dlgRename *ui;

 protected:
  void keyReleaseEvent(QKeyEvent *event) override;

 private slots:
  void on_btnCancel_clicked();

  void on_btnOk_clicked();

 private:
};

#endif  // DLGRENAME_H
