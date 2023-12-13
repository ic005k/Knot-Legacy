#include <QApplication>
#include <QObject>
#include <QQuickWindow>
#include <QTranslator>
#include <QWidget>
#include <QtWebView/QtWebView>

#include "it/ltdev/qt/cpp/components/qtpdfviewerinitializer.h"
#include "MainWindow.h"

extern QString iniFile, txtFile, appName, iniDir, privateDir, fontname;
extern int fontSize;
extern void RegJni(const char* myClassName);
void loadLocal();
bool zh_cn = false;
bool isAndroid, isIOS;

int main(int argc, char* argv[]) {
  LTDev::QtPdfViewerInitializer::initialize();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
  {
#ifdef Q_OS_ANDROID
    qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "1");
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

#else
    qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "1");
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
#if (QT_VERSION >= QT_VERSION_CHECK(6, 4, 0))
    QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGLRhi);
#endif

#endif
  }
#endif

#ifdef Q_OS_LINUX
  // Fixed an issue where QtWebView in Linux could not display web pages.

  char ARG_DISABLE_WEB_SECURITY[] = "--disable-web-security";
  char ARG_DISABLE_SECCOMP_SECURITY[] = "--disable-seccomp-filter-sandbox";

  // ARG_DISABLE_WEB_SECURITY + ARG_DISABLE_SECCOMP_SECURITY + nullptr
  int newArgc = argc + 1 + 1 + 1;
  char** newArgv = new char*[newArgc];
  for (int i = 0; i < argc; i++) {
    newArgv[i] = argv[i];
  }
  newArgv[argc] = ARG_DISABLE_WEB_SECURITY;
  newArgv[argc + 1] = ARG_DISABLE_SECCOMP_SECURITY;
  newArgv[argc + 2] = nullptr;

  QApplication app(newArgc, newArgv);
#else
  QApplication app(argc, argv);
#endif

  // Delete QtPdfViewerInitializer instance on app close
  QObject::connect(&app, &QGuiApplication::aboutToQuit,
                   LTDev::QtPdfViewerInitializer::getInstance(),
                   LTDev::QtPdfViewerInitializer::deleteInstance);

  QTextCodec* codec = QTextCodec::codecForName("UTF-8");
  QTextCodec::setCodecForLocale(codec);

  // 禁用文本选择（针对所有的可输入的编辑框）
  qputenv("QT_QPA_NO_TEXT_HANDLES", "1");

  QDir dir;
  QString path;
  path = dir.currentPath();
  qDebug() << "Path:" << path;

  int defaultFontSize;

#ifdef Q_OS_ANDROID
  defaultFontSize = 17;

  RegJni("com/x/MyService");
  RegJni("com/x/MyActivity");
  RegJni("com/x/ClockActivity");

  isAndroid = true;
  isIOS = false;

  // privateDir = path + "/";
  iniDir = "/storage/emulated/0/KnotData/";
  privateDir = "/storage/emulated/0/.Knot/";

#else
  defaultFontSize = 12;

  isAndroid = false;
  iniDir = QDir::homePath() + "/" + appName + "Data/";
  privateDir = QDir::homePath() + "/." + appName + "/";

#endif

  iniFile = iniDir + appName + ".ini";

  QDir dir0;
  dir0.mkpath(iniDir);

  QSettings Reg(privateDir + "options.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif

  fontSize = Reg.value("/Options/FontSize", defaultFontSize).toInt();
  bool isReaderFont = Reg.value("/Options/chkReaderFont", false).toBool();
  bool isUIFont = Reg.value("/Options/chkUIFont", false).toBool();
  QString customFontPath = Reg.value("/Options/CustomFont").toString();

  QString fontName;
  QFont font;

#ifdef Q_OS_WIN
  font.setFamily("Microsoft YaHei UI");
#endif

#ifdef Q_OS_ANDROID
  font.setFamily("DroidSansFallback");
#endif

  if (QFile(customFontPath).exists()) {
    int loadedFontID = QFontDatabase::addApplicationFont(customFontPath);
    QStringList loadedFontFamilies =
        QFontDatabase::applicationFontFamilies(loadedFontID);
    if (!loadedFontFamilies.empty()) {
      fontName = loadedFontFamilies.at(0);
      if (isUIFont) font.setFamily(fontName);
      if (isReaderFont)
        fontname = fontName;
      else
        fontname = "DroidSansFallback";
    }
  }

  font.setPointSize(fontSize);
  app.setFont(font);
  loadLocal();

  MainWindow w;
  w.show();

  return app.exec();
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
    tr1 = translator1.load(":/tr/qt_zh_CN.qm");
    if (tr1) {
      qApp->installTranslator(&translator1);
      zh_cn = true;
    }

    bool tr2 = false;
    tr2 = translator2.load(":/tr/widgets_zh_cn.qm");
    if (tr2) {
      qApp->installTranslator(&translator2);
      zh_cn = true;
    }
  }
}
