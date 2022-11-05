#ifndef PREFERENCES_H
#define PREFERENCES_H

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

  QString chkStyle;
  bool isFontChange = false;
  void saveOptions();

  void setFontDemo(QString customFontPath);

 protected:
  void keyReleaseEvent(QKeyEvent *event) override;

  bool eventFilter(QObject *watch, QEvent *evn) override;
 public slots:
  void on_chkDebug_clicked();

 private slots:
  void on_btnBack_clicked();

  void on_sliderFontSize_sliderMoved(int position);

  void on_chkReaderFont_clicked();

  void on_btnCustomFont_clicked();

  void on_chkCustomFont_clicked();

  void on_sliderFontSize_valueChanged(int value);

 private:
};

#endif  // PREFERENCES_H
