#include "dlgmainnotes.h"

#include "mainwindow.h"
#include "ui_dlgmainnotes.h"
#include "ui_mainwindow.h"

extern MainWindow* mw_one;
extern QString iniFile, iniDir;
extern bool isImport;

dlgMainNotes::dlgMainNotes(QWidget* parent)
    : QDialog(parent), ui(new Ui::dlgMainNotes) {
  ui->setupUi(this);
  ui->textEdit->installEventFilter(this);
  this->installEventFilter(this);
  this->setModal(true);
  ui->gridLayout_2->setMargin(1);

  connect(pAndroidKeyboard, &QInputMethod::visibleChanged, this,
          &dlgMainNotes::on_KVChanged);

  vScrollBar = new SmoothScrollBar();
  vScrollBar->setOrientation(Qt::Orientation::Vertical);  //将滚动条设置为纵向

  // ui->textEdit->setVerticalScrollBar(vScrollBar);
  QScroller::grabGesture(ui->textEdit, QScroller::LeftMouseButtonGesture);
  ui->textEdit->verticalScrollBar()->setStyleSheet(mw_one->vsbarStyleSmall);

  // ui->textBrowser->setVerticalScrollBar(vScrollBar);
  QScroller::grabGesture(ui->textBrowser, QScroller::LeftMouseButtonGesture);
  ui->textBrowser->verticalScrollBar()->setStyleSheet(mw_one->vsbarStyleSmall);
  ui->textBrowser->setTextInteractionFlags(Qt::NoTextInteraction);
  ui->textBrowser->setHidden(true);

  mw_one->setSCrollPro(ui->textBrowser);
  mw_one->setSCrollPro(ui->textEdit);

  ui->textEdit->setAcceptRichText(true);
  connect(ui->textEdit->verticalScrollBar(), SIGNAL(valueChanged(int)), this,
          SLOT(editVSBarValueChanged()));
}

void dlgMainNotes::init() {
  this->setGeometry(mw_one->geometry().x(), mw_one->geometry().y(),
                    mw_one->width(), mw_one->height());
}

void dlgMainNotes::wheelEvent(QWheelEvent* e) {
  //当捕获到事件后，调用相对滚动的槽函数
  vScrollBar->scroll(e->angleDelta().y());
}

dlgMainNotes::~dlgMainNotes() { delete ui; }

void dlgMainNotes::keyReleaseEvent(QKeyEvent* event) { event->accept(); }

void dlgMainNotes::editVSBarValueChanged() {
  if (pAndroidKeyboard->isVisible()) {
    minSliderMax = ui->textEdit->verticalScrollBar()->maximum();
    minSliderPosition = ui->textEdit->verticalScrollBar()->sliderPosition();
    qDebug() << "min=" << minSliderPosition << minSliderMax;
  } else {
    maxSliderMax = ui->textEdit->verticalScrollBar()->maximum();
    maxSliderPosition = ui->textEdit->verticalScrollBar()->sliderPosition();

    qDebug() << "max=" << maxSliderPosition << maxSliderMax;
  }
}

void dlgMainNotes::resizeEvent(QResizeEvent* event) {
  Q_UNUSED(event);

  if (isShow) {
    if (this->height() != mw_one->mainHeight) {
      newHeight = this->height();
    }

    if (pAndroidKeyboard->isVisible()) {
      this->setGeometry(mw_one->geometry().x(), mw_one->geometry().y(),
                        mw_one->width(), newHeight);

      minSliderMax = ui->textEdit->verticalScrollBar()->maximum();
      minSliderPosition = ui->textEdit->verticalScrollBar()->sliderPosition();

      minSliderPosition = minSliderMax * maxSliderPosition / maxSliderMax;
      ui->textEdit->verticalScrollBar()->setSliderPosition(minSliderPosition);
    } else {
      maxSliderMax = ui->textEdit->verticalScrollBar()->maximum();
      maxSliderPosition = ui->textEdit->verticalScrollBar()->sliderPosition();

      maxSliderPosition = maxSliderMax * minSliderPosition / minSliderMax;
      ui->textEdit->verticalScrollBar()->setSliderPosition(maxSliderPosition);
    }
  }

  qDebug() << pAndroidKeyboard->keyboardRectangle().height()
           << "this height=" << this->height();
  qDebug() << "newHeight=" << newHeight << "main height=" << mw_one->mainHeight;
}

void dlgMainNotes::on_btnBack_clicked() {
  saveMainNotes();
  close();
}

