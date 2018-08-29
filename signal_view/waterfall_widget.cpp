#include <QPainter>
#include <QKeyEvent>
#include <QWheelEvent>
#include <assert.h>

#include "waterfall_widget.h"
#include "waterfall.h"
#include "misc.h"

WaterfallWidget::WaterfallWidget(QWidget *parent)
	: QWidget(parent)
{
	m_waterfall.reset(new Waterfall());
}

WaterfallWidget::~WaterfallWidget()
{
	assert(m_waterfall);

	m_waterfall.reset();
}

bool WaterfallWidget::load(QString filename, int type, double samplerate)
{
	assert(m_waterfall);

	bool loaded = m_waterfall->load(filename.toStdString(), type, samplerate);
	if (loaded) {
		//m_totalArea = m_waterfall->totalArea();
		m_visibleArea = m_waterfall->totalArea();

		//m_dataArea = m_totalArea;		
	}

	return loaded;
}

void WaterfallWidget::close()
{
	assert(m_waterfall);

	m_waterfall->close();
}

void WaterfallWidget::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);

	painter.setRenderHint(QPainter::SmoothPixmapTransform);

	QRect bound = rect();
	painter.fillRect(bound, QColor::fromRgb(0, 0, 0));

	// 绘制频谱数据.
	QRect viewport = rect(); // 控件视口（像素，设备）
	//viewport.setWidth(viewport.width() + 1);
	//viewport.setHeight(viewport.height() + 1);

	QRectF visibleArea = m_visibleArea;

	if (m_waterfall->prepare(visibleArea, { 200, 100 })) {
		painter.setClipRect(viewport);

		QRectF pixmapArea = m_waterfall->currentArea();
		QPixmap & pixmap = m_waterfall->pixmap();
		QRect pixmapRect = pixmap.rect();
		
		QRectF drawArea = map(pixmapArea, visibleArea, viewport);
		painter.drawPixmap(drawArea, pixmap, pixmapRect);

		painter.setClipRect(QRect());
	}
}

void WaterfallWidget::resizeEvent(QResizeEvent *event)
{
	repaint();
}

void WaterfallWidget::showEvent(QShowEvent * evt)
{
	repaint();
}

QRectF WaterfallWidget::totalArea()
{
	return m_waterfall->totalArea();
}

QRectF WaterfallWidget::visibleArea()
{
	return m_visibleArea;
}

void WaterfallWidget::setVisibleArea(QRectF r)
{
	m_visibleArea = r.normalized();
}



QRectF WaterfallWidget::map(QRectF worldB, QRectF worldA, QRectF viewA2)
{
	QRectF viewA = viewA2;

	double ratioX = viewA.width() / worldA.width();
	double ratioY = viewA.height() / worldA.height();

	double startX = viewA.left() + (worldB.left() - worldA.left()) * ratioX;
	double startY = viewA.top() + (worldB.top() - worldA.top()) * ratioY;

	QRectF ret(QPointF(startX, startY), QSizeF(worldB.width() * ratioX, worldB.height() * ratioY));

	return ret;
}

void WaterfallWidget::wheelEvent(QWheelEvent * evt)
{
	auto angles = evt->angleDelta();

	if (! angles.isNull()) {

		if (angles.ry() > 0) {
			executeCommand(FreqZoomIn, 0.1);
		}
		else {
			executeCommand(FreqZoomOut, 0.1);
		}
	}

	evt->accept();
}

void WaterfallWidget::keyPressEvent(QKeyEvent * evt)
{
	bool ctrl = evt->modifiers() & Qt::ControlModifier;
	bool shift = evt->modifiers() & Qt::ShiftModifier;
	bool alt = evt->modifiers() & Qt::AltModifier;

	switch (evt->key()) {
	case Qt::Key_Left:
		if (ctrl)
			executeCommand(FreqZoomIn, 0.1);
		else
			executeCommand(TimeBackward, 0.1);
		break;

	case Qt::Key_Right:
		if (ctrl)
			executeCommand(FreqZoomOut, 0.1);
		else
			executeCommand(TimeForward, 0.1);
		break;

	case Qt::Key_Up:
		if (ctrl)
			executeCommand(FreqZoomIn, 0.1);
		else
			executeCommand(FreqForward, 0.1);
		break;

	case Qt::Key_Down:
		if (ctrl)
			executeCommand(FreqZoomOut, 0.1);
		else
			executeCommand(FreqBackward, 0.1);
		break;

	case Qt::Key_A:
		executeCommand(ColorRangeUp);
		break;

	case Qt::Key_D:
		executeCommand(ColorRangeDown);
		break;
			
	case Qt::Key_S:
		executeCommand(ColorRangeDec);
		break;

	case Qt::Key_W:
		executeCommand(ColorRangeAdd);
		break;

	case Qt::Key_Q:
		executeCommand(Reset);
		break;

	default:
		return;
	}

	evt->accept();
}

