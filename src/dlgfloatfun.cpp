#include "dlgfloatfun.h"

#include "src/mainwindow.h"
#include "ui_dlgfloatfun.h"
#include "ui_mainwindow.h"

extern MainWindow* mw_one;

dlgFloatFun::dlgFloatFun(QWidget* parent)
    : QDialog(parent), ui(new Ui::dlgFloatFun) {
  ui->setupUi(this);

  int s = 28;
  ui->btnAdd->setFixedHeight(s);
  ui->btnAdd->setFixedWidth(s);
  ui->btnDel->setFixedHeight(s);
  ui->btnDel->setFixedWidth(s);
}

void dlgFloatFun::init() {
  // setAttribute(Qt::WA_TranslucentBackground);

  setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint |
                 Qt::WindowDoesNotAcceptFocus);

  QPalette pal = palette();
  QColor color = QColor(Qt::Key_Blue);
  pal.setColor(QPalette::Background, color);
  // setPalette(pal);

  this->setContentsMargins(1, 1, 1, 1);

  ui->btnAdd->setStyleSheet("border:none");
  ui->btnDel->setStyleSheet("border:none");

  this->setGeometry(
      mw_one->geometry().x() + mw_one->width() - this->width() - 14,
      mw_one->geometry().y() + mw_one->ui->frameMenu->height() +
          mw_one->ui->tabWidget->height() - this->height(),
      this->width(), this->height());

  QPropertyAnimation* m_pAnimation = new QPropertyAnimation();
  m_pAnimation->setTargetObject(this);
  m_pAnimation->setDuration(500);
  QGraphicsOpacityEffect* m_pOpacity = new QGraphicsOpacityEffect();
  this->setGraphicsEffect(m_pOpacity);
  m_pOpacity->setOpacity(1);
  m_pAnimation->setTargetObject(m_pOpacity);
  m_pAnimation->setPropertyName("opacity");
  m_pAnimation->setStartValue(0);
  m_pAnimation->setEndValue(1);
  // m_pAnimation->start();

  this->show();

  qDebug() << "floatfun=" << this->x(), this->y();
}

void dlgFloatFun::setY(int y) {
  this->setGeometry(
      mw_one->geometry().x() + mw_one->width() - this->width() - 10, y,
      this->width(), this->height());
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

  QPropertyAnimation* m_pAnimation = new QPropertyAnimation();
  m_pAnimation->setTargetObject(this);
  m_pAnimation->setDuration(2000);
  QGraphicsOpacityEffect* m_pOpacity = new QGraphicsOpacityEffect();
  this->setGraphicsEffect(m_pOpacity);
  m_pOpacity->setOpacity(1);
  m_pAnimation->setTargetObject(m_pOpacity);
  m_pAnimation->setPropertyName("opacity");
  m_pAnimation->setStartValue(1);
  m_pAnimation->setEndValue(0);
  // m_pAnimation->start();
}

void dlgFloatFun::paintEvent(QPaintEvent* event) {
  Q_UNUSED(event);
  return;

  /*QStyleOption opt;
  opt.init(this);
  QPainter painter(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);*/

  QPainter painter(this);
  painter.fillRect(this->rect(), QColor(0, 0, 0, 0));
  QWidget::paintEvent(event);
}

dlgFloatFun::~dlgFloatFun() { delete ui; }

void dlgFloatFun::on_btnAdd_clicked() { mw_one->on_btnPlus_clicked(); }

void dlgFloatFun::on_btnDel_clicked() { mw_one->on_btnLess_clicked(); }
