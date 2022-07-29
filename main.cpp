#include <QApplication>
#include <QTranslator>

#include "mainwindow.h"
#include "specialaccelerometerpedometer.h"

extern QString iniFile, txtFile, appName, iniDir;
extern int fontSize;
extern void RegJni();
extern void RegJniMyActivity();
void loadLocal();
bool zh_cn = false;
bool isAndroid, isIOS;
int main(int argc, char *argv[]) {
#ifdef Q_OS_ANDROID
  RegJni();
  RegJniMyActivity();
  //禁用文本选择
  // qputenv("QT_QPA_NO_TEXT_HANDLES", "1");
  qDebug() << "OS=Linux Android";
  isAndroid = true;
  isIOS = false;
#endif

#ifdef Q_OS_UNIX
  isAndroid = false;
  isIOS = true;
  qDebug() << "OS=UNIX IOS";
#endif

#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
  {
    qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "1");
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
  }
#endif

  QApplication a(argc, argv);

  QDir dir;
  QString path;
  path = dir.currentPath();
  qDebug() << "Path:" << path;
  if (isIOS) {
    QString str1 = QDir::homePath() + "/" + appName + "/";
    QDir dir0;
    dir0.mkpath(str1);
    iniDir = str1;
    iniFile = iniDir + appName + ".ini";
  }
  if (isAndroid) {
    iniDir = path + "/";
    iniFile = iniDir + appName + ".ini";
  }

  QSettings Reg(iniDir + "options.ini", QSettings::IniFormat);
  Reg.setIniCodec("utf-8");
  int fontSize = Reg.value("/Options/FontSize", 16).toInt();

  QString fontName;
  int loadedFontID =
      QFontDatabase::addApplicationFont(":/src/CangErJinKai01-9128-W02-3.otf");
  QStringList loadedFontFamilies =
      QFontDatabase::applicationFontFamilies(loadedFontID);
  if (!loadedFontFamilies.empty()) {
    fontName = loadedFontFamilies.at(0);
    QFont font(fontName, fontSize);
    // a.setFont(font);
  }

  QSettings Reg2(iniDir + "reader.ini", QSettings::IniFormat);
  Reg2.setIniCodec("utf-8");
  fontName = Reg2.value("/Reader/FontName", "").toString();
  qDebug() << "fontName" << fontName << fontSize << iniDir;
  if (fontName != "") {
    QFont font(fontName, fontSize);
    a.setFont(font);
  }

  loadLocal();

  MainWindow w;
  w.show();

  return a.exec();
}

void loadLocal() {
  static QTranslator translator;
  static QTranslator translator1;
  static QTranslator translator2;

  QLocale locale;
  if (locale.language() == QLocale::English) {
    zh_cn = false;

  } else if (locale.language() == QLocale::Chinese) {
    bool tr = false;
    tr = translator.load(":/cn.qm");
    if (tr) {
      qApp->installTranslator(&translator);
      zh_cn = true;
    }

    bool tr1 = false;
    tr1 = translator1.load(":/qt_zh_CN.qm");
    if (tr1) {
      qApp->installTranslator(&translator1);
      zh_cn = true;
    }

    bool tr2 = false;
    tr2 = translator2.load(":/widgets_zh_cn.qm");
    if (tr2) {
      qApp->installTranslator(&translator2);
      zh_cn = true;
    }
  }
}
