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
extern bool zh_cn;

namespace LTDev {

/**
 * Initializes the library
 */
void QtPdfViewerInitializer::initialize() {
  // To make the Qt WebView module function correctly across all platforms,
  // it is necessary to call QtWebView::initialize() before in Qt>= 5.15.0, or
  // after in Qt<5.15.0, creating the QGuiApplication instance
  QtWebView::initialize();

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
    //#ifdef Q_OS_ANDROID
    //    _initialized = true;
    //   emit viewerChanged();
    //#else
    FileUtils::removeDir(this->_root);

    // Copy pdf js folder in the application path (it depends on platform)
    // @note: the copy of the folder is necessary for Android devices
    // because webview cannot access files from qrc directly, so
    // we create a local folder from which files will be loaded.
    bool copied = FileUtils::copyDirs(":/pdfjs", this->_root);
    bool notify = _initialized != copied;
    if (notify) {
      _initialized = copied;
      if (_initialized) {
        // Notify viewer
        emit viewerChanged();
      }
    }
    //#endif
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
  //#ifdef Q_OS_ANDROID
  // this->_viewer = "/android_asset/pdfjs/viewer.html";
  //#else
  QString appDir =
      QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
  QString dir = FileUtils::joinPaths(QStringList() << appDir << DIR::path);

  this->_root = dir;

  if (zh_cn)
    this->_viewer = FileUtils::joinPaths(QStringList() << dir << "viewer.html");
  else
    this->_viewer = FileUtils::joinPaths(QStringList() << dir << "viewer.html");
  //#endif

  qDebug() << "bbbb..." << this->_viewer;

  this->_initialized = false;
}

}  // namespace LTDev
