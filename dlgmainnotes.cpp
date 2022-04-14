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
}

void dlgMainNotes::wheelEvent(QWheelEvent* e) {
  //当捕获到事件后，调用相对滚动的槽函数
  vScrollBar->scroll(e->angleDelta().y());
}

dlgMainNotes::~dlgMainNotes() { delete ui; }

void dlgMainNotes::keyReleaseEvent(QKeyEvent* event) { event->accept(); }

void dlgMainNotes::resizeEvent(QResizeEvent* event) {
  Q_UNUSED(event);
  qDebug() << "Resize: " << ui->textEdit->height() << this->height();
}

void dlgMainNotes::on_btnBack_clicked() {
  if (ui->textEdit->isHidden()) isOpenText = true;
  if (ui->textBrowser->isHidden()) isOpenText = false;

  saveMainNotes(isOpenText);
  close();
}

void dlgMainNotes::saveMainNotes(bool isOpenText) {
  QSettings Reg(iniDir + "mainnotes.ini", QSettings::IniFormat);
  Reg.setValue("/MainNotes/isOpenText", isOpenText);

  // Notes
  if (!ui->textEdit->isHidden()) {
    Reg.setValue("/MainNotes/Text", ui->textEdit->toPlainText());
    curPos = ui->textEdit->textCursor().position();
    sliderPos = ui->textEdit->verticalScrollBar()->sliderPosition();
    Reg.setValue("/MainNotes/CurPos", curPos);
    Reg.setValue("/MainNotes/SlidePos", sliderPos);
  }

  // Browse Text
  if (!ui->textBrowser->isHidden()) {
    Reg.setValue("/MainNotes/FileName", fileName);
    curPos = ui->textBrowser->textCursor().position();
    sliderPos = ui->textBrowser->verticalScrollBar()->sliderPosition();
    Reg.setValue("/MainNotes/" + fileName + "-OpenCurPos", curPos);
    Reg.setValue("/MainNotes/" + fileName + "-OpenSlidePos", sliderPos);
  }
}

void dlgMainNotes::init_MainNotes(bool isOpenText) {
  QString ini_file;
  if (isImport)
    ini_file = iniFile;
  else
    ini_file = iniDir + "mainnotes.ini";
  QSettings Reg(ini_file, QSettings::IniFormat);

  if (!isOpenText) {
    ui->textEdit->setPlainText(Reg.value("/MainNotes/Text").toString());
    sliderPos = Reg.value("/MainNotes/SlidePos").toLongLong();
    curPos = Reg.value("/MainNotes/CurPos").toLongLong();

    QTextCursor tmpCursor = ui->textEdit->textCursor();
    tmpCursor.setPosition(curPos);
    ui->textEdit->setTextCursor(tmpCursor);
    ui->textEdit->verticalScrollBar()->setSliderPosition(sliderPos);

    ui->textBrowser->setHidden(true);
    ui->textEdit->setHidden(false);

  } else {
    fileName = Reg.value("/MainNotes/FileName").toString();
    ui->textBrowser->setPlainText(mw_one->loadText(fileName));
    sliderPos =
        Reg.value("/MainNotes/" + fileName + "-OpenSlidePos").toLongLong();
    curPos = Reg.value("/MainNotes/" + fileName + "-OpenCurPos").toLongLong();

    QTime dieTime = QTime::currentTime().addMSecs(100);
    while (QTime::currentTime() < dieTime) {
      QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
    ui->textBrowser->verticalScrollBar()->setSliderPosition(sliderPos);

    ui->textBrowser->setHidden(false);
    ui->textEdit->setHidden(true);
  }
}

void dlgMainNotes::on_btnOpenText_clicked() {
  fileName = QFileDialog::getOpenFileName(this, tr("Xcounter"), "",
                                          tr("Txt Files (*.*)"));
  if (!fileName.isNull()) {
    QString txt = mw_one->loadText(fileName);
    ui->textBrowser->setPlainText(txt);
    isOpenText = true;

    ui->textBrowser->setHidden(false);
    ui->textEdit->setHidden(true);
  }
}

void dlgMainNotes::on_btnCloseText_clicked() {
  saveMainNotes(true);

  init_MainNotes(false);
}

void dlgMainNotes::on_btnLastBrowse_clicked() {
  saveMainNotes(false);

  init_MainNotes(true);
}

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
