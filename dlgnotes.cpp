#include "dlgnotes.h"

#include "mainwindow.h"
#include "ui_dlgnotes.h"
#include "ui_mainwindow.h"
extern MainWindow* mw_one;
extern bool loading;
extern QString noteText;
extern int curPos;
extern int sliderPos;

dlgNotes::dlgNotes(QWidget* parent) : QDialog(parent), ui(new Ui::dlgNotes) {
  ui->setupUi(this);
  this->installEventFilter(this);
  //鼠标不可选中文本
  ui->textBrowser->setTextInteractionFlags(Qt::NoTextInteraction);
  QScroller::grabGesture(ui->textEdit, QScroller::LeftMouseButtonGesture);
  QScroller::grabGesture(ui->textBrowser, QScroller::LeftMouseButtonGesture);
}

dlgNotes::~dlgNotes() { delete ui; }

void dlgNotes::on_btnBack_clicked() {
  noteText = ui->textEdit->toPlainText();
  curPos = ui->textEdit->textCursor().position();
  sliderPos = ui->textEdit->verticalScrollBar()->sliderPosition();
  if (!ui->textEdit->isHidden()) MainWindow::saveNotes();
  ui->textEdit->clear();
  close();
}

void dlgNotes::on_textEdit_textChanged() {}

bool dlgNotes::eventFilter(QObject* obj, QEvent* event) {
  return QWidget::eventFilter(obj, event);
  if (event->type() == QEvent::ActivationChange) {
    if (QApplication::activeWindow() != this) {
      this->close();
    }
  }

  return QWidget::eventFilter(obj, event);
}

void dlgNotes::keyReleaseEvent(QKeyEvent* event) { event->accept(); }

void dlgNotes::resizeEvent(QResizeEvent* event) {
  Q_UNUSED(event);
  qDebug() << "resize" << ui->textEdit->height();
}
