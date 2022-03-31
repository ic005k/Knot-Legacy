#include "dlgmainnotes.h"

#include "mainwindow.h"
#include "ui_dlgmainnotes.h"
#include "ui_mainwindow.h"

extern MainWindow* mw_one;
extern QString iniDir;

dlgMainNotes::dlgMainNotes(QWidget* parent)
    : QDialog(parent), ui(new Ui::dlgMainNotes) {
  ui->setupUi(this);

  QScroller::grabGesture(ui->textEdit, QScroller::LeftMouseButtonGesture);
  ui->textEdit->verticalScrollBar()->setStyleSheet(mw_one->vsbarStyleSmall);
  ui->textEdit->installEventFilter(this);
  this->installEventFilter(this);
}

dlgMainNotes::~dlgMainNotes() { delete ui; }

void dlgMainNotes::keyReleaseEvent(QKeyEvent* event) { event->accept(); }

void dlgMainNotes::resizeEvent(QResizeEvent* event) {
  Q_UNUSED(event);
  qDebug() << "resize" << ui->textEdit->height() << this->height();
}

void dlgMainNotes::on_btnBack_clicked() {
  saveMainNotes();
  close();
}

bool dlgMainNotes::eventFilter(QObject* obj, QEvent* event) {
  if (obj == ui->textEdit) {
    if (event->type() == QInputMethodEvent::ShowToParent) {
      // this->setFixedHeight(mw_one->height() / 2);
      qDebug() << "Show";
    }
    if (event->type() == QInputMethodEvent::HideToParent) {
      // this->setFixedHeight(mw_one->height());
      qDebug() << "Close";
    }
  }

  return QWidget::eventFilter(obj, event);
}

void dlgMainNotes::saveMainNotes() {
  QSettings Reg(iniDir + "mainnotes.ini", QSettings::IniFormat);
  Reg.setValue("/MainNotes/isOpenText", isOpenText);
  if (!isOpenText) {
    Reg.setValue("/MainNotes/Text", ui->textEdit->toPlainText());
    curPos = ui->textEdit->textCursor().position();
    sliderPos = ui->textEdit->verticalScrollBar()->sliderPosition();
    Reg.setValue("/MainNotes/CurPos", curPos);
    Reg.setValue("/MainNotes/SlidePos", sliderPos);
  } else {
    Reg.setValue("/MainNotes/OpenText", ui->textEdit->toPlainText());
    curPos = ui->textEdit->textCursor().position();
    sliderPos = ui->textEdit->verticalScrollBar()->sliderPosition();
    Reg.setValue("/MainNotes/OpenCurPos", curPos);
    Reg.setValue("/MainNotes/OpenSlidePos", sliderPos);
  }
}

void dlgMainNotes::init_MainNotes() {
  QSettings Reg(iniDir + "mainnotes.ini", QSettings::IniFormat);
  isOpenText = Reg.value("/MainNotes/isOpenText").toBool();
  if (!isOpenText) {
    ui->textEdit->setPlainText(Reg.value("/MainNotes/Text").toString());
    sliderPos = Reg.value("/MainNotes/SlidePos").toInt();
    curPos = Reg.value("/MainNotes/CurPos").toInt();

  } else {
    ui->textEdit->setPlainText(Reg.value("/MainNotes/OpenText").toString());
    sliderPos = Reg.value("/MainNotes/OpenSlidePos").toInt();
    curPos = Reg.value("/MainNotes/OpenCurPos").toInt();
  }

  QTextCursor tmpCursor = ui->textEdit->textCursor();
  tmpCursor.setPosition(curPos);
  ui->textEdit->setTextCursor(tmpCursor);
  ui->textEdit->verticalScrollBar()->setSliderPosition(sliderPos);

  if (!isOpenText) {
    ui->textEdit->setReadOnly(false);
    ui->textEdit->setTextInteractionFlags(Qt::TextEditorInteraction);
  } else {
    ui->textEdit->setReadOnly(true);
    ui->textEdit->setTextInteractionFlags(Qt::NoTextInteraction);
  }
}

void dlgMainNotes::on_btnOpenText_clicked() {
  QString fileName;
  fileName = QFileDialog::getOpenFileName(this, tr("Xcounter"), "",
                                          tr("Txt Files (*.*)"));
  if (!fileName.isNull()) {
    ui->textEdit->setPlainText(mw_one->loadText(fileName));
    isOpenText = true;
    ui->textEdit->setReadOnly(true);
    ui->textEdit->setTextInteractionFlags(Qt::NoTextInteraction);
  }
}

void dlgMainNotes::on_btnCloseText_clicked() {
  isOpenText = false;
  QSettings Reg(iniDir + "mainnotes.ini", QSettings::IniFormat);
  Reg.setValue("/MainNotes/isOpenText", isOpenText);
  init_MainNotes();
}
