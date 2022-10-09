
#include "imgview.h"

#include <math.h>

#include <QDebug>
#include <QDrag>
#include <QGraphicsSceneDragDropEvent>
#include <QGraphicsSceneMouseEvent>
#include <QPointF>

QImageWidget::QImageWidget(QPixmap* pixmap) {
  m_pix = *pixmap;
  setAcceptDrops(false);
  m_scaleValue = 0;
  m_scaleDafault = 0;
  m_isMove = false;
}

QRectF QImageWidget::boundingRect() const {
  return QRectF(-m_pix.width() / 2, -m_pix.height() / 2, m_pix.width(),
                m_pix.height());
}

void QImageWidget::paint(QPainter* painter, const QStyleOptionGraphicsItem*,
                         QWidget*) {
  painter->drawPixmap(-m_pix.width() / 2, -m_pix.height() / 2, m_pix);
}

void QImageWidget::paintImg() {}

void QImageWidget::mousePressEvent(QGraphicsSceneMouseEvent* event) {
  if (event->button() == Qt::LeftButton) {
    m_startPos.setX(event->pos().rx());
    m_startPos.setY(event->pos().ry());
    m_isMove = true;
  } else if (event->button() == Qt::RightButton) {
    ResetItemPos();
  }
}
void QImageWidget::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
  if (m_isMove) {
    QPoint p;
    p.setX(event->pos().rx());
    p.setY(event->pos().ry());
    QPoint point = (p - m_startPos) * m_scaleValue;

    moveBy(point.x(), point.y());

    qDebug() << point;
  }
}

void QImageWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent*) {
  m_isMove = false;
}

void QImageWidget::wheelEvent(QGraphicsSceneWheelEvent* event) {
  if ((event->delta() > 0) && (m_scaleValue >= 50))  //最大放大到原始图像的50倍
  {
    return;
  } else if ((event->delta() < 0) &&
             (m_scaleValue <=
              m_scaleDafault))  //图像缩小到自适应大小之后就不继续缩小
  {
    ResetItemPos();  //重置图片大小和位置，使之自适应控件窗口大小
  } else {
    qreal qrealOriginScale = m_scaleValue;
    if (event->delta() > 0)  //鼠标滚轮向前滚动
    {
      m_scaleValue *= 1.1;  //每次放大10%
    } else {
      m_scaleValue *= 0.9;  //每次缩小10%
    }
    setScale(m_scaleValue);
    if (event->delta() > 0) {
      moveBy(-event->pos().x() * qrealOriginScale * 0.1,
             -event->pos().y() * qrealOriginScale *
                 0.1);  //使图片缩放的效果看起来像是以鼠标所在点为中心进行缩放的
    } else {
      moveBy(event->pos().x() * qrealOriginScale * 0.1,
             event->pos().y() * qrealOriginScale *
                 0.1);  //使图片缩放的效果看起来像是以鼠标所在点为中心进行缩放的
    }
  }
}

void QImageWidget::setQGraphicsViewWH(int nwidth, int nheight) {
  int nImgWidth = m_pix.width();
  int nImgHeight = m_pix.height();
  qreal temp1 = nwidth * 1.0 / nImgWidth;
  qreal temp2 = nheight * 1.0 / nImgHeight;
  if (temp1 > temp2) {
    m_scaleDafault = temp2;
  } else {
    m_scaleDafault = temp1;
  }
  setScale(m_scaleDafault);
  m_scaleValue = m_scaleDafault;
}

void QImageWidget::ResetItemPos() {
  m_scaleValue = m_scaleDafault;  //缩放比例回到一开始的自适应比例
  setScale(m_scaleDafault);
  setPos(0, 0);
}

qreal QImageWidget::getScaleValue() const { return m_scaleValue; }

void QImageWidget::scaleImg(bool zoom) {
  if (zoom) {
    m_scaleValue *= 1.1;
  } else {
    m_scaleValue *= 0.9;
  }
  setScale(m_scaleValue);
}
