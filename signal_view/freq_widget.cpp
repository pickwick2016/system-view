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

QT_CHARTS_USE_NAMESPACE

#include "freq_widget.h"
#include "fft.h"
#include "reader.h"

FreqWidget::FreqWidget(QWidget *parent) : QWidget(parent)
{
	m_currentTime = -1;
	m_fft.reset(new Fft());

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
	axisX->setRange(0, 100);
	axisX->setLabelsColor(QColor(255, 255, 255));

	QValueAxis *axisY = new QValueAxis;
	axisY->setRange(-40, 50);
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
}

FreqWidget::~FreqWidget()
{
}

void FreqWidget::load(std::shared_ptr<Reader> reader)
{
	m_reader = reader;

	double fs = m_reader ? m_reader->sampleRate() : 1;
	m_chart->axisX()->setRange(0, m_reader->maxFreq());

	std::vector<float> values;
	reload(0, values);
	if (!values.empty()) {
		auto mm = std::minmax_element(values.begin(), values.end());
		float m1 = *(mm.first), m2 = *(mm.second);
		float dev = (m2 - m1) * 0.1;
		m_chart->axisY()->setRange(m1 - dev, m2 + dev);

		drawValues(values);
	}
}

void FreqWidget::reload(double t, std::vector<float> & values)
{
	int fftlen = m_fft->length();
	double fs = m_reader->sampleRate();
	int readPos = std::floor(t * fs);

	std::vector<char> buffer(fftlen * sizeof(double) * 2);
	values.clear();
		
	if (readPos >= 0 && readPos < m_reader->count()) {
		values.resize(fftlen);
		
		m_reader->read(buffer.data(), m_fft->length(), readPos);
		int count2 = m_fft->power(buffer.data(), values.data(), fftlen, m_reader->type());
		values.resize(count2);
	}
}

void FreqWidget::drawValues(const std::vector<float> & values)
{
	int fftlen = m_fft->length();
	double fs = m_reader->sampleRate();
	double freqStep = fs / fftlen;

	QList<QPointF> points;
	for (int i = 0; i < values.size(); i++) {
		points.append(QPointF(i * freqStep, values[i]));
	}

	m_line->replace(points);
}

void FreqWidget::visibleChanged(QRectF r)
{
	if (m_currentTime != r.left()) {
		m_currentTime = r.left();

		std::vector<float> values;
		reload(m_currentTime, values);
		drawValues(values);
	}
}
