#ifdef _DEBUG
#pragma comment(lib, "Qt5Chartsd.lib")
#else
#pragma comment(lib, "Qt5Charts.lib")
#endif

#include <QDebug>
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

WaveWidget::WaveWidget(QWidget *parent) : SignalWidget(parent)
{
	m_needNotify = true;

	for (int i = 0; i < 2; i++) {
		m_valueMarker[i].setDirection(1);
		m_lines[i].setPen(QPen(QColor(67, 217, 150)));
	}
	
}

WaveWidget::~WaveWidget()
{
}

void WaveWidget::syncView(QRectF area)
{
	tool::ValueGuard<bool> notifyGuard(m_needNotify, false);

	QRectF newArea = visibleArea();
	if (area.width() > 0) {
		newArea.setLeft(area.left());
		newArea.setRight(area.right());
	}

	setVisibleArea(newArea);
}

void WaveWidget::load(std::shared_ptr<Reader> reader)
{
	m_loader.load(reader);

	if (reload(0, reader->maxTime())) {
		executeCommand(SignalCommand::XY_Reset);
	}
}

bool WaveWidget::reload(double t1, double t2)
{
	auto visible = visibleArea();
	std::pair<double, double> currentTime = { visible.left(), visible.right() };
	if (currentTime == std::pair<double, double>({t1, t2})) {
		return true;
	}

	int countHint = rect().width() + 1;
	if (m_loader.prepare({ t1, t2 }, countHint)) {
		updateChannel(0);
		updateChannel(1);

		visible.setLeft(t1);
		visible.setRight(t2);

		setVisibleArea(visible);

		return true;
	}

	return false;
}

void WaveWidget::updateChannel(int idx)
{
	if (idx >= m_loader.channel()) {
		if (idx < 2) {
			m_lines[idx].setVisible(false);
		}
		return;
	}
	
	m_lines[idx].setVisible(true);
	
	double timeStep = m_loader.timeStep();
	auto state = m_loader.state();
	int start = std::get<0>(state);
	int stop = std::get<1>(state);
	int step = std::get<2>(state);

	std::vector<QPointF> points;
	if (m_loader.isZipped()) {
		std::pair<double, double> * data = m_loader.value2(idx);
		if (data != nullptr) {
			for (int pos = start, i = 0; pos < stop; pos += step, i++) {
				points.push_back(QPointF(pos * timeStep, data[i].first));
				points.push_back(QPointF(pos * timeStep, data[i].second));
			}
		}
	}
	else {
		double * data = m_loader.value(idx);
		if (data != nullptr) {
			for (int pos = start, i = 0; pos < stop; pos += step, i++) {
				points.push_back(QPointF(pos * timeStep, data[i]));
			}
		}
	}
	
	m_lines[idx].setLine(points.data(), points.size());
}


QRectF WaveWidget::viewport(int tag)
{
	QRectF bound = rect();
	bound = tool::rectExpand(bound, -1);

	QRectF temp;

	float rightMargin = 64;
	float upMargin = 24;

	float channelBand = (bound.height() - upMargin - 1) / m_loader.channel();

	int channel = (int) (tag / 10) - 1;

	if (tag == 0) {
		// 时间坐标.
		QPointF pt = bound.topLeft();
		QSizeF size = { bound.width() - rightMargin, upMargin };
		return QRectF(pt, size);
	}
	else if (tag == 10) {
		// 通道1数据
		QPointF pt = { bound.left(), bound.top() + upMargin + channel * channelBand };
		QSizeF size = { bound.width() - rightMargin, channelBand};
		return QRectF(pt, size);
	}
	else if (tag == 11) {
		// 通道1坐标
		QPointF pt = { bound.right() - rightMargin , bound.top() + upMargin + channel * channelBand };
		QSizeF size = { rightMargin, channelBand };
		return QRectF(pt, size);
	}
	else if (tag == 20) {
		// 通道2数据
		QPointF pt = { bound.left(), bound.top() + upMargin + channel * channelBand };
		QSizeF size = { bound.width() - rightMargin, channelBand };
		return QRectF(pt, size);
	}
	else if (tag == 21) {
		// 通道2坐标
		QPointF pt = { bound.right() - rightMargin , bound.top() + upMargin + channel * channelBand };
		QSizeF size = { rightMargin, channelBand };
		return QRectF(pt, size);
	}

	return QRectF();
}

void WaveWidget::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);

	painter.setRenderHint(QPainter::Antialiasing);

	QRect bound = rect();
	painter.fillRect(bound, QColor::fromRgb(0, 0, 0));

	drawTimeMarker(& painter);
	drawChannel(& painter, 0);
	drawChannel(& painter, 1);

#ifdef _DEBUG
	static int counter = 0;
	qDebug() << QString::asprintf("wave_widget refresh counter : %d", counter++);
#endif //_DEBUG

}

void WaveWidget::drawTimeMarker(QPainter * painter)
{
	assert(painter != nullptr);

	auto visible = visibleArea();
	auto bound = viewport(0);

	MarkerPanel panel;
	panel.setRect(bound);
	panel.setRange({ visible.left(), visible.right()});

	panel.paint(painter);
}

void WaveWidget::drawChannel(QPainter * painter, int idx)
{
	auto visible = visibleArea();
	
	if (idx >= m_loader.channel() || visible.isEmpty())
		return;
	
	// mark区.
	QRectF bound = viewport(idx * 10 + 11);
	MarkerPanel panel;
	panel.setRect(bound);
	panel.setDirection(1);
	panel.setRange({ visible.top(), visible.bottom() });
	panel.paint(painter);

	// 数据区.
	bound = viewport(idx * 10 + 10);
	m_lines[idx].setRect(bound);
	m_lines[idx].setArea(visible);
	m_lines[idx].paint(painter);
}

void WaveWidget::setVisibleArea(QRectF r)
{
	auto area = r.normalized();
	auto visible = visibleArea();
	auto total = totalArea();
	
	area.setLeft(std::max<double>(total.left(), area.left()));
	area.setRight(std::min<double>(total.right(), area.right()));

	if (visible != area) {
		SignalWidget::setVisibleArea(area);
		update();
	}
}

QRectF WaveWidget::totalArea()
{
	auto xrng = m_loader.timeRange();

	auto r1 = m_loader.valueRange(0);
	auto r2 = m_loader.valueRange(1);

	double minr = std::min<double>(r1.first, r2.first);
	double maxr = std::max<double>(r1.second, r2.second);

	return QRectF(QPointF(xrng.first, minr), QPointF(xrng.second, maxr));
}
