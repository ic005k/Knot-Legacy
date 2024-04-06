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
#ifndef QTPDFVIEWERINITIALIZER_H
#define QTPDFVIEWERINITIALIZER_H

#include <QObject>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QtWebView/QtWebView>

#include "WebSocketTransport.h"
#include "fileutils.h"
#include "singleton.cpp"

#define PDF_VIEWER_VERSION "1.0.003"

namespace LTDev {

namespace DIR {
static const QString &path = "libs/pdfjs/";
}

class QtPdfViewerInitializer : public QObject,
                               public Singleton<QtPdfViewerInitializer> {
  Q_OBJECT

  Q_PROPERTY(QString viewer READ viewer NOTIFY viewerChanged)

  // friend class Singleton;  // clang for macosx

  friend class Singleton<QtPdfViewerInitializer>;  // vs2019

 public:
  Q_INVOKABLE static QString getCurrentUrl();
  /**
   * @brief Initializes the library
   */
  static void initialize();

  /**
   * @brief Initializes the viewer creating the folders needed for the pdf view.
   * Returns true if created or already initialized, false otherwise.
   */
  Q_INVOKABLE bool initializeViewer(bool force = false);

  /**
   * @brief Returns the base64 string of the given pdf file.
   **/
  Q_INVOKABLE static QByteArray pdfToBase64(const QString &path);

  Q_INVOKABLE const uint8_t *ByteArrayToUnit8Array(QByteArray base64);

  Q_INVOKABLE QString root() { return this->_root; }
  QString viewer() { return this->_viewer; }

  Q_INVOKABLE bool initialized() { return this->_initialized; }

  /**
   * @brief Returns the qml instance.
   **/
  static QObject *getQmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine);

 protected:
  /**
   * @brief the directory in which the library is exported
   */
  QString _root;

  /**
   * @brief flag to know if library has been already exported
   */
  bool _initialized;

  /**
   * @brief Constructor
   */
  QtPdfViewerInitializer();

  /**
   * @brief the path of the html viewer
   */
  QString _viewer;

 signals:
  void viewerChanged();

 private:
};

}  // namespace LTDev
#endif  // QTPDFVIEWERINITIALIZER_H
