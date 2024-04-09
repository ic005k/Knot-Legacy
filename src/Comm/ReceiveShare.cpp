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

void ReceiveShare::closeEvent(QCloseEvent* event) {
  Q_UNUSED(event)
  QClipboard* m_Clip = QApplication::clipboard();
  m_Clip->setText(m_Method->strReceiveShareData);

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

void ReceiveShare::init() {
  int x, y, w, h;
  x = mw_one->geometry().x();
  y = mw_one->geometry().y();
  w = mw_one->width();
  h = mw_one->height() / 3;
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
  if (mw_one->ui->frameTodo->isHidden()) {
    closeAllActiveWindows();
    mw_one->on_btnTodo_clicked();
    mw_one->ui->editTodo->setText(m_Method->strReceiveShareData);
    mw_one->Sleep(500);
    mw_one->on_btnAddTodo_clicked();

  } else {
    mw_one->ui->editTodo->setText(m_Method->strReceiveShareData);
    mw_one->Sleep(500);
    mw_one->on_btnAddTodo_clicked();
  }
}

void ReceiveShare::on_btnAddToNote_clicked() {
  Close();
  if (mw_one->m_Notes->isHidden()) {
    closeAllActiveWindows();
    mw_one->on_btnNotes_clicked();
    mw_one->on_btnEdit_clicked();
    mw_one->m_Notes->m_EditSource->append("\n" + m_Method->strReceiveShareData);

  } else {
    mw_one->m_Notes->m_EditSource->append("\n" + m_Method->strReceiveShareData);
  }

  QTextCursor cursor = mw_one->m_Notes->m_EditSource->textCursor();
  cursor.movePosition(QTextCursor::End);
  mw_one->m_Notes->m_EditSource->setTextCursor(cursor);
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

void ReceiveShare::on_btnInsertToNote_clicked() {
  Close();
  if (mw_one->m_Notes->isHidden()) {
    closeAllActiveWindows();
    mw_one->on_btnNotes_clicked();
    mw_one->on_btnEdit_clicked();
    mw_one->m_Notes->m_EditSource->insertPlainText(
        m_Method->strReceiveShareData);

  } else {
    mw_one->m_Notes->m_EditSource->insertPlainText(
        m_Method->strReceiveShareData);
  }
}

void ReceiveShare::shareString(const QString& title, const QString& content) {
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
}

void ReceiveShare::shareImage(const QString& title, const QString& path) {
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  QAndroidJniObject jTitle = QAndroidJniObject::fromString(title);
  QAndroidJniObject jPath = QAndroidJniObject::fromString(path);
  QAndroidJniObject activity = QtAndroid::androidActivity();
  QAndroidJniObject::callStaticMethod<void>(
      "com.x/MyActivity", "shareImage",
      "(Ljava/lang/String;Ljava/lang/String;Lorg/qtproject/qt5/android/"
      "bindings/QtActivity;)V",
      jTitle.object<jstring>(), jPath.object<jstring>(),
      activity.object<jobject>());
#else
  QJniObject jTitle = QJniObject::fromString(title);
  QJniObject jPath = QJniObject::fromString(path);
  QJniObject activity = QtAndroid::androidActivity();
  QJniObject::callStaticMethod<void>(
      "com.x/MyActivity", "shareImage",
      "(Ljava/lang/String;Ljava/lang/String;Lorg/qtproject/qt/android/"
      "bindings/QtActivity;)V",
      jTitle.object<jstring>(), jPath.object<jstring>(),
      activity.object<jobject>());

#endif
}

void ReceiveShare::shareImages(const QString& title,
                               const QStringList& imagesPathList) {
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
}
