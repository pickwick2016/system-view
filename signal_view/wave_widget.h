#ifndef WAVE_WIDGET_H
#define WAVE_WIDGET_H

#include <memory>

#include <QWidget>
#include <QtCharts/QChartGlobal>

QT_CHARTS_BEGIN_NAMESPACE
class QChart;
class QLineSeries;
QT_CHARTS_END_NAMESPACE

#include "signal_loader.h"


// 频谱控件.
class WaveWidget : public QWidget
{
	Q_OBJECT

public:
	WaveWidget(QWidget *parent = nullptr);
	~WaveWidget();

public slots:
	void visibleChanged(QRectF);

public:
	// 载入数据.
	void load(std::shared_ptr<Reader> reader);

private:
	// 重新载入指定时刻的内容.
	bool reload(double t1, double t2);

	// 绘制数值.
	void drawValues();

	void drawChannel(int idx);

	// 重绘坐标轴.
	void drawAxis();

private:
	QtCharts::QChart * m_chart;

	std::vector<QtCharts::QLineSeries *> m_lines;
	
	WaveLoader m_loader;

	std::pair<double, double> m_currentTime;
};

#endif //WAVE_WIDGET_H
