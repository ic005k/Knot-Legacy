#include "Preferences.h"

#include <QKeyEvent>

#include "mainwindow.h"
#include "ui_Preferences.h"
#include "ui_mainwindow.h"
extern QString iniFile, iniDir, privateDir, hsStyle, fontname;
extern MainWindow* mw_one;
extern bool isBreak;
extern int fontSize;

dlgPreferences::dlgPreferences(QWidget* parent)
    : QDialog(parent), ui(new Ui::dlgPreferences) {
  ui->setupUi(this);

  mw_one->set_btnStyle(this);

  this->installEventFilter(this);
  ui->lblFontSize->installEventFilter(this);

  ui->chkClose->hide();
  ui->chkShowCY->hide();
  ui->lblTip->hide();

  ui->sliderFontSize->setValue(fontSize);
  ui->lblFontSize->setText(tr("Font Size") + " : " + QString::number(fontSize));
  isFontChange = false;
  hsStyle = ui->sliderFontSize->styleSheet();

  chkStyle = ui->chkAutoTime->styleSheet();
  ui->chkClose->setStyleSheet(chkStyle);
  ui->chkDebug->setStyleSheet(chkStyle);
  ui->chkShowCY->setStyleSheet(chkStyle);
  ui->chkReaderFont->setStyleSheet(chkStyle);
  ui->chkUIFont->setStyleSheet(chkStyle);
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
  QMouseEvent* event = static_cast<QMouseEvent*>(evn);
  if (evn->type() == QEvent::KeyPress) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      on_btnBack_clicked();
      return true;
    }
  }

  if (watch == ui->lblFontSize) {
    if (event->type() == QEvent::MouseButtonDblClick) {
      QSettings Reg(privateDir + "options.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
      Reg.setIniCodec("utf-8");
#endif

      if (devMode)
        devMode = false;
      else
        devMode = true;
      Reg.setValue("/Options/DevMode", devMode);
      qDebug() << "devMode=" << devMode;
    }
  }

  return QWidget::eventFilter(watch, evn);
}

void dlgPreferences::on_btnBack_clicked() {
  saveOptions();
  close();
}

