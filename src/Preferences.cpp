#include "Preferences.h"

#include <QKeyEvent>

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "ui_Preferences.h"
extern QString iniFile, iniDir, privateDir, defaultFontFamily, customFontFamily,
    infoStr;
extern MainWindow* mw_one;
extern Method* m_Method;
extern bool isBreak, isDark;
extern int fontSize;
extern QSettings* iniPreferences;

Preferences::Preferences(QWidget* parent)
    : QDialog(parent), ui(new Ui::Preferences) {
  ui->setupUi(this);

  QFont font0 = m_Method->getNewFont(20);
  ui->btnCustomFont->setFont(font0);
  ui->chkUIFont->setFont(font0);
  ui->chkAutoTime->setFont(font0);
  ui->chkDebug->setFont(font0);
  ui->btnReStart->setFont(font0);

#ifdef Q_OS_ANDROID
  ui->sliderFontSize->setMinimum(15);
#else
  ui->sliderFontSize->setMinimum(9);
#endif

  mw_one->set_ToolButtonStyle(this);

  this->installEventFilter(this);
  ui->lblFontSize->installEventFilter(this);

  ui->chkAutoTime->hide();
  ui->chkDebug->hide();

  ui->sliderFontSize->setValue(fontSize);
  ui->lblFontSize->setText(tr("Font Size") + " : " + QString::number(fontSize));
  isFontChange = false;

  chkStyle = ui->chkAutoTime->styleSheet();
  ui->chkDark->setStyleSheet(chkStyle);
  ui->chkDebug->setStyleSheet(chkStyle);
  ui->chkUIFont->setStyleSheet(chkStyle);
  ui->lblFontSize->setFixedHeight(40);

  ui->btnCustomFont->adjustSize();
  ui->btnCustomFont->setStyleSheet(
      "background-color: rgb(255, 255, 255);color:black;");
}

Preferences::~Preferences() { delete ui; }

void Preferences::keyReleaseEvent(QKeyEvent* event) { Q_UNUSED(event); }

bool Preferences::eventFilter(QObject* watch, QEvent* evn) {
  QMouseEvent* event = static_cast<QMouseEvent*>(evn);
  if (evn->type() == QEvent::KeyRelease) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      on_btnBack_clicked();
      return true;
    }
  }

  if (watch == ui->lblFontSize) {
    if (event->type() == QEvent::MouseButtonDblClick) {
      if (devMode)
        devMode = false;
      else
        devMode = true;
      iniPreferences->setValue("/Options/DevMode", devMode);
      qDebug() << "devMode=" << devMode;
    }
  }

  return QWidget::eventFilter(watch, evn);
}

void Preferences::on_btnBack_clicked() {
  saveOptions();
  close();
}

void Preferences::saveOptions() {
  iniPreferences->setValue("/Options/FontSize", ui->sliderFontSize->value());
  iniPreferences->setValue("/Options/Dark", ui->chkDark->isChecked());
  iniPreferences->setValue("/Options/AutoTimeY", ui->chkAutoTime->isChecked());
  iniPreferences->setValue("/Options/Debug", ui->chkDebug->isChecked());
  iniPreferences->setValue("/Options/chkUIFont", ui->chkUIFont->isChecked());
  iniPreferences->setValue("/Options/chkAniEffects",
                           ui->chkAniEffects->isChecked());
}

void Preferences::on_chkDebug_clicked() {
  if (ui->chkDebug->isChecked()) {
    mw_one->ui->frameDebug->show();

  } else if (!ui->chkDebug->isChecked()) {
    mw_one->ui->frameDebug->hide();
  }
}

void Preferences::on_sliderFontSize_sliderMoved(int position) {
  QFont font;
  font.setPointSize(position);
  ui->lblFontSize->setText(tr("Font Size") + " : " + QString::number(position));
  ui->lblFontSize->setFont(font);
  isFontChange = true;

  getCheckStatusChange();
}

void Preferences::on_btnCustomFont_clicked() {
  QString fileName;
  fileName = QFileDialog::getOpenFileName(this, tr("Font"), "",
                                          tr("Font Files (*.*)"));
  if (fileName == "") return;

#ifdef Q_OS_ANDROID
  fileName = m_Method->getRealPathFile(fileName);
#endif

  setFontDemo(fileName, ui->btnCustomFont, ui->sliderFontSize->value());
  isFontChange = true;

  iniPreferences->setValue("/Options/CustomFont", fileName);

  getCheckStatusChange();
}

