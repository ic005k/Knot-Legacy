#include "src/dlgMainNotes.h"

#include "mainwindow.h"
#include "ui_dlgMainNotes.h"
#include "ui_mainwindow.h"

extern MainWindow* mw_one;
extern QString iniFile, iniDir, fontname;
extern bool isImport, isAndroid, isIOS;
extern int fontSize;
extern QRegularExpression regxNumber;

dlgMainNotes::dlgMainNotes(QWidget* parent)
    : QDialog(parent), ui(new Ui::dlgMainNotes) {
  ui->setupUi(this);

  mw_one->set_btnStyle(this);

  m_SetEditText = new dlgSetEditText(this);
  m_Left = new dlgLeft(this);
  m_Right = new dlgRight(this);

  this->installEventFilter(this);
  mw_one->ui->editSource->installEventFilter(this);
  mw_one->ui->editSource->viewport()->installEventFilter(this);
  this->setModal(true);

  connect(pAndroidKeyboard, &QInputMethod::visibleChanged, this,
          &dlgMainNotes::on_KVChanged);

  QScroller::grabGesture(mw_one->ui->editSource,
                         QScroller::LeftMouseButtonGesture);
  mw_one->ui->editSource->verticalScrollBar()->setStyleSheet(
      mw_one->vsbarStyleSmall);
  // ui->editSource->setTextInteractionFlags(Qt::NoTextInteraction);
  QPalette pt = palette();
  pt.setBrush(QPalette::Text, Qt::black);
  pt.setBrush(QPalette::Base, QColor(255, 255, 255));
  pt.setBrush(QPalette::Highlight, Qt::red);
  pt.setBrush(QPalette::HighlightedText, Qt::white);
  mw_one->ui->editSource->setPalette(pt);
  mw_one->ui->editSource->setStyleSheet("border:none");
  QFontMetrics fm(this->font());
  mw_one->ui->editSource->setCursorWidth(2);

  QFont f = this->font();
  f.setPointSize(fontSize - 1);
  mw_one->ui->lblInfo->setFont(f);
  mw_one->ui->frameFun->setFont(f);

  mw_one->setSCrollPro(mw_one->ui->editSource);

  connect(mw_one->ui->editSource->verticalScrollBar(),
          SIGNAL(valueChanged(int)), this, SLOT(editVSBarValueChanged()));

  QValidator* validator =
      new QRegularExpressionValidator(regxNumber, mw_one->ui->editRow);
  mw_one->ui->editRow->setValidator(validator);
  mw_one->ui->editRow->setPlaceholderText(tr("Row"));
  mw_one->ui->editCol->setValidator(validator);
  mw_one->ui->editCol->setPlaceholderText(tr("Column"));

  connect(mw_one->ui->editSource, &QTextEdit::cursorPositionChanged, this,
          &dlgMainNotes::highlightCurrentLine);

  highlightCurrentLine();

  QFont font;
  font.setPixelSize(fontSize);

  lastLine = 1;
  font.setLetterSpacing(QFont::AbsoluteSpacing, 2);  //字间距
  mw_one->ui->editSource->setFont(font);
  mw_one->ui->editSource->setAcceptRichText(false);

  connect(mw_one->ui->editSource, &QTextEdit::textChanged, this,
          &dlgMainNotes::onTextChange);

  timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(showFunPanel()));

  bCursorVisible = true;
  timerCur = new QTimer(this);
  connect(this, SIGNAL(sendUpdate()), this, SLOT(update()));
  connect(timerCur, SIGNAL(timeout()), this, SLOT(timerSlot()));

  int a = 500;
  int b = 50;
  mw_one->ui->btnLeft->setAutoRepeat(true);
  mw_one->ui->btnLeft->setAutoRepeatDelay(a);
  mw_one->ui->btnLeft->setAutoRepeatInterval(b);

  mw_one->ui->btnRight->setAutoRepeat(true);
  mw_one->ui->btnRight->setAutoRepeatDelay(a);
  mw_one->ui->btnRight->setAutoRepeatInterval(b);

  mw_one->ui->editSource->setFocus();
}

