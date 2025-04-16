// WheelWidget.cpp
#include "WheelWidget.h"

#include <QElapsedTimer>
#include <QMouseEvent>
#include <QPainter>
#include <QWheelEvent>
#include <QtMath>

WheelWidget::WheelWidget(QWidget* parent) : QWidget(parent) {
  // 惯性动画定时器（60FPS）
  m_inertiaTimer.setInterval(16);
  connect(&m_inertiaTimer, &QTimer::timeout, this,
          &WheelWidget::animateInertia);
}

void WheelWidget::paintEvent(QPaintEvent* event) {
  Q_UNUSED(event);
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);
  const int centerY = height() / 2;

  // 动态计算需要绘制的项数（覆盖两倍屏幕高度）
  const int visibleItemCount = qCeil(height() / (qreal)m_itemHeight) * 2 + 5;
  for (int i = -visibleItemCount; i <= visibleItemCount; ++i) {
    // 循环核心算法：虚拟索引计算
    int virtualIndex =
        (m_currentIndex + i + m_items.size() * 10) % m_items.size();

    // 项位置计算（包含循环偏移）
    qreal itemOffset = qreal(m_offset) / m_itemHeight;
    qreal yPos = centerY + (i - itemOffset) * m_itemHeight;

    // 自动跳过完全不可见的项（优化性能）
    if (yPos < -m_itemHeight || yPos > height() + m_itemHeight) continue;

    // 动态样式计算（保持iOS风格）
    qreal distance = qAbs(yPos - centerY) / m_itemHeight;
    qreal opacity = qBound(0.2, 1.0 - distance * 0.4, 1.0);
    int fontSize = qBound(14, 24 - static_cast<int>(distance * 8), 24);

    painter.setOpacity(opacity);
    painter.setPen(m_textColor);
    painter.setFont(QFont("Helvetica Neue", fontSize));
    painter.drawText(QRect(0, yPos - m_itemHeight / 2, width(), m_itemHeight),
                     Qt::AlignCenter, m_items[virtualIndex]);
  }

  // iOS风格高亮区域
  painter.fillRect(QRect(0, centerY - m_itemHeight / 2, width(), m_itemHeight),
                   QColor(0, 122, 255, 38));
}

void WheelWidget::mousePressEvent(QMouseEvent* event) {
  m_dragStartY = event->pos().y();
  m_baseOffset = m_offset;  // 保存当前偏移量基准
  m_velocity = 0;
  m_dragTimer.start();
  m_inertiaTimer.stop();
  QWidget::mousePressEvent(event);
}

void WheelWidget::mouseReleaseEvent(QMouseEvent* event) {
  // 计算惯性速度（像素/毫秒）
  qreal elapsed = m_dragTimer.elapsed();
  m_velocity = (elapsed > 0) ? (m_offset - m_baseOffset) / elapsed : 0;

  if (qAbs(m_velocity) > 0.5) {
    m_inertiaTimer.start();
  } else {
    updateIndex();
  }
  QWidget::mouseReleaseEvent(event);
}

void WheelWidget::mouseMoveEvent(QMouseEvent* event) {
  if (!m_dragTimer.isValid()) return;

  // 原始拖拽距离计算（保持1:1跟随）
  const int deltaY = event->pos().y() - m_dragStartY;
  m_offset = m_baseOffset + deltaY;

  // 循环滚动核心：当偏移超过阈值时重置
  const int cycleThreshold = m_itemHeight * m_items.size();
  if (qAbs(m_offset) > cycleThreshold) {
    const int cycleThreshold = m_itemHeight * m_items.size();
    if (cycleThreshold != 0) {  // 避免除零
      m_offset = std::fmod(m_offset, static_cast<qreal>(cycleThreshold));

      // 同步调整到正区间（若需要）
      if (m_offset < 0) {
        m_offset += cycleThreshold;
      }
    }
  }

  update();
}

void WheelWidget::updateIndex() {
  // 正确方向：offset正 → 项下移 → 索引递增
  const qreal position = m_offset / m_itemHeight;  // 移除负号
  int deltaIndex = static_cast<int>(std::round(position));

  // 循环模式计算
  if (m_circular) {
    m_currentIndex =
        (m_currentIndex + deltaIndex + m_items.size()) % m_items.size();
  } else {
    m_currentIndex = qBound(0, m_currentIndex + deltaIndex, m_items.size() - 1);
  }

  m_offset = 0;  // 强制对齐项中心
  emit currentIndexChanged(m_currentIndex);
  update();
}

void WheelWidget::animateInertia() {
  m_offset += m_velocity * 16;
  m_velocity *= 0.88;

  // 惯性停止时强制精准定位
  if (qAbs(m_velocity) < 0.5) {
    m_inertiaTimer.stop();
    updateIndex();  // ✅ 关键调用
  }
  update();
}

// 滚轮事件：逐项滚动
void WheelWidget::wheelEvent(QWheelEvent* event) {
  int delta = event->angleDelta().y();
  setCurrentIndex((m_currentIndex + (delta > 0 ? -1 : 1) + m_items.size()) %
                  m_items.size());
}

// 公共接口实现
void WheelWidget::setItems(const QStringList& items) {
  if (m_items != items) {
    m_items = items;
    m_currentIndex = 0;
    update();
  }
}

void WheelWidget::setCurrentIndex(int index) {
  if (index >= 0 && index < m_items.size() && index != m_currentIndex) {
    m_currentIndex = index;
    emit currentIndexChanged(m_currentIndex);
    update();
  }
}

QString WheelWidget::currentText() const {
  // 安全访问当前项文本
  if (m_currentIndex >= 0 && m_currentIndex < m_items.size()) {
    return m_items.at(m_currentIndex);
  }
  return QString();  // 越界时返回空字符串
}
