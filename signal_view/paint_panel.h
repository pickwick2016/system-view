#ifndef PAINT_PANEL_H
#define PAINT_PANEL_H

#include <QPainter>

// 绘制面板.
class PaintPanel
{
public:
	PaintPanel();
	virtual ~PaintPanel() {}

public:
	virtual void paint(QPainter * painter) {}

public:
	void setRect(QRectF);
	QRectF rect() { return m_rect; }

	void setVisible(bool visible) { m_visible = visible; }
	bool isVisible() { return m_visible; }

private:
	QRectF m_rect;
	bool m_visible;
};

// 坐标面板.
class MarkerPanel : public PaintPanel
{
public:
	MarkerPanel(int dir = 0, QRect rect = QRect(), std::pair<double, double> range = { 0.0, 1.0 }, int divides = 10);

public:
	virtual void paint(QPainter * painter);

private:
	void paintHorz(QPainter & painter, const std::vector<double> & values, QString fmtStr);
	void paintVert(QPainter & painter, const std::vector<double> & values, QString fmtStr);

public:
	void setDirection(int dir);
	int direction() { return m_direction; }

	void setRange(std::pair<double, double> range);
	std::pair<double, double> range() { return m_range; }

	void setDivides(int count);
	int divides() { return m_divides; }

private:
	int	m_direction;
	std::pair<double, double> m_range;
	int m_divides;

	QColor m_bgColor;
	QPen m_gridPen;
	QPen m_labelPen;
};

// 绘线面板.
class LinePanel : public PaintPanel
{
public:
	LinePanel();

public:
	virtual void paint(QPainter * painter);

private:
	void paintLine(QPainter & painter);

public:
	// 设置逻辑范围.
	void setArea(QRectF area);
	QRectF area() { return m_area; }

	QPen pen() { return m_pen; }
	void setPen(QPen pen) { m_pen = pen; }

	// 设置线条.
	void setLine(QPointF * points, int count);
	// 设置线条.
	template <class T>
	void setLine(std::pair<T, T> * points, int count);

	// 获取线条
	std::vector<QPointF> & line() { return m_points; }
	// 获取线条
	const std::vector<QPointF> & line() const { return m_points; }

	std::pair<double, double> rangeX();
	std::pair<double, double> rangeY();

private:
	QRectF m_area;
	std::vector<QPointF> m_points;

	QPen m_pen;
};

template <class T>
void LinePanel::setLine(std::pair<T, T> * points, int count)
{
	if (count <= 0) {
		m_points.clear();
	}
	else {
		m_points.resize(count);
		for (int i = 0; i < count; i++) {
			m_points[i].rx() = points[i].first;
			m_points[i].ry() = points[i].second;
		}
	}
}

#endif //PAINT_PANEL_H