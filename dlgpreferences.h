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

  static void saveFontSize();

 protected:
  void keyReleaseEvent(QKeyEvent *event) override;

 private slots:
  void on_btnBack_clicked();

  void on_rb0_clicked(bool checked);

  void on_rb1_clicked(bool checked);

  void on_rb2_clicked(bool checked);

  void on_chkClose_clicked(bool checked);

 private:
};

#endif  // DLGPREFERENCES_H
