#include <QDebug>
#include <QLayout>

#include <boost/variant.hpp>

#include "wave_widget.h"
#include "fft.h"
#include "reader.h"
#include "misc.h"

WaveWidget::WaveWidget(QWidget *parent) : SignalWidget(parent)
{
	m_needNotify = true;

	for (int i = 0; i < 2; i++) {
		m_valueMarker[i].setDirection(1);
		m_valueMarker[i].setDivides(5);
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
	assert(tag >= 0);

	QRectF bound = rect();

	int border = 2;
	float rightBand = 64;
	float upBand = 24;

	float w[2] = { bound.width() - 3 * border - rightBand, rightBand };

	int channel = m_loader.channel();
	float channelHeight = bound.height() - upBand - 2 * border - channel * border;
	float h[3] = { upBand, channelHeight / channel, channelHeight / channel };

	int col = tag % 10;
	int row = tag / 10;

	if ((tag == 0) || (col < 2 && row >= 1 && row <= channel)) {
		float offsetx = std::accumulate(w, w + col, 0) + (col + 1) * border;
		float offsety = std::accumulate(h, h + row, 0) + (row + 1) * border;

		QPointF pt = bound.topLeft();
		pt.rx() = pt.rx() + offsetx;
		pt.ry() = pt.ry() + offsety;

		return QRectF(pt, QSizeF(w[col], h[row]));
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

	if (hasFocus()) {
		painter.setClipRect(bound);
		painter.setPen(QPen(QColor(215, 215, 215), 2));
		painter.drawRect(bound);
	}

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

	if (visible.isEmpty() || bound.isEmpty()) {
		return;
	}

	MarkerPanel panel;
	panel.setRect(bound);
	panel.setRange({ visible.left(), visible.right()});

	panel.paint(painter);
}

void WaveWidget::drawChannel(QPainter * painter, int idx)
{
	auto visible = visibleArea();
	
	if (idx >= m_loader.channel() || visible.isEmpty()) {
		return;
	}
	
	// mark区.
	QRectF bound = viewport(idx * 10 + 11);
	m_valueMarker[idx].setRect(bound);
	m_valueMarker[idx].setRange({ visible.top(), visible.bottom() });
	m_valueMarker[idx].paint(painter);

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
	total.setTop(-std::numeric_limits<double>::max());
	total.setBottom(std::numeric_limits<double>::max());

	// TODO:应该限制矩形.
	area = tool::adjust(area, total, true);
	
	if (visible != area) {
		if (reload(area.left(), area.right())) {
			SignalWidget::setVisibleArea(area);
			update();
		}
	}
}

QRectF WaveWidget::totalArea()
{
	double maxTime = m_loader.maxTime();
	auto xrng = m_loader.timeRange();

	auto r1 = m_loader.valueRange(0);
	auto r2 = m_loader.valueRange(1);

	double minr = std::min<double>(r1.first, r2.first);
	double maxr = std::max<double>(r1.second, r2.second);

	return QRectF(QPointF(0, minr), QPointF(maxTime, maxr));
}