void dlgMainNotes::saveMainNotes() {
  QSettings Reg(iniDir + "mainnotes.ini", QSettings::IniFormat);

  QString path = iniDir + "memo/";
  QDir dir;
  dir.mkpath(path);

  QString strMD = iniDir + "memo/memo.md";
  QFile memofile(strMD);
  if (memofile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
    QTextStream stream(&memofile);
    if (!ui->btnPic->isEnabled())
      ui->textEdit->setMarkdown(ui->textEdit->toPlainText());
    stream << ui->textEdit->toMarkdown(
        QTextDocument::MarkdownDialectCommonMark);
    memofile.close();
  }

  QFile memofile1(iniDir + "memo/memo.html");

  if (memofile1.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
    QTextStream stream(&memofile1);

    ui->textEdit->setMarkdown(mw_one->loadText(strMD));
    ui->textEdit->verticalScrollBar()->setSliderPosition(sliderPos);
    stream << ui->textEdit->toHtml().toUtf8();
    memofile1.close();
  }

  sliderPos = ui->textEdit->verticalScrollBar()->sliderPosition();

  loadMemoQML();

  Reg.setValue("/MainNotes/CurPos", curPos);
  Reg.setValue("/MainNotes/SlidePos", sliderPos);
  Reg.setValue("/MainNotes/CurrentOSIniDir", iniDir);

  QStringList list = getImgFileFromHtml(iniDir + "memo/memo.html");
  int count = list.count();
  Reg.setValue("/MainNotes/imgcount", count);
  for (int i = 0; i < count; i++) {
    Reg.setValue("/MainNotes/img" + QString::number(i), list.at(i));
  }

  QString file = iniDir + "mainnotes.txt";
  mw_one->TextEditToFile(ui->textEdit, file);
  encryption(file);
  encode(file);

  Reg.setValue("/MainNotes/Text", mw_one->loadText(file));

  QFile::remove(file);
}

void dlgMainNotes::init_MainNotes() {
  QString ini_file;
  if (isImport)
    ini_file = iniFile;
  else
    ini_file = iniDir + "mainnotes.ini";
  QSettings Reg(ini_file, QSettings::IniFormat);
  ui->textEdit->setPlainText(Reg.value("/MainNotes/Text").toString());
}

void dlgMainNotes::setCursorPosition() {
  QString ini_file = iniDir + "mainnotes.ini";
  QSettings Reg(ini_file, QSettings::IniFormat);

  sliderPos = Reg.value("/MainNotes/SlidePos").toReal();
  curPos = Reg.value("/MainNotes/CurPos").toLongLong();

  QTextCursor tmpCursor = ui->textEdit->textCursor();
  tmpCursor.setPosition(curPos);

  if (ui->textEdit->isHidden()) {
    ui->textEdit->verticalScrollBar()->setSliderPosition(sliderPos);
  }

  if (!mw_one->ui->quickWidgetMemo->isHidden()) {
    QQuickItem* root = mw_one->ui->quickWidgetMemo->rootObject();
    QMetaObject::invokeMethod((QObject*)root, "setVPos",
                              Q_ARG(QVariant, sliderPos));
  }
}

void dlgMainNotes::on_btnCloseText_clicked() {}

bool dlgMainNotes::eventFilter(QObject* obj, QEvent* evn) {
  if (evn->type() == QEvent::KeyPress) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      if (pAndroidKeyboard->isVisible())
        pAndroidKeyboard->hide();
      else
        on_btnBack_clicked();
      return true;
    }
  }

  return QWidget::eventFilter(obj, evn);
}

void dlgMainNotes::on_KVChanged() {
  if (!pAndroidKeyboard->isVisible()) {
    this->setGeometry(mw_one->geometry().x(), mw_one->geometry().y(),
                      mw_one->width(), mw_one->mainHeight);
  }
}

/*
加密文件
将文件读到字符串中，将每个字符都减1，然后将字符串写到文件中
*/
void dlgMainNotes::encode(QString filename) {
  QFile file(filename);
  QTextStream in(&file);
  QString str;
  if (file.open(QIODevice::ReadWrite)) {
    str = in.readAll();
    qDebug() << str;

    int len = str.length();
    for (int i = 0; i < len; ++i) {
      str[i] = QChar::fromLatin1(str[i].toLatin1() - 1);
    }

    qDebug() << str;
  }
  file.close();

  QTextStream out(&file);
  file.open(QIODevice::WriteOnly);
  out << str;
  file.close();
}

