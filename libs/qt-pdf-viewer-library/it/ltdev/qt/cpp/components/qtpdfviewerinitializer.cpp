/*
 *
 *  Copyright 2022 Leonardo Tarollo (LTDev) <develtar@gmail.com>
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */
#include "qtpdfviewerinitializer.h"

#include "src/MainWindow.h"
QString pDir;
QString url;
namespace LTDev {

/**
 * Initializes the library
 */
void QtPdfViewerInitializer::initialize(QString p_dir) {
  // To make the Qt WebView module function correctly across all platforms,
  // it is necessary to call QtWebView::initialize() before in Qt>= 5.15.0, or
  // after in Qt<5.15.0, creating the QGuiApplication instance
  QtWebView::initialize();

  pDir = p_dir;

  // Expose classes to qml
  qmlRegisterType<LTDev::WebSocketTransport>("it.ltdev.qt.cpp.components", 1, 0,
                                             "WebSocketTransport");
  qmlRegisterSingletonType<QtPdfViewerInitializer>(
      "it.ltdev.qt.cpp.components", 1, 0, "QtPdfViewerInitializer",
      &QtPdfViewerInitializer::getQmlInstance);
}

/**
 * Initializes the viewer creating the folders needed for the pdf view.
 * Returns true if created or already initialized, false otherwise.
 *
 * @return bool
 */
bool QtPdfViewerInitializer::initializeViewer(bool force) {
  if (!this->_initialized || force) {
    // #ifdef Q_OS_ANDROID
    //     _initialized = true;
    //    emit viewerChanged();
    // #else
    FileUtils::removeDir(this->_root);

    // Copy pdf js folder in the application path (it depends on platform)
    // @note: the copy of the folder is necessary for Android devices
    // because webview cannot access files from qrc directly, so
    // we create a local folder from which files will be loaded.

    bool copied = true;  // FileUtils::copyDirs(":/pdfjs", this->_root);

    bool notify = _initialized != copied;
    if (notify) {
      _initialized = copied;
      if (_initialized) {
        // Notify viewer
        emit viewerChanged();
      }
    }
    // #endif
  }

  return this->_initialized;
}

/**
 *
 * Returns the base64 string of the given pdf file.
 *
 * @param path: the pdf file
 *
 * @return QByteArray
 *
 **/
QByteArray QtPdfViewerInitializer::pdfToBase64(const QString &path) {
  if (FileUtils::exists(path)) {
    QFile input(path);
    if (input.open(QIODevice::ReadOnly)) {
      QByteArray base64 = input.readAll().toBase64();
      qDebug() << "base64 path=" << path << "ok";
      return base64;
    }
  }

  return QByteArray();
}

/**
 * Returns the qml instance.
 **/
QObject *QtPdfViewerInitializer::getQmlInstance(QQmlEngine *engine,
                                                QJSEngine *scriptEngine) {
  Q_UNUSED(engine);
  Q_UNUSED(scriptEngine);
  return QtPdfViewerInitializer::getInstance();
}

/**
 * @brief Constructor
 */
QtPdfViewerInitializer::QtPdfViewerInitializer() {
#ifdef Q_OS_ANDROID

  pDir = "/storage/emulated/0/.Knot/";

#else

  pDir = QDir::homePath() + "/.Knot/";

#endif

  QString appDir =
      QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
  QString dir = FileUtils::joinPaths(QStringList() << appDir << DIR::path);
  this->_root = dir;

  // dir = "/data/user/0/com.x/files/libs/pdfjs/"
  // appDir = "/data/user/0/com.x/files"
  qDebug() << "=============="
           << "this->_root =" << dir << appDir;

  bool isCN = false;
  QLocale locale;
  if (locale.language() == QLocale::Chinese) isCN = true;

  if (isCN)
    this->_viewer =
        FileUtils::joinPaths(QStringList() << dir << "viewer_cn.html");
  else
    this->_viewer = FileUtils::joinPaths(QStringList() << dir << "viewer.html");

  this->_viewer = pDir + "pdfjs/web/viewer.html";

  url = this->_viewer;
  qDebug() << "this viewer.html=" << this->_viewer;

  this->_initialized = false;
}

QString QtPdfViewerInitializer::getCurrentUrl() { return url; }

}  // namespace LTDev
