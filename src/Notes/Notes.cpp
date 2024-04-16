#include "src/Notes/Notes.h"

#include "src/MainWindow.h"
#include "src/Notes/MarkdownHighlighter.h"
#include "ui_MainWindow.h"
#include "ui_Notes.h"

extern MainWindow *mw_one;
extern Method *m_Method;
extern QString iniFile, iniDir, privateDir, currentMDFile, imgFileName;
extern bool isAndroid, isIOS, isDark;
extern int fontSize;
extern QRegularExpression regxNumber;

extern int deleteDirfile(QString dirName);
extern QString loadText(QString textFile);
extern QString getTextEditLineText(QTextEdit *txtEdit, int i);
extern void TextEditToFile(QTextEdit *txtEdit, QString fileName);
extern void StringToFile(QString buffers, QString fileName);

Notes::Notes(QWidget *parent) : QDialog(parent), ui(new Ui::Notes) {
  ui->setupUi(this);

  m_EditSource = new QTextEditHighlighter();
  ui->frameEdit->layout()->addWidget(m_EditSource);

  ui->editSource->hide();

  QFont font0 = m_Method->getNewFont(15);
  QObjectList btnList0 =
      mw_one->getAllToolButton(mw_one->getAllUIControls(ui->f_ToolBar));
  for (int i = 0; i < btnList0.count(); i++) {
    QToolButton *btn = (QToolButton *)btnList0.at(i);
    btn->setFont(font0);

#ifdef Q_OS_ANDROID
    btn->setFixedHeight(25);
    ui->editCol->setFixedHeight(25);
    ui->editRow->setFixedHeight(25);
    ui->editFind->setFixedHeight(25);
#else
    btn->setFixedHeight(30);
    ui->editCol->setFixedHeight(30);
    ui->editRow->setFixedHeight(30);
    ui->editFind->setFixedHeight(30);
#endif
  }

  QObjectList btnList = mw_one->getAllToolButton(
      mw_one->getAllUIControls(mw_one->ui->f_ToolBar_Note));
  for (int i = 0; i < btnList.count(); i++) {
    QToolButton *btn = (QToolButton *)btnList.at(i);
    btn->setFont(font0);

#ifdef Q_OS_ANDROID
    btn->setFixedHeight(25);
    mw_one->ui->editCol->setFixedHeight(25);
    mw_one->ui->editRow->setFixedHeight(25);
    mw_one->ui->editFind->setFixedHeight(25);
#else
    btn->setFixedHeight(30);
    mw_one->ui->editCol->setFixedHeight(30);
    mw_one->ui->editRow->setFixedHeight(30);
    mw_one->ui->editFind->setFixedHeight(30);
#endif
  }

  ui->editFind->setFont(font0);
  ui->editCol->setFont(font0);
  ui->editRow->setFont(font0);
  ui->lblCol->setFont(font0);
  ui->lblRow->setFont(font0);

  mw_one->ui->editFind->setFont(font0);
  mw_one->ui->editCol->setFont(font0);
  mw_one->ui->editRow->setFont(font0);
  mw_one->ui->lblCol->setFont(font0);
  mw_one->ui->lblRow->setFont(font0);

  QFont font1 = m_Method->getNewFont(13);
  QObjectList btnList1 =
      mw_one->getAllToolButton(mw_one->getAllUIControls(ui->f_Panel));
  for (int i = 0; i < btnList1.count(); i++) {
    QToolButton *btn = (QToolButton *)btnList1.at(i);
    btn->setFont(font1);
  }

  QObjectList btnList2 = mw_one->getAllToolButton(
      mw_one->getAllUIControls(mw_one->ui->f_Panel_Note));
  for (int i = 0; i < btnList2.count(); i++) {
    QToolButton *btn = (QToolButton *)btnList2.at(i);
    btn->setFont(font1);
  }

  mw_one->set_ToolButtonStyle(this);

  ui->lblInfo->hide();
  ui->btnFind->hide();
  ui->lblCount->hide();
  mw_one->ui->lblCount->hide();
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
  m_EditSource->installEventFilter(this);
  m_EditSource->viewport()->installEventFilter(this);
  this->setModal(true);
  this->layout()->setContentsMargins(5, 5, 5, 5);

  if (isAndroid) {
    QScroller::grabGesture(m_EditSource, QScroller::LeftMouseButtonGesture);
    m_Method->setSCrollPro(m_EditSource);
  }

  if (!isDark) {
    // m_EditSource->verticalScrollBar()->setStyleSheet(
    //     m_Method->vsbarStyleSmall);
  }

  ui->frameEdit->layout()->setContentsMargins(0, 0, 0, 0);
  m_EditSource->setContentsMargins(1, 1, 1, 1);
  m_EditSource->setStyleSheet("border:none");
  m_EditSource->setCursorWidth(2);

  QFont f = this->font();
  f.setPointSize(fontSize - 1);
  ui->lblInfo->setFont(f);
  ui->f_ToolBar->setFont(f);

  connect(m_EditSource->verticalScrollBar(), SIGNAL(valueChanged(int)), this,
          SLOT(editVSBarValueChanged()));

  QValidator *validator =
      new QRegularExpressionValidator(regxNumber, ui->editRow);
  ui->editRow->setValidator(validator);
  ui->editRow->setPlaceholderText(tr("Row"));
  ui->editCol->setValidator(validator);
  ui->editCol->setPlaceholderText(tr("Column"));

  connect(m_EditSource, &QTextEdit::undoAvailable, this,
          &Notes::on_editSource_undoAvailable);
  connect(m_EditSource, &QTextEdit::redoAvailable, this,
          &Notes::on_editSource_redoAvailable);
  connect(m_EditSource, &QTextEdit::textChanged, this,
          &Notes::on_editSource_textChanged);
  connect(m_EditSource, &QTextEdit::cursorPositionChanged, this,
          &Notes::highlightCurrentLine);
  highlightCurrentLine();

  QFont font = this->font();
  font.setLetterSpacing(QFont::AbsoluteSpacing, 2);  // 字间距
  m_EditSource->setFont(font);
  m_EditSource->setAcceptRichText(false);

  connect(m_EditSource, &QTextEdit::textChanged, this, &Notes::onTextChange);

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

  m_EditSource->setFocus();
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

#ifdef Q_OS_ANDROID

  if (!this->isHidden()) {
    if (this->height() != mw_one->mainHeight) {
      newHeight = this->height();
      androidKeyH = mw_one->mainHeight - newHeight;

      QSettings Reg(privateDir + "android.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
      Reg.setIniCodec("utf-8");
#endif
      Reg.setValue("KeyHeight", androidKeyH);
      Reg.setValue("newHeight", newHeight);
    }

    if (pAndroidKeyboard->isVisible()) {
      this->setGeometry(mw_one->geometry().x(), mw_one->geometry().y(),
                        mw_one->width(), newHeight);
    }

    if (this->height() == newHeight) {
      int p = m_EditSource->textCursor().position();
      QTextCursor tmpCursor = m_EditSource->textCursor();
      tmpCursor.setPosition(p);
      m_EditSource->setTextCursor(tmpCursor);
    }
  }

  qDebug() << pAndroidKeyboard->keyboardRectangle().height()
           << "this height=" << this->height();
  qDebug() << "newHeight=" << newHeight << "main height=" << mw_one->mainHeight;

#endif
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
    QString strmd = loadText(mdFile);
    if (strmd.contains(imgDir)) {
#ifdef Q_OS_WIN
      strmd = strmd.replace(imgDir, "/" + iniDir);
#else
      strmd = strmd.replace(imgDir, iniDir);
#endif
    }

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

  if (isTextChange) {
    QString text = m_EditSource->toPlainText();
    text = formatMDText(text);
    StringToFile(text, currentMDFile);
    MD2Html(currentMDFile);

    qDebug() << "Save Note: " << currentMDFile;
  }

  QString strTag = currentMDFile;
  strTag.replace(iniDir, "");

  QSettings *iniNotes =
      new QSettings(iniDir + "mainnotes.ini", QSettings::IniFormat, NULL);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  iniNotes->setIniCodec("utf-8");
#endif

  iniNotes->setValue("/MainNotes/editVPos" + strTag,
                     m_EditSource->verticalScrollBar()->sliderPosition());
  iniNotes->setValue("/MainNotes/editCPos" + strTag,
                     m_EditSource->textCursor().position());

  isNeedSave = false;
  isTextChange = false;
}

void Notes::init_MainNotes() { loadNoteToQML(); }

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

      if (textEdit == m_EditSource) {
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
  if (obj == m_EditSource->viewport()) {
    getEditPanel(m_EditSource, evn);
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
  if (obj == m_EditSource) {
    if (evn->type() == QEvent::KeyPress) {
      if (keyEvent->key() != Qt::Key_Back) {
      }
    }
  }

  // Mouse
  if (obj == m_EditSource->viewport()) {
    if (evn->type() == QEvent::MouseButtonPress) {
    }
  }

#ifdef Q_OS_ANDROID
  if (obj == m_EditSource->viewport()) {
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

void Notes::on_btnUndo_clicked() { m_EditSource->undo(); }

void Notes::on_btnRedo_clicked() { m_EditSource->redo(); }

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
  QString fileName;
  fileName = QFileDialog::getOpenFileName(NULL, tr("Knot"), "",
                                          tr("Picture Files (*.*)"));

  insertImage(fileName);
}

QString Notes::insertImage(QString fileName) {
  QFileInfo fi(fileName);
  QString strImage;
  if (fi.exists()) {
    QDir dir;
    dir.mkpath(iniDir + "memo/images/");

    QString strTar = iniDir + "memo/images/" + getDateTimeStr() + ".png";
    if (QFile(strTar).exists()) QFile(strTar).remove();

    int nLeftMargin = 9 + 9 + 6;

    QImage img(fileName);
    double w, h;
    int new_w, new_h;
    w = img.width();
    h = img.height();

    int w0 = mw_one->width();
    double r = (double)w / h;
    if (w > w0 - nLeftMargin) {
      new_w = w0 - nLeftMargin;
      new_h = new_w / r;

    } else {
      new_w = w;
      new_h = h;
    }

    if (!isAndroid) {
      ShowMessage *msg = new ShowMessage();
      msg->ui->btnCancel->setText(tr("No"));
      msg->ui->btnOk->setText(tr("Yes"));
      bool isYes = msg->showMsg(
          "Knot", tr("Is the original size of the image used?"), 2);
      if (isYes) {
        new_w = w;
        new_h = h;
      }
    }

    QPixmap pix;
    pix = QPixmap::fromImage(img);
    pix =
        pix.scaled(new_w, new_h, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    pix.save(strTar);

    strTar = strTar.replace(iniDir, imgDir);
    strImage = "\n\n![image](file://" + strTar + ")\n\n";

    if (!isAndroid) {
      m_EditSource->insertPlainText(strImage);
    } else {
      insertNote(strImage);
    }

    qDebug() << "pic=" << strTar << nLeftMargin;
  }

  return strImage;
}

QStringList Notes::getImgFileFromHtml(QString htmlfile) {
  QStringList list;
  QString strHtml = loadText(htmlfile);
  strHtml = strHtml.replace("><", ">\n<");
  QTextEdit *edit = new QTextEdit;
  edit->setPlainText(strHtml);
  for (int i = 0; i < edit->document()->lineCount(); i++) {
    QString str = getTextEditLineText(edit, i).trimmed();
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
  TextEditToFile(txtEdit, fileName);

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
  deleteDirfile(iniDir + "memo");
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
  TextEditToFile(txtEdit, fileName);

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

void Notes::loadNoteToQML() {
  QString htmlFileName = privateDir + "memo.html";
  QTextEdit *edit = new QTextEdit;
  QPlainTextEdit *edit1 = new QPlainTextEdit;
  QString strhtml = loadText(htmlFileName);
  strhtml = strhtml.replace("><", ">\n<");
  edit->setPlainText(strhtml);
  QString str;
  for (int i = 0; i < edit->document()->lineCount(); i++) {
    str = getTextEditLineText(edit, i);
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
      strSrc = "\"file://" + iniDir + "memo/" + list1.at(1) + " ";

      QStringList list2 = str1.split("/memo/");
      QString str_2 = list2.at(1);
      str = "<img src=\"file:///" + iniDir + "memo/" + str_2;
      str = "<a href=" + strSrc + ">" + str + "</a>";

      QString imgFile = iniDir + "memo/" + str_2.split("\"").at(0);
      QImage img(imgFile);
      if (img.width() >= mw_one->width() - 25) {
        QString strW = QString::number(mw_one->width() - 25);
        QString a1 = "width = ";
        str = str.replace("/>", a1 + "\"" + strW + "\"" + " />");
      }
    }

    edit1->appendPlainText(str);
  }

  QQuickItem *root = mw_one->ui->qwNotes->rootObject();

  mw_one->m_Reader->PlainTextEditToFile(edit1, htmlFileName);
  // QMetaObject::invokeMethod((QObject *)root, "loadHtml",
  //                           Q_ARG(QVariant, htmlFileName));

  htmlBuffer = edit1->toPlainText();
  QMetaObject::invokeMethod((QObject *)root, "loadHtmlBuffer",
                            Q_ARG(QVariant, htmlBuffer));

  setVPos(-0.01);
}

void Notes::saveQMLVPos() {
  QSettings Reg(privateDir + "notes.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif

  if (QFile(currentMDFile).exists()) {
    sliderPos = getVPos();
    Reg.setValue("/MainNotes/SlidePos" + currentMDFile, sliderPos);
    Reg.setValue("/MainNotes/Editor" + currentMDFile, getEditorVPos());
  }
}

void Notes::setVPos(qreal pos) {
  qreal m_pos;
  if (pos < 0) {
    QSettings Reg(privateDir + "notes.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    Reg.setIniCodec("utf-8");
#endif

    sliderPos = Reg.value("/MainNotes/SlidePos" + currentMDFile).toReal();
    m_pos = sliderPos;
  } else {
    m_pos = pos;
  }

  QQuickItem *root = mw_one->ui->qwNotes->rootObject();
  QMetaObject::invokeMethod((QObject *)root, "setVPos", Q_ARG(QVariant, m_pos));
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

  if (!m_EditSource->isHidden()) {
    m_EditSource->insertPlainText(strCol + "\n" + strHead + "\n");
    for (int j = 0; j < row; j++) {
      m_EditSource->insertPlainText(strRow + "\n");
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

void Notes::on_btnSeparator_clicked() { m_EditSource->insertPlainText("-"); }

void Notes::on_btnWells_clicked() { m_EditSource->insertPlainText("#"); }

void Notes::on_btnVLine_clicked() { m_EditSource->insertPlainText("|"); }

void Notes::on_btnAsterisk_clicked() { m_EditSource->insertPlainText("*"); }

void Notes::on_btnS1_clicked() {
  QString str = m_EditSource->textCursor().selectedText();
  if (str == "") str = tr("Bold Italic");
  m_EditSource->insertPlainText("_**" + str + "**_");
}

void Notes::on_btnS2_clicked() {
  QString str = m_EditSource->textCursor().selectedText();
  if (str == "") str = tr("Italic");
  m_EditSource->insertPlainText("_" + str + "_");
}

void Notes::on_btnS3_clicked() {
  QString str = m_EditSource->textCursor().selectedText();
  if (str == "") str = tr("Underline");
  m_EditSource->insertPlainText("<u>" + str + "</u>");
}

void Notes::on_btnS4_clicked() {
  QString str = m_EditSource->textCursor().selectedText();
  if (str == "") str = tr("Strickout");
  m_EditSource->insertPlainText("~~" + str + "~~");
}

void Notes::on_btnColor_clicked() {
  QString strColor = m_Method->getCustomColor();
  if (strColor.isEmpty()) return;

  QString str = m_EditSource->textCursor().selectedText();
  if (str == "") str = tr("Color");
  m_EditSource->insertPlainText("<font color=" + strColor + ">" + str +
                                "</font>");
}

QColor Notes::StringToColor(QString mRgbStr) {
  QColor color(mRgbStr.toUInt(NULL, 16));
  return color;
}

void Notes::on_btnS5_clicked() {
  QString str = m_EditSource->textCursor().selectedText();
  if (str == "") str = tr("Bold");
  m_EditSource->insertPlainText("**" + str + "**");
}

void Notes::on_btnLink_clicked() {
  m_EditSource->insertPlainText("[]()");
  on_btnLeft_clicked();
  on_btnLeft_clicked();
  on_btnLeft_clicked();
}

void Notes::on_btnS6_clicked() { m_EditSource->insertPlainText("~"); }

void Notes::on_btnS7_clicked() {
  m_EditSource->insertPlainText("[]");
  ui->btnLeft->click();
}

void Notes::on_btnS8_clicked() {
  m_EditSource->insertPlainText("()");
  ui->btnLeft->click();
}

void Notes::on_btnS9_clicked() {
  m_EditSource->insertPlainText("{}");
  ui->btnLeft->click();
}

void Notes::on_btnS10_clicked() { m_EditSource->insertPlainText("_"); }

void Notes::highlightCurrentLine() {
  isNeedSave = true;

  QList<QTextEdit::ExtraSelection> extraSelections;

  QTextEdit::ExtraSelection selection;

  QColor lineColor;

  if (isDark)
    lineColor = QColor(QColor(25, 125, 25, 25));
  else
    lineColor = QColor(Qt::yellow).lighter(160);

  selection.format.setBackground(lineColor);
  selection.format.setProperty(QTextFormat::FullWidthSelection, true);
  selection.cursor = m_EditSource->textCursor();
  selection.cursor.clearSelection();
  extraSelections.append(selection);

  m_EditSource->setExtraSelections(extraSelections);

  QString str1, str2, str3, str4;

  // 当前光标
  QTextCursor tc = m_EditSource->textCursor();
  // QTextLayout* lay = tc.block().layout();
  // 当前光标在本BLOCK内的相对位置
  int iCurPos = tc.position() - tc.block().position();
  // 光标所在行号
  //  int iCurrentLine = lay->lineForTextPosition(iCurPos).lineNumber() +
  //                    tc.block().firstLineNumber();
  int iLineCount = m_EditSource->document()->lineCount();
  // 或者  获取光标所在行的行号
  int iRowNum = tc.blockNumber() + 1;

  str1 = QString::number(iLineCount);
  str2 = QString::number(m_EditSource->textCursor().position());
  str3 = QString::number(iCurPos);
  str4 = QString::number(iRowNum);
  ui->lblInfo->setText(" " + str4 + " , " + str3);
}

void Notes::onTextChange() {}

void Notes::on_btnPaste_clicked() {
  const QClipboard *clipboard = QApplication::clipboard();
  const QMimeData *mimeData = clipboard->mimeData();
  if (mimeData->hasImage()) {
    QImage img = qvariant_cast<QImage>(mimeData->imageData());
    if (!img.isNull()) {
      QPixmap pix;
      QString strTar = privateDir + "temppic.png";
      pix = QPixmap::fromImage(img);
      pix = pix.scaled(img.width(), img.height(), Qt::KeepAspectRatio,
                       Qt::SmoothTransformation);
      pix.save(strTar);
      insertImage(strTar);
    }
  } else
    m_EditSource->paste();
}

bool Notes::eventFilterQwNote(QObject *watch, QEvent *event) {
  if (watch == mw_one->ui->qwNotes) {
    if (event->type() == QEvent::MouseButtonPress) {
      isMousePress = true;
      isMouseMove = false;

      if (!isMouseMove) {
        timerEditNote->start(1600);
      }
    }

    if (event->type() == QEvent::MouseMove) {
      if (isMousePress) isMouseMove = true;
    }

    if (event->type() == QEvent::MouseButtonRelease) {
      isMousePress = false;
      isMouseMove = false;

      timerEditNote->stop();
    }

    if (event->type() == QEvent::MouseButtonDblClick) {
      mw_one->on_btnNotesList_clicked();
    }
  }

  return QWidget::eventFilter(watch, event);
}

bool Notes::eventFilterEditTodo(QObject *watch, QEvent *evn) {
  if (watch == mw_one->ui->editTodo->viewport()) {
    getEditPanel(mw_one->ui->editTodo, evn);

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

  if (m_EditSource->hasFocus()) {
    if (bCursorVisible) {
      const QRect qRect = m_EditSource->cursorRect(m_EditSource->textCursor());
      QPainter qPainter(m_EditSource->viewport());
      qPainter.fillRect(qRect, QColor(255, 0, 0, 255));
    } else {
      const QRect qRect = m_EditSource->cursorRect(m_EditSource->textCursor());
      QPainter qPainter(m_EditSource->viewport());
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
  m_EditSource->moveCursor(QTextCursor::PreviousCharacter,
                           QTextCursor::MoveAnchor);
}

void Notes::on_btnRight_clicked() {
  m_EditSource->moveCursor(QTextCursor::NextCharacter, QTextCursor::MoveAnchor);
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
      loadNoteToQML();
    } else {
      if (isTextChange) {
        m_Method->m_widget = new QWidget(this);
        ShowMessage *msg = new ShowMessage(this);
        if (msg->showMsg(tr("Notes"), tr("Do you want to save the notes?"),
                         2)) {
          saveMainNotes();
          loadNoteToQML();
        }

      } else {
        saveMainNotes();
        loadNoteToQML();
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
  QString str = m_EditSource->textCursor().selectedText();
  m_EditSource->insertPlainText("> " + str);
}

void Notes::show_findText() {
  QString findtext = ui->editFind->text().trimmed().toLower();
  if (findtext == "") return;
  // 获得对话框的内容
  if (m_EditSource->find(findtext, QTextDocument::FindCaseSensitively))
  // 查找后一个
  {
    // 查找到后高亮显示
    QPalette palette = m_EditSource->palette();
    palette.setColor(QPalette::Highlight,
                     palette.color(QPalette::Active, QPalette::Highlight));
    m_EditSource->setPalette(palette);
  } else {
    m_Method->m_widget = new QWidget(this);
    ShowMessage *m_ShowMsg = new ShowMessage(this);
    m_ShowMsg->showMsg("Knot", tr("The end of the document has been reached."),
                       0);
  }
}

void Notes::show_findTextBack() {
  QString findtext = ui->editFind->text().trimmed().toLower();
  if (findtext == "") return;
  // 获得对话框的内容
  if (m_EditSource->find(findtext, QTextDocument::FindBackward))
  // 查找后一个
  {
    // 查找到后高亮显示
    QPalette palette = m_EditSource->palette();
    palette.setColor(QPalette::Highlight,
                     palette.color(QPalette::Active, QPalette::Highlight));
    m_EditSource->setPalette(palette);
  } else {
    m_Method->m_widget = new QWidget(this);
    ShowMessage *m_ShowMsg = new ShowMessage(this);
    m_ShowMsg->showMsg(
        "Knot", tr("The beginning of the document has been reached."), 0);
  }
}

void Notes::findText() {
  QString search_text = ui->editFind->text().trimmed().toLower();
  if (search_text.trimmed().isEmpty()) {
    return;
  } else {
    QTextDocument *document = m_EditSource->document();
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
  m_Method->m_widget = new QWidget(this);
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
  QString html = loadText(privateDir + "memo.html");
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
  QJniObject javaUriPath = QJniObject::fromString("uripath");
  QJniObject m_activity = QtAndroidPrivate::activity();
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

  if (m_EditSource->textCursor().position() > 0) {
    on_btnLeft_clicked();
    on_btnRight_clicked();
  }

  isNeedSave = old;

  QSettings *iniNotes =
      new QSettings(iniDir + "mainnotes.ini", QSettings::IniFormat, NULL);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  iniNotes->setIniCodec("utf-8");
#endif
  iniNotes->setValue("/MainNotes/toolBarVisible", ui->f_ToolBar->isVisible());
}

void Notes::editNote() { mw_one->on_btnEdit_clicked(); }

void Notes::showNoteList() { mw_one->on_btnNotesList_clicked(); }

void Notes::on_editNote() {
  timerEditNote->stop();
  mw_one->on_btnEdit_clicked();
}

void Notes::on_btnDate_clicked() {
  m_EditSource->insertPlainText(QDate::currentDate().toString());
}

void Notes::on_btnTime_clicked() {
  m_EditSource->insertPlainText(QTime::currentTime().toString());
}

void Notes::on_btnS11_clicked() { m_EditSource->insertPlainText("!"); }

void Notes::setEditorVPos() {
  QSettings Reg(privateDir + "notes.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif
  qreal pos = 0;
  if (QFile(currentMDFile).exists()) {
    pos = Reg.value("/MainNotes/Editor" + currentMDFile, 0).toReal();
  }

  QQuickItem *root;
  root = mw_one->ui->qwNoteEditor->rootObject();
  QMetaObject::invokeMethod((QObject *)root, "setVPos", Q_ARG(QVariant, pos));
}

qreal Notes::getEditorVPos() {
  QVariant itemCount;
  QQuickItem *root;
  root = mw_one->ui->qwNoteEditor->rootObject();

  QMetaObject::invokeMethod((QObject *)root, "getVPos",
                            Q_RETURN_ARG(QVariant, itemCount));
  qreal textPos = itemCount.toDouble();
  return textPos;
}

QString Notes::getEditorText() {
  QVariant itemCount;
  QQuickItem *root;
  root = mw_one->ui->qwNoteEditor->rootObject();

  QMetaObject::invokeMethod((QObject *)root, "getText",
                            Q_RETURN_ARG(QVariant, itemCount));
  return itemCount.toString();
}

void Notes::showTextSelector() {
  m_TextSelector->setGeometry(mw_one->geometry().x(), mw_one->geometry().y(),
                              mw_one->width(), m_TextSelector->height());
}

void Notes::openNoteEditor() {
#ifdef Q_OS_ANDROID

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  QAndroidJniObject activity = QtAndroid::androidActivity();
  activity.callMethod<void>("openNoteEditor", "()V");
#else
  QJniObject activity = QtAndroid::androidActivity();
  activity.callMethod<void>("openNoteEditor", "()V");
#endif

#endif
}

void Notes::appendNote(QString str) {
  Q_UNUSED(str);
#ifdef Q_OS_ANDROID

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  QAndroidJniObject jTitle = QAndroidJniObject::fromString(str);
  QAndroidJniObject activity = QtAndroid::androidActivity();
  QAndroidJniObject m_activity =
      QAndroidJniObject::fromString("com.x/NoteEditor");
  m_activity.callStaticMethod<void>("com.x/NoteEditor", "appendNote",
                                    "(Ljava/lang/String;)V",
                                    jTitle.object<jstring>());
#else
  QJniObject jTitle = QJniObject::fromString(str);
  QJniObject m_activity = QJniObject::fromString("com.x/NoteEditor");
  m_activity.callStaticMethod<void>("com.x/NoteEditor", "appendNote",
                                    "(Ljava/lang/String;)V",
                                    jTitle.object<jstring>());
#endif

#endif
}

void Notes::insertNote(QString str) {
  Q_UNUSED(str);
#ifdef Q_OS_ANDROID

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  QAndroidJniObject jTitle = QAndroidJniObject::fromString(str);
  QAndroidJniObject activity = QtAndroid::androidActivity();
  QAndroidJniObject m_activity =
      QAndroidJniObject::fromString("com.x/NoteEditor");
  m_activity.callStaticMethod<void>("com.x/NoteEditor", "insertNote",
                                    "(Ljava/lang/String;)V",
                                    jTitle.object<jstring>());
#else
  QJniObject jTitle = QJniObject::fromString(str);
  QAndroidJniObject m_activity =
      QAndroidJniObject::fromString("com.x/NoteEditor");
  m_activity.callStaticMethod<void>("com.x/NoteEditor", "insertNote",
                                    "(Ljava/lang/String;)V",
                                    jTitle.object<jstring>());
#endif

#endif
}

auto Notes::getAndroidNoteConfig(QString key) {
  QSettings Reg(privateDir + "note_text.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif
  auto value = Reg.value(key);
  return value;
}

void Notes::setAndroidNoteConfig(QString key, QString value) {
  QSettings Reg(privateDir + "note_text.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif

  Reg.setValue(key, value);
}

void Notes::delImage() {
  ShowMessage *m_ShowMsg = new ShowMessage(this);
  if (!m_ShowMsg->showMsg("Knot", tr("Delete this image?"), 2)) return;

  qreal oldPos = getVPos();
  QImage img(imgFileName);
  int nImagHeight = img.height();
  qreal newPos = oldPos - nImagHeight;
  QFileInfo fi(imgFileName);
  QString name = fi.fileName();
  QString strImg = "![image](file://===KnotData===memo/images/" + name + ")";
  QString buffers = loadText(currentMDFile);
  buffers.replace(strImg, "");
  buffers = formatMDText(buffers);

  StringToFile(buffers, currentMDFile);
  MD2Html(currentMDFile);
  loadNoteToQML();
  mw_one->on_btnBackImg_clicked();
  setVPos(newPos);
}

void Notes::delLink(QString link) {
  qreal oldPos = getVPos();
  QString mdBurrers = loadText(currentMDFile);

  if (!mdBurrers.contains(link)) {
    link.replace("http://", "");
  }

  mdBurrers.replace(link, "");
  mdBurrers = formatMDText(mdBurrers);
  StringToFile(mdBurrers, currentMDFile);
  MD2Html(currentMDFile);
  loadNoteToQML();
  setVPos(oldPos);
}

void Notes::javaNoteToQMLNote() {
  QString mdString;
  mdString = loadText(privateDir + "note_text.txt");
  mdString = formatMDText(mdString);
  StringToFile(mdString, currentMDFile);
  mw_one->m_Notes->MD2Html(currentMDFile);
  mw_one->m_Notes->loadNoteToQML();
}

QString Notes::formatMDText(QString text) {
  for (int i = 0; i < 10; i++) text.replace("\n\n\n", "\n\n");

  return text;
}

void Notes::init_all_notes() {
  mw_one->m_NotesList->initNotesList();
  mw_one->m_NotesList->initRecycle();

  // load note
  currentMDFile = mw_one->m_NotesList->getCurrentMDFile();
  if (QFile::exists(currentMDFile)) {
    MD2Html(currentMDFile);
    loadNoteToQML();
  } else {
    loadEmptyNote();
  }

  mw_one->m_Notes->setVPos(-0.01);
}

void Notes::loadEmptyNote() {
  currentMDFile = "";
  MD2Html(currentMDFile);
  loadNoteToQML();
  mw_one->ui->lblNoteName->setText("");
}
