#include "dlgleft.h"

#include "src/mainwindow.h"
#include "ui_dlgleft.h"
#include "ui_mainwindow.h"

extern MainWindow* mw_one;

dlgLeft::dlgLeft(QWidget* parent) : QDialog(parent), ui(new Ui::dlgLeft) {
  ui->setupUi(this);
  this->installEventFilter(this);
  ui->btnLeft->installEventFilter(this);
}

dlgLeft::~dlgLeft() { delete ui; }

void dlgLeft::init(int x, int y) {
  setWindowFlags(Qt::WindowStaysOnTopHint);

#ifdef Q_OS_MAC
  setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint |
                 Qt::WindowDoesNotAcceptFocus);
#endif

  this->setContentsMargins(1, 1, 1, 1);

  ui->btnLeft->setStyleSheet("border:none");

  this->setGeometry(x - width(), y, this->width(), this->height());
  show();
  ;
}

bool dlgLeft::eventFilter(QObject* obj, QEvent* evn) {
  QMouseEvent* event = static_cast<QMouseEvent*>(evn);
  if (obj == ui->btnLeft) {
    int x0 = 0;
    int y0 = 0;
    int x1 = 0;
    int y1 = 0;

    if (event->type() == QEvent::MouseButtonPress) {
      isMousePress = true;
      iMouseMove = false;
      x0 = event->globalX();
      y0 = event->globalY();
    }

    if (event->type() == QEvent::MouseButtonRelease) {
      isMouseRelease = true;
      isMousePress = false;
      iMouseMove = false;
    }

    if (event->type() == QEvent::MouseMove) {
      iMouseMove = true;
      x1 = event->globalX();
      y1 = event->globalY();
      if (isMousePress) {
        setGeometry(event->globalX(), event->globalY(), width(), height());

        if (x1 - x0 <= 0 || y1 - y0 <= 0) mw_one->mydlgMainNotes->start--;
        if (x1 - x0 > 0 || y1 - y0 > 0) mw_one->mydlgMainNotes->start++;
        mw_one->mydlgMainNotes->selectText(mw_one->mydlgMainNotes->start,
                                           mw_one->mydlgMainNotes->end);

        qDebug() << mw_one->mydlgMainNotes->start;
      }
    }

    if (evn->type() == QEvent::KeyPress) {
      QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
      if (keyEvent->key() == Qt::Key_Back) {
        close();
        return true;
      }
    }
  }

  return QWidget::eventFilter(obj, evn);
}
