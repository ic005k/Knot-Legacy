#include "Preferences.h"

#include <QKeyEvent>

#include "mainwindow.h"
#include "ui_Preferences.h"
#include "ui_mainwindow.h"
extern QString iniFile, iniDir, hsStyle, fontname;
extern MainWindow* mw_one;
extern bool isBreak;
extern int fontSize;

dlgPreferences::dlgPreferences(QWidget* parent)
    : QDialog(parent), ui(new Ui::dlgPreferences) {
  ui->setupUi(this);

  mw_one->set_btnStyle(this);

  this->installEventFilter(this);

  ui->chkClose->hide();

  ui->sliderFontSize->setValue(fontSize);
  ui->lblFontSize->setText(tr("Font Size") + " : " + QString::number(fontSize));
  isFontChange = false;
  ui->chkMute->setStyleSheet(ui->chkClose->styleSheet());
  ui->chkMute->hide();
  hsStyle = ui->sliderFontSize->styleSheet();

  chkStyle = ui->chkAutoTime->styleSheet();
  ui->chkClose->setStyleSheet(chkStyle);
  ui->chkDebug->setStyleSheet(chkStyle);
  ui->chkMute->setStyleSheet(chkStyle);
  ui->chkReaderFont->setStyleSheet(chkStyle);
  ui->chkCustomFont->setStyleSheet(chkStyle);
  ui->lblTip->setStyleSheet("color:red;");
  ui->lblFontSize->setFixedHeight(40);
  ui->lblFontPath->setWordWrap(true);
  ui->lblFontPath->adjustSize();
  ui->lblFontPath->setFixedHeight(100);
  ui->lblFontPath->hide();
  ui->lblTip->setWordWrap(true);
  ui->lblTip->adjustSize();
  ui->btnCustomFont->setFixedHeight(130);
  ui->btnCustomFont->adjustSize();
  ui->btnCustomFont->setStyleSheet(
      "background-color: rgb(255, 255, 255);color:black;");
}

dlgPreferences::~dlgPreferences() { delete ui; }

void dlgPreferences::keyReleaseEvent(QKeyEvent* event) { Q_UNUSED(event); }

bool dlgPreferences::eventFilter(QObject* watch, QEvent* evn) {
  if (evn->type() == QEvent::KeyPress) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      on_btnBack_clicked();
      return true;
    }
  }

  return QWidget::eventFilter(watch, evn);
}

void dlgPreferences::on_btnBack_clicked() {
  saveOptions();
  close();
}

void dlgPreferences::saveOptions() {
  if (isBreak) return;
  QSettings Reg(iniDir + "options.ini", QSettings::IniFormat);
  Reg.setValue("/Options/FontSize", ui->sliderFontSize->value());
  Reg.setValue("/Options/ReaderFont", ui->chkReaderFont->isChecked());
  Reg.setValue("/Options/Close", ui->chkClose->isChecked());
  Reg.setValue("/Options/AutoTimeY", ui->chkAutoTime->isChecked());
  Reg.setValue("/Options/Debug", ui->chkDebug->isChecked());
  Reg.setValue("/Options/Mute", ui->chkMute->isChecked());
  Reg.setValue("/Options/chkCustomFont", ui->chkCustomFont->isChecked());
}

void dlgPreferences::on_chkDebug_clicked() {
  if (ui->chkDebug->isChecked()) {
    mw_one->mydlgSteps->ui->lblX->show();
    mw_one->mydlgSteps->ui->lblY->show();
    mw_one->mydlgSteps->ui->lblZ->show();

    if (mw_one->mydlgSteps->ui->rbAlg1->isChecked())
      mw_one->mydlgSteps->ui->frameWay1->show();
    if (mw_one->mydlgSteps->ui->rbAlg2->isChecked())
      mw_one->mydlgSteps->ui->frameWay1->hide();
  } else if (!ui->chkDebug->isChecked()) {
    mw_one->mydlgSteps->ui->lblX->hide();
    mw_one->mydlgSteps->ui->lblY->hide();
    mw_one->mydlgSteps->ui->lblZ->hide();
    mw_one->mydlgSteps->ui->frameWay1->hide();
  }
}

void dlgPreferences::on_sliderFontSize_sliderMoved(int position) {
  QFont font;
  font.setPointSize(position);
  ui->lblFontSize->setText(tr("Font Size") + " : " + QString::number(position));
  ui->lblFontSize->setFont(font);
  isFontChange = true;
}

void dlgPreferences::on_chkReaderFont_clicked() { isFontChange = true; }

void dlgPreferences::on_chkMute_clicked() { mw_one->mydlgTodo->refreshAlarm(); }

void dlgPreferences::on_btnCustomFont_clicked() {
  QString fileName;
  fileName = QFileDialog::getOpenFileName(this, tr("Font"), "",
                                          tr("Font Files (*.*)"));
  if (fileName == "") return;

  setFontDemo(fileName);
  isFontChange = true;
  QSettings Reg(iniDir + "options.ini", QSettings::IniFormat);
  Reg.setValue("/Options/CustomFont", fileName);
}

void dlgPreferences::setFontDemo(QString customFontPath) {
  QString fontName;
  int loadedFontID = QFontDatabase::addApplicationFont(customFontPath);
  QStringList loadedFontFamilies =
      QFontDatabase::applicationFontFamilies(loadedFontID);
  if (!loadedFontFamilies.empty()) {
    fontName = loadedFontFamilies.at(0);
    QFont f;
    f.setFamily(fontName);
    f.setPointSize(20);

    ui->lblFontPath->setFont(f);
    ui->btnCustomFont->setFont(f);

    QString str = customFontPath;
#ifdef Q_OS_ANDROID
    str = mw_one->mydlgReader->getUriRealPath(customFontPath);
#endif
    ui->lblFontPath->setText(str);

    QStringList list = str.split("/");
    QString str1 = list.at(list.count() - 1);
    ui->btnCustomFont->setText(tr("Custom Font (Select External Fonts)") +
                               "\n\n" + str1);

    if (ui->chkReaderFont->isChecked()) {
      fontname = fontName;
      mw_one->mydlgReader->savePageVPos();
      mw_one->ui->quickWidget->rootContext()->setContextProperty("FontName",
                                                                 fontname);
      mw_one->mydlgReader->setPageVPos();
    }
  }

  if (QFile(customFontPath).exists())
    ui->btnCustomFont->setStyleSheet(
        "background-color: rgb(75, 75, 75);color:white;");
  else
    ui->btnCustomFont->setStyleSheet(
        "background-color: rgb(255, 255, 255);color:red;");
}

void dlgPreferences::on_chkCustomFont_clicked() { isFontChange = true; }

void dlgPreferences::on_sliderFontSize_valueChanged(int value) {
  on_sliderFontSize_sliderMoved(value);
}