void dlgMainNotes::init() {
  mw_one->ui->frameNoteEdit->setGeometry(mw_one->geometry().x(),
                                         mw_one->geometry().y(),
                                         mw_one->width(), mw_one->height());
}

void dlgMainNotes::wheelEvent(QWheelEvent* e) { Q_UNUSED(e); }

dlgMainNotes::~dlgMainNotes() { delete ui; }

void dlgMainNotes::keyReleaseEvent(QKeyEvent* event) { event->accept(); }

void dlgMainNotes::editVSBarValueChanged() {}

void dlgMainNotes::resizeEvent(QResizeEvent* event) {
  Q_UNUSED(event);

  /*if (isShow) {
    if (this->height() != mw_one->mainHeight) {
      newHeight = this->height();
      androidKeyH = mw_one->mainHeight - newHeight;

      QSettings Reg(iniDir + "android.ini", QSettings::IniFormat);
      Reg.setValue("KeyHeight", androidKeyH);
      Reg.setValue("newHeight", newHeight);
    }

    if (pAndroidKeyboard->isVisible()) {
      this->setGeometry(mw_one->geometry().x(), mw_one->geometry().y(),
                        mw_one->width(), newHeight);
    }

    if (this->height() == newHeight) {
      int p = ui->editSource->textCursor().position();
      QTextCursor tmpCursor = ui->editSource->textCursor();
      tmpCursor.setPosition(p);
      ui->editSource->setTextCursor(tmpCursor);
    }

    qDebug() << "newHeight=" << newHeight << "notes height=" << this->height()
             << "main height=" << mw_one->mainHeight
             << "mw_one h=" << mw_one->height() << mw_one->geometry().y();
    qDebug() << "memo h====" << mw_one->ui->frameMemo->height()
             << mw_one->ui->frameMemo->y();
  }*/
}

void dlgMainNotes::on_btnBack_clicked() {
  if (!m_SetEditText->isHidden()) {
    m_SetEditText->close();
  }

  saveMainNotes();
  saveQMLVPos();
  loadMemoQML();
  setVPos();
  mw_one->ui->frameNoteEdit->hide();
  mw_one->ui->frameMemo->show();
}

void dlgMainNotes::MD2Html(QString mdFile) {
  QString htmlFileName = iniDir + "memo/memo.html";
  QFile memofile1(htmlFileName);
  if (memofile1.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
    QTextStream stream(&memofile1);
    QTextEdit* edit = new QTextEdit();
    edit->setMarkdown(mw_one->loadText(mdFile));
    stream << edit->toHtml().toUtf8();
    memofile1.close();
  }
}

