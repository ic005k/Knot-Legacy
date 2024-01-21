#include "src/Notes.h"

#include "MainWindow.h"
#include "src/MarkdownHighlighter.h"
#include "ui_MainWindow.h"
#include "ui_Notes.h"

extern MainWindow *mw_one;
extern Method *m_Method;
extern QString iniFile, iniDir, privateDir, currentMDFile;
extern bool isAndroid, isIOS, isDark;
extern int fontSize;
extern QRegularExpression regxNumber;

Notes::Notes(QWidget *parent) : QDialog(parent), ui(new Ui::Notes) {
  ui->setupUi(this);

  QFont font0 = m_Method->getNewFont(16);
  QObjectList btnList =
      mw_one->getAllToolButton(mw_one->getAllUIControls(ui->f_ToolBar));
  for (int i = 0; i < btnList.count(); i++) {
    QToolButton *btn = (QToolButton *)btnList.at(i);
    btn->setFont(font0);

#ifdef Q_OS_ANDROID
#else
    btn->setFixedHeight(30);

    ui->editCol->setFixedHeight(30);
    ui->editRow->setFixedHeight(30);
    ui->editFind->setFixedHeight(30);
#endif
  }

  ui->editFind->setFont(font0);
  ui->editCol->setFont(font0);
  ui->editRow->setFont(font0);
  ui->lblCol->setFont(font0);
  ui->lblRow->setFont(font0);

  QFont font1 = m_Method->getNewFont(19);
  QObjectList btnList1 =
      mw_one->getAllToolButton(mw_one->getAllUIControls(ui->f_Panel));
  for (int i = 0; i < btnList1.count(); i++) {
    QToolButton *btn = (QToolButton *)btnList1.at(i);
    btn->setFont(font1);
  }

  mw_one->set_btnStyle(this);

  ui->lblInfo->hide();
  ui->btnFind->hide();
  ui->lblCount->hide();
  ui->f_ToolBar->hide();
  ui->btnGetShare->hide();

#ifdef Q_OS_ANDROID
#else
  ui->btnHideKey->hide();
#endif

  m_TextSelector = new TextSelector(this);

  QString path = iniDir + "memo/";
  QDir dir(path);
  if (!dir.exists()) dir.mkdir(path);

  connect(pAndroidKeyboard, &QInputMethod::visibleChanged, this,
          &Notes::on_KVChanged);

  this->installEventFilter(this);
  ui->editSource->installEventFilter(this);
  ui->editSource->viewport()->installEventFilter(this);
  this->setModal(true);
  this->layout()->setContentsMargins(5, 5, 5, 5);

  QScroller::grabGesture(ui->editSource, QScroller::LeftMouseButtonGesture);
  m_Method->setSCrollPro(ui->editSource);
  ui->editSource->verticalScrollBar()->setStyleSheet(m_Method->vsbarStyleSmall);

  ui->editSource->setContentsMargins(15, 15, 15, 15);
  ui->editSource->setStyleSheet("border:none");
  ui->editSource->setCursorWidth(2);

  QFont f = this->font();
  f.setPointSize(fontSize - 1);
  ui->lblInfo->setFont(f);
  ui->f_ToolBar->setFont(f);

  connect(ui->editSource->verticalScrollBar(), SIGNAL(valueChanged(int)), this,
          SLOT(editVSBarValueChanged()));

  QValidator *validator =
      new QRegularExpressionValidator(regxNumber, ui->editRow);
  ui->editRow->setValidator(validator);
  ui->editRow->setPlaceholderText(tr("Row"));
  ui->editCol->setValidator(validator);
  ui->editCol->setPlaceholderText(tr("Column"));

  connect(ui->editSource, &QTextEdit::cursorPositionChanged, this,
          &Notes::highlightCurrentLine);

  highlightCurrentLine();
  QFont font = this->font();
  font.setLetterSpacing(QFont::AbsoluteSpacing, 2);  // 字间距
  ui->editSource->setFont(font);
  ui->editSource->setAcceptRichText(false);

  connect(ui->editSource, &QTextEdit::textChanged, this, &Notes::onTextChange);

  timerEditPanel = new QTimer(this);
  connect(timerEditPanel, SIGNAL(timeout()), this, SLOT(on_showEditPanel()));

  timerEditNote = new QTimer(this);
  connect(timerEditNote, SIGNAL(timeout()), this, SLOT(on_editNote()));

  bCursorVisible = true;
  timerCur = new QTimer(this);
  connect(this, SIGNAL(sendUpdate()), this, SLOT(update()));
  connect(timerCur, SIGNAL(timeout()), this, SLOT(timerSlot()));

  int a = 500;
  int b = 50;
  ui->btnLeft->setAutoRepeat(true);
  ui->btnLeft->setAutoRepeatDelay(a);
  ui->btnLeft->setAutoRepeatInterval(b);

  ui->btnRight->setAutoRepeat(true);
  ui->btnRight->setAutoRepeatDelay(a);
  ui->btnRight->setAutoRepeatInterval(b);

  ui->editSource->setFocus();
}