void dlgPreferences::saveOptions() {
  QSettings Reg(privateDir + "options.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif

  Reg.setValue("/Options/FontSize", ui->sliderFontSize->value());
  Reg.setValue("/Options/chkReaderFont", ui->chkReaderFont->isChecked());
  Reg.setValue("/Options/Close", ui->chkClose->isChecked());
  Reg.setValue("/Options/AutoTimeY", ui->chkAutoTime->isChecked());
  Reg.setValue("/Options/Debug", ui->chkDebug->isChecked());
  Reg.setValue("/Options/ShowCurrentYear", ui->chkShowCY->isChecked());
  Reg.setValue("/Options/chkUIFont", ui->chkUIFont->isChecked());
}

void dlgPreferences::on_chkDebug_clicked() {
  if (ui->chkDebug->isChecked()) {
    mw_one->ui->frameDebug->show();

  } else if (!ui->chkDebug->isChecked()) {
    mw_one->ui->frameDebug->hide();
  }
}

void dlgPreferences::on_sliderFontSize_sliderMoved(int position) {
  QFont font;
  font.setPointSize(position);
  ui->lblFontSize->setText(tr("Font Size") + " : " + QString::number(position));
  ui->lblFontSize->setFont(font);
  isFontChange = true;

  ui->btnReStart->show();
}

void dlgPreferences::on_chkReaderFont_clicked() {
  isFontChange = true;

  ui->btnReStart->show();
}

void dlgPreferences::on_btnCustomFont_clicked() {
  QString fileName;
  fileName = QFileDialog::getOpenFileName(this, tr("Font"), "",
                                          tr("Font Files (*.*)"));
  if (fileName == "") return;

  setFontDemo(fileName);
  isFontChange = true;
  QSettings Reg(privateDir + "options.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif
  Reg.setValue("/Options/CustomFont", fileName);

  ui->btnReStart->show();
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
      mw_one->ui->qwReader->rootContext()->setContextProperty("FontName",
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

void dlgPreferences::on_chkUIFont_clicked() {
  isFontChange = true;

  ui->btnReStart->show();
}

void dlgPreferences::on_sliderFontSize_valueChanged(int value) {
  on_sliderFontSize_sliderMoved(value);
}

void dlgPreferences::initValues() {
  QSettings Reg(privateDir + "options.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif

  bool chkUIFont = Reg.value("/Options/chkUIFont", false).toBool();
  ui->chkUIFont->setChecked(chkUIFont);
  ui->chkReaderFont->setChecked(
      Reg.value("/Options/chkReaderFont", false).toBool());
  ui->chkClose->setChecked(Reg.value("/Options/Close", false).toBool());
  ui->chkAutoTime->setChecked(Reg.value("/Options/AutoTimeY", true).toBool());
  ui->chkShowCY->setChecked(
      Reg.value("/Options/ShowCurrentYear", true).toBool());

  bool debugmode = Reg.value("/Options/Debug", false).toBool();
  ui->chkDebug->setChecked(debugmode);
  on_chkDebug_clicked();

  devMode = Reg.value("/Options/DevMode", false).toBool();
#ifdef Q_OS_ANDROID
#else
  ui->gboxAdditional->hide();
  if (!devMode) {
    mw_one->ui->frame_charts->hide();
    mw_one->ui->frame_tab->hide();
    mw_one->ui->frame_find->hide();
    mw_one->ui->btnSteps->hide();
    mw_one->ui->btnMax->hide();
    mw_one->ui->btnReader->hide();
    mw_one->ui->btnAdd->hide();
    mw_one->ui->btnDel->hide();
    mw_one->ui->btnSync->hide();
    mw_one->ui->btnFind->hide();
    mw_one->ui->btnRemarks->hide();
    mw_one->ui->btnReport->hide();
    mw_one->ui->btnSync->hide();

    ui->chkAutoTime->hide();
    ui->chkReaderFont->hide();

    int s = 120;
    int qs = s - 40;
    mw_one->ui->btnTodo->setFixedHeight(s);
    mw_one->ui->btnTodo->setFixedWidth(s);
    mw_one->ui->btnTodo->setIconSize(QSize(qs, qs));
    mw_one->ui->btnNotes->setFixedHeight(s);
    mw_one->ui->btnNotes->setFixedWidth(s);
    mw_one->ui->btnNotes->setIconSize(QSize(qs, qs));
  }
#endif

  QString strf = Reg.value("/Options/CustomFont").toString();
  setFontDemo(strf);
}

void dlgPreferences::on_btnReStart_clicked() {
  saveOptions();

#ifdef Q_OS_ANDROID

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  QAndroidJniObject jo = QAndroidJniObject::fromString("setReOpen");
  jo.callStaticMethod<int>("com.x/MyActivity", "setReOpen", "()I");
#else
  QJniObject jo = QJniObject::fromString("setReOpen");
  jo.callStaticMethod<int>("com.x/MyActivity", "setReOpen", "()I");
#endif

#endif

  mw_one->close();

#ifdef Q_OS_MACX
  QProcess::startDetached(qApp->applicationFilePath(), QStringList());
#endif
}

void dlgPreferences::autoBakData() {
  QSettings Reg(privateDir + "options.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif
  int bakCount = Reg.value("/AutoBak/BakCount").toInt();
  if (bakCount > 100000) {
    bakCount = 0;
    Reg.setValue("/AutoBak/BakCount", 0);
    Reg.setValue("/AutoBak/NextDel", 0);
  }
  int nextDel = Reg.value("/AutoBak/NextDel").toInt();
  bakCount++;
  Reg.setValue("/AutoBak/File" + QString::number(bakCount),
               mw_one->bakData("android", false));
  if (bakCount - nextDel > 15) {
    nextDel++;
    QString bakFile =
        Reg.value("/AutoBak/File" + QString::number(nextDel)).toString();
    QFile file(bakFile);
    file.remove(bakFile);
    Reg.remove("/AutoBak/File" + QString::number(nextDel));
    Reg.setValue("/AutoBak/NextDel", nextDel);
  }
  Reg.setValue("/AutoBak/BakCount", bakCount);
}
