#ifndef SYNCINFO_H
#define SYNCINFO_H

#include <QDialog>
#include <QKeyEvent>

namespace Ui {
class SyncInfo;
}

class SyncInfo : public QDialog {
  Q_OBJECT

 public:
  explicit SyncInfo(QWidget *parent = nullptr);
  ~SyncInfo();
  Ui::SyncInfo *ui;

  void init();
  protected:
  bool eventFilter(QObject *watch, QEvent *evn) override;
 private slots:
  void on_btnClose_clicked();

 private:
};

#endif  // SYNCINFO_H
