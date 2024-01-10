#include "RollingBox.h"

#include "MainWindow.h"

extern MainWindow *mw_one;

RollingBox::RollingBox(QWidget *parent)
    : QWidget(parent),
      m_minRange(0),
      m_maxRange(100),
      m_currentValue(50),
      isDragging(0),
      m_deviation(0),
      m_numSize(6) {
  homingAni = new QPropertyAnimation(this, "deviation");
  homingAni->setDuration(300);
  homingAni->setEasingCurve(QEasingCurve::OutBack);  // OutQuad
}

void RollingBox::setRange(int min, int max) {
  m_minRange = min;
  m_maxRange = max;

  if (m_currentValue < min) m_currentValue = min;
  if (m_currentValue > max) m_currentValue = max;

  // 计算字符尺寸
  m_numSize = 8;
  int temp = m_maxRange;
  while (temp > 0) {
    temp /= 10;
    m_numSize++;
  }

  repaint();
}

int RollingBox::readValue() { return m_currentValue; }

void RollingBox::setValue(int value) { m_currentValue = value; }

void RollingBox::mousePressEvent(QMouseEvent *e) {
  homingAni->stop();
  isDragging = 1;
  m_mouseSrcPos = e->pos().x();
}

void RollingBox::mouseMoveEvent(QMouseEvent *e) {
  if (isDragging) {
    // 数值到边界时，阻止继续往对应方向移动
    /* if ((m_currentValue == m_minRange && e->pos().x() >= m_mouseSrcPos) ||
        (m_currentValue == m_maxRange && e->pos().x() <= m_mouseSrcPos)) {
       return;
    } */

    // 循环移动
    if (m_currentValue == m_minRange && e->pos().x() >= m_mouseSrcPos) {
      m_currentValue = m_maxRange;
    }

    if (m_currentValue == m_maxRange && e->pos().x() <= m_mouseSrcPos) {
      m_currentValue = m_minRange;
    }

    m_deviation = e->pos().x() - m_mouseSrcPos;

    // 若移动速度过快时进行限制
    if (m_deviation > (width() - 1) / 4)
      m_deviation = (width() - 1) / 4;
    else if (m_deviation < -(width() - 1) / 4)
      m_deviation = -(width() - 1) / 4;

    emit deviationChange((float)m_deviation / ((width() - 1) / 4));

    repaint();
  }
}

void RollingBox::mouseReleaseEvent(QMouseEvent *) {
  if (isDragging) {
    isDragging = 0;
    homing();
  }
}

void RollingBox::paintEvent(QPaintEvent *) {
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing, true);

  int Width = width() - 1;
  int Height = height() - 1;

  if (m_deviation >= Width / 4 && m_currentValue > m_minRange) {
    m_mouseSrcPos += Width / 4;
    m_deviation -= Width / 4;
    m_currentValue -= 1;
  }
  if (m_deviation <= -Width / 4 && m_currentValue < m_maxRange) {
    m_mouseSrcPos -= Width / 4;
    m_deviation += Width / 4;
    m_currentValue += 1;
  }

  // 中间数字

  paintNum(painter, m_currentValue, m_deviation);

  // 两侧数字1

  if (m_currentValue != m_minRange)
    paintNum(painter, m_currentValue - 1, m_deviation - Width / 4);
  if (m_currentValue != m_maxRange)
    paintNum(painter, m_currentValue + 1, m_deviation + Width / 4);

  // 两侧数字2,超出则不显示
  if (m_deviation >= 0 && m_currentValue - 2 >= m_minRange)
    paintNum(painter, m_currentValue - 2, m_deviation - Width / 2);
  if (m_deviation <= 0 && m_currentValue + 2 <= m_maxRange)
    paintNum(painter, m_currentValue + 2, m_deviation + Width / 2);

  // 边框
  if (mw_one->isDark)
    painter.setPen(QPen(QColor(0, 255, 255, 225), 2));
  else
    painter.setPen(QPen(QColor(0, 0, 255, 225), 2));

  painter.drawLine(Width / 8 * 3, 0, Width / 8 * 3, Height);
  painter.drawLine(Width / 8 * 5, 0, Width / 8 * 5, Height);
}

void RollingBox::paintNum(QPainter &painter, int num, int deviation) {
  int Width = width() - 1;
  int Height = height() - 1;

  int size = (Width - qAbs(deviation)) / m_numSize;
  int transparency = 255 - 510 * qAbs(deviation) / Width;
  int width = Width / 2 - 3 * qAbs(deviation) / 4;
  int x = Width / 2 + deviation - width / 2;

  QFont font;
  font.setPixelSize(size);
  painter.setFont(font);

  if (!mw_one->isDark)
    painter.setPen(QColor(0, 0, 0, transparency));
  else
    painter.setPen(QColor(255, 255, 255, transparency));
  painter.drawText(QRectF(x, 0, width, Height), Qt::AlignCenter,
                   QString::number(num));
}

void RollingBox::homing() {
  // 将数字矫正到中心
  if (m_deviation > width() / 8) {
    homingAni->setStartValue((width() - 1) / 8 - m_deviation);
    homingAni->setEndValue(0);
    m_currentValue--;
  } else if (m_deviation > -width() / 8) {
    homingAni->setStartValue(m_deviation);
    homingAni->setEndValue(0);
  } else if (m_deviation < -width() / 8) {
    homingAni->setStartValue(-(width() - 1) / 8 - m_deviation);
    homingAni->setEndValue(0);
    m_currentValue++;
  }
  emit currentValueChanged(m_currentValue);
  homingAni->start();
}

int RollingBox::readDeviation() { return m_deviation; }

void RollingBox::setDeviation(int n) {
  m_deviation = n;
  repaint();
}
