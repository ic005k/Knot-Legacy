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
  ui->btnTest->hide();

  QString path = iniDir + "memo/";
  QDir dir;
  dir.mkpath(path);

  m_SetEditText = new dlgSetEditText(this);
  m_Left = new dlgLeft(this);
  m_Right = new dlgRight(this);

  this->installEventFilter(this);
  ui->editSource->installEventFilter(this);
  ui->editSource->viewport()->installEventFilter(this);
  this->setModal(true);

  connect(pAndroidKeyboard, &QInputMethod::visibleChanged, this,
          &dlgMainNotes::on_KVChanged);

  QScroller::grabGesture(ui->editSource, QScroller::LeftMouseButtonGesture);
  ui->editSource->verticalScrollBar()->setStyleSheet(mw_one->vsbarStyleSmall);
  // ui->editSource->setTextInteractionFlags(Qt::NoTextInteraction);
  QPalette pt = palette();
  pt.setBrush(QPalette::Text, Qt::black);
  pt.setBrush(QPalette::Base, QColor(255, 255, 255));
  pt.setBrush(QPalette::Highlight, Qt::red);
  pt.setBrush(QPalette::HighlightedText, Qt::white);
  ui->editSource->setPalette(pt);
  ui->editSource->setStyleSheet("border:none");
  QFontMetrics fm(this->font());
  ui->editSource->setCursorWidth(2);

  QFont f = this->font();
  f.setPointSize(fontSize - 1);
  ui->lblInfo->setFont(f);
  ui->frameFun->setFont(f);

  mw_one->setSCrollPro(ui->editSource);

  connect(ui->editSource->verticalScrollBar(), SIGNAL(valueChanged(int)), this,
          SLOT(editVSBarValueChanged()));

  QValidator* validator =
      new QRegularExpressionValidator(regxNumber, ui->editRow);
  ui->editRow->setValidator(validator);
  ui->editRow->setPlaceholderText(tr("Row"));
  ui->editCol->setValidator(validator);
  ui->editCol->setPlaceholderText(tr("Column"));

  connect(ui->editSource, &QTextEdit::cursorPositionChanged, this,
          &dlgMainNotes::highlightCurrentLine);

  highlightCurrentLine();
  ui->editLineSn->hide();
  ui->editLineSn->setStyleSheet("background-color:#fafafa;");
  ui->editLineSn->verticalScrollBar()->hide();
  ui->editLineSn->horizontalScrollBar()->hide();
  ui->editLineSn->insertPlainText(QStringLiteral("1\n"));
  ui->editLineSn->setFocusPolicy(Qt::NoFocus);
  ui->editLineSn->setContextMenuPolicy(Qt::NoContextMenu);
  QFont font;
  font.setPixelSize(fontSize);
  ui->editLineSn->setFont(font);
  ui->editLineSn->setFixedWidth(ui->editLineSn->font().pixelSize() + 10);
  lastLine = 1;
  font.setLetterSpacing(QFont::AbsoluteSpacing, 2);  //字间距
  ui->editSource->setFont(font);
  ui->editSource->setAcceptRichText(false);

  connect(ui->editSource, &QTextEdit::textChanged, this,
          &dlgMainNotes::onTextChange);

  timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(showFunPanel()));

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

void dlgMainNotes::init() {
  this->setGeometry(mw_one->geometry().x(), mw_one->geometry().y(),
                    mw_one->width(), mw_one->height());
}

void dlgMainNotes::wheelEvent(QWheelEvent* e) {}

dlgMainNotes::~dlgMainNotes() { delete ui; }

void dlgMainNotes::keyReleaseEvent(QKeyEvent* event) { event->accept(); }

void dlgMainNotes::editVSBarValueChanged() {
  if (!ui->editLineSn->isHidden()) {
    ui->editLineSn->verticalScrollBar()->setValue(
        ui->editSource->verticalScrollBar()->value());
  }
}