void dlgMainNotes::saveMainNotes() {
  QSettings Reg(iniDir + "mainnotes.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif

  mw_one->TextEditToFile(mw_one->ui->editSource,
                         mw_one->m_NotesList->currentMDFile);
  MD2Html(mw_one->m_NotesList->currentMDFile);

  QString path = iniDir + "memo/";
  QDir dir(path);
  if (!dir.exists()) dir.mkdir(path);

  QString strTag = mw_one->m_NotesList->currentMDFile;
  strTag.replace(iniDir, "");
  Reg.setValue("/MainNotes/CurrentOSIniDir", iniDir);
  Reg.setValue("/MainNotes/editVPos" + strTag,
               mw_one->ui->editSource->verticalScrollBar()->sliderPosition());
  Reg.setValue("/MainNotes/editCPos" + strTag,
               mw_one->ui->editSource->textCursor().position());
}

void dlgMainNotes::init_MainNotes() { loadMemoQML(); }

void dlgMainNotes::getEditPanel(QTextEdit* textEdit, QEvent* evn) {
  QMouseEvent* event = static_cast<QMouseEvent*>(evn);
  byTextEdit = textEdit;

  if (event->type() == QEvent::MouseButtonPress) {
    if (event->button() == Qt::LeftButton) {
      isMousePress = true;
      iMouseMove = false;
      m_SetEditText->close();

      int a = 30;
      if (event->globalY() - a - m_SetEditText->height() >= 0)
        y1 = event->globalY() - a - m_SetEditText->height();
      else
        y1 = event->globalY() + a;

      m_SetEditText->setFixedWidth(mw_one->width() - 20);

      textEdit->cursor().setPos(event->globalPos());

      if (m_SetEditText->isHidden()) {
        if (isAndroid) {
          if (pAndroidKeyboard->isVisible()) {
            timer->start(1300);
          }
        } else
          timer->start(1300);
      }
    }
  }

  if (event->type() == QEvent::MouseButtonRelease) {
    isMouseRelease = true;
    isMousePress = false;
    iMouseMove = false;

    if (m_SetEditText->ui->lineEdit->text() != "") {
      if (isFunShow) {
        isFunShow = false;

        m_SetEditText->init(y1);
        textEdit->setFocus();

        QTextCursor cursor = textEdit->textCursor();
        cursor.setPosition(start);
        cursor.setPosition(end, QTextCursor::KeepAnchor);
        textEdit->setTextCursor(cursor);
      }
    }
  }

  if (event->type() == QEvent::MouseMove) {
    iMouseMove = true;
    if (isMousePress) {
      QString str = textEdit->textCursor().selectedText().trimmed();
      m_SetEditText->ui->lineEdit->setText(str);
      if (str != "") {
        int y1;
        int a = 30;
        if (event->globalY() - a - m_SetEditText->height() >= 0)
          y1 = event->globalY() - a - m_SetEditText->height();
        else
          y1 = event->globalY() + a;

        m_SetEditText->init(y1);
      }
    }
  }
}

bool dlgMainNotes::eventFilter(QObject* obj, QEvent* evn) {
  if (obj == mw_one->ui->editSource->viewport()) {
    getEditPanel(mw_one->ui->editSource, evn);
  }

  if (evn->type() == QEvent::KeyPress) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      if (!m_SetEditText->isHidden()) {
        m_SetEditText->close();

      } else if (pAndroidKeyboard->isVisible()) {
        pAndroidKeyboard->hide();
        setGeometry(mw_one->geometry().x(), mw_one->geometry().y(), width(),
                    mw_one->mainHeight);
      } else
        on_btnBack_clicked();
      return true;
    }
  }

  return QWidget::eventFilter(obj, evn);
}

