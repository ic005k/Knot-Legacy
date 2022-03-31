#include <QApplication>
#include <QTranslator>

#include "mainwindow.h"

extern QString iniFile, txtFile, appName, iniDir;
extern int fontSize;
void loadLocal();
bool zh_cn = false;
bool isAndroid, isIOS;
int main(int argc, char *argv[]) {
#ifdef Q_OS_ANDROID
  qDebug() << "OS=Linux Android";
  isAndroid = true;
  isIOS = false;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
  {
    qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "1");
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
  }
#endif
#endif

#ifdef Q_OS_UNIX
  isAndroid = false;
  isIOS = true;
  qDebug() << "OS=UNIX IOS";
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
