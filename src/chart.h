#ifndef CHART_H
#define CHART_H

#include <QChart>
#include <QChartView>
#include <QHBoxLayout>
#include <QScatterSeries>
#include <QSplineSeries>
#include <QValueAxis>

QT_CHARTS_USE_NAMESPACE
class Chart : public QWidget {
  Q_OBJECT
  QChart *qchart;
  QChartView *chartview;

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
  QSplineSeries *series;
  QScatterSeries *m_scatterSeries;
  Chart(QWidget *parent = 0, QString _chartname = "曲线图");
  ~Chart() {}
  void setAxis(QString _xname, qreal _xmin, qreal _xmax, int _xtickc,
               QString _yname, qreal _ymin, qreal _ymax, int _ytickc);
  void buildChart();
  void setXY(int maxX, double maxY);
  void addSeries();
};
#endif  // CHART_H