void Notes::init() {
  this->setGeometry(mw_one->geometry().x(), mw_one->geometry().y(),
                    mw_one->width(), mw_one->height());
}

void Notes::wheelEvent(QWheelEvent *e) { Q_UNUSED(e); }

Notes::~Notes() { delete ui; }

void Notes::keyReleaseEvent(QKeyEvent *event) { event->accept(); }

void Notes::editVSBarValueChanged() {}

void Notes::resizeEvent(QResizeEvent *event) {
  Q_UNUSED(event);

  if (isShow) {
    if (this->height() != mw_one->mainHeight) {
      newHeight = this->height();
      androidKeyH = mw_one->mainHeight - newHeight;

      QSettings Reg(privateDir + "android.ini", QSettings::IniFormat);
      Reg.setValue("KeyHeight", androidKeyH);
      Reg.setValue("newHeight", newHeight);
    }

    if (!ui->editSource->isHidden()) {
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
    }
  }

  qDebug() << pAndroidKeyboard->keyboardRectangle().height()
           << "this height=" << this->height();
  qDebug() << "newHeight=" << newHeight << "main height=" << mw_one->mainHeight;
}

void Notes::on_btnDone_clicked() {
  isDone = true;
  close();
}

void Notes::MD2Html(QString mdFile) {
  QString htmlFileName = privateDir + "memo.html";
  QFile memofile1(htmlFileName);
  if (memofile1.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
    QTextStream stream(&memofile1);
    QTextEdit *edit = new QTextEdit();
    QString strmd = mw_one->loadText(mdFile);
    edit->setPlainText(strmd);
    edit->document()->setMarkdown(strmd, QTextDocument::MarkdownDialectGitHub);
    stream << edit->toHtml().toUtf8();
    memofile1.close();
  }
}