QString Preferences::setFontDemo(QString customFontPath, QToolButton* btn,
                                 int fontSize) {
  QString fontName;

  int loadedFontID = QFontDatabase::addApplicationFont(customFontPath);
  QStringList loadedFontFamilies =
      QFontDatabase::applicationFontFamilies(loadedFontID);
  if (!loadedFontFamilies.empty()) {
    fontName = loadedFontFamilies.at(0);
    QFont f;
    f.setFamily(fontName);
    f.setPointSize(fontSize);

    btn->setFont(f);

    QString str = customFontPath;
#ifdef Q_OS_ANDROID
    str = mw_one->m_Reader->getUriRealPath(customFontPath);
#endif

    QStringList list = str.split("/");
    QString str1 = list.at(list.count() - 1);
    btn->setText(tr("Custom Font") + "\n" + fontName + "\n" + str1);
  }

  return fontName;
}

void Preferences::on_chkUIFont_clicked() {
  isFontChange = true;

  getCheckStatusChange();
}

void Preferences::on_sliderFontSize_valueChanged(int value) {
  on_sliderFontSize_sliderMoved(value);
}

void Preferences::setDefaultFont(QString fontFamily) {
  iniPreferences->setValue("/Options/DefaultFont", fontFamily);
}

QString Preferences::getDefaultFont() {
  return iniPreferences->value("/Options/DefaultFont", "None").toString();
}

bool Preferences::isOverUIFont() {
  bool chkUIFont = iniPreferences->value("/Options/chkUIFont", false).toBool();
  return chkUIFont;
}

bool Preferences::isOverReaderFont() {
  bool chkReaderFont =
      iniPreferences->value("/Options/chkReaderFont", false).toBool();
  return chkReaderFont;
}

