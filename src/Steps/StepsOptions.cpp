#include "StepsOptions.h"

#include "src/MainWindow.h"
#include "ui_StepsOptions.h"

extern MainWindow *mw_one;
extern Method *m_Method;

StepsOptions::StepsOptions(QWidget *parent)
    : QDialog(parent), ui(new Ui::StepsOptions) {
  ui->setupUi(this);
  this->installEventFilter(this);
  mw_one->set_ToolButtonStyle(this);
  setModal(true);
}

StepsOptions::~StepsOptions() { delete ui; }

void StepsOptions::closeEvent(QCloseEvent *event) {
  Q_UNUSED(event);
  m_Method->closeGrayWindows();
}

bool StepsOptions::eventFilter(QObject *obj, QEvent *evn) {
  if (evn->type() == QEvent::KeyRelease) {
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      on_btnBack_clicked();
      return true;
    }
  }

  return QWidget::eventFilter(obj, evn);
}

void StepsOptions::init() {
  int x, y, w, h;
  x = mw_one->geometry().x();
  y = 0;
  w = mw_one->width();
  h = this->height();
  setGeometry(x, y, w, h);

  m_Method->m_widget = new QWidget(mw_one);
  m_Method->showGrayWindows();
  show();
}

void StepsOptions::on_btnBack_clicked() { close(); }
