#include "ReceiveShare.h"

#include "src/MainWindow.h"
#include "ui_MainWindow.h"
#include "ui_ReceiveShare.h"

extern MainWindow* mw_one;
extern Method* m_Method;

ReceiveShare::ReceiveShare(QWidget* parent)
    : QDialog(parent), ui(new Ui::ReceiveShare) {
  ui->setupUi(this);
  installEventFilter(this);
  setModal(true);
  ui->lblTip->setWordWrap(true);
  ui->lblTip->adjustSize();
  ui->lblTip->setText(tr("Tip: You can paste this data wherever you need it."));
  mw_one->set_ToolButtonStyle(this);

  ui->btnTest->hide();
}

ReceiveShare::~ReceiveShare() { delete ui; }

void ReceiveShare::on_setReceiveShareData() { addToNote(isInsertToNote); }

void ReceiveShare::closeEvent(QCloseEvent* event) {
  Q_UNUSED(event)
  QClipboard* m_Clip = QApplication::clipboard();
  m_Clip->setText(strReceiveShareData);

  m_Method->closeGrayWindows();
}

bool ReceiveShare::eventFilter(QObject* watch, QEvent* evn) {
  if (evn->type() == QEvent::KeyRelease) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      Close();

      return true;
    }
  }

  return QWidget::eventFilter(watch, evn);
}

