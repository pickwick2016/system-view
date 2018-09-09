#ifndef FREQ_WIDGET_H
#define FREQ_WIDGET_H

#include <memory>

#include <QWidget>
#include <QtCharts/QChartGlobal>

QT_CHARTS_BEGIN_NAMESPACE
class QChart;
class QLineSeries;
QT_CHARTS_END_NAMESPACE

#include "signal_loader.h"


// 频谱控件.
class FreqWidget : public QWidget
{
	Q_OBJECT

public:
	FreqWidget(QWidget *parent = nullptr);
	~FreqWidget();

public slots:
	void syncView(QRectF);

public:
	// 载入数据.
	void load(std::shared_ptr<Reader> reader);

private:
	// 重新载入指定时刻的内容.
	bool reload(double t);

	// 绘制数值.
	void drawValues();

	// 重绘坐标轴.
	void drawAxis();

private:
	QtCharts::QChart * m_chart;
	QtCharts::QLineSeries * m_line;

	FreqLoader m_loader;

	double m_currentTime;
};

#endif //FREQ_WIDGET_H
