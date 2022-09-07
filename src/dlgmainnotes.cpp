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

  ui->btnOpenText->hide();
  ui->btnCloseText->hide();
  ui->btnLastBrowse->hide();

  ui->edit1->setEchoMode(QLineEdit::Password);
  ui->edit2->setEchoMode(QLineEdit::Password);
  ui->frameSetKey->hide();
  ui->btnSetKey->hide();
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

void dlgMainNotes::resizeEvent(QResizeEvent* event) {
  Q_UNUSED(event);
  if (this->height() == mw_one->height()) return;
  if (!one) {
    one = true;
    newHeight = this->height();
  }
  qDebug() << pAndroidKeyboard->keyboardRectangle().height() << newHeight
           << this->height();
}

void dlgMainNotes::on_btnBack_clicked() {
  saveMainNotes();
  close();
}

void dlgMainNotes::saveMainNotes() {
  QSettings Reg(iniDir + "mainnotes.ini", QSettings::IniFormat);

  sliderPos = ui->textEdit->verticalScrollBar()->sliderPosition();
  QQuickItem* root = mw_one->ui->quickWidgetMemo->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "setVPos",
                            Q_ARG(QVariant, sliderPos));
  mw_one->ui->quickWidgetMemo->rootContext()->setContextProperty(
      "strText", ui->textEdit->toPlainText());

  Reg.setValue("/MainNotes/CurPos", curPos);
  Reg.setValue("/MainNotes/SlidePos", sliderPos);

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

void dlgMainNotes::on_btnOpenText_clicked() {
  fileName = QFileDialog::getOpenFileName(this, tr("Xcounter"), "",
                                          tr("Txt Files (*.*)"));
  if (!fileName.isNull()) {
    QString txt = mw_one->loadText(fileName);
    ui->textBrowser->setPlainText(txt);

    ui->textBrowser->setHidden(false);
    ui->textEdit->setHidden(true);
  }
}

void dlgMainNotes::on_btnCloseText_clicked() {}

void dlgMainNotes::on_btnLastBrowse_clicked() {}

void dlgMainNotes::on_textBrowser_cursorPositionChanged() {}

bool dlgMainNotes::eventFilter(QObject* obj, QEvent* evn) {
  if (evn->type() == QEvent::KeyPress) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      on_btnBack_clicked();
      return true;
    }
  }

  return QWidget::eventFilter(obj, evn);
}

void dlgMainNotes::on_KVChanged() {
  if (pAndroidKeyboard->isVisible()) {
    this->setFixedHeight(mw_one->height() * 2 / 3 + 10);
  } else {
    this->setFixedHeight(mw_one->height());
  }
}

void dlgMainNotes::on_btnSetKey_clicked() {
  if (ui->frameSetKey->isHidden())
    ui->frameSetKey->show();
  else
    ui->frameSetKey->hide();
}

void dlgMainNotes::on_btnOK_clicked() {
  QSettings Reg(iniDir + "mainnotes.ini", QSettings::IniFormat);
  if (ui->edit1->text().trimmed() == "" && ui->edit2->text().trimmed() == "") {
    Reg.remove("/MainNotes/UserKey");
    ui->frameSetKey->hide();
    QMessageBox msgBox;
    msgBox.setText("Knot");
    msgBox.setInformativeText(tr("The password is removed."));
    QPushButton* btnOk = msgBox.addButton(tr("Ok"), QMessageBox::AcceptRole);
    btnOk->setFocus();
    msgBox.exec();

    return;
  }

  if (ui->edit1->text().trimmed() == ui->edit2->text().trimmed()) {
    QString strPw = ui->edit1->text().trimmed();
    QByteArray baPw = strPw.toUtf8();
    for (int i = 0; i < baPw.size(); i++) {
      baPw[i] = baPw[i] + 66;  //加密User的密码
    }
    strPw = baPw;
    Reg.setValue("/MainNotes/UserKey", strPw);

    QMessageBox msgBox;
    msgBox.setText("Knot");
    msgBox.setInformativeText(tr("The password is set successfully."));
    QPushButton* btnOk = msgBox.addButton(tr("Ok"), QMessageBox::AcceptRole);
    btnOk->setFocus();
    msgBox.exec();
    ui->frameSetKey->hide();

  } else {
    QMessageBox msgBox;
    msgBox.setText("Knot");
    msgBox.setInformativeText(tr("The entered password does not match."));
    QPushButton* btnOk = msgBox.addButton(tr("Ok"), QMessageBox::AcceptRole);
    btnOk->setFocus();
    msgBox.exec();
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
