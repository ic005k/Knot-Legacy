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

  bool devMode = false;
  QString chkStyle;
  bool isFontChange = false;
  void saveOptions();

  void setFontDemo(QString customFontPath);

  void initOptions();
  void autoBakData();

  void setBakStatus(bool status);
  bool getBakStatus();
  void setLatestAction(QString action);
  QString getLatestAction();

  void appendBakFile(QString action, QString bakfile);

  QStringList getBakFilesList();

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

  void on_chkUIFont_clicked();

  void on_sliderFontSize_valueChanged(int value);

  void on_btnReStart_clicked();

 private:
  QString iniBakFiles = "BakFiles.ini";
};

#endif  // PREFERENCES_H
