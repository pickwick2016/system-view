#ifdef _DEBUG
#pragma comment(lib, "Qt5Chartsd.lib")
#else
#pragma comment(lib, "Qt5Charts.lib")
#endif

#include <QLayout>

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>

#include <boost/variant.hpp>

QT_CHARTS_USE_NAMESPACE

#include "freq_widget.h"
#include "fft.h"
#include "reader.h"

FreqWidget::FreqWidget(QWidget *parent) : QWidget(parent)
{
	m_currentTime = -1;

	QChartView *chartView = new QChartView(this);
	chartView->setRubberBand(QChartView::VerticalRubberBand);

	m_chart = new QChart();
	chartView->setChart(m_chart);

	m_line = new QLineSeries();
	for (int i = 0; i < 100; i++) {
		((QLineSeries *)m_line)->append(i, i * 0.5);
	}

	m_chart->addSeries(m_line);

	QValueAxis *axisX = new QValueAxis;
	axisX->setRange(0, 1);
	axisX->setLabelsColor(QColor(255, 255, 255));

	QValueAxis *axisY = new QValueAxis;
	axisY->setRange(0, 1);
	axisY->setLabelsColor(QColor(255, 255, 255));

	m_chart->setAxisX(axisX, m_line);
	m_chart->setAxisY(axisY, m_line);

	m_chart->setBackgroundRoundness(0);
	m_chart->setBackgroundBrush(QColor(0, 0, 0));
	m_chart->legend()->hide();

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->addWidget(chartView);
	layout->setMargin(0);
	layout->setContentsMargins(QMargins(0, 0, 0, 0));

	setMinimumSize(QSize(100, 75));
}

FreqWidget::~FreqWidget()
{
}

void FreqWidget::load(std::shared_ptr<Reader> reader)
{
	m_loader.load(reader);

	if (reload(0)) {
		drawAxis();
	}
}

bool FreqWidget::reload(double t)
{
	if (m_currentTime == t) {
		return true;
	}

	if (m_loader.prepare(t)) {
		drawValues();
		m_currentTime = t;
		return true;
	}

	return false;
}

void FreqWidget::drawValues()
{
	auto values = m_loader.values();
	float * data = values.first;
	int count = values.second;

	double freqStep = m_loader.freqStep();

	QList<QPointF> points;
	for (int i = 0; i < count; i++) {
		points.append(QPointF(i * freqStep, data[i]));
	}

	m_line->replace(points);
}

void FreqWidget::drawAxis()
{
	auto rangeX = m_loader.freqRange();
	m_chart->axisX()->setRange(rangeX.first, rangeX.second);

	auto rangeY = m_loader.valueRange();
	m_chart->axisY()->setRange(rangeY.first, rangeY.second);
}

void FreqWidget::syncView(QRectF r)
{
	reload(r.left());
}
