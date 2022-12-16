#include "dlgleft.h"

#include "src/mainwindow.h"
#include "ui_dlgleft.h"
#include "ui_mainwindow.h"

extern MainWindow* mw_one;

dlgLeft::dlgLeft(QWidget* parent) : QDialog(parent), ui(new Ui::dlgLeft) {
  ui->setupUi(this);
  this->installEventFilter(this);
  setModal(true);
}

dlgLeft::~dlgLeft() { delete ui; }

void dlgLeft::init() {
  this->setContentsMargins(1, 1, 1, 1);

  this->setGeometry(mw_one->geometry().x(), mw_one->geometry().y(),
                    mw_one->width(), mw_one->height() / 2);
  show();
}

bool dlgLeft::eventFilter(QObject* obj, QEvent* evn) {
  QMouseEvent* event = static_cast<QMouseEvent*>(evn);

  if (event->type() == QEvent::MouseButtonPress) {
    isMousePress = true;
    iMouseMove = false;
  }

  if (event->type() == QEvent::MouseButtonRelease) {
    isMouseRelease = true;
    isMousePress = false;
    iMouseMove = false;
  }

  if (event->type() == QEvent::MouseMove) {
    iMouseMove = true;
  }

  if (evn->type() == QEvent::KeyPress) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      close();
      return true;
    }
  }

  return QWidget::eventFilter(obj, evn);
}

void dlgLeft::on_btnClose_clicked() {
  close();
  ui->textBrowser->clear();
}
