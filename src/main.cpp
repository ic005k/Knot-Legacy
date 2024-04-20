#include <QApplication>
#include <QObject>
#include <QQuickWindow>
#include <QTranslator>
#include <QWidget>
#include <QtWebView/QtWebView>

#include "MainWindow.h"
#include "it/ltdev/qt/cpp/components/qtpdfviewerinitializer.h"
#include "src/Comm/qzipfile.h"

extern QString iniFile, txtFile, appName, iniDir, privateDir, customFontFamily,
    defaultFontFamily;
extern int fontSize;
extern void RegJni(const char* myClassName);
extern bool isDark;
extern MainWindow* mw_one;
extern QSettings* iniPreferences;

void loadLocal();
void unzip(QString zipFile, QString unzipDir);
int deleteDirfile(QString dirName);
QString loadText(QString textFile);
QString getTextEditLineText(QTextEdit* txtEdit, int i);
void TextEditToFile(QTextEdit* txtEdit, QString fileName);
void StringToFile(QString buffers, QString fileName);

bool zh_cn = false;
bool isAndroid, isIOS;

#define Cross_Origin

int main(int argc, char* argv[]) {
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
  RegJni("com/x/ShareReceiveActivity");
  RegJni("com/x/NoteEditor");
  RegJni("com/x/NewTodo");
  RegJni("com/x/NewNote");
  RegJni("com/x/AddRecord");
  RegJni("com/x/ContinueReading");

  isAndroid = true;
  isIOS = false;

  iniDir = "/storage/emulated/0/KnotData/";
  privateDir = "/storage/emulated/0/.Knot/";

#else
  defaultFontSize = QApplication::font().pointSize();

  isAndroid = false;
  iniDir = QDir::homePath() + "/" + appName + "Data/";
  privateDir = QDir::homePath() + "/." + appName + "/";

#endif

  QString pdfjsDir;
#ifdef Q_OS_ANDROID
  pdfjsDir = "/data/user/0/com.x/files/";

#else
  pdfjsDir = privateDir;
#endif
  pdfjsDir = privateDir;
  QString resFile = ":/res/pdf/pdfjs.zip";
  if (QFile::exists(resFile)) {
    deleteDirfile(privateDir + "pdfjs");
    QString zipFile = pdfjsDir + "pdfjs.zip";
    QFile::remove(zipFile);
    QFile::copy(resFile, zipFile);
    unzip(zipFile, pdfjsDir);

    QFile::copy(":/res/pdf/pdf.viewer.bridge.js",
                pdfjsDir + "pdfjs/web/pdf.viewer.bridge.js");
    QFile::copy(":/res/pdf/qwebchannel.js",
                pdfjsDir + "pdfjs/web/qwebchannel.js");

    QTextEdit* text_edit = new QTextEdit();
    QString view_html = pdfjsDir + "pdfjs/web/viewer.html";
    text_edit->setPlainText(loadText(view_html));
    for (int i = 0; i < text_edit->document()->lineCount(); i++) {
      QString str = getTextEditLineText(text_edit, i);
      str = str.trimmed();
      if (str.contains("</head>")) {
        QString s1, s2;
        s1 = "<script type=\"text/javascript\" "
             "src=\"qwebchannel.js\"></script>\n";
        s2 = "<script src=\"pdf.viewer.bridge.js\"></script>\n";
        text_edit->insertPlainText(s1);
        text_edit->insertPlainText(s2);
        break;
      }
    }
    TextEditToFile(text_edit, view_html);

    QString v_js = pdfjsDir + "pdfjs/web/viewer.js";
    QString v_jsBuffers = loadText(v_js);
    v_jsBuffers.replace("value: \"compressed.tracemonkey-pldi-09.pdf\",",
                        "value: \"\",");
    v_jsBuffers.replace("loadingErrorMessage = _this7.l10n.get",
                        "// loadingErrorMessage = _this7.l10n.get");
    v_jsBuffers.replace("scale = pageWidthScale",
                        "scale = pageWidthScale * 1.05");
    StringToFile(v_jsBuffers, v_js);
  }

  iniFile = iniDir + appName + ".ini";

  QDir dir0;
  dir0.mkpath(iniDir);

  iniPreferences =
      new QSettings(privateDir + "options.ini", QSettings::IniFormat, NULL);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  iniPreferences->setIniCodec("utf-8");
#endif
  fontSize =
      iniPreferences->value("/Options/FontSize", defaultFontSize).toInt();
  bool isOverUIFont =
      iniPreferences->value("/Options/chkUIFont", false).toBool();
  QString customFontPath =
      iniPreferences->value("/Options/CustomFont").toString();
  isDark = iniPreferences->value("/Options/Dark", false).toBool();

#ifdef Q_OS_WIN
  defaultFontFamily = "Microsoft YaHei UI";

#endif

#ifdef Q_OS_ANDROID
  defaultFontFamily = "DroidSansFallback";

#endif

#ifdef Q_OS_LINUX
  defaultFontFamily = "Sans";

#endif

  if (QFile(customFontPath).exists()) {
    int loadedFontID = QFontDatabase::addApplicationFont(customFontPath);
    QStringList loadedFontFamilies =
        QFontDatabase::applicationFontFamilies(loadedFontID);
    if (!loadedFontFamilies.empty()) {
      customFontFamily = loadedFontFamilies.at(0);
    }
  } else
    customFontFamily = defaultFontFamily;

  // Set Theme
  QString fileTheme;
  if (isDark)
    fileTheme = ":/theme/dark/darkstyle.qss";
  else
    fileTheme = ":/theme/light/lightstyle.qss";
  QFile f_theme(fileTheme);
  if (!f_theme.exists()) {
    qDebug() << "Unable to set stylesheet, file not found";
  } else {
    f_theme.open(QFile::ReadOnly | QFile::Text);
    QTextStream ts(&f_theme);
    QString qssAll = ts.readAll();
    qssAll = qssAll.replace("QSlider", "CancelQSlider");
    qssAll = qssAll.replace("width: 16px;", "width: 8px;");
    qssAll = qssAll.replace("margin: 16px 2px 16px 2px;",
                            "margin: 1px 2px 1px 2px;");
    app.setStyleSheet(qssAll);
  }

  // Set Font
  QFont font;
  if (isOverUIFont) {
    if (customFontFamily.length() > 0) {
      font.setFamily(customFontFamily);
    }
  } else {
    if (defaultFontFamily.length() > 0) {
      font.setFamily(defaultFontFamily);
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

void unzip(QString zipFile, QString unzipDir) {
  qompress::QZipFile zf(zipFile);
  if (!zf.open(qompress::QZipFile::ReadOnly)) {
    qDebug() << "Failed to open " << zipFile << ": " << zf.errorString();

    return;
  }
  do {
    qompress::QZipFileEntry file = zf.currentEntry();
    QString strFile = file.name();
    qDebug() << strFile << ":\t\t" << file.uncompressedSize()
             << "bytes (uncompressed)";

    QFileInfo fi(strFile);
    QString path = unzipDir + fi.path();
    QDir dir0(path);
    if (!dir0.exists()) dir0.mkpath(path);

    QFile myfile(unzipDir + strFile);
    myfile.open(QIODevice::WriteOnly);
    zf.extractCurrentEntry(myfile, "", file.uncompressedSize());
    myfile.close();

  } while (zf.nextEntry());
  zf.close();
}

int deleteDirfile(QString dirName) {
  QDir directory(dirName);
  if (!directory.exists()) {
    return true;
  }

  QString srcPath = QDir::toNativeSeparators(dirName);
  if (!srcPath.endsWith(QDir::separator())) srcPath += QDir::separator();

  QStringList fileNames = directory.entryList(
      QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);
  bool error = false;
  for (QStringList::size_type i = 0; i != fileNames.size(); ++i) {
    QString filePath = srcPath + fileNames.at(i);
    QFileInfo fileInfo(filePath);
    if (fileInfo.isFile() || fileInfo.isSymLink()) {
      QFile::setPermissions(filePath, QFile::WriteOwner);
      if (!QFile::remove(filePath)) {
        error = true;
      }
    } else if (fileInfo.isDir()) {
      if (!deleteDirfile(filePath)) {
        error = true;
      }
    }
  }

  if (!directory.rmdir(QDir::toNativeSeparators(directory.path()))) {
    error = true;
  }
  return !error;
}

QString loadText(QString textFile) {
  bool isExists = QFile(textFile).exists();
  if (isExists) {
    QFile file(textFile);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
      qDebug() << "Cannot read file";

    } else {
      QTextStream in(&file);

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
      in.setCodec("UTF-8");
#endif

      QString text = in.readAll();
      return text;
    }
    file.close();
  }

  return "";
}

QString getTextEditLineText(QTextEdit* txtEdit, int i) {
  QTextBlock block = txtEdit->document()->findBlockByNumber(i);
  txtEdit->setTextCursor(QTextCursor(block));
  QString lineText = txtEdit->document()->findBlockByNumber(i).text();
  return lineText;
}

void TextEditToFile(QTextEdit* txtEdit, QString fileName) {
  QFile* file;
  file = new QFile;
  file->setFileName(fileName);
  bool ok = file->open(QIODevice::WriteOnly | QIODevice::Text);
  if (ok) {
    QTextStream out(file);
    out << txtEdit->toPlainText();
    file->close();
    delete file;
  } else
    qDebug() << "Write failure!" << fileName;
}

void StringToFile(QString buffers, QString fileName) {
  QFile* file;
  file = new QFile;
  file->setFileName(fileName);
  bool ok = file->open(QIODevice::WriteOnly | QIODevice::Text);
  if (ok) {
    QTextStream out(file);
    out << buffers;
    file->close();
    delete file;
  } else
    qDebug() << "Write failure!" << fileName;
}