/*
解密文件
将文件读到字符串中，将每个字符加1，将字符写到文件中
*/
void dlgMainNotes::decode(QString filename) {
  QFile file(filename);
  QTextStream fin(&file);
  QString str;
  if (file.open(QIODevice::ReadOnly)) {
    str = fin.readAll();
    qDebug() << str;

    int len = str.length();
    for (int i = 0; i < len; ++i) {
      str[i] = QChar::fromLatin1(str[i].toLatin1() + 1);
    }

    qDebug() << str;
  }
  file.close();

  QTextStream fout(&file);
  file.open(QIODevice::WriteOnly);
  fout << str;
  file.close();
}

void dlgMainNotes::encryption(const QString& fileName) {
  QFile original(fileName);
  if (!original.open(QIODevice::ReadOnly)) {
    QMessageBox::warning(0, "Read11", "Read error!", QMessageBox::Yes);
  }
  QByteArray ba = original.readAll().toBase64();
  original.close();

  QFile dest(fileName);
  if (!dest.open(QIODevice::WriteOnly)) {
    QMessageBox::warning(0, "Write11", "Write error!", QMessageBox::Yes);
  }
  dest.write(ba);
  dest.close();
}

QString dlgMainNotes::Deciphering(const QString& fileName) {
  QFile file(fileName);
  if (!file.open(QIODevice::ReadOnly)) {
    QMessageBox::warning(this, tr("Load Ds File"), file.errorString(),
                         QMessageBox::Yes);
  }

  return QByteArray::fromBase64(file.readAll());

  file.close();
}

void dlgMainNotes::on_btnUndo_clicked() { ui->textEdit->undo(); }

void dlgMainNotes::on_btnRedo_clicked() { ui->textEdit->redo(); }

void dlgMainNotes::on_textEdit_textChanged() {}

void dlgMainNotes::on_textEdit_redoAvailable(bool b) {
  if (b)
    ui->btnRedo->setEnabled(true);
  else
    ui->btnRedo->setEnabled(false);
}

void dlgMainNotes::on_textEdit_undoAvailable(bool b) {
  if (b)
    ui->btnUndo->setEnabled(true);
  else
    ui->btnUndo->setEnabled(false);
}

void dlgMainNotes::on_btnPic_clicked() {
  pAndroidKeyboard->hide();

  QString fileName;
  fileName = QFileDialog::getOpenFileName(this, tr("Knot"), "",
                                          tr("Picture Files (*.*)"));

  if (QFileInfo(fileName).exists()) {
    QStringList list = fileName.split(".");
    QDir dir;
    dir.mkpath(iniDir + "memo/images/");

    int y, m, d, hh, mm, s;
    y = QDate::currentDate().year();
    m = QDate::currentDate().month();
    d = QDate::currentDate().day();
    hh = QTime::currentTime().hour();
    mm = QTime::currentTime().minute();
    s = QTime::currentTime().second();
    QString newname = QString::number(y) + QString::number(m) +
                      QString::number(d) + "_" + QString::number(hh) +
                      QString::number(mm) + QString::number(s);

    QString strTar = iniDir + "memo/images/" + newname +
                     ".png";  // + list.at(list.count() - 1);
    if (QFile(strTar).exists()) QFile(strTar).remove();

    QImage img(fileName);
    double w, h;
    int new_w, new_h;
    w = img.width();
    h = img.height();
    double r = (double)w / h;
    if (w > ui->textEdit->width() - 26) {
      new_w = ui->textEdit->width() - 26;
      new_h = new_w / r;
      QPixmap pix;
      pix = QPixmap::fromImage(img);
      pix = pix.scaled(new_w, new_h, Qt::KeepAspectRatio,
                       Qt::SmoothTransformation);
      pix.save(strTar);
    }

    /*QTextDocumentFragment fragment;
    fragment = QTextDocumentFragment::fromHtml("<img src=" + strTar + ">");
    ui->textEdit->textCursor().insertFragment(fragment);*/

    QUrl Uri(QString("file://%1").arg(strTar));
    QImage image = QImageReader(strTar).read();

    QTextDocument* textDocument = ui->textEdit->document();
    textDocument->addResource(QTextDocument::ImageResource, Uri,
                              QVariant(image));
    QTextCursor cursor = ui->textEdit->textCursor();
    QTextImageFormat imageFormat;
    imageFormat.setWidth(image.width());
    imageFormat.setHeight(image.height());
    imageFormat.setName(Uri.toString());
    cursor.insertImage(imageFormat);

    QMessageBox box;
    box.setText(strTar);
    box.exec();
    qDebug() << "pic=" << strTar;
  }
}

