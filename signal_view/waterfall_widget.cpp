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
	QRectF viewport = this->viewport(); // 控件视口（像素，设备）

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

QRectF WaterfallWidget::viewport()
{
	QRectF ret = rect();
	ret = tool::rectExpand(ret, -1);
	return ret;
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
	bool ctrl = evt->modifiers() & Qt::ControlModifier;

	auto pos = evt->pos();
	m_visibleArea;

	auto viewport = this->viewport();
	
	double dx = pos.rx() - viewport.left();
	double dy = pos.ry() - viewport.top();

	double px = m_visibleArea.left() + dx / viewport.width() * m_visibleArea.width();
	double py = m_visibleArea.top() + dy / viewport.height() * m_visibleArea.height();

	auto angles = evt->angleDelta();

	if (!angles.isNull()) {

		if (ctrl) {
			if (angles.ry() > 0) {
				executeCommand(FreqZoomInAt, py);
			}
			else {
				executeCommand(FreqZoomOutAt, py);
			}
		}
		else {
			if (angles.ry() > 0) {
				executeCommand(TimeZoomInAt, px);
			}
			else {
				executeCommand(TimeZoomOutAt, px);
			}
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
			executeCommand(TimeZoomIn, 0.1);
		else
			executeCommand(TimeBackward, 0.1);
		break;

	case Qt::Key_Right:
		if (ctrl)
			executeCommand(TimeZoomOut, 0.1);
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

	case Qt::Key_O:
		executeCommand(ColorRangeAuto);
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
	bool dirty = false;

	switch (type) {
	case Reset:
	case ResetTime:
	case ResetFreq:
	case TimeBackward:
	case TimeForward:
	case TimeZoomIn:
	case TimeZoomOut:
	case TimeZoomInAt:
	case TimeZoomOutAt:
	case FreqForward:
	case FreqBackward:
	case FreqZoomIn:
	case FreqZoomOut:
	case FreqZoomInAt:
	case FreqZoomOutAt:
	{
		auto visible = setVisible(type, param);
		if (visible != m_visibleArea) {
			m_visibleArea = visible;
			dirty = true;
		}
		break;
	}

	case ColorRangeDown:
	case ColorRangeUp:
	{
		auto rng = m_waterfall->colorRange();
		float diff = (type == ColorRangeUp) ? 5 : -5;
		rng = tool::range_move<float>(rng, diff);
		m_waterfall->setColorRange(rng);
		dirty = true;
		break;
	}

	case ColorRangeAdd:
	case ColorRangeDec:
	{
		auto rng = m_waterfall->colorRange();
		float width = tool::range_length(rng);
		float diff = (type == ColorRangeAdd) ? width * 0.05 : width * -0.05;

		rng = tool::range_expand<float>(rng, diff);
		m_waterfall->setColorRange(rng);
		dirty = true;
		break;
	}

	case ColorRangeAuto:
	{
		std::pair<float, float> rng = m_waterfall->currentValueRange();
		if (rng != std::pair<float, float>(0, 0)) {
			float val = tool::range_length(rng) * 0.2;
			m_waterfall->setColorRange({ rng.first - val, rng.second + val });
			dirty = true;
		}
		break;
	}
	default:
		return;
	}

	if (dirty) {
		repaint();
	}
}



//QRectF WaterfallWidget::limitArea(QRectF vis, QRectF total, bool keepsize)
//{
//	QRectF ret = vis;
//	double left, right;
//	QSizeF sz = vis.size();
//
//	if (ret.left() < total.left() && ret.right() > total.right()) {
//		ret.setLeft(total.left());
//		ret.setRight(total.right());
//	}
//	else {
//		if (ret.left() < total.left()) {
//			ret.setLeft(total.left());
//			if (keepsize) {
//				ret.setRight(ret.left() + vis.width());
//			}
//
//			ret.setRight(std::max<double>(ret.left(), ret.right()));
//		}
//
//		if (ret.right() > total.right()) {
//			ret.setRight(total.right());
//			if (keepsize) {
//				ret.setLeft(ret.right() - vis.width());
//			}
//			ret.setLeft(std::min<double>(ret.left(), ret.right()));
//		}
//	}
//
//	if (ret.top() < total.top() && ret.bottom() > total.bottom()) {
//		ret.setTop(total.top());
//		ret.setBottom(total.bottom());
//	}
//	else {
//		if (ret.top() < total.top()) {
//			ret.setTop(total.top());
//			if (keepsize) {
//				ret.setBottom(ret.top() + vis.height());
//			}
//
//			ret.setBottom(std::max<double>(ret.top(), ret.bottom()));
//		}
//
//		if (ret.bottom() > total.bottom()) {
//			ret.setBottom(total.bottom());
//			if (keepsize) {
//				ret.setTop(ret.bottom() - vis.height());
//			}
//			ret.setTop(std::min<double>(ret.top(), ret.bottom()));
//		}
//	}
//
//	ret = tool::clip(ret, total);
//
//	return ret;
//}

QRectF WaterfallWidget::setVisible(WaterfallWidget::Command cmd, double param)
{
	QRectF ret = m_visibleArea;
	QRectF totalArea = this->totalArea();

	switch (cmd) {
	case Reset:
		ret = totalArea;
		break;

	case ResetTime:
		ret.setLeft(totalArea.left());
		ret.setRight(totalArea.right());
		break;

	case ResetFreq:
		ret.setTop(totalArea.top());
		ret.setBottom(totalArea.bottom());
		break;

	case TimeForward:
		ret = tool::rectMoveX(m_visibleArea, m_visibleArea.width() * param);
		break;

	case TimeBackward:
		ret = tool::rectMoveX(m_visibleArea, - m_visibleArea.width() * param);
		break;

	case TimeZoomIn:
		ret = tool::rectExpandX(m_visibleArea, m_visibleArea.width() * param);
		break;

	case TimeZoomOut:
		ret = tool::rectExpandX(m_visibleArea, -m_visibleArea.width() * param);
		break;

	case TimeZoomInAt:
		if (tool::range_contain({ m_visibleArea.left(), m_visibleArea.right() }, param) > 0) {
			double r0 = param - m_visibleArea.left();
			double r1 = m_visibleArea.right() - param;
			ret.setLeft(param - r0 * 1.1);
			ret.setRight(param + r1 * 1.1);
		}
		break;

	case TimeZoomOutAt:
		if (tool::range_contain({ m_visibleArea.left(), m_visibleArea.right() }, param) > 0) {
			double r0 = param - m_visibleArea.left();
			double r1 = m_visibleArea.right() - param;
			ret.setLeft(param - r0 * 0.9);
			ret.setRight(param + r1 * 0.9);
		}
		break;

	case FreqForward:
		ret = tool::rectMoveY(m_visibleArea, m_visibleArea.height() * param);
		break;

	case FreqBackward:
		ret = tool::rectMoveY(m_visibleArea, -m_visibleArea.height() * param);
		break;
	
	case FreqZoomIn:
		ret = tool::rectExpandY(m_visibleArea, m_visibleArea.height() * param);
		break;

	case FreqZoomOut:
		ret = tool::rectExpandY(m_visibleArea, -m_visibleArea.height() * param);
		break;

	case FreqZoomInAt:
		if (tool::range_contain({ m_visibleArea.top(), m_visibleArea.bottom() }, param) > 0) {
			double r0 = param - m_visibleArea.top();
			double r1 = m_visibleArea.bottom() - param;
			ret.setTop(param - r0 * 1.1);
			ret.setBottom(param + r1 * 1.1);

		}
		break;

	case FreqZoomOutAt:
		if (tool::range_contain({ m_visibleArea.top(), m_visibleArea.bottom() }, param) > 0) {
			double r0 = param - m_visibleArea.top();
			double r1 = m_visibleArea.bottom() - param;
			ret.setTop(param - r0 * 0.9);
			ret.setBottom(param + r1 * 0.9);

		}
		break;

	default:
		return QRectF();
	}
	
	ret = tool::adjust(ret, totalArea);
	return ret;
}