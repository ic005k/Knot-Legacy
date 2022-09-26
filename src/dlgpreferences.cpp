#include "dlgpreferences.h"

#include <QKeyEvent>

#include "mainwindow.h"
#include "ui_dlgpreferences.h"
#include "ui_mainwindow.h"
extern QString iniFile, iniDir, hsStyle;
extern MainWindow* mw_one;
extern bool isBreak;
extern int fontSize;

dlgPreferences::dlgPreferences(QWidget* parent)
    : QDialog(parent), ui(new Ui::dlgPreferences) {
  ui->setupUi(this);
  this->installEventFilter(this);

  ui->sliderFontSize->setValue(fontSize);
  ui->lblFontDemo->setText(tr("Font Size") + " : " + QString::number(fontSize));
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
  ui->lblFontDemo->setFixedHeight(40);
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
  Reg.setValue("/Options/CustomFont", ui->lblCustomFont->text());
  Reg.setValue("/Options/chkCustomFont", ui->chkCustomFont->isChecked());
}

void dlgPreferences::on_chkDebug_clicked() {
  if (ui->chkDebug->isChecked()) {
    mw_one->mydlgSteps->ui->btnLogs->show();
    mw_one->mydlgSteps->ui->lblX->show();
    mw_one->mydlgSteps->ui->lblY->show();
    mw_one->mydlgSteps->ui->lblZ->show();

    if (mw_one->mydlgSteps->ui->rbAlg1->isChecked())
      mw_one->mydlgSteps->ui->frameWay1->show();
    if (mw_one->mydlgSteps->ui->rbAlg2->isChecked())
      mw_one->mydlgSteps->ui->frameWay1->hide();
  } else if (!ui->chkDebug->isChecked()) {
    mw_one->mydlgSteps->ui->btnLogs->hide();
    mw_one->mydlgSteps->ui->lblX->hide();
    mw_one->mydlgSteps->ui->lblY->hide();
    mw_one->mydlgSteps->ui->lblZ->hide();
    mw_one->mydlgSteps->ui->frameWay1->hide();
  }
}

void dlgPreferences::on_sliderFontSize_sliderMoved(int position) {
  QFont font;
  font.setPointSize(position);
  ui->lblFontDemo->setText(tr("Font Size") + " : " + QString::number(position));
  ui->lblFontDemo->setFont(font);
  isFontChange = true;
}

void dlgPreferences::on_chkReaderFont_clicked() { isFontChange = true; }

void dlgPreferences::on_chkMute_clicked() { mw_one->mydlgTodo->refreshAlarm(); }

void dlgPreferences::on_btnCustomFont_clicked() {
  QString fileName;
  fileName = QFileDialog::getOpenFileName(this, tr("Font"), "",
                                          tr("Font Files (*.*)"));

  if (QFile(fileName).exists()) {
#ifdef Q_OS_ANDROID
    fileName = mw_one->mydlgReader->getUriRealPath(fileName);
#endif
    ui->lblCustomFont->setText(fileName);
    isFontChange = true;
  }
}
