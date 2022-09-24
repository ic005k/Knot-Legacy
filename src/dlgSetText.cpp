#include "dlgSetText.h"

#include "ui_dlgSetText.h"

dlgSetText::dlgSetText(QWidget* parent)
    : QDialog(parent), ui(new Ui::dlgSetText) {
  ui->setupUi(this);
}

dlgSetText::~dlgSetText() { delete ui; }

void dlgSetText::init(int x, int y, int w, int h) {
  setGeometry(x, y, w, h);
  // setWindowFlags(Qt::WindowStaysOnTopHint);

  setWindowFlags(Qt::WindowStaysOnTopHint | Qt::Tool | Qt::FramelessWindowHint);
  ui->textEdit->clear();
  show();
}

bool dlgSetText::eventFilter(QObject* watch, QEvent* evn) {
  if (evn->type() == QEvent::KeyPress) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      close();
      return true;
    }
  }

  return QWidget::eventFilter(watch, evn);
}
