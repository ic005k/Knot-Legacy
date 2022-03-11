#include <QApplication>

#include "mainwindow.h"
extern QString iniFile, txtFile;
int main(int argc, char *argv[]) {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
  {
    qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "1");
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
  }
#endif
  QApplication a(argc, argv);
  MainWindow w;
  w.show();

  return a.exec();
}