void WaterfallWidget::executeCommand(WaterfallWidget::Command type, double param)
{
	auto totalArea = this->totalArea();
	bool dirty = false;
	double value;

	switch (type) {
	case Reset:
		if (m_visibleArea != totalArea) {
			m_visibleArea = totalArea;
			dirty = true;
		}
		break;

	case TimeForward:
		value = std::max<double>(m_visibleArea.left() - m_visibleArea.width() * param, totalArea.left());
		if (value != m_visibleArea.left()) {
			m_visibleArea.moveLeft(value);
			dirty = true;
		}
		break;

	case TimeBackward:
		value = std::min<double>(m_visibleArea.right() + m_visibleArea.width() * param, totalArea.right());
		if (value != m_visibleArea.right()) {
			m_visibleArea.moveRight(value);
			dirty = true;
		}
		break;

	case FreqForward:
		value = std::max<double>(m_visibleArea.top() - m_visibleArea.height() * param, totalArea.top());
		if (value != m_visibleArea.top()) {
			m_visibleArea.moveTop(value);
			dirty = true;
		}
		break;

	case FreqBackward:
		value = std::min<double>(m_visibleArea.bottom() + m_visibleArea.height() * param, totalArea.bottom());
		if (value != m_visibleArea.bottom()) {
			m_visibleArea.moveBottom(value);
			dirty = true;
		}
		break;

	case FreqZoomIn:
		value = std::min<double>(m_visibleArea.width() * (1 + param), totalArea.width());
		if (value != m_visibleArea.width()) {
			auto center = m_visibleArea.center();
			m_visibleArea.setWidth(value);
			m_visibleArea.moveCenter(center);
			dirty = true;
		}
		break;

	case FreqZoomOut:
		value = std::min<double>(m_visibleArea.width() * (1 - param), totalArea.width());
		if (value != m_visibleArea.width()) {
			auto center = m_visibleArea.center();
			m_visibleArea.setWidth(value);
			m_visibleArea.moveCenter(center);
			dirty = true;
		}
		break;

	case TimeZoomIn:
		value = std::min<double>(m_visibleArea.height() * (1 + param), totalArea.height());
		if (value != m_visibleArea.height()) {
			auto center = m_visibleArea.center();
			m_visibleArea.setHeight(value);
			m_visibleArea.moveCenter(center);
			dirty = true;
		}
		break;

	case TimeZoomOut:
		value = std::min<double>(m_visibleArea.height() * (1 - param), totalArea.height());
		if (value != m_visibleArea.height()) {
			auto center = m_visibleArea.center();
			m_visibleArea.setHeight(value);
			m_visibleArea.moveCenter(center);
			dirty = true;
		}
		break;

	case ColorRangeDown:
	case ColorRangeUp:
	{
		auto rng = m_waterfall->colorRange();
		double diff = (type == ColorRangeUp) ? 5 : -5;
		rng.first += diff;
		rng.second += diff;
		m_waterfall->setColorRange(rng);
		dirty = true;
		break;
	}

	case ColorRangeAdd:
	case ColorRangeDec:
	{
		auto rng = m_waterfall->colorRange();
		float width = tool::length(rng);
		float diff = (type == ColorRangeAdd) ? width * 0.05 : width * -0.05;
		rng.first -= diff;
		rng.second += diff;
		m_waterfall->setColorRange(rng);
		dirty = true;
		break;
	}
	
	default:
		return;
	}

	if (dirty) {
		m_visibleArea = limitArea(m_visibleArea, totalArea);
		repaint();
	}
}



QRectF WaterfallWidget::limitArea(QRectF vis, QRectF total, bool keepsize)
{
	QRectF ret = vis;
	double left, right;
	QSizeF sz = vis.size();

	if (ret.left() < total.left() && ret.right() > total.right()) {
		ret.setLeft(total.left());
		ret.setRight(total.right());
	}
	else {
		if (ret.left() < total.left()) {
			ret.setLeft(total.left());
			if (keepsize) {
				ret.setRight(ret.left() + vis.width());
			}

			ret.setRight(std::max<double>(ret.left(), ret.right()));
		}

		if (ret.right() > total.right()) {
			ret.setRight(total.right());
			if (keepsize) {
				ret.setLeft(ret.right() - vis.width());
			}
			ret.setLeft(std::min<double>(ret.left(), ret.right()));
		}
	}

	if (ret.top() < total.top() && ret.bottom() > total.bottom()) {
		ret.setTop(total.top());
		ret.setBottom(total.bottom());
	}
	else {
		if (ret.top() < total.top()) {
			ret.setTop(total.top());
			if (keepsize) {
				ret.setBottom(ret.top() + vis.height());
			}

			ret.setBottom(std::max<double>(ret.top(), ret.bottom()));
		}

		if (ret.bottom() > total.bottom()) {
			ret.setBottom(total.bottom());
			if (keepsize) {
				ret.setTop(ret.bottom() - vis.height());
			}
			ret.setTop(std::min<double>(ret.top(), ret.bottom()));
		}
	}
	
	ret.setLeft(std::max<double>(ret.left(), total.left()));
	ret.setRight(std::min<double>(ret.right(), total.right()));
	ret.setTop(std::max<double>(ret.top(), total.top()));
	ret.setBottom(std::min<double>(ret.bottom(), total.bottom()));

	return ret;
}