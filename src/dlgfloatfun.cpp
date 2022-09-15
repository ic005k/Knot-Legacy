#include "dlgfloatfun.h"

#include "src/mainwindow.h"
#include "ui_dlgfloatfun.h"
#include "ui_mainwindow.h"

extern MainWindow* mw_one;

dlgFloatFun::dlgFloatFun(QWidget* parent)
    : QDialog(parent), ui(new Ui::dlgFloatFun) {
  ui->setupUi(this);
}

void dlgFloatFun::init() {
  setAttribute(Qt::WA_TranslucentBackground);
  // QPalette pal = palette();
  // QColor color = QColor(Qt::black);
  // pal.setColor(QPalette::Background, color);
  // setPalette(pal);

  setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint |
                 Qt::WindowDoesNotAcceptFocus);
  this->setContentsMargins(1, 1, 1, 1);

  ui->btnAdd->setStyleSheet("border:none");
  ui->btnDel->setStyleSheet("border:none");

  this->setGeometry(
      mw_one->geometry().x() + mw_one->width() - this->width() - 10,
      mw_one->geometry().y() + mw_one->ui->frameMenu->height() +
          mw_one->ui->tabWidget->height() - this->height(),
      this->width(), this->height());

  QPropertyAnimation* m_pAnimation = new QPropertyAnimation();
  m_pAnimation->setTargetObject(ui->btnAdd);
  m_pAnimation->setDuration(1500);
  QGraphicsOpacityEffect* m_pOpacity = new QGraphicsOpacityEffect();
  ui->btnAdd->setGraphicsEffect(m_pOpacity);
  m_pOpacity->setOpacity(1);
  m_pAnimation->setTargetObject(m_pOpacity);
  m_pAnimation->setPropertyName("opacity");
  m_pAnimation->setStartValue(0);
  m_pAnimation->setEndValue(1);
  // m_pAnimation->start();

  QPropertyAnimation* m_pAnimation2 = new QPropertyAnimation();
  m_pAnimation2->setTargetObject(ui->btnDel);
  m_pAnimation2->setDuration(1500);
  QGraphicsOpacityEffect* m_pOpacity2 = new QGraphicsOpacityEffect();
  ui->btnDel->setGraphicsEffect(m_pOpacity2);
  m_pOpacity2->setOpacity(1);
  m_pAnimation2->setTargetObject(m_pOpacity2);
  m_pAnimation2->setPropertyName("opacity");
  m_pAnimation2->setStartValue(0);
  m_pAnimation2->setEndValue(1);
  // m_pAnimation2->start();

  this->show();

  qDebug() << "floatfun=" << this->x(), this->y();
}

bool dlgFloatFun::eventFilter(QObject* watch, QEvent* evn) {
  if (evn->type() == QEvent::KeyPress) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      return true;
    }
  }

  return QWidget::eventFilter(watch, evn);
}

void dlgFloatFun::closeEvent(QCloseEvent* event) {
  Q_UNUSED(event);

  /*QPropertyAnimation* m_pAnimation = new QPropertyAnimation();
  m_pAnimation->setTargetObject(this);
  m_pAnimation->setDuration(2000);
  QGraphicsOpacityEffect* m_pOpacity = new QGraphicsOpacityEffect();
  this->setGraphicsEffect(m_pOpacity);
  m_pOpacity->setOpacity(1);
  m_pAnimation->setTargetObject(m_pOpacity);
  m_pAnimation->setPropertyName("opacity");
  m_pAnimation->setStartValue(1);
  m_pAnimation->setEndValue(0);
  m_pAnimation->start();*/
}

dlgFloatFun::~dlgFloatFun() { delete ui; }

void dlgFloatFun::on_btnAdd_clicked() { mw_one->on_btnPlus_clicked(); }

void dlgFloatFun::on_btnDel_clicked() { mw_one->on_btnLess_clicked(); }
