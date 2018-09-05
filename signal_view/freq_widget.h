#ifndef FREQ_WIDGET_H
#define FREQ_WIDGET_H

#include <memory>

#include <QWidget>
#include <QtCharts/QChartGlobal>

QT_CHARTS_BEGIN_NAMESPACE
class QChart;
class QLineSeries;
QT_CHARTS_END_NAMESPACE

class Fft;
class Reader;

// 频谱控件.
class FreqWidget : public QWidget
{
	Q_OBJECT

public:
	FreqWidget(QWidget *parent = nullptr);
	~FreqWidget();

public slots:
	void visibleChanged(QRectF);

public:
	// 载入数据.
	void load(std::shared_ptr<Reader> reader);

private:
	// 重新载入指定时刻的内容.
	void reload(double t, std::vector<float> & values);

	// 绘制数值.
	void drawValues(const std::vector<float> & values);

private:
	QtCharts::QChart * m_chart;
	QtCharts::QLineSeries * m_line;

	std::shared_ptr<Fft> m_fft;
	std::shared_ptr<Reader> m_reader;

	double m_currentTime;
};

#endif //FREQ_WIDGET_H
