#include <QApplication>
#include <QTranslator>
#include <QtWebView/QtWebView>

#include "mainwindow.h"
#include "specialaccelerometerpedometer.h"

extern QString iniFile, txtFile, appName, iniDir, fontname;
extern int fontSize;
extern void RegJni(const char* myClassName);
void loadLocal();
bool zh_cn = false;
bool isAndroid, isIOS;

int main(int argc, char* argv[]) {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
  {
#ifdef Q_OS_ANDROID
    qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "1");
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

#ifdef Q_OS_WIN
    qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "1");
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
  }
#endif
  QtWebView::initialize();
  QApplication a(argc, argv);

  QTextCodec* codec = QTextCodec::codecForName("UTF-8");
  QTextCodec::setCodecForLocale(codec);

  //禁用文本选择（针对所有的可输入的编辑框）
  qputenv("QT_QPA_NO_TEXT_HANDLES", "1");

  QDir dir;
  QString path;
  path = dir.currentPath();
  qDebug() << "Path:" << path;

#ifdef Q_OS_ANDROID
  RegJni("com/x/MyService");
  RegJni("com/x/MyActivity");
  RegJni("com/x/ClockActivity");

  isAndroid = true;
  isIOS = false;

  iniDir = path + "/";
  iniFile = iniDir + appName + ".ini";
#endif

#ifdef Q_OS_UNIX
  isAndroid = false;
  isIOS = true;
  qDebug() << "OS=UNIX IOS";

  QString str1 = QDir::homePath() + "/" + appName + "/";
  QDir dir0;
  dir0.mkpath(str1);
  iniDir = str1;
  iniFile = iniDir + appName + ".ini";
#endif

#ifdef Q_OS_WIN
  isAndroid = false;
  isIOS = true;

  QString str1 = QDir::homePath() + "/" + appName + "/";
  QDir dir0;
  dir0.mkpath(str1);
  iniDir = str1;
  iniFile = iniDir + appName + ".ini";
#endif

  QSettings Reg(iniDir + "options.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif

  fontSize = Reg.value("/Options/FontSize", 16).toInt();
  bool isReaderFont = Reg.value("/Options/chkReaderFont", false).toBool();
  bool isUIFont = Reg.value("/Options/chkUIFont", false).toBool();
  QString customFontPath = Reg.value("/Options/CustomFont").toString();

  // fontname = "Droid Sans Fallback";
  QString fontName;
  if (QFile(customFontPath).exists()) {
    int loadedFontID = QFontDatabase::addApplicationFont(
        customFontPath);  //":/res/CangErJinKai01-9128-W02-3.otf"
    QStringList loadedFontFamilies =
        QFontDatabase::applicationFontFamilies(loadedFontID);
    if (!loadedFontFamilies.empty()) {
      fontName = loadedFontFamilies.at(0);
    }
  }

  QFont font;
  if (isUIFont) {
    fontname = fontName;
    font.setFamily(fontname);
  }
  font.setPointSize(fontSize);
  a.setFont(font);

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
    tr = translator.load(":/src/cn.qm");
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
