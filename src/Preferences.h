#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QDialog>
#include <QFontDatabase>
#include <QToolButton>

namespace Ui {
class Preferences;
}

class Preferences : public QDialog {
  Q_OBJECT

 public:
  explicit Preferences(QWidget *parent = nullptr);
  ~Preferences();
  Ui::Preferences *ui;

  bool devMode = false;
  QString chkStyle;
  bool isFontChange = false;
  void saveOptions();

  void initOptions();
  void autoBakData();

  void setBakStatus(bool status);
  bool getBakStatus();
  void setLatestAction(QString action);
  QString getLatestAction();

  void appendBakFile(QString action, QString bakfile);

  QStringList getBakFilesList();

  void initCheckStatus();

  bool isOverUIFont();
  bool isOverReaderFont();
  void setDefaultFont(QString fontFamily);
  QString getDefaultFont();

  QString setFontDemo(QString customFontPath, QToolButton *btn, int fontSize,
                      bool isReaderFont);

 protected:
  void keyReleaseEvent(QKeyEvent *event) override;

  bool eventFilter(QObject *watch, QEvent *evn) override;
 public slots:
  void on_chkDebug_clicked();

 private slots:
  void on_btnBack_clicked();

  void on_sliderFontSize_sliderMoved(int position);

  void on_btnCustomFont_clicked();

  void on_chkUIFont_clicked();

  void on_sliderFontSize_valueChanged(int value);

  void on_btnReStart_clicked();

  void on_chkDark_clicked(bool checked);

  void on_chkAniEffects_clicked(bool checked);

 private:
  QString iniBakFiles = "BakFiles.ini";
  void getCheckStatusChange();
  QString orgCustomFontText;
  QList<int> listCheckStatus;
  int readerFontID = 0;
  int uiFontID = 0;
  QFontDatabase fontDatabase;
};

#endif  // PREFERENCES_H