void Preferences::initOptions() {
  bool chkUIFont = iniPreferences->value("/Options/chkUIFont", false).toBool();
  ui->chkUIFont->setChecked(chkUIFont);
  ui->chkDark->setChecked(
      iniPreferences->value("/Options/Dark", false).toBool());
  isDark = ui->chkDark->isChecked();
  ui->chkAutoTime->setChecked(
      iniPreferences->value("/Options/AutoTimeY", true).toBool());
  ui->chkAniEffects->setChecked(
      iniPreferences->value("/Options/chkAniEffects", true).toBool());
  bool debugmode = iniPreferences->value("/Options/Debug", false).toBool();
  ui->chkDebug->setChecked(debugmode);
  on_chkDebug_clicked();

  devMode = iniPreferences->value("/Options/DevMode", false).toBool();
#ifdef Q_OS_ANDROID
#else

  if (!devMode) {
    mw_one->ui->frame_charts->hide();
    mw_one->ui->frame_tab->hide();
    mw_one->ui->btnSteps->hide();
    mw_one->ui->btnChart->hide();
    mw_one->ui->btnReader->hide();
    mw_one->ui->btnAdd->hide();
    mw_one->ui->btnDel->hide();
    mw_one->ui->btnFind->hide();
    mw_one->ui->btnRemarks->hide();
    mw_one->ui->btnReport->hide();
    mw_one->ui->frame_cw->hide();
    mw_one->ui->qwMainTab->hide();
    mw_one->ui->btnSelTab->hide();
    mw_one->ui->lblStats->hide();

    ui->chkAniEffects->hide();

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

  QString customFontFile =
      iniPreferences->value("/Options/CustomFont").toString();
  setFontDemo(customFontFile, ui->btnCustomFont, ui->sliderFontSize->value());

  QString readerFontFile =
      iniPreferences->value("/Options/ReaderFont").toString();
  QString readerFont =
      setFontDemo(readerFontFile, mw_one->m_ReaderSet->ui->btnFont, fontSize);
  mw_one->ui->qwReader->rootContext()->setContextProperty("FontName",
                                                          readerFont);
}

void Preferences::on_btnReStart_clicked() {
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

void Preferences::autoBakData() {
  if (!mw_one->isNeedAutoBackup) return;

  int bakCount = iniPreferences->value("/AutoBak/BakCount").toInt();
  if (bakCount > 100000) {
    bakCount = 0;
    iniPreferences->setValue("/AutoBak/BakCount", 0);
    iniPreferences->setValue("/AutoBak/NextDel", 0);
  }
  int nextDel = iniPreferences->value("/AutoBak/NextDel").toInt();
  bakCount++;
  QString fileName = mw_one->bakData("android", false);
  iniPreferences->setValue("/AutoBak/File" + QString::number(bakCount),
                           fileName);
  if (bakCount - nextDel > 15) {
    nextDel++;
    QString oldBakFile =
        iniPreferences->value("/AutoBak/File" + QString::number(nextDel))
            .toString();
    QFile file(oldBakFile);
    file.remove(oldBakFile);
    iniPreferences->remove("/AutoBak/File" + QString::number(nextDel));
    iniPreferences->setValue("/AutoBak/NextDel", nextDel);
  }
  iniPreferences->setValue("/AutoBak/BakCount", bakCount);

  appendBakFile(QDateTime::currentDateTime().toString("yyyy-M-d HH:mm:ss") +
                    "\n" + tr("Auto Backup") + "\n" + mw_one->strLatestModify,
                infoStr);

  mw_one->isNeedAutoBackup = false;
}

void Preferences::setBakStatus(bool status) {
  QSettings Reg(privateDir + iniBakFiles, QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif

  Reg.setValue("/BakFiles/BakStatus", status);
}

bool Preferences::getBakStatus() {
  QSettings Reg(privateDir + iniBakFiles, QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif

  return Reg.value("/BakFiles/BakStatus", 0).toBool();
}

void Preferences::setLatestAction(QString action) {
  QSettings Reg(privateDir + iniBakFiles, QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif

  Reg.setValue("/BakFiles/BakAction", action);

  setBakStatus(false);
}

QString Preferences::getLatestAction() {
  QSettings Reg(privateDir + iniBakFiles, QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif

  return Reg.value("/BakFiles/BakAction").toString();
}

void Preferences::appendBakFile(QString action, QString bakfile) {
  QSettings Reg(privateDir + iniBakFiles, QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif

  int count = Reg.value("/BakFiles/BakCount", 0).toInt();
  count++;
  Reg.setValue("BakFiles/BakCount", count);
  Reg.setValue("/BakFiles/Action" + QString::number(count - 1), action);
  Reg.setValue("/BakFiles/File" + QString::number(count - 1), bakfile);
}

QStringList Preferences::getBakFilesList() {
  QSettings Reg(privateDir + iniBakFiles, QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif

  QStringList fileList;
  QString action, bakfile;
  int count = Reg.value("/BakFiles/BakCount", 0).toInt();
  for (int i = 0; i < count; i++) {
    action = Reg.value("/BakFiles/Action" + QString::number(i)).toString();
    bakfile = Reg.value("/BakFiles/File" + QString::number(i)).toString();

    QFile file(bakfile);
    if (file.exists()) {
      fileList.append(action + "-===-" + bakfile);
    }
  }

  Reg.setValue("/BakFiles/BakCount", 0);
  for (int i = 0; i < fileList.count(); i++) {
    QString str = fileList.at(i);
    appendBakFile(str.split("-===-").at(0), str.split("-===-").at(1));
  }

  return fileList;
}

void Preferences::on_chkDark_clicked(bool checked) {
  isDark = checked;
  getCheckStatusChange();
}

void Preferences::initCheckStatus() {
  listCheckStatus.clear();
  listCheckStatus.append(ui->chkUIFont->isChecked());

  listCheckStatus.append(ui->chkDark->isChecked());

  listCheckStatus.append(ui->sliderFontSize->value());

  orgCustomFontText = ui->btnCustomFont->text().trimmed();
}

void Preferences::getCheckStatusChange() {
  bool isChanged = false;
  if (ui->chkUIFont->isChecked() != listCheckStatus.at(0)) isChanged = true;

  if (ui->chkDark->isChecked() != listCheckStatus.at(1)) isChanged = true;

  if (ui->sliderFontSize->value() != listCheckStatus.at(2)) isChanged = true;

  if (orgCustomFontText != ui->btnCustomFont->text().trimmed())
    isChanged = true;

  if (isChanged)
    ui->btnReStart->show();
  else
    ui->btnReStart->hide();
}

void Preferences::on_chkAniEffects_clicked(bool checked) { Q_UNUSED(checked); }
