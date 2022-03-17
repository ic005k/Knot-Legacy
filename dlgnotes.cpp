#include "dlgnotes.h"

#include <mainwindow.h>

#include "ui_dlgnotes.h"
extern MainWindow* mw_one;
extern bool loading;

dlgNotes::dlgNotes(QWidget* parent) : QDialog(parent), ui(new Ui::dlgNotes) {
  ui->setupUi(this);
  this->installEventFilter(this);
}

dlgNotes::~dlgNotes() { delete ui; }

void dlgNotes::on_btnBack_clicked() {
  mw_one->saveNotes();
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
