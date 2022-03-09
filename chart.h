#ifndef CHART_H
#define CHART_H

#include <QChart>
#include <QChartView>
#include <QHBoxLayout>
#include <QSplineSeries>
#include <QValueAxis>

QT_CHARTS_USE_NAMESPACE
class Chart : public QWidget {
  Q_OBJECT
  QChart *qchart;
  QChartView *chartview;
  QSplineSeries *series;

  QHBoxLayout *layout;
  QValueAxis *axisX;
  QValueAxis *axisY;

  QString chartname;
  //坐标轴参数
  QString xname;
  qreal xmin;
  qreal xmax;
  int xtickc;
  QString yname;
  qreal ymin;
  qreal ymax;
  int ytickc;

 public:
  Chart(QWidget *parent = 0, QString _chartname = "曲线图");
  ~Chart() {}
  void setAxis(QString _xname, qreal _xmin, qreal _xmax, int _xtickc,
               QString _yname, qreal _ymin, qreal _ymax, int _ytickc);
  void buildChart(QList<QPointF> pointlist);
};
#endif  // CHART_H
