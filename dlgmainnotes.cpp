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

  QScroller::grabGesture(ui->textBrowser, QScroller::LeftMouseButtonGesture);
  ui->textBrowser->verticalScrollBar()->setStyleSheet(mw_one->vsbarStyleSmall);
  ui->textBrowser->setTextInteractionFlags(Qt::NoTextInteraction);
  ui->textBrowser->setHidden(true);
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

bool dlgMainNotes::eventFilter(QObject* obj, QEvent* event) {
  if (obj == ui->textEdit) {
    if (event->type() == QInputMethodEvent::Enter) {
      qDebug() << "Show";
    }
    if (event->type() == QInputMethodEvent::Close) {
      qDebug() << "Close";
    }
  }

  return QWidget::eventFilter(obj, event);
}

void dlgMainNotes::saveMainNotes(bool isOpenText) {
  return;

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

  ui->lblInfo->setText(tr("Position") + " : " + QString::number(sliderPos));
}

void dlgMainNotes::init_MainNotes(bool isOpenText) {
  return;

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

  ui->lblInfo->setText(tr("Position") + " : " + QString::number(sliderPos));
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

  ui->lblInfo->setText(tr("Position") + " : " + QString::number(sliderPos));
}