void Notes::saveMainNotes() {
  if (!isNeedSave) return;

  mw_one->isSelf = true;

  mw_one->isNeedAutoBackup = true;
  mw_one->strLatestModify = tr("Modi Notes");

  saveQMLVPos();

  QSettings Reg(iniDir + "mainnotes.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif

  if (isTextChange) {
    mw_one->TextEditToFile(ui->editSource, currentMDFile);
    MD2Html(currentMDFile);

    qDebug() << "Save Note: " << currentMDFile;
  }

  QString strTag = currentMDFile;
  strTag.replace(iniDir, "");

  Reg.setValue("/MainNotes/editVPos" + strTag,
               ui->editSource->verticalScrollBar()->sliderPosition());
  Reg.setValue("/MainNotes/editCPos" + strTag,
               ui->editSource->textCursor().position());
  Reg.setValue("/MainNotes/toolBarVisible", ui->f_ToolBar->isVisible());

  isNeedSave = false;
  isTextChange = false;
}

void Notes::init_MainNotes() { loadMemoQML(); }

void Notes::getEditPanel(QTextEdit *textEdit, QEvent *evn) {
  QMouseEvent *event = static_cast<QMouseEvent *>(evn);
  byTextEdit = textEdit;

  QPalette pt = palette();
  pt.setBrush(QPalette::Text, Qt::black);
  pt.setBrush(QPalette::Base, QColor(255, 255, 255));
  pt.setBrush(QPalette::Highlight, QColor(90, 90, 255));
  pt.setBrush(QPalette::HighlightedText, Qt::white);
  textEdit->setPalette(pt);

  if (event->type() == QEvent::MouseButtonPress) {
    if (event->button() == Qt::LeftButton) {
      isMousePress = true;
      isMouseMove = false;

      if (!isAndroid) {
        m_TextSelector->on_btnClose_clicked();
      }

      px = event->globalX();
      py = event->globalY();

      int a = 100;
      int hy = py - a - m_TextSelector->height();
      if (hy >= 0)
        y1 = hy;
      else
        y1 = py + a;

#ifdef Q_OS_ANDROID
      m_TextSelector->setFixedWidth(mw_one->width() - 6);

      if (textEdit == ui->editSource) {
        y1 = 2;
        if (!ui->f_ToolBar->isHidden()) {
          m_TextSelector->setFixedHeight(ui->f_ToolBar->height());
        } else
          m_TextSelector->setFixedHeight(m_TextSelector->oriHeight);
      }

      if (textEdit == mw_one->ui->editTodo)
        y1 = mw_one->geometry().y() + mw_one->ui->editTodo->y() +
             mw_one->ui->editTodo->height() + 2;

      if (textEdit == mw_one->ui->editDetails)
        y1 = mw_one->geometry().y() + mw_one->ui->editDetails->y() +
             mw_one->ui->editDetails->height() + 2;
#else

#endif

      textEdit->cursor().setPos(event->globalPos());

      if (m_TextSelector->isHidden()) {
        if (isAndroid) {
          if (!pAndroidKeyboard->isVisible()) {
            pAndroidKeyboard->setVisible(true);
          }
          timerEditPanel->start(1000);
        }
      }
    }
  }

  if (event->type() == QEvent::MouseButtonRelease) {
    isMouseRelease = true;
    isMousePress = false;
    isMouseMove = false;

    if (m_TextSelector->ui->lineEdit->text() != "") {
      m_TextSelector->show();
      if (isFunShow) {
        isFunShow = false;

        m_TextSelector->init(y1);
        textEdit->setFocus();

        QTextCursor cursor = textEdit->textCursor();
        cursor.setPosition(start);
        cursor.setPosition(end, QTextCursor::KeepAnchor);
        textEdit->setTextCursor(cursor);
      }
    }
  }

  if (event->type() == QEvent::MouseMove) {
    isMouseMove = true;
    if (isMousePress) {
      textEdit->cursor().setPos(event->globalPos());

      mx = event->globalX();
      my = event->globalY();

      if (mx <= px) {
        m_TextSelector->on_btnClose_clicked();
        return;
      }

      QString str = textEdit->textCursor().selectedText().trimmed();

      end = textEdit->textCursor().position();
      start = end - textEdit->textCursor().selectedText().length();

      QTextCursor cursor;
      cursor = byTextEdit->textCursor();
      cursor.setPosition(start);
      cursor.setPosition(end, QTextCursor::KeepAnchor);
      byTextEdit->setTextCursor(cursor);

      m_TextSelector->ui->lineEdit->setText(str);
      if (str != "") {
        m_TextSelector->init(y1);
      }
    }
  }
}

bool Notes::eventFilter(QObject *obj, QEvent *evn) {
  if (obj == ui->editSource->viewport()) {
    getEditPanel(ui->editSource, evn);
  }

  QKeyEvent *keyEvent = static_cast<QKeyEvent *>(evn);
  if (evn->type() == QEvent::KeyRelease) {
    if (keyEvent->key() == Qt::Key_Back) {
      if (!m_TextSelector->isHidden()) {
        m_TextSelector->close();
        return true;
      }

      if (pAndroidKeyboard->isVisible()) {
        pAndroidKeyboard->hide();
        setGeometry(mw_one->geometry().x(), mw_one->geometry().y(), width(),
                    mw_one->mainHeight);
        return true;
      }

      close();

      return true;
    }
  }

  // Keyboard
  if (obj == ui->editSource) {
    if (evn->type() == QEvent::KeyPress) {
      if (keyEvent->key() != Qt::Key_Back) {
      }
    }
  }

  // Mouse
  if (obj == ui->editSource->viewport()) {
    if (evn->type() == QEvent::MouseButtonPress) {
    }
  }

#ifdef Q_OS_ANDROID
  if (obj == ui->editSource->viewport()) {
    if (evn->type() == QEvent::MouseButtonDblClick) {
      y1 = 2;
      isMousePress = true;
      on_showEditPanel();
    }
  }
#endif

  return QWidget::eventFilter(obj, evn);
}

void Notes::on_KVChanged() {
  if (!pAndroidKeyboard->isVisible()) {
    this->setGeometry(mw_one->geometry().x(), mw_one->geometry().y(),
                      mw_one->width(), mw_one->mainHeight);
  } else {
    if (newHeight > 0) {
      this->setGeometry(mw_one->geometry().x(), mw_one->geometry().y(),
                        mw_one->width(), newHeight);

      if (!m_TextSelector->isHidden()) {
        m_TextSelector->setGeometry(m_TextSelector->geometry().x(), 10,
                                    m_TextSelector->width(),
                                    m_TextSelector->height());
      }
    }
  }
}

/*
加密文件
将文件读到字符串中，将每个字符都减1，然后将字符串写到文件中
*/
void Notes::encode(QString filename) {
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
void Notes::decode(QString filename) {
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

void Notes::encryption(const QString &fileName) {
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

QString Notes::Deciphering(const QString &fileName) {
  QFile file(fileName);
  if (!file.open(QIODevice::ReadOnly)) {
    QMessageBox::warning(this, tr("Load Ds File"), file.errorString(),
                         QMessageBox::Yes);
  }

  return QByteArray::fromBase64(file.readAll());

  file.close();
}

void Notes::on_btnUndo_clicked() { ui->editSource->undo(); }

void Notes::on_btnRedo_clicked() { ui->editSource->redo(); }

QString Notes::getDateTimeStr() {
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

void Notes::on_btnPic_clicked() {
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
    int w0 = ui->editSource->width();
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

    ui->editSource->insertPlainText("![image](file://" + strTar + ")\n");

    QMessageBox box;
    box.setText(strTar);
    box.exec();
    qDebug() << "pic=" << strTar;
  }
}

QStringList Notes::getImgFileFromHtml(QString htmlfile) {
  QStringList list;
  QString strHtml = mw_one->loadText(htmlfile);
  strHtml = strHtml.replace("><", ">\n<");
  QTextEdit *edit = new QTextEdit;
  edit->setPlainText(strHtml);
  for (int i = 0; i < edit->document()->lineCount(); i++) {
    QString str = mw_one->m_Reader->getTextEditLineText(edit, i).trimmed();
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

void Notes::zipMemo() {
  QDir::setCurrent(iniDir);

#ifdef Q_OS_LINUX
  QProcess *pro = new QProcess;
  pro->execute("zip", QStringList() << "-r"
                                    << "memo.zip"
                                    << "memo");
  pro->waitForFinished();
#endif

#ifdef Q_OS_MACOS
  QProcess *pro = new QProcess;
  pro->execute("zip", QStringList() << "-r"
                                    << "memo.zip"
                                    << "memo");
  pro->waitForFinished();
#endif

#ifdef Q_OS_WIN

  QString strZip, strExec, strzip, tagDir;
  tagDir = "memo";
  strZip = iniDir + "memo.zip";
  QTextEdit *txtEdit = new QTextEdit();
  strzip = qApp->applicationDirPath() + "/zip.exe";
  strzip = "\"" + strzip + "\"";
  strZip = "\"" + strZip + "\"";
  strExec = iniDir;
  strExec = "\"" + strExec + "\"";
  QString strCommand1;
  QString strx = "\"" + tagDir + "\"";
  strCommand1 = strzip + " -r " + strZip + " " + strx;
  txtEdit->append(strCommand1);
  QString fileName = iniDir + "zip.bat";
  mw_one->TextEditToFile(txtEdit, fileName);

  QString exefile = iniDir + "zip.bat";
  QProcess *pro = new QProcess;
  pro->execute("cmd.exe", QStringList() << "/c" << exefile);
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

void Notes::unzip(QString zipfile) {
  mw_one->m_Reader->deleteDirfile(iniDir + "memo");
  QDir::setCurrent(iniDir);
#ifdef Q_OS_MACOS
  QProcess *pro = new QProcess;
  pro->execute("unzip", QStringList() << "-o" << zipfile << "-d" << iniDir);
  pro->waitForFinished();
#endif

#ifdef Q_OS_LINUX
  QProcess *pro = new QProcess;
  pro->execute("unzip", QStringList() << "-o" << zipfile << "-d" << iniDir);
  pro->waitForFinished();
#endif

#ifdef Q_OS_WIN
  QString strZip, strExec, strUnzip, tagDir;
  tagDir = iniDir;
  strZip = zipfile;
  QTextEdit *txtEdit = new QTextEdit();
  strUnzip = qApp->applicationDirPath() + "/unzip.exe";
  qDebug() << qApp->applicationDirPath() << ".....";
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
  QAndroidJniObject javaZipFile = QAndroidJniObject::fromString(zipfile);
  QAndroidJniObject javaZipDir = QAndroidJniObject::fromString(iniDir);
  QAndroidJniObject m_activity = QAndroidJniObject::fromString("Unzip");
  m_activity.callStaticMethod<void>(
      "com.x/MyActivity", "Unzip", "(Ljava/lang/String;Ljava/lang/String;)V",
      javaZipFile.object<jstring>(), javaZipDir.object<jstring>());
#else
  QJniObject javaZipFile = QJniObject::fromString(zipfile);
  QJniObject javaZipDir = QJniObject::fromString(iniDir);
  QJniObject m_activity = QJniObject::fromString("Unzip");
  m_activity.callStaticMethod<void>(
      "com.x/MyActivity", "Unzip", "(Ljava/lang/String;Ljava/lang/String;)V",
      javaZipFile.object<jstring>(), javaZipDir.object<jstring>());
#endif

#endif
}

void Notes::loadMemoQML() {
  QString htmlFileName = privateDir + "memo.html";
  QTextEdit *edit = new QTextEdit;
  QPlainTextEdit *edit1 = new QPlainTextEdit;
  QString strhtml = mw_one->loadText(htmlFileName);
  strhtml = strhtml.replace("><", ">\n<");
  edit->setPlainText(strhtml);
  QString str;
  for (int i = 0; i < edit->document()->lineCount(); i++) {
    str = mw_one->m_Reader->getTextEditLineText(edit, i);
    str = str.trimmed();
    if (str.mid(0, 4) == "<img" && str.contains("file://")) {
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

      QStringList list1 = strSrc.split("/memo/");
      strSrc = "\"file://" + iniDir + "memo/" + list1.at(1);

      QStringList list2 = str1.split("/memo/");
      str = "<img src=\"file://" + iniDir + "memo/" + list2.at(1);

      str = "<a href=" + strSrc + ">" + str + "</a>";

      str = str.replace("width=", "width1=");
      str = str.replace("height=", "height1=");
    }

    edit1->appendPlainText(str);
  }

  QQuickItem *root = mw_one->ui->qwNotes->rootObject();

  // mw_one->m_Reader->TextEditToFile(edit1, htmlFileName);
  // QMetaObject::invokeMethod((QObject*)root, "loadHtml",
  //                          Q_ARG(QVariant, htmlFileName));

  htmlBuffer = edit1->toPlainText();
  QMetaObject::invokeMethod((QObject *)root, "loadHtmlBuffer",
                            Q_ARG(QVariant, htmlBuffer));

  setVPos();
}

void Notes::saveQMLVPos() {
  QSettings Reg(privateDir + "notes.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif

  if (QFile(currentMDFile).exists()) {
    sliderPos = getVPos();
    Reg.setValue("/MainNotes/SlidePos" + currentMDFile, sliderPos);
  }
}

void Notes::setVPos() {
  QSettings Reg(privateDir + "notes.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif

  sliderPos = Reg.value("/MainNotes/SlidePos" + currentMDFile).toReal();

  QQuickItem *root = mw_one->ui->qwNotes->rootObject();
  QMetaObject::invokeMethod((QObject *)root, "setVPos",
                            Q_ARG(QVariant, sliderPos));
}

qreal Notes::getVPos() {
  QVariant itemCount;
  QQuickItem *root = mw_one->ui->qwNotes->rootObject();
  QMetaObject::invokeMethod((QObject *)root, "getVPos",
                            Q_RETURN_ARG(QVariant, itemCount));
  sliderPos = itemCount.toReal();

  return sliderPos;
}

qreal Notes::getVHeight() {
  QVariant h;
  QQuickItem *root = mw_one->ui->qwNotes->rootObject();
  QMetaObject::invokeMethod((QObject *)root, "getVHeight",
                            Q_RETURN_ARG(QVariant, h));
  textHeight = h.toReal();
  return textHeight;
}

void Notes::on_btnInsertTable_clicked() {
  int row = ui->editRow->text().trimmed().toInt();
  int col = ui->editCol->text().trimmed().toInt();

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

  if (!ui->editSource->isHidden()) {
    ui->editSource->insertPlainText(strCol + "\n" + strHead + "\n");
    for (int j = 0; j < row; j++) {
      ui->editSource->insertPlainText(strRow + "\n");
    }
  }
}

void Notes::on_editSource_redoAvailable(bool b) {
  if (b)
    ui->btnRedo->setEnabled(true);
  else
    ui->btnRedo->setEnabled(false);
}

void Notes::on_editSource_undoAvailable(bool b) {
  if (b)
    ui->btnUndo->setEnabled(true);
  else
    ui->btnUndo->setEnabled(false);
}

void Notes::on_btnSeparator_clicked() { ui->editSource->insertPlainText("-"); }

void Notes::on_btnWells_clicked() { ui->editSource->insertPlainText("#"); }

void Notes::on_btnVLine_clicked() { ui->editSource->insertPlainText("|"); }

void Notes::on_btnAsterisk_clicked() { ui->editSource->insertPlainText("*"); }

void Notes::on_btnS1_clicked() {
  QString str = ui->editSource->textCursor().selectedText();
  if (str == "") str = tr("Bold Italic");
  ui->editSource->insertPlainText("_**" + str + "**_");
}

void Notes::on_btnS2_clicked() {
  QString str = ui->editSource->textCursor().selectedText();
  if (str == "") str = tr("Italic");
  ui->editSource->insertPlainText("_" + str + "_");
}

void Notes::on_btnS3_clicked() {
  QString str = ui->editSource->textCursor().selectedText();
  if (str == "") str = tr("Underline");
  ui->editSource->insertPlainText("<u>" + str + "</u>");
}

void Notes::on_btnS4_clicked() {
  QString str = ui->editSource->textCursor().selectedText();
  if (str == "") str = tr("Strickout");
  ui->editSource->insertPlainText("~~" + str + "~~");
}

void Notes::on_btnColor_clicked() {
  QString str = ui->editSource->textCursor().selectedText();
  if (str == "") str = tr("Red");
  ui->editSource->insertPlainText("<font color=#FF0000 >" + str + "</font>");
}

void Notes::on_btnS5_clicked() {
  QString str = ui->editSource->textCursor().selectedText();
  if (str == "") str = tr("Bold");
  ui->editSource->insertPlainText("**" + str + "**");
}

void Notes::on_btnLink_clicked() {
  ui->editSource->insertPlainText("[]()");
  on_btnLeft_clicked();
  on_btnLeft_clicked();
  on_btnLeft_clicked();
}

void Notes::on_btnS6_clicked() { ui->editSource->insertPlainText("~"); }

void Notes::on_btnS7_clicked() {
  ui->editSource->insertPlainText("[]");
  ui->btnLeft->click();
}

void Notes::on_btnS8_clicked() {
  ui->editSource->insertPlainText("()");
  ui->btnLeft->click();
}

void Notes::on_btnS9_clicked() {
  ui->editSource->insertPlainText("{}");
  ui->btnLeft->click();
}

void Notes::on_btnS10_clicked() { ui->editSource->insertPlainText("_"); }

void Notes::highlightCurrentLine() {
  QList<QTextEdit::ExtraSelection> extraSelections;

  QTextEdit::ExtraSelection selection;

  QColor lineColor;

  if (isDark)
    lineColor = QColor(QColor(25, 125, 25, 25));
  else
    lineColor = QColor(Qt::yellow).lighter(160);

  selection.format.setBackground(lineColor);
  selection.format.setProperty(QTextFormat::FullWidthSelection, true);
  selection.cursor = ui->editSource->textCursor();
  selection.cursor.clearSelection();
  extraSelections.append(selection);

  ui->editSource->setExtraSelections(extraSelections);

  QString str1, str2, str3, str4;

  // 当前光标
  QTextCursor tc = ui->editSource->textCursor();
  // QTextLayout* lay = tc.block().layout();
  // 当前光标在本BLOCK内的相对位置
  int iCurPos = tc.position() - tc.block().position();
  // 光标所在行号
  //  int iCurrentLine = lay->lineForTextPosition(iCurPos).lineNumber() +
  //                    tc.block().firstLineNumber();
  int iLineCount = ui->editSource->document()->lineCount();
  // 或者  获取光标所在行的行号
  int iRowNum = tc.blockNumber() + 1;

  str1 = QString::number(iLineCount);
  str2 = QString::number(ui->editSource->textCursor().position());
  str3 = QString::number(iCurPos);
  str4 = QString::number(iRowNum);
  ui->lblInfo->setText(" " + str4 + " , " + str3);
}

void Notes::onTextChange() {}

void Notes::on_btnPaste_clicked() { ui->editSource->paste(); }

bool Notes::eventFilterTodo(QObject *watch, QEvent *evn) {
  if (watch == mw_one->ui->editTodo->viewport()) {
    mw_one->m_Notes->getEditPanel(mw_one->ui->editTodo, evn);

    if (evn->type() == QEvent::MouseButtonDblClick) {
      y1 = mw_one->geometry().y() + mw_one->ui->editTodo->y() +
           mw_one->ui->editTodo->height() + 2;
      isMousePress = true;
      on_showEditPanel();
    }
  }

  return QWidget::eventFilter(watch, evn);
}

bool Notes::eventFilterEditRecord(QObject *watch, QEvent *evn) {
  if (watch == mw_one->ui->editDetails->viewport()) {
    mw_one->m_Notes->getEditPanel(mw_one->ui->editDetails, evn);

    if (evn->type() == QEvent::MouseButtonDblClick) {
      y1 = mw_one->geometry().y() + mw_one->ui->editDetails->y() +
           mw_one->ui->editDetails->height() + 2;
      isMousePress = true;
      on_showEditPanel();
    }
  }

  return QWidget::eventFilter(watch, evn);
}

void Notes::on_showEditPanel() {
  timerEditPanel->stop();
  if (isMousePress) {
    isFunShow = true;

    QTextCursor cursor;
    start = byTextEdit->textCursor().position();
    end = start + 2;
    cursor = byTextEdit->textCursor();
    cursor.setPosition(start);
    cursor.setPosition(end, QTextCursor::KeepAnchor);
    byTextEdit->setTextCursor(cursor);

    m_TextSelector->ui->lineEdit->setText(cursor.selectedText());

    m_TextSelector->init(y1);
  }
}

void Notes::selectText(int start, int end) {
  QTextCursor cursor;
  cursor = byTextEdit->textCursor();
  cursor.setPosition(start);
  cursor.setPosition(end, QTextCursor::KeepAnchor);
  byTextEdit->setTextCursor(cursor);
  m_TextSelector->ui->lineEdit->setText(cursor.selectedText());
}

void Notes::paintEvent(QPaintEvent *event) {
  Q_UNUSED(event);
  return;

  if (ui->editSource->hasFocus()) {
    if (bCursorVisible) {
      const QRect qRect =
          ui->editSource->cursorRect(ui->editSource->textCursor());
      QPainter qPainter(ui->editSource->viewport());
      qPainter.fillRect(qRect, QColor(255, 0, 0, 255));
    } else {
      const QRect qRect =
          ui->editSource->cursorRect(ui->editSource->textCursor());
      QPainter qPainter(ui->editSource->viewport());
      qPainter.fillRect(qRect, QColor(0, 255, 0, 255));
    }
  }
}

void Notes::timerSlot() {
  if (bCursorVisible) {
    bCursorVisible = false;
  } else {
    bCursorVisible = true;
  }

  emit sendUpdate();
}

void Notes::on_btnLeft_clicked() {
  ui->editSource->moveCursor(QTextCursor::PreviousCharacter,
                             QTextCursor::MoveAnchor);
}

void Notes::on_btnRight_clicked() {
  ui->editSource->moveCursor(QTextCursor::NextCharacter,
                             QTextCursor::MoveAnchor);
}

bool Notes::androidCopyFile(QString src, QString des) {
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

void Notes::closeEvent(QCloseEvent *event) {
  Q_UNUSED(event);
  mw_one->ui->frameNotes->show();
  if (!m_TextSelector->isHidden()) {
    m_TextSelector->close();
  }
  if (pAndroidKeyboard->isVisible()) pAndroidKeyboard->hide();
  mw_one->Sleep(100);

  if (isNeedSave) {
    if (isDone) {
      saveMainNotes();
      loadMemoQML();
    } else {
      if (isTextChange) {
        ShowMessage *msg = new ShowMessage(this);
        if (msg->showMsg(tr("Notes"), tr("Do you want to save the notes?"),
                         2)) {
          saveMainNotes();
          loadMemoQML();
        }
      } else {
        saveMainNotes();
        loadMemoQML();
      }
    }
  }
}

void Notes::on_editSource_textChanged() {
  isNeedSave = true;
  isTextChange = true;
}

void Notes::on_editSource_cursorPositionChanged() { isNeedSave = true; }

void Notes::on_btnReference_clicked() {
  QString str = ui->editSource->textCursor().selectedText();
  ui->editSource->insertPlainText("> " + str);
}

void Notes::show_findText() {
  QString findtext = ui->editFind->text().trimmed().toLower();
  if (findtext == "") return;
  // 获得对话框的内容
  if (ui->editSource->find(findtext, QTextDocument::FindCaseSensitively))
  // 查找后一个
  {
    // 查找到后高亮显示
    QPalette palette = ui->editSource->palette();
    palette.setColor(QPalette::Highlight,
                     palette.color(QPalette::Active, QPalette::Highlight));
    ui->editSource->setPalette(palette);
  } else {
    ShowMessage *m_ShowMsg = new ShowMessage(mw_one->m_Notes);
    m_ShowMsg->showMsg("Knot", tr("The end of the document has been reached."),
                       0);
  }
}

void Notes::show_findTextBack() {
  QString findtext = ui->editFind->text().trimmed().toLower();
  if (findtext == "") return;
  // 获得对话框的内容
  if (ui->editSource->find(findtext, QTextDocument::FindBackward))
  // 查找后一个
  {
    // 查找到后高亮显示
    QPalette palette = ui->editSource->palette();
    palette.setColor(QPalette::Highlight,
                     palette.color(QPalette::Active, QPalette::Highlight));
    ui->editSource->setPalette(palette);
  } else {
    ShowMessage *m_ShowMsg = new ShowMessage(mw_one->m_Notes);
    m_ShowMsg->showMsg(
        "Knot", tr("The beginning of the document has been reached."), 0);
  }
}

void Notes::findText() {
  QString search_text = ui->editFind->text().trimmed().toLower();
  if (search_text.trimmed().isEmpty()) {
    return;
  } else {
    QTextDocument *document = ui->editSource->document();
    bool found = false;
    QTextCursor highlight_cursor(document);
    QTextCursor cursor(document);
    // 开始
    cursor.beginEditBlock();
    QTextCharFormat color_format(highlight_cursor.charFormat());
    color_format.setForeground(Qt::red);
    while (!highlight_cursor.isNull() && !highlight_cursor.atEnd()) {
      // 查找指定的文本，匹配整个单词
      highlight_cursor = document->find(search_text, highlight_cursor,
                                        QTextDocument::FindCaseSensitively);
      if (!highlight_cursor.isNull()) {
        if (!found) found = true;
        highlight_cursor.mergeCharFormat(color_format);
      }
    }
    cursor.endEditBlock();
    // 结束
    if (found == false) {
      QMessageBox::information(this, tr("Word not found"),
                               tr("Sorry,the word cannot be found."));
    }
  }
}

void Notes::on_btnFind_clicked() {
  if (ui->editFind->text().trimmed() == "") return;
  show_findText();
}

void Notes::on_btnPrev_clicked() { show_findTextBack(); }

void Notes::on_btnNext_clicked() { show_findText(); }

void Notes::on_editFind_returnPressed() { on_btnFind_clicked(); }

void Notes::on_editFind_textChanged(const QString &arg1) { Q_UNUSED(arg1); }

bool Notes::selectPDFFormat(QPrinter *printer) {
  QSettings settings;

  // select the page size
  QStringList pageSizeStrings;
  pageSizeStrings << QStringLiteral("A0") << QStringLiteral("A1")
                  << QStringLiteral("A2") << QStringLiteral("A3")
                  << QStringLiteral("A4") << QStringLiteral("A5")
                  << QStringLiteral("A6") << QStringLiteral("A7")
                  << QStringLiteral("A8") << QStringLiteral("A9")
                  << tr("Letter");
  QList<QPageSize::PageSizeId> pageSizes;
  pageSizes << QPageSize::A0 << QPageSize::A1 << QPageSize::A2 << QPageSize::A3
            << QPageSize::A4 << QPageSize::A5 << QPageSize::A6 << QPageSize::A7
            << QPageSize::A8 << QPageSize::A9 << QPageSize::Letter;

  PrintPDF *idlg1 = new PrintPDF(this);
  QString pageSizeString =
      idlg1->getItem(tr("Page size"), tr("Page size"), pageSizeStrings, 4);

  /*QString pageSizeString = QInputDialog::getItem(
      this, tr("Page size"), tr("Page size:"), pageSizeStrings,
      settings.value(QStringLiteral("Printer/NotePDFExportPageSize"), 4)
          .toInt(),
      false, &ok);*/

  if (pageSizeString.isEmpty()) {
    return false;
  }

  int pageSizeIndex = pageSizeStrings.indexOf(pageSizeString);
  if (pageSizeIndex == -1) {
    return false;
  }

  QPageSize pageSize(pageSizes.at(pageSizeIndex));
  settings.setValue(QStringLiteral("Printer/NotePDFExportPageSize"),
                    pageSizeIndex);
  printer->setPageSize(pageSize);

  // select the orientation
  QStringList orientationStrings;
  orientationStrings << tr("Portrait") << tr("Landscape");
  QList<QPageLayout::Orientation> orientations;
  orientations << QPageLayout::Portrait << QPageLayout::Landscape;

  PrintPDF *idlg2 = new PrintPDF(this);
  QString orientationString = idlg2->getItem(
      tr("Orientation"), tr("Orientation"), orientationStrings, 0);

  /*QString orientationString = QInputDialog::getItem(
      this, tr("Orientation"), tr("Orientation:"), orientationStrings,
      settings.value(QStringLiteral("Printer/NotePDFExportOrientation"), 0)
          .toInt(),
      false, &ok);*/

  if (orientationString.isEmpty()) {
    return false;
  }

  int orientationIndex = orientationStrings.indexOf(orientationString);
  if (orientationIndex == -1) {
    return false;
  }

  printer->setPageOrientation(orientations.at(orientationIndex));
  settings.setValue(QStringLiteral("Printer/NotePDFExportOrientation"),
                    orientationIndex);

  QString fileName;
#ifdef Q_OS_ANDROID
  fileName = "/storage/emulated/0/KnotBak/" + mw_one->ui->lblNoteName->text() +
             QStringLiteral(".pdf");
  ShowMessage *msg = new ShowMessage(this);
  msg->showMsg("PDF",
               tr("The PDF file is successfully exported.") + "\n\n" + fileName,
               0);

#else
  QFileDialog dialog(NULL, QStringLiteral("NotePDFExport"));
  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setAcceptMode(QFileDialog::AcceptSave);
  dialog.setNameFilter(tr("PDF files") + QStringLiteral(" (*.pdf)"));
  dialog.setWindowTitle(tr("Export current note as PDF"));
  dialog.selectFile(mw_one->ui->lblNoteName->text() + QStringLiteral(".pdf"));
  int ret = dialog.exec();

  if (ret != QDialog::Accepted) {
    return false;
  }

  fileName = dialog.selectedFiles().at(0);
#endif

  if (fileName.isEmpty()) {
    return false;
  }

  if (QFileInfo(fileName).suffix().isEmpty()) {
    fileName.append(QLatin1String(".pdf"));
  }

  printer->setOutputFormat(QPrinter::PdfFormat);
  printer->setOutputFileName(fileName);
  return true;
}

void Notes::on_btnPDF_clicked() {
  QString html = mw_one->loadText(iniDir + "memo.html");
  auto doc = new QTextDocument(this);
  doc->setHtml(html);

  auto *printer = new QPrinter(QPrinter::HighResolution);

  if (selectPDFFormat(printer)) {
    doc->print(printer);
  }

  delete printer;
}

void Notes::on_btnGetShare_clicked() {
#ifdef Q_OS_ANDROID
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  QAndroidJniObject javaUriPath = QAndroidJniObject::fromString("uripath");
  QAndroidJniObject m_activity = QtAndroid::androidActivity();
  QAndroidJniObject s = m_activity.callObjectMethod(
      "getShare", "(Ljava/lang/String;)Ljava/lang/String;",
      javaUriPath.object<jstring>());

#else
  QJniObject javaUriPath = QAndroidJniObject::fromString("uripath");
  QJniObject m_activity = QtAndroid::androidActivity();
  QJniObject s = m_activity.callObjectMethod(
      "getShare", "(Ljava/lang/String;)Ljava/lang/String;",
      javaUriPath.object<jstring>());
#endif
  on_btnPaste_clicked();
#endif
}

void Notes::on_btnHideKey_clicked() {
  if (pAndroidKeyboard->isVisible()) {
    pAndroidKeyboard->hide();
    setGeometry(mw_one->geometry().x(), mw_one->geometry().y(), width(),
                mw_one->mainHeight);
  }
}

void Notes::on_btnShowTools_clicked() {
  bool old = isNeedSave;

  if (ui->f_ToolBar->isHidden()) {
    ui->f_ToolBar->show();

  } else {
    ui->f_ToolBar->hide();
  }

  if (ui->editSource->textCursor().position() > 0) {
    on_btnLeft_clicked();
    on_btnRight_clicked();
  }

  isNeedSave = old;
}

void Notes::editNote() { mw_one->on_btnEdit_clicked(); }

void Notes::showNoteList() { mw_one->on_btnNotesList_clicked(); }

void Notes::on_editNote() {
  timerEditNote->stop();
  mw_one->on_btnEdit_clicked();
}
