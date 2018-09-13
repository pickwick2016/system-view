#include <assert.h>

#include "paint_panel.h"
#include "misc.h"

PaintPanel::PaintPanel() : m_visible(true), m_rect(QRectF())
{
	m_bgColor = QColor(0, 0, 0);
}

void PaintPanel::setRect(QRectF r)
{
	m_rect = r.normalized();
}

void PaintPanel::paint(QPainter * painter)
{
	painter->fillRect(rect(), m_bgColor);
}

MarkerPanel::MarkerPanel(int dir, QRect rect, std::pair<double, double> range, int divides)
{
	setDirection(dir);
	setDivides(divides);
	setRect(rect);
	setRange(range);

	m_labelPen = QPen(QColor(255, 255, 255));
}


void MarkerPanel::paint(QPainter * painter)
{	
	assert(painter);

	QRectF bound = rect();
	if (!isVisible() || bound.isEmpty()) {
		return;
	}

	painter->setClipRect(bound);
	painter->fillRect(bound, m_bgColor);

	std::vector<double> values;
	double step = tool::DivideRange(m_range, m_divides, values);
	if (values.empty()) {
		return;
	}

	int eps = std::floorf(std::log10f(step));
	QString fmtStr = "%.0f";
	if (eps < 0) {
		fmtStr = QString::asprintf("%.%df", -eps);
	}

	if (m_direction == 0) {
		paintHorz(painter, values, fmtStr);
	}
	else {
		paintVert(painter, values, fmtStr);

	}

	painter->setClipRect(QRect());
}

void MarkerPanel::paintHorz(QPainter * painter, const std::vector<double> & values, QString fmtStr)
{
	QRectF bound = rect();
	QPointF boundCenter = bound.center();

	QRectF textRect(bound.topLeft(), QSizeF(bound.width() / m_divides, bound.height()));
	painter->setPen(m_labelPen);
	for (auto time : values) {
		double ptx = tool::Map<double>(time, m_range, { bound.left(), bound.right() });

		painter->drawLine(QPointF(ptx, bound.bottom()), QPointF(ptx, bound.bottom() - 5));

		textRect.moveCenter({ ptx, boundCenter.ry() });

		QString text = QString::asprintf(fmtStr.toStdString().c_str(), time);
		painter->drawText(textRect, Qt::AlignCenter, text);
	}
}

void MarkerPanel::paintVert(QPainter * painter, const std::vector<double> & values, QString fmtStr)
{
	QRectF bound = rect();
	QPointF boundCenter = bound.center();

	QRectF textRect(bound.topLeft(), QSizeF(bound.width(), bound.height() / m_divides));
	painter->setPen(m_labelPen);
	for (auto time : values) {
		double ptx = tool::Map<double>(time, m_range, { bound.bottom(), bound.top() });

		painter->drawLine(QPointF(bound.left(), ptx), QPointF(bound.left() + 5, ptx));

		textRect.moveCenter({ boundCenter.rx(), ptx });

		QString text = QString::asprintf(fmtStr.toStdString().c_str(), time);
		painter->drawText(textRect, Qt::AlignCenter, text);
	}
}

void MarkerPanel::setDirection(int dir)
{
	m_direction = dir;
}

void MarkerPanel::setRange(std::pair<double, double> range)
{
	m_range = tool::range_normalize(range);
}

void MarkerPanel::setDivides(int count)
{
	m_divides = std::max<int>(5, count);
}

LinePanel::LinePanel()
{
	m_bgColor = QColor(85, 85, 85);
	m_area = QRectF(QPointF(0, 0), QSizeF(1, 1));
	m_pen = QPen(QColor(255, 255, 255));
}

void LinePanel::paint(QPainter * painter)
{
	assert(painter);

	QRectF bound = rect();
	if (! isVisible() || bound.isEmpty() || area().isEmpty()) {
		return;
	}

	painter->setClipRect(bound);
	painter->fillRect(bound, m_bgColor);

	paintLine(painter);;
}

void LinePanel::setArea(QRectF area)
{
	m_area = area.normalized();
}

void LinePanel::paintLine(QPainter * painter)
{
	QRectF bound = rect();
	int count = m_points.size();

	if (count > 0) {
		std::vector<QPointF> points(count);
		for (int i = 0; i < count; i++) {
			points[i] = tool::Map(m_points[i], m_area, bound);
		}

		painter->setPen(m_pen);
		painter->drawPolyline(points.data(), points.size());
	}
}

void LinePanel::setLine(QPointF * points, int count)
{
	if (count <= 0) {
		m_points.clear();
	}
	else {
		m_points.resize(count);
		for (int i = 0; i < count; i++) {
			m_points[i] = points[i];
		}
	}
}

std::pair<double, double> LinePanel::rangeX()
{
	return { 0.0, 0.0 };
}

std::pair<double, double> LinePanel::rangeY()
{
	return { 0.0, 0.0 };
}