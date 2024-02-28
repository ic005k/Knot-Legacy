#ifndef STEPSOPTIONS_H
#define STEPSOPTIONS_H

#include <QDialog>

#include "src/Method.h"

namespace Ui {
class StepsOptions;
}

class StepsOptions : public QDialog {
  Q_OBJECT

 public:
  explicit StepsOptions(QWidget *parent = nullptr);
  ~StepsOptions();
  Ui::StepsOptions *ui;

  void init();

 protected:
  bool eventFilter(QObject *obj, QEvent *evn) override;
  void closeEvent(QCloseEvent *event) override;
 private slots:
  void on_btnBack_clicked();

 private:
};

#endif  // STEPSOPTIONS_H
