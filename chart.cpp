#include "chart.h"

Chart::Chart(QWidget* parent, QString _chartname) {
  setParent(parent);
  chartname = _chartname;
  series = new QSplineSeries(this);
  m_scatterSeries = new QScatterSeries(this);  //创建散点
  qchart = new QChart;
  chartview = new QChartView(qchart);
  qchart->setMargins(QMargins(0, 0, 0, 0));
  qchart->setContentsMargins(0, 0, 0, 0);
  layout = new QHBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  layout->setMargin(0);
  axisX = new QValueAxis(this);
  axisY = new QValueAxis(this);

  //在ui里面添加了一个Widget并把曲线图添加进去
  layout->addWidget(chartview);
  setLayout(layout);
  chartview->setRenderHint(QPainter::Antialiasing);  //防止图形走样
}

void Chart::setAxis(QString _xname, qreal _xmin, qreal _xmax, int _xtickc,
                    QString _yname, qreal _ymin, qreal _ymax, int _ytickc) {
  xname = _xname;
  xmin = _xmin;
  xmax = _xmax;
  xtickc = _xtickc;
  yname = _yname;
  ymin = _ymin;
  ymax = _ymax;
  ytickc = _ytickc;

  axisX->setRange(xmin, xmax);  //设置范围
  axisX->setLabelFormat("%u");  //设置刻度的格式
  /************************************
      %u 无符号十进制整数
      %s 字符串
      %c 一个字符
      %d 有符号十进制整数
      %e 浮点数、e-记数法
      %f 浮点数、十进制记数法
      %s 字符串
  ****************************************/
  axisX->setGridLineVisible(true);  //网格线可见
  axisX->setTickCount(xtickc);      //设置多少个大格
  axisX->setMinorTickCount(1);  //设置每个大格里面小刻度线的数目
  axisX->setTitleText(xname);   //设置描述
  axisY->setRange(ymin, ymax);
  axisY->setLabelFormat("%u");
  axisY->setGridLineVisible(true);
  axisY->setTickCount(ytickc);
  axisY->setMinorTickCount(1);
  axisY->setTitleText(yname);
  qchart->addAxis(axisX,
                  Qt::AlignBottom);  //下：Qt::AlignBottom  上：Qt::AlignTop
  qchart->addAxis(axisY, Qt::AlignLeft);  //左：Qt::AlignLeft 右：Qt::AlignRight
}

void Chart::buildChart(QList<QPointF> pointlist) {
  //创建数据源
  series->setPen(QPen(Qt::blue, 3, Qt::SolidLine));
  series->clear();

  m_scatterSeries->clear();
  m_scatterSeries->setMarkerShape(
      QScatterSeries::MarkerShapeCircle);  //设置散点样式
  m_scatterSeries->setMarkerSize(10);      //设置散点大小

  for (int i = 0; i < pointlist.size(); i++) {
    series->append(pointlist.at(i).x(), pointlist.at(i).y());
    m_scatterSeries->append(pointlist.at(i).x(), pointlist.at(i).y());
  }

  qchart->setTitle(chartname);

  qchart->setAnimationOptions(QChart::SeriesAnimations);  //设置曲线动画模式
  qchart->legend()->hide();                               //隐藏图例

  qchart->addSeries(series);  //输入数据
  qchart->addSeries(m_scatterSeries);
  series->attachAxis(axisX);
  series->attachAxis(axisY);
  m_scatterSeries->attachAxis(axisX);
  m_scatterSeries->attachAxis(axisY);
  //   qchart->setAxisX(axisX, series);
  //   qchart->setAxisY(axisY, series);
}
