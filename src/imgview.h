#pragma once
#include <QDragEnterEvent>
#include <QGraphicsItem>
#include <QGraphicsSceneWheelEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>
#include <QPointF>
#include <QRectF>
#include <QWidget>
#include <QtGui>
enum Enum_ZoomState { NO_STATE, RESET, ZOOM_IN, ZOOM_OUT };
class QImageWidget : public QGraphicsItem {
 public:
  QImageWidget(QPixmap* pixmap);
  QRectF boundingRect() const;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
             QWidget* widget);
  void wheelEvent(QGraphicsSceneWheelEvent* event);
  void ResetItemPos();
  void mousePressEvent(QGraphicsSceneMouseEvent* event);
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
  qreal getScaleValue() const;
  void setQGraphicsViewWH(int nwidth, int nheight);

  void scaleImg(bool zoom);
  qreal m_scaleValue;

  void paintImg();

 private:
  qreal m_scaleDafault;
  QPixmap m_pix;
  int m_zoomState;
  bool m_isMove;
  QPoint m_startPos;
};