void dlgMainNotes::on_KVChanged() {
  /*QSettings Reg(iniDir + "android.ini", QSettings::IniFormat);
  if (!pAndroidKeyboard->isVisible()) {

    this->setGeometry(mw_one->geometry().x(), mw_one->geometry().y(),
                      mw_one->width(), mw_one->mainHeight);

  } else {
    int newh = Reg.value("newHeight").toInt();

    this->setGeometry(mw_one->geometry().x(), mw_one->geometry().y(),
                      mw_one->width(), newh);

    if (!m_SetEditText->isHidden()) {
      m_SetEditText->setGeometry(m_SetEditText->geometry().x(), 10,
                                 m_SetEditText->width(),
                                 m_SetEditText->height());
    }
  }*/
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

void dlgMainNotes::on_btnUndo_clicked() { mw_one->ui->editSource->undo(); }

void dlgMainNotes::on_btnRedo_clicked() { mw_one->ui->editSource->redo(); }

QString dlgMainNotes::getDateTimeStr() {
  int y, m, d, hh, mm, s;
  y = QDate::currentDate().year();
  m = QDate::currentDate().month();
  d = QDate::currentDate().day();
  hh = QTime::currentTime().hour();
  mm = QTime::currentTime().minute();
  s = QTime::currentTime().second();

  QString s_m, s_d, s_hh, s_mm, s_s;
  s_m = QString::number(m);
  if (s_m.length() == 1) s_m = "0" + s_m;

  s_d = QString::number(d);
  if (s_d.length() == 1) s_d = "0" + s_d;

  s_hh = QString::number(hh);
  if (s_hh.length() == 1) s_hh = "0" + s_hh;

  s_mm = QString::number(mm);
  if (s_mm.length() == 1) s_mm = "0" + s_mm;

  s_s = QString::number(s);
  if (s_s.length() == 1) s_s = "0" + s_s;

  QString newname = QString::number(y) + s_m + s_d + "_" + s_hh + s_mm + s_s;
  return newname;
}

void dlgMainNotes::on_btnPic_clicked() {
  pAndroidKeyboard->hide();

  QString fileName;
  fileName = QFileDialog::getOpenFileName(this, tr("Knot"), "",
                                          tr("Picture Files (*.*)"));

  if (QFileInfo(fileName).exists()) {
    QDir dir;
    dir.mkpath(iniDir + "memo/images/");

    QString strTar = iniDir + "memo/images/" + getDateTimeStr() +
                     ".png";  // + list.at(list.count() - 1);
    if (QFile(strTar).exists()) QFile(strTar).remove();

    QImage img(fileName);
    double w, h;
    int new_w, new_h;
    w = img.width();
    h = img.height();
    int w0 = mw_one->ui->editSource->width();
    double r = (double)w / h;
    if (w > w0 - 26) {
      new_w = w0 - 26;
      new_h = new_w / r;

    } else {
      new_w = w;
      new_h = h;
    }

    QPixmap pix;
    pix = QPixmap::fromImage(img);
    pix =
        pix.scaled(new_w, new_h, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    pix.save(strTar);

    mw_one->ui->editSource->insertPlainText("![image](file://" + strTar +
                                            ")\n");

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
#ifdef Q_OS_MACOS
  QProcess* pro = new QProcess;
  pro->execute("zip", QStringList() << "-r"
                                    << "memo.zip"
                                    << "memo");
  pro->waitForFinished();
#endif

#ifdef Q_OS_ANDROID

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  QAndroidJniObject javaZipFile =
      QAndroidJniObject::fromString(iniDir + "memo.zip");
  QAndroidJniObject javaZipDir = QAndroidJniObject::fromString(iniDir + "memo");
  QAndroidJniObject m_activity = QAndroidJniObject::fromString("zip");
  m_activity.callStaticMethod<void>("com.x/MyActivity", "compressFileToZip",
                                    "(Ljava/lang/String;Ljava/lang/String;)V",
                                    javaZipDir.object<jstring>(),
                                    javaZipFile.object<jstring>());
#else
  QJniObject javaZipFile = QJniObject::fromString(iniDir + "memo.zip");
  QJniObject javaZipDir = QJniObject::fromString(iniDir + "memo");
  QJniObject m_activity = QJniObject::fromString("zip");
  m_activity.callStaticMethod<void>("com.x/MyActivity", "compressFileToZip",
                                    "(Ljava/lang/String;Ljava/lang/String;)V",
                                    javaZipDir.object<jstring>(),
                                    javaZipFile.object<jstring>());
#endif

#endif
}

void dlgMainNotes::unzipMemo() {
  mw_one->mydlgReader->deleteDirfile(iniDir + "memo");
  QDir::setCurrent(iniDir);
#ifdef Q_OS_MACOS
  QProcess* pro = new QProcess;
  pro->execute("unzip", QStringList() << "-o" << iniDir + "memo.zip"
                                      << "-d" << iniDir);
  pro->waitForFinished();
#endif

#ifdef Q_OS_WIN
  QString strZip, strExec, strUnzip, tagDir;
  tagDir = iniDir;
  strZip = iniDir + "memo.zip";
  QTextEdit* txtEdit = new QTextEdit();
  strUnzip = iniDir + "unzip.exe";
  strUnzip = "\"" + strUnzip + "\"";
  strZip = "\"" + strZip + "\"";
  strExec = iniDir;
  strExec = "\"" + strExec + "\"";
  QString strCommand1;
  QString strx = "\"" + tagDir + "\"";
  strCommand1 = strUnzip + " -o " + strZip + " -d " + strx;
  txtEdit->append(strCommand1);
  QString fileName = iniDir + "un.bat";
  mw_one->TextEditToFile(txtEdit, fileName);

  QProcess::execute("cmd.exe", QStringList() << "/c" << fileName);

#endif

#ifdef Q_OS_ANDROID

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  QAndroidJniObject javaZipFile =
      QAndroidJniObject::fromString(iniDir + "memo.zip");
  QAndroidJniObject javaZipDir = QAndroidJniObject::fromString(iniDir);
  QAndroidJniObject m_activity = QAndroidJniObject::fromString("Unzip");
  m_activity.callStaticMethod<void>(
      "com.x/MyActivity", "Unzip", "(Ljava/lang/String;Ljava/lang/String;)V",
      javaZipFile.object<jstring>(), javaZipDir.object<jstring>());
#else
  QJniObject javaZipFile = QJniObject::fromString(iniDir + "memo.zip");
  QJniObject javaZipDir = QJniObject::fromString(iniDir);
  QJniObject m_activity = QJniObject::fromString("Unzip");
  m_activity.callStaticMethod<void>(
      "com.x/MyActivity", "Unzip", "(Ljava/lang/String;Ljava/lang/String;)V",
      javaZipFile.object<jstring>(), javaZipDir.object<jstring>());
#endif

#endif
}

void dlgMainNotes::loadMemoQML() {
  QSettings Reg(iniDir + "mainnotes.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif
  QString strIniDir;
  strIniDir = Reg.value("/MainNotes/CurrentOSIniDir").toString();

  QString htmlFileName = iniDir + "memo/memo.html";
  QTextEdit* edit = new QTextEdit;
  QPlainTextEdit* edit1 = new QPlainTextEdit;
  QString strhtml = mw_one->loadText(htmlFileName);
  strhtml = strhtml.replace("><", ">\n<");
  edit->setPlainText(strhtml);
  QString str;
  for (int i = 0; i < edit->document()->lineCount(); i++) {
    str = mw_one->mydlgReader->getTextEditLineText(edit, i);
    str = str.trimmed();
    if (str.mid(0, 4) == "<img") {
      QString str1 = str;
      QStringList list = str1.split(" ");
      QString strSrc;
      for (int k = 0; k < list.count(); k++) {
        QString s1 = list.at(k);
        if (s1.contains("src=")) {
          strSrc = s1;
          break;
        }
      }
      strSrc = strSrc.replace("src=", "");
      strSrc = strSrc.replace("/>", "");
      str = "<a href=" + strSrc + ">" + str + "</a>";
      // qDebug() << "strSrc=" << strSrc << str;

      str = str.replace("width=", "width1=");
      str = str.replace("height=", "height1=");
    }

    edit1->appendPlainText(str);
  }

  QString str1 = edit1->toPlainText();
  if (strIniDir != "") {
    str1.replace(strIniDir, iniDir);
  }
  htmlBuffer = str1;

  QQuickItem* root = mw_one->ui->quickWidgetMemo->rootObject();

  // QMetaObject::invokeMethod((QObject*)root, "loadHtml", Q_ARG(QVariant,
  // htmlFileName));
  QMetaObject::invokeMethod((QObject*)root, "loadHtmlBuffer",
                            Q_ARG(QVariant, htmlBuffer));

  getVHeight();
}

void dlgMainNotes::saveQMLVPos() {
  QSettings Reg(iniDir + "mainnotes.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif
  if (QFile(mw_one->m_NotesList->currentMDFile).exists()) {
    QString strTag = mw_one->m_NotesList->currentMDFile;
    strTag.replace(iniDir, "");
    Reg.setValue("/MainNotes/SlidePos" + strTag, sliderPos);
  }
}

void dlgMainNotes::setVPos() {
  QSettings Reg(iniDir + "mainnotes.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif
  QString a = mw_one->m_NotesList->currentMDFile;

  sliderPos = Reg.value("/MainNotes/SlidePos" + a.replace(iniDir, "")).toReal();
  if (sliderPos < 0) sliderPos = 0;
  QQuickItem* root = mw_one->ui->quickWidgetMemo->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "setVPos",
                            Q_ARG(QVariant, sliderPos));
}

qreal dlgMainNotes::getVHeight() {
  QQuickItem* root = mw_one->ui->quickWidgetMemo->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "getVHeight");
  return textHeight;
}

void dlgMainNotes::on_btnInsertTable_clicked() {
  int row = mw_one->ui->editRow->text().trimmed().toInt();
  int col = mw_one->ui->editCol->text().trimmed().toInt();

  if (row == 0 || col == 0) return;

  QString strTitle = tr("Title");
  QString strCol = "|" + strTitle + "1|";
  QString strHead = "|------|";
  QString strRow = "|      |";

  for (int i = 0; i < col - 1; i++) {
    strCol = strCol + strTitle + QString::number(i + 2) + "|";
    strHead = strHead + "------|";
    strRow = strRow + "      |";
  }

  if (!mw_one->ui->editSource->isHidden()) {
    mw_one->ui->editSource->insertPlainText(strCol + "\n" + strHead + "\n");
    for (int j = 0; j < row; j++) {
      mw_one->ui->editSource->insertPlainText(strRow + "\n");
    }
  }
}

void dlgMainNotes::on_btnSeparator_clicked() {
  mw_one->ui->editSource->insertPlainText("-");
}

void dlgMainNotes::on_btnWells_clicked() {
  mw_one->ui->editSource->insertPlainText("#");
}

void dlgMainNotes::on_btnVLine_clicked() {
  mw_one->ui->editSource->insertPlainText("|");
}

void dlgMainNotes::on_btnAsterisk_clicked() {
  mw_one->ui->editSource->insertPlainText("*");
}

void dlgMainNotes::on_btnS1_clicked() {
  QString str = mw_one->ui->editSource->textCursor().selectedText();
  if (str == "") str = tr("Bold Italic");
  mw_one->ui->editSource->insertPlainText("_**" + str + "**_");
}

void dlgMainNotes::on_btnS2_clicked() {
  QString str = mw_one->ui->editSource->textCursor().selectedText();
  if (str == "") str = tr("Italic");
  mw_one->ui->editSource->insertPlainText("_" + str + "_");
}

void dlgMainNotes::on_btnS3_clicked() {
  QString str = mw_one->ui->editSource->textCursor().selectedText();
  if (str == "") str = tr("Underline");
  mw_one->ui->editSource->insertPlainText("<u>" + str + "</u>");
}

void dlgMainNotes::on_btnS4_clicked() {
  QString str = mw_one->ui->editSource->textCursor().selectedText();
  if (str == "") str = tr("Strickout");
  mw_one->ui->editSource->insertPlainText("~~" + str + "~~");
}

void dlgMainNotes::on_btnColor_clicked() {
  QString str = mw_one->ui->editSource->textCursor().selectedText();
  if (str == "") str = tr("Red");
  mw_one->ui->editSource->insertPlainText("<font color=#FF0000 >" + str +
                                          "</font>");
}

void dlgMainNotes::on_btnS5_clicked() {
  QString str = mw_one->ui->editSource->textCursor().selectedText();
  if (str == "") str = tr("Bold");
  mw_one->ui->editSource->insertPlainText("**" + str + "**");
}

void dlgMainNotes::on_btnLink_clicked() {
  mw_one->ui->editSource->insertPlainText(
      "[Knot](https://github.com/ic005k/Knot)");
}

void dlgMainNotes::on_btnS6_clicked() {
  mw_one->ui->editSource->insertPlainText("~");
}

void dlgMainNotes::on_btnS7_clicked() {
  mw_one->ui->editSource->insertPlainText("[]");
  mw_one->ui->btnLeft->click();
}

void dlgMainNotes::on_btnS8_clicked() {
  mw_one->ui->editSource->insertPlainText("()");
  mw_one->ui->btnLeft->click();
}

void dlgMainNotes::on_btnS9_clicked() {
  mw_one->ui->editSource->insertPlainText("{}");
  mw_one->ui->btnLeft->click();
}

void dlgMainNotes::on_btnS10_clicked() {
  mw_one->ui->editSource->insertPlainText("_");
}

void dlgMainNotes::highlightCurrentLine() {
  QList<QTextEdit::ExtraSelection> extraSelections;

  QTextEdit::ExtraSelection selection;

  QColor lineColor = QColor(Qt::yellow).lighter(160);

  selection.format.setBackground(lineColor);
  selection.format.setProperty(QTextFormat::FullWidthSelection, true);
  selection.cursor = mw_one->ui->editSource->textCursor();
  selection.cursor.clearSelection();
  extraSelections.append(selection);

  mw_one->ui->editSource->setExtraSelections(extraSelections);

  QString str1, str2, str3, str4;

  //当前光标
  QTextCursor tc = mw_one->ui->editSource->textCursor();
  // QTextLayout* lay = tc.block().layout();
  //当前光标在本BLOCK内的相对位置
  int iCurPos = tc.position() - tc.block().position();
  //光标所在行号
  // int iCurrentLine = lay->lineForTextPosition(iCurPos).lineNumber() +
  //                   tc.block().firstLineNumber();
  int iLineCount = mw_one->ui->editSource->document()->lineCount();
  //或者  获取光标所在行的行号
  int iRowNum = tc.blockNumber() + 1;

  str1 = QString::number(iLineCount);
  str2 = QString::number(mw_one->ui->editSource->textCursor().position());
  str3 = QString::number(iCurPos);
  str4 = QString::number(iRowNum);
  mw_one->ui->lblInfo->setText(" " + str4 + " , " + str3);
}

void dlgMainNotes::onTextChange() {}

void dlgMainNotes::on_btnPaste_clicked() { mw_one->ui->editSource->paste(); }

void dlgMainNotes::showFunPanel() {
  timer->stop();
  if (isMousePress) {
    isFunShow = true;

    QTextCursor cursor;
    start = byTextEdit->textCursor().position();
    end = start + 2;
    cursor = byTextEdit->textCursor();
    cursor.setPosition(start);
    cursor.setPosition(end, QTextCursor::KeepAnchor);
    byTextEdit->setTextCursor(cursor);

    m_SetEditText->ui->lineEdit->setText(cursor.selectedText());

    m_SetEditText->init(y1);
  }
}

void dlgMainNotes::selectText(int start, int end) {
  QTextCursor cursor;
  cursor = byTextEdit->textCursor();
  cursor.setPosition(start);
  cursor.setPosition(end, QTextCursor::KeepAnchor);
  byTextEdit->setTextCursor(cursor);
  m_SetEditText->ui->lineEdit->setText(cursor.selectedText());
}

void dlgMainNotes::paintEvent(QPaintEvent* event) {
  Q_UNUSED(event);
  return;

  if (mw_one->ui->editSource->hasFocus()) {
    if (bCursorVisible) {
      const QRect qRect = mw_one->ui->editSource->cursorRect(
          mw_one->ui->editSource->textCursor());
      QPainter qPainter(mw_one->ui->editSource->viewport());
      qPainter.fillRect(qRect, QColor(255, 0, 0, 255));
    } else {
      const QRect qRect = mw_one->ui->editSource->cursorRect(
          mw_one->ui->editSource->textCursor());
      QPainter qPainter(mw_one->ui->editSource->viewport());
      qPainter.fillRect(qRect, QColor(0, 255, 0, 255));
    }
  }
}

void dlgMainNotes::timerSlot() {
  if (bCursorVisible) {
    bCursorVisible = false;
  } else {
    bCursorVisible = true;
  }

  emit sendUpdate();
}

void dlgMainNotes::on_btnLeft_clicked() {
  mw_one->ui->editSource->moveCursor(QTextCursor::PreviousCharacter,
                                     QTextCursor::MoveAnchor);
}

void dlgMainNotes::on_btnRight_clicked() {
  mw_one->ui->editSource->moveCursor(QTextCursor::NextCharacter,
                                     QTextCursor::MoveAnchor);
}

bool dlgMainNotes::androidCopyFile(QString src, QString des) {
  bool result = false;

#ifdef Q_OS_ANDROID

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  QAndroidJniObject srcObj = QAndroidJniObject::fromString(src);
  QAndroidJniObject desObj = QAndroidJniObject::fromString(des);
  QAndroidJniObject m_activity = QAndroidJniObject::fromString("copyFile");
  result = m_activity.callStaticMethod<int>(
      "com.x/MyActivity", "copyFile", "(Ljava/lang/String;Ljava/lang/String;)I",
      srcObj.object<jstring>(), desObj.object<jstring>());
#else
  QJniObject srcObj = QJniObject::fromString(src);
  QJniObject desObj = QJniObject::fromString(des);
  QJniObject m_activity = QJniObject::fromString("copyFile");
  result = m_activity.callStaticMethod<int>(
      "com.x/MyActivity", "copyFile", "(Ljava/lang/String;Ljava/lang/String;)I",
      srcObj.object<jstring>(), desObj.object<jstring>());
#endif

#endif
  qDebug() << "copyFile " << src << des;
  return result;
}
