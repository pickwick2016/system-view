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

#include "wave_widget.h"
#include "fft.h"
#include "reader.h"
#include "misc.h"

WaveWidget::WaveWidget(QWidget *parent) : QWidget(parent)
{
	m_needNotify = true;
	m_currentTime = {-1, -1};

	QChartView *chartView = new QChartView(this);
	chartView->setRubberBand(QChartView::VerticalRubberBand);

	m_chart = new QChart();
	chartView->setChart(m_chart);

	m_lines.resize(2);
	for (unsigned int i = 0; i < m_lines.size(); i++) {
		m_lines[i] = new QLineSeries();
		m_chart->addSeries(m_lines[i]);
	}
	
	QValueAxis *axisX = new QValueAxis;
	axisX->setRange(0, 1);
	axisX->setLabelsColor(QColor(125, 255, 125));

	QValueAxis *axisY = new QValueAxis;
	axisY->setRange(-1, 1);
	axisY->setLabelsColor(QColor(255, 125, 125));

	m_chart->setAxisX(axisX, m_lines[0]);
	axisX->setVisible(false);
	m_chart->setAxisY(axisY, m_lines[0]);
	
	m_chart->setBackgroundRoundness(0);
	m_chart->setBackgroundBrush(QColor(0, 0, 0));
	m_chart->legend()->hide();

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->addWidget(chartView);
	layout->setMargin(0);
	layout->setContentsMargins(QMargins(0, 0, 0, 0));
}

WaveWidget::~WaveWidget()
{
}

void WaveWidget::syncView(QRectF area)
{
	tool::ValueGuard<bool> notifyGuard(m_needNotify, false);

	QRectF newArea = m_visibleArea;
	if (area.width() > 0) {
		newArea.setLeft(area.left());
		newArea.setRight(area.right());
	}

	if (newArea != m_visibleArea) {
		//setVisibleArea(newArea);
	}
}

void WaveWidget::load(std::shared_ptr<Reader> reader)
{
	m_loader.load(reader);

	if (reload(0, 100/*reader->maxTime()*/)) {
		drawAxis();
	}
}

bool WaveWidget::reload(double t1, double t2)
{
	if (m_currentTime == std::pair<double, double>({t1, t2})) {
		return true;
	}

	int countHint = rect().width() + 1;
	if (m_loader.prepare({ t1, t2 }, countHint)) {
		drawValues();
		m_currentTime = std::pair<double, double>({ t1, t2 });
		return true;
	}

	return false;
}

void WaveWidget::drawChannel(int idx)
{
	if (idx >= m_loader.channel()) {
		if (idx < m_lines.size()) {
			m_lines[idx]->setVisible(false);
		}
		return;
	}
	
	m_lines[idx]->setVisible(true);
	
	double timeStep = m_loader.timeStep();
	auto state = m_loader.state();
	int start = std::get<0>(state);
	int stop = std::get<1>(state);
	int step = std::get<2>(state);

	QList<QPointF> points;
	if (m_loader.isZipped()) {
		std::pair<double, double> * data = m_loader.value2(idx);
		if (data != nullptr) {
			for (int pos = start, i = 0; pos < stop; pos += step, i++) {
				points.append(QPointF(pos * timeStep, data[i].first));
				points.append(QPointF(pos * timeStep, data[i].second));
			}
		}
	}
	else {
		double * data = m_loader.value(idx);
		if (data != nullptr) {
			for (int pos = start, i = 0; pos < stop; pos += step, i++) {
				points.append(QPointF(pos * timeStep, data[i]));
			}
		}
	}
	
	m_lines[idx]->replace(points);
}

void WaveWidget::drawValues()
{
	drawChannel(0);
	drawChannel(1);
}

void WaveWidget::drawAxis()
{
	auto rangeX = m_loader.timeRange();
	m_chart->axisX()->setRange(rangeX.first, rangeX.second);

	//auto rangeY = m_loader.valueRange(0);
	//m_chart->axisY()->setRange(rangeY.first, rangeY.second);
}


