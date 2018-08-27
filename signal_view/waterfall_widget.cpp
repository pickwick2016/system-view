#include <QPainter>
#include <QKeyEvent>
#include <QWheelEvent>
#include <assert.h>

#include "waterfall_widget.h"
#include "waterfall.h"

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
	QRectF visibleArea = m_visibleArea;

	if (m_waterfall->prepare(visibleArea, 200)) {
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
			operate(HorzZoomIn, 0.1);
		}
		else {
			operate(HorzZoomOut, 0.1);
		}
	}

	evt->accept();
}

void WaterfallWidget::keyPressEvent(QKeyEvent * evt)
{
	switch (evt->key()) {
	case Qt::Key_Left:
		operate(Left, 0.1);
		break;

	case Qt::Key_A:
		operate(HorzZoomIn, 0.1);
		break;

	case Qt::Key_Right:
		operate(Right, 0.1);
		break;

	case Qt::Key_D:
		operate(HorzZoomOut, 0.1);
		break;

	case Qt::Key_Up:
		operate(Up, 0.1);
		break;

	case Qt::Key_S:
		operate(VertZoomIn, 0.1);
		break;

	case Qt::Key_Down:
		operate(Down, 0.1);
		break;

	case Qt::Key_W:
		operate(VertZoomOut, 0.1);
		break;

	case Qt::Key_Q:
		operate(Reset);
		break;

	default:
		return;
	}

	evt->accept();
}

void WaterfallWidget::operate(WaterfallWidget::OpType type, double param)
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

	case Left:
		value = std::max<double>(m_visibleArea.left() - m_visibleArea.width() * param, totalArea.left());
		if (value != m_visibleArea.left()) {
			m_visibleArea.moveLeft(value);
			dirty = true;
		}
		break;

	case Right:
		value = std::min<double>(m_visibleArea.right() + m_visibleArea.width() * param, totalArea.right());
		if (value != m_visibleArea.right()) {
			m_visibleArea.moveRight(value);
			dirty = true;
		}
		break;

	case Up:
		value = std::max<double>(m_visibleArea.top() - m_visibleArea.height() * param, totalArea.top());
		if (value != m_visibleArea.top()) {
			m_visibleArea.moveTop(value);
			dirty = true;
		}
		break;

	case Down:
		value = std::min<double>(m_visibleArea.bottom() + m_visibleArea.height() * param, totalArea.bottom());
		if (value != m_visibleArea.bottom()) {
			m_visibleArea.moveBottom(value);
			dirty = true;
		}
		break;

	case HorzZoomIn:
		value = std::min<double>(m_visibleArea.width() * (1 + param), totalArea.width());
		if (value != m_visibleArea.width()) {
			auto center = m_visibleArea.center();
			m_visibleArea.setWidth(value);
			m_visibleArea.moveCenter(center);
			dirty = true;
		}
		break;

	case HorzZoomOut:
		value = std::min<double>(m_visibleArea.width() * (1 - param), totalArea.width());
		if (value != m_visibleArea.width()) {
			auto center = m_visibleArea.center();
			m_visibleArea.setWidth(value);
			m_visibleArea.moveCenter(center);
			dirty = true;
		}
		break;

	case VertZoomIn:
		value = std::min<double>(m_visibleArea.height() * (1 + param), totalArea.height());
		if (value != m_visibleArea.height()) {
			auto center = m_visibleArea.center();
			m_visibleArea.setHeight(value);
			m_visibleArea.moveCenter(center);
			dirty = true;
		}
		break;

	case VertZoomOut:
		value = std::min<double>(m_visibleArea.height() * (1 - param), totalArea.height());
		if (value != m_visibleArea.height()) {
			auto center = m_visibleArea.center();
			m_visibleArea.setHeight(value);
			m_visibleArea.moveCenter(center);
			dirty = true;
		}
		break;

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