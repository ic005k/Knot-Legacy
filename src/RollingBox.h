#ifndef ROLLINGBOX_H
#define ROLLINGBOX_H

#include <QMouseEvent>
#include <QPainter>
#include <QPropertyAnimation>
#include <QWidget>

class RollingBox : public QWidget  // GIG_C_GOD
{
  Q_OBJECT
  Q_PROPERTY(int deviation READ readDeviation WRITE setDeviation)
 public:
  explicit RollingBox(QWidget *parent = 0);

  void setRange(int min, int max);
  int readValue();
  void setValue(int value);

 protected:
  int m_minRange;
  int m_maxRange;
  int m_currentValue;
  bool isDragging;
  int m_deviation;
  int m_mouseSrcPos;
  int m_numSize;  // 计算所得的数字字符尺寸，以最长的情况为准
  QPropertyAnimation *homingAni;

  void mousePressEvent(QMouseEvent *);
  void mouseMoveEvent(QMouseEvent *);
  void mouseReleaseEvent(QMouseEvent *);
  void paintEvent(QPaintEvent *);

  void paintNum(QPainter &painter, int num, int deviation);
  void homing();
  int readDeviation();
  void setDeviation(int n);
 signals:
  void currentValueChanged(int value);
  void deviationChange(float deviation);
 public slots:
};

#endif  // ROLLINGBOX_H
