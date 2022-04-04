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

  void saveOptions();

 protected:
  void keyReleaseEvent(QKeyEvent *event) override;

 private slots:
  void on_btnBack_clicked();

 private:
};

#endif  // DLGPREFERENCES_H
