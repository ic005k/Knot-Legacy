#ifndef DLGPREFERENCES_H
#define DLGPREFERENCES_H

#include <QDialog>

namespace Ui {
class dlgPreferences;
}

class dlgPreferences : public QDialog {
  Q_OBJECT

 public:
  explicit dlgPreferences(QWidget *parent = nullptr);
  ~dlgPreferences();
  Ui::dlgPreferences *ui;

  bool isFontChange = false;
  void saveOptions();

 protected:
  void keyReleaseEvent(QKeyEvent *event) override;

  bool eventFilter(QObject *watch, QEvent *evn) override;
 private slots:
  void on_btnBack_clicked();

  void on_rb0_clicked();

  void on_rb1_clicked();

  void on_rb2_clicked();

  void on_rbSM1_clicked();

  void on_rbSM2_clicked();

  void on_chkLogs_clicked(bool checked);

 private:
};

#endif  // DLGPREFERENCES_H