void ReceiveShare::setShareDone(QString strDone) {
  QSettings Reg("/storage/emulated/0/.Knot/myshare.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif
  Reg.setValue("shareDone", strDone);
}

QString ReceiveShare::getShareDone() {
  QSettings Reg("/storage/emulated/0/.Knot/myshare.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif
  return Reg.value("/share/shareDone", "true").toString();
}

QString ReceiveShare::getShareType() {
  QSettings Reg("/storage/emulated/0/.Knot/myshare.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif
  return Reg.value("/share/shareType", "text/plain").toString();
}

QString ReceiveShare::getShareString() {
  QSettings Reg("/storage/emulated/0/.Knot/myshare.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif
  return Reg.value("/share/receiveData", "").toString();
}

void ReceiveShare::init() {
  shareType = getShareType();

  if (shareType == "text/plain") {
    ui->btnAddToTodo->show();
    ui->btnInsertToNote->show();
    ui->btnAppendToNote->show();
    ui->lblTip->show();
    ui->lblDataType->setText(tr("Data Type : Text"));
  }

  if (shareType == "image/*") {
    ui->btnAddToTodo->hide();
    ui->lblTip->hide();
    ui->lblDataType->setText(tr("Data Type : Image"));
  }

  moveTaskToFront();

  int x, y, w, h;
  x = mw_one->geometry().x();
  y = mw_one->geometry().y();
  w = mw_one->width();
  h = mw_one->height() / 2;
  setGeometry(x, y, w, h);
  m_Method->showGrayWindows();
  show();
}

void ReceiveShare::Close() {
  m_Method->closeGrayWindows();
  close();
}

void ReceiveShare::on_btnAddToTodo_clicked() {
  Close();
  strReceiveShareData = getShareString();
  if (mw_one->ui->frameTodo->isHidden()) {
    closeAllActiveWindows();
    mw_one->on_btnTodo_clicked();
    mw_one->ui->editTodo->setText(strReceiveShareData);
    mw_one->Sleep(500);
    mw_one->on_btnAddTodo_clicked();

  } else {
    mw_one->ui->editTodo->setText(strReceiveShareData);
    mw_one->Sleep(500);
    mw_one->on_btnAddTodo_clicked();
  }
}

void ReceiveShare::addToNote(bool isInsert) {
  QString imgFile = "/storage/emulated/0/.Knot/receive_share_pic.png";
  strReceiveShareData = getShareString();
  if (isInsert) {
    if (shareType == "text/plain") {
      mw_one->m_Notes->insertNote(strReceiveShareData);
    }
    if (shareType == "image/*") {
      QString strImg = mw_one->m_Notes->insertImage(imgFile);
      mw_one->m_Notes->insertNote(strImg);
    }
  } else {
    // append
    if (shareType == "text/plain") {
      mw_one->m_Notes->appendNote(strReceiveShareData);
    }
    if (shareType == "image/*") {
      QString strImg = mw_one->m_Notes->insertImage(imgFile);
      mw_one->m_Notes->appendNote(strImg);
    }
  }

  qDebug() << "strReceiveShareData=" << strReceiveShareData;
}

void ReceiveShare::on_btnAppendToNote_clicked() {
  Close();
  closeAllActiveWindows();
  mw_one->on_btnNotes_clicked();
  mw_one->on_btnEdit_clicked();

  isInsertToNote = false;
  QTimer::singleShot(100, this, SLOT(on_setReceiveShareData()));
}

void ReceiveShare::on_btnInsertToNote_clicked() {
  Close();
  closeAllActiveWindows();
  mw_one->on_btnNotes_clicked();
  mw_one->on_btnEdit_clicked();

  isInsertToNote = true;
  QTimer::singleShot(100, this, SLOT(on_setReceiveShareData()));
}

QObjectList ReceiveShare::getAllFrame(QObjectList lstUIControls) {
  QObjectList lst;
  foreach (QObject* obj, lstUIControls) {
    if (obj->metaObject()->className() == QStringLiteral("QFrame")) {
      lst.append(obj);
    }
  }
  return lst;
}

void ReceiveShare::closeAllActiveWindows() {
  if (mw_one->m_Notes->isVisible()) {
    mw_one->m_Notes->on_btnDone_clicked();
  }

  QObjectList frameList;
  frameList = getAllFrame(mw_one->getAllUIControls(mw_one));
  for (int i = 0; i < frameList.count(); i++) {
    QFrame* frame = (QFrame*)frameList.at(i);
    if (frame->parent() == mw_one->ui->centralwidget &&
        frame->objectName() != "frameMain") {
      qDebug() << frame->objectName();
      if (frame->isVisible()) {
        frame->hide();
        mw_one->ui->frameMain->show();
      }
    }
  }
}

void ReceiveShare::on_btnTest_clicked() { closeAllActiveWindows(); }

void ReceiveShare::shareString(const QString& title, const QString& content) {
#ifdef Q_OS_ANDROID

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  QAndroidJniObject jTitle = QAndroidJniObject::fromString(title);
  QAndroidJniObject jPath = QAndroidJniObject::fromString(content);
  QAndroidJniObject activity = QtAndroid::androidActivity();
  QAndroidJniObject m_activity = QAndroidJniObject::fromString("shareString");
  activity.callMethod<void>(
      "shareString",
      "(Ljava/lang/String;Ljava/lang/String;Lorg/qtproject/qt5/android/"
      "bindings/QtActivity;)V",
      jTitle.object<jstring>(), jPath.object<jstring>(),
      activity.object<jobject>());

#else
  QJniObject jTitle = QJniObject::fromString(title);
  QJniObject jPath = QJniObject::fromString(content);
  QJniObject activity = QtAndroid::androidActivity();
  QJniObject::callStaticMethod<void>(
      "com.x/MyActivity", "shareString",
      "(Ljava/lang/String;Ljava/lang/String;Lorg/qtproject/qt/android/"
      "bindings/QtActivity;)V",
      jTitle.object<jstring>(), jPath.object<jstring>(),
      activity.object<jobject>());

#endif

#endif
}

void ReceiveShare::shareImage(const QString& title, const QString& path,
                              const QString& fileType) {
#ifdef Q_OS_ANDROID

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  QAndroidJniObject jTitle = QAndroidJniObject::fromString(title);
  QAndroidJniObject jPath = QAndroidJniObject::fromString(path);
  QAndroidJniObject jType = QAndroidJniObject::fromString(fileType);
  QAndroidJniObject activity = QtAndroid::androidActivity();
  activity.callMethod<void>("shareImage",
                            "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/"
                            "String;Lorg/qtproject/qt5/android/"
                            "bindings/QtActivity;)V",
                            jTitle.object<jstring>(), jPath.object<jstring>(),
                            jType.object<jstring>(),
                            activity.object<jobject>());
#else
  QJniObject jTitle = QJniObject::fromString(title);
  QJniObject jPath = QJniObject::fromString(path);
  QAndroidJniObject jType = QAndroidJniObject::fromString(fileType);
  QJniObject activity = QtAndroid::androidActivity();
  QJniObject::callStaticMethod<void>(
      "com.x/MyActivity", "shareImage",
      "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/"
      "String;Lorg/qtproject/qt5/android/"
      "bindings/QtActivity;)V",
      jTitle.object<jstring>(), jPath.object<jstring>(),
      jType.object<jstring>(), activity.object<jobject>());

#endif

#endif
}

void ReceiveShare::shareImages(const QString& title,
                               const QStringList& imagesPathList) {
#ifdef Q_OS_ANDROID

  QString imagesPath;
  foreach (QString str, imagesPathList) {
    imagesPath += str + "|";
  }
  imagesPath = imagesPath.remove(imagesPath.size() - 1, 1).trimmed();

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  QAndroidJniObject jTitle = QAndroidJniObject::fromString(title);
  QAndroidJniObject jPathList = QAndroidJniObject::fromString(imagesPath);
  QAndroidJniObject activity = QtAndroid::androidActivity();
  QAndroidJniObject::callStaticMethod<void>(
      "com.x/MyActivity", "shareImages",
      "(Ljava/lang/String;Ljava/lang/String;Lorg/qtproject/qt5/android/"
      "bindings/QtActivity;)V",
      jTitle.object<jstring>(), jPathList.object<jstring>(),
      activity.object<jobject>());
#else
  QJniObject jTitle = QJniObject::fromString(title);
  QJniObject jPathList = QJniObject::fromString(imagesPath);
  QJniObject activity = QtAndroid::androidActivity();
  QJniObject::callStaticMethod<void>(
      "com.x/MyActivity", "shareImages",
      "(Ljava/lang/String;Ljava/lang/String;Lorg/qtproject/qt/android/"
      "bindings/QtActivity;)V",
      jTitle.object<jstring>(), jPathList.object<jstring>(),
      activity.object<jobject>());

#endif

#endif
}

void ReceiveShare::moveTaskToFront() {
#ifdef Q_OS_ANDROID

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))

  QAndroidJniObject m_activity =
      QAndroidJniObject::fromString("com.x/MyActivity");
  m_activity.callStaticMethod<void>("com.x/MyActivity", "setMax", "()V");
#else
  QJniObject m_activity = QJniObject::fromString("com.x/MyActivity");
  m_activity.callStaticMethod<void>("com.x/MyActivity", "setMax", "()V");
#endif

#endif
}