QStringList dlgMainNotes::getImgFileFromHtml(QString htmlfile) {
  QStringList list;
  QString strHtml = mw_one->loadText(htmlfile);
  strHtml = strHtml.replace("><", ">\n<");
  QTextEdit* edit = new QTextEdit;
  edit->setPlainText(strHtml);
  for (int i = 0; i < edit->document()->lineCount(); i++) {
    QString str = mw_one->mydlgReader->getTextEditLineText(edit, i).trimmed();
    if (str.contains("<img src=")) {
      str = str.replace("<img src=", "");
      str = str.replace("/>", "");
      str = str.replace("\"", "");
      str = str.trimmed();
      qDebug() << str;
      list.append(str);
    }
  }
  return list;
}

void dlgMainNotes::zipMemo() {
  QDir::setCurrent(iniDir);
#ifdef Q_OS_MAC
  QProcess* pro = new QProcess;
  pro->execute("zip", QStringList() << "-r"
                                    << "memo.zip"
                                    << "memo");
  pro->waitForFinished();
#endif

#ifdef Q_OS_ANDROID
  QAndroidJniObject javaZipFile =
      QAndroidJniObject::fromString(iniDir + "memo.zip");
  QAndroidJniObject javaZipDir = QAndroidJniObject::fromString(iniDir + "memo");
  QAndroidJniObject m_activity = QAndroidJniObject::fromString("zip");
  m_activity.callStaticMethod<void>("com.x/MyActivity", "compressFileToZip",
                                    "(Ljava/lang/String;Ljava/lang/String;)V",
                                    javaZipDir.object<jstring>(),
                                    javaZipFile.object<jstring>());

#endif
}

void dlgMainNotes::unzipMemo() {
  mw_one->mydlgReader->deleteDirfile(iniDir + "memo");
  QDir::setCurrent(iniDir);
#ifdef Q_OS_MAC
  QProcess* pro = new QProcess;
  pro->execute("unzip", QStringList() << "-o" << iniDir + "memo.zip"
                                      << "-d" << iniDir);
  pro->waitForFinished();
#endif

#ifdef Q_OS_ANDROID
  QAndroidJniObject javaZipFile =
      QAndroidJniObject::fromString(iniDir + "memo.zip");
  QAndroidJniObject javaZipDir = QAndroidJniObject::fromString(iniDir);
  QAndroidJniObject m_activity = QAndroidJniObject::fromString("Unzip");
  m_activity.callStaticMethod<void>(
      "com.x/MyActivity", "Unzip", "(Ljava/lang/String;Ljava/lang/String;)V",
      javaZipFile.object<jstring>(), javaZipDir.object<jstring>());

#endif
}

void dlgMainNotes::loadMemoQML() {
  QString file = iniDir + "memo/memo.html";

  QSettings Reg(iniDir + "mainnotes.ini", QSettings::IniFormat);
  QString strIniDir;
  strIniDir = Reg.value("/MainNotes/CurrentOSIniDir").toString();
  QString str = mw_one->loadText(file);
  if (strIniDir != "") {
    str.replace(strIniDir, iniDir);

    QTextEdit* edit = new QTextEdit;
    edit->setPlainText(str);
    mw_one->TextEditToFile(edit, file);
  }

  mw_one->ui->quickWidgetMemo->setSource(
      QUrl(QStringLiteral("qrc:/src/memo.qml")));
  QQuickItem* root = mw_one->ui->quickWidgetMemo->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "loadHtml", Q_ARG(QVariant, file));

  // mw_one->ui->quickWidgetMemo->rootContext()->setContextProperty(
  //     "strText", mw_one->loadText(file));

  QMetaObject::invokeMethod((QObject*)root, "setVPos",
                            Q_ARG(QVariant, sliderPos));
}

void dlgMainNotes::on_btnEditSource_clicked() {
  saveMainNotes();

  ui->textEdit->setPlainText(mw_one->loadText(iniDir + "/memo/memo.md"));
  ui->textEdit->verticalScrollBar()->setSliderPosition(sliderPos);
  ui->btnPic->setEnabled(false);
  ui->btnEditSource->setEnabled(false);
}
