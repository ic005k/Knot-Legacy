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

  connect(pAndroidKeyboard, &QInputMethod::visibleChanged, this,
          &dlgMainNotes::on_KVChanged);

  QScroller::grabGesture(ui->textEdit, QScroller::LeftMouseButtonGesture);
  ui->textEdit->verticalScrollBar()->setStyleSheet(mw_one->vsbarStyleSmall);
  ui->textEdit->installEventFilter(this);
  this->installEventFilter(this);

  vScrollBar = new SmoothScrollBar();
  vScrollBar->setOrientation(Qt::Orientation::Vertical);  //将滚动条设置为纵向
  // ui->textBrowser->setVerticalScrollBar(vScrollBar);
  QScroller::grabGesture(ui->textBrowser, QScroller::LeftMouseButtonGesture);
  ui->textBrowser->verticalScrollBar()->setStyleSheet(mw_one->vsbarStyleSmall);
  ui->textBrowser->setTextInteractionFlags(Qt::NoTextInteraction);
  ui->textBrowser->setHidden(true);

  ui->btnOpenText->hide();
  ui->btnCloseText->hide();
  ui->btnLastBrowse->hide();
  ui->btnStartEdit->hide();
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
  // Notes
  if (!ui->textEdit->isHidden()) {
    Reg.setValue("/MainNotes/Text", ui->textEdit->toPlainText());
    curPos = ui->textEdit->textCursor().position();
    sliderPos = ui->textEdit->verticalScrollBar()->sliderPosition();
    Reg.setValue("/MainNotes/CurPos", curPos);
    Reg.setValue("/MainNotes/SlidePos", sliderPos);
  }
}

void dlgMainNotes::init_MainNotes() {
  QString ini_file;
  if (isImport)
    ini_file = iniFile;
  else
    ini_file = iniDir + "mainnotes.ini";
  QSettings Reg(ini_file, QSettings::IniFormat);

  ui->textEdit->setPlainText(Reg.value("/MainNotes/Text").toString());
  sliderPos = Reg.value("/MainNotes/SlidePos").toLongLong();
  curPos = Reg.value("/MainNotes/CurPos").toLongLong();

  QTextCursor tmpCursor = ui->textEdit->textCursor();
  tmpCursor.setPosition(curPos);
  ui->textEdit->setTextCursor(tmpCursor);
  ui->textEdit->verticalScrollBar()->setSliderPosition(sliderPos);
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

void dlgMainNotes::on_textBrowser_cursorPositionChanged() {
  sliderPos = ui->textBrowser->verticalScrollBar()->sliderPosition();
}

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

void dlgMainNotes::on_btnStartEdit_clicked() {
  if (ui->btnStartEdit->text() == tr("Start")) {
    this->setFixedHeight(mw_one->height() / 2);
    ui->btnStartEdit->setText(tr("End"));
    ui->textEdit->setReadOnly(false);
    ui->textEdit->setTextInteractionFlags(Qt::TextEditable);
    pAndroidKeyboard->show();
    qDebug() << pAndroidKeyboard->keyboardRectangle().height();
  } else {
    this->setFixedHeight(mw_one->height());
    ui->btnStartEdit->setText(tr("Start"));
    ui->textEdit->setReadOnly(true);
    ui->textEdit->setTextInteractionFlags(Qt::NoTextInteraction);
    pAndroidKeyboard->hide();
  }
}

void dlgMainNotes::on_KVChanged() {
  if (pAndroidKeyboard->isVisible()) {
    this->setFixedHeight(mw_one->height() * 2 / 3 + 10);
  } else {
    this->setFixedHeight(mw_one->height());
  }
}