void dlgMainNotes::resizeEvent(QResizeEvent* event) {
  Q_UNUSED(event);

  if (isShow) {
    if (this->height() != mw_one->mainHeight) {
      newHeight = this->height();
      androidKeyH = mw_one->mainHeight - newHeight;

      QSettings Reg(iniDir + "android.ini", QSettings::IniFormat);
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

void dlgMainNotes::on_btnBack_clicked() {
  if (!m_SetEditText->isHidden()) {
    m_SetEditText->close();
  }
  pAndroidKeyboard->hide();
  mw_one->Sleep(100);

  saveMainNotes();
  saveQMLVPos();
  loadMemoQML();
  setVPos();
  close();
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

  mw_one->TextEditToFile(ui->editSource, mw_one->mydlgNotesList->currentMDFile);
  MD2Html(mw_one->mydlgNotesList->currentMDFile);

  QString htmlFileName = iniDir + "memo/memo.html";
  QTextEdit* edit = new QTextEdit;
  QPlainTextEdit* edit1 = new QPlainTextEdit;
  QString strhtml = mw_one->loadText(htmlFileName);
  strhtml = strhtml.replace("><", ">\n<");
  edit->setPlainText(strhtml);

  for (int i = 0; i < edit->document()->lineCount(); i++) {
    QString str = mw_one->mydlgReader->getTextEditLineText(edit, i);
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
      qDebug() << "strSrc=" << strSrc << str;

      str = str.replace("width=", "width1=");
      str = str.replace("height=", "height1=");
    }

    edit1->appendPlainText(str);
  }

  mw_one->mydlgReader->TextEditToFile(edit1, htmlFileName);

  Reg.setValue("/MainNotes/CurrentOSIniDir", iniDir);
  Reg.setValue("/MainNotes/editVPos" + mw_one->mydlgNotesList->currentMDFile,
               ui->editSource->verticalScrollBar()->sliderPosition());
  Reg.setValue("/MainNotes/editCPos" + mw_one->mydlgNotesList->currentMDFile,
               ui->editSource->textCursor().position());
}

void dlgMainNotes::init_MainNotes() { loadMemoQML(); }

void dlgMainNotes::on_btnCloseText_clicked() {}

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
  if (obj == ui->editSource->viewport()) {
    getEditPanel(ui->editSource, evn);
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
  if (!pAndroidKeyboard->isVisible()) {
    this->setGeometry(mw_one->geometry().x(), mw_one->geometry().y(),
                      mw_one->width(), mw_one->mainHeight);
  } else {
    QSettings Reg(iniDir + "android.ini", QSettings::IniFormat);
    int newh = Reg.value("newHeight").toInt();
    if (newh > 0) {
      this->setGeometry(mw_one->geometry().x(), mw_one->geometry().y(),
                        mw_one->width(), newh);
      if (!m_SetEditText->isHidden()) {
        m_SetEditText->setGeometry(m_SetEditText->geometry().x(), 10,
                                   m_SetEditText->width(),
                                   m_SetEditText->height());
      }
    }
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

void dlgMainNotes::on_btnUndo_clicked() { ui->editSource->undo(); }

void dlgMainNotes::on_btnRedo_clicked() { ui->editSource->redo(); }

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

QString dlgMainNotes::getDateTimeStr() {
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
    edit->setAcceptRichText(false);
    edit->setPlainText(str);
    mw_one->TextEditToFile(edit, file);
  }

  mw_one->ui->quickWidgetMemo->setSource(
      QUrl(QStringLiteral("qrc:/src/memo.qml")));
  QQuickItem* root = mw_one->ui->quickWidgetMemo->rootObject();
  // QMetaObject::invokeMethod((QObject*)root, "loadHtml", Q_ARG(QVariant,
  // file));
  QMetaObject::invokeMethod((QObject*)root, "loadHtmlBuffer",
                            Q_ARG(QVariant, str));

  getVHeight();
}

void dlgMainNotes::saveQMLVPos() {
  QSettings Reg(iniDir + "mainnotes.ini", QSettings::IniFormat);
  Reg.setValue("/MainNotes/SlidePos" + mw_one->mydlgNotesList->currentMDFile,
               sliderPos);
}

void dlgMainNotes::setVPos() {
  QSettings Reg(iniDir + "mainnotes.ini", QSettings::IniFormat);
  sliderPos =
      Reg.value("/MainNotes/SlidePos" + mw_one->mydlgNotesList->currentMDFile)
          .toReal();
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

void dlgMainNotes::on_editSource_redoAvailable(bool b) {
  if (b)
    ui->btnRedo->setEnabled(true);
  else
    ui->btnRedo->setEnabled(false);
}

void dlgMainNotes::on_editSource_undoAvailable(bool b) {
  if (b)
    ui->btnUndo->setEnabled(true);
  else
    ui->btnUndo->setEnabled(false);
}

void dlgMainNotes::on_btnSeparator_clicked() {
  ui->editSource->insertPlainText("-");
}

void dlgMainNotes::on_btnWells_clicked() {
  ui->editSource->insertPlainText("#");
}

void dlgMainNotes::on_btnVLine_clicked() {
  ui->editSource->insertPlainText("|");
}

void dlgMainNotes::on_btnAsterisk_clicked() {
  ui->editSource->insertPlainText("*");
}

void dlgMainNotes::on_btnS1_clicked() {
  QString str = ui->editSource->textCursor().selectedText();
  if (str == "") str = tr("Bold Italic");
  ui->editSource->insertPlainText("_**" + str + "**_");
}

void dlgMainNotes::on_btnS2_clicked() {
  QString str = ui->editSource->textCursor().selectedText();
  if (str == "") str = tr("Italic");
  ui->editSource->insertPlainText("_" + str + "_");
}

void dlgMainNotes::on_btnS3_clicked() {
  QString str = ui->editSource->textCursor().selectedText();
  if (str == "") str = tr("Underline");
  ui->editSource->insertPlainText("<u>" + str + "</u>");
}

void dlgMainNotes::on_btnS4_clicked() {
  QString str = ui->editSource->textCursor().selectedText();
  if (str == "") str = tr("Strickout");
  ui->editSource->insertPlainText("~~" + str + "~~");
}

void dlgMainNotes::on_btnColor_clicked() {
  QString str = ui->editSource->textCursor().selectedText();
  if (str == "") str = tr("Red");
  ui->editSource->insertPlainText("<font color=#FF0000 >" + str + "</font>");
}

void dlgMainNotes::on_btnS5_clicked() {
  QString str = ui->editSource->textCursor().selectedText();
  if (str == "") str = tr("Bold");
  ui->editSource->insertPlainText("**" + str + "**");
}

void dlgMainNotes::on_btnLink_clicked() {
  ui->editSource->insertPlainText("[Knot](https://github.com/ic005k/Knot)");
}

void dlgMainNotes::on_btnS6_clicked() { ui->editSource->insertPlainText("~"); }

void dlgMainNotes::on_btnS7_clicked() {
  ui->editSource->insertPlainText("[ ]");
}

void dlgMainNotes::on_btnS8_clicked() {
  ui->editSource->insertPlainText("( )");
}

void dlgMainNotes::on_btnS9_clicked() {
  ui->editSource->insertPlainText("{ }");
}

void dlgMainNotes::on_btnS10_clicked() { ui->editSource->insertPlainText("_"); }

void dlgMainNotes::highlightCurrentLine() {
  QList<QTextEdit::ExtraSelection> extraSelections;

  QTextEdit::ExtraSelection selection;

  QColor lineColor = QColor(Qt::yellow).lighter(160);

  selection.format.setBackground(lineColor);
  selection.format.setProperty(QTextFormat::FullWidthSelection, true);
  selection.cursor = ui->editSource->textCursor();
  selection.cursor.clearSelection();
  extraSelections.append(selection);

  ui->editSource->setExtraSelections(extraSelections);

  if (!ui->editLineSn->isHidden()) {
    ui->editLineSn->blockSignals(true);
    ui->editSource->blockSignals(true);
    ui->editLineSn->verticalScrollBar()->setValue(
        ui->editSource->verticalScrollBar()->value());
    ui->editLineSn->blockSignals(false);
    ui->editSource->blockSignals(false);
  }

  QString str1, str2, str3, str4;

  //当前光标
  QTextCursor tc = ui->editSource->textCursor();
  QTextLayout* lay = tc.block().layout();
  //当前光标在本BLOCK内的相对位置
  int iCurPos = tc.position() - tc.block().position();
  //光标所在行号
  int iCurrentLine = lay->lineForTextPosition(iCurPos).lineNumber() +
                     tc.block().firstLineNumber();
  int iLineCount = ui->editSource->document()->lineCount();
  //或者  获取光标所在行的行号
  int iRowNum = tc.blockNumber() + 1;

  str1 = QString::number(iLineCount);
  str2 = QString::number(ui->editSource->textCursor().position());
  str3 = QString::number(iCurPos);
  str4 = QString::number(iRowNum);
  ui->lblInfo->setText(" " + str4 + " , " + str3);
}

void dlgMainNotes::onTextChange() {
  if (ui->editLineSn->isHidden()) return;

  int jsonTextEditRow = ui->editSource->document()->lineCount();
  if (jsonTextEditRow == lastLine) return;

  ui->editLineSn->blockSignals(true);
  ui->editSource->blockSignals(true);

  int countOfRow = 0;
  int temp = jsonTextEditRow;
  while (temp != 0) {
    temp = temp / 10;
    ++countOfRow;
  }
  ui->editLineSn->setFixedWidth(
      ui->editLineSn->font().pixelSize() * countOfRow + 10);

  ui->editLineSn->clear();
  QString str;
  ++jsonTextEditRow;
  for (int i = 1; i < jsonTextEditRow; ++i) {
    str.append(QString("%1\n").arg(i));
  }
  ui->editLineSn->setPlainText(str);

  lastLine = ui->editSource->document()->lineCount();

  ui->editLineSn->verticalScrollBar()->setValue(
      ui->editSource->verticalScrollBar()->value());
  ui->editLineSn->blockSignals(false);
  ui->editSource->blockSignals(false);
}

void dlgMainNotes::openMD(QString mdFileName) {}

void dlgMainNotes::saveMD(QString mdFileName) {}

void dlgMainNotes::on_btnPaste_clicked() { ui->editSource->paste(); }

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

void dlgMainNotes::timerSlot() {
  if (bCursorVisible) {
    bCursorVisible = false;
  } else {
    bCursorVisible = true;
  }

  emit sendUpdate();
}

void dlgMainNotes::on_btnLeft_clicked() {
  ui->editSource->moveCursor(QTextCursor::PreviousCharacter,
                             QTextCursor::MoveAnchor);
}

void dlgMainNotes::on_btnRight_clicked() {
  ui->editSource->moveCursor(QTextCursor::NextCharacter,
                             QTextCursor::MoveAnchor);
}
