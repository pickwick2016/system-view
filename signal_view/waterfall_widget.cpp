#include <QPainter>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QDebug>
#include <assert.h>

#include "waterfall_widget.h"
#include "waterfall.h"
#include "misc.h"
#include "paint_panel.h"

WaterfallWidget::WaterfallWidget(QWidget *parent) : SignalWidget(parent)
{
	m_needNotify = true;
	m_mousePressed = false;
	m_loader.reset(new WaterfallLoader());

	setMouseTracking(true);
	setMinimumSize(QSize(100, 75));

	appendShortcuts();
}

WaterfallWidget::~WaterfallWidget()
{
	assert(m_loader);

	m_loader.reset();
	m_shortcuts.clear();
}

void WaterfallWidget::syncView(QRectF area)
{
	tool::ValueGuard<bool> notifyGuard(m_needNotify, false);

	QRectF newArea = visibleArea();
	if (area.width() > 0) {
		newArea.setLeft(area.left());
		newArea.setRight(area.right());
	}

	if (newArea != visibleArea()) {
		setVisibleArea(newArea);
	}
}

void WaterfallWidget::reload(QRectF area)
{

}

void WaterfallWidget::load(std::shared_ptr<Reader> reader)
{
	m_loader->load(reader);

	executeCommand(SignalCommand::XY_Reset);
	
	//bool loaded = m_loader->isLoaded();
	//if (loaded) {
	//	m_visibleArea = m_loader->totalArea();
	//}
	//else {
	//	m_visibleArea = QRectF();
	//}
}

bool WaterfallWidget::load(QString filename, int type, double samplerate)
{
	assert(m_loader);

	m_loader->load(filename.toStdString(), type, samplerate);

	executeCommand(SignalCommand::XY_Reset);
	//
	bool loaded = m_loader->isLoaded();
	//if (loaded) {
	//	m_visibleArea = m_loader->totalArea();
	//}
	//else {
	//	m_visibleArea = QRectF();
	//}

	return loaded;
}

void WaterfallWidget::close()
{
	assert(m_loader);

	m_loader->close();
}


void WaterfallWidget::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);

	painter.setRenderHint(QPainter::SmoothPixmapTransform);

	QRect bound = rect();
	painter.fillRect(bound, QColor::fromRgb(0, 0, 0));

	drawTimeBar(painter);
	drawFreqBar(painter);
	drawData(painter);
	drawSelection(painter);

#ifdef _DEBUG
	static int counter = 0;
	QString str = QString::asprintf("waterfall_widget refresh counter : %d", counter++);
	qDebug() << str;
#endif //_DEBUG

}

void WaterfallWidget::drawTimeBar(QPainter & painter)
{
	auto bound = this->viewport(1);
	if (bound.isEmpty())
		return;

	QRectF visibleArea = this->visibleArea();

	MarkerPanel marker;
	marker.setRect(bound);
	marker.setRange({ visibleArea.left(), visibleArea.right() });
	marker.paint(&painter);
}

void WaterfallWidget::drawFreqBar(QPainter & painter)
{
	auto bound = this->viewport(2);
	if (bound.isEmpty())
		return;

	QRectF visibleArea = this->visibleArea();

	MarkerPanel marker;
	marker.setRect(bound);
	marker.setDirection(1);
	marker.setRange({ visibleArea.top(), visibleArea.bottom() });
	marker.paint(&painter);
}

void WaterfallWidget::drawData(QPainter & painter)
{
	QRectF bound = this->viewport(); // 控件视口（像素，设备）

	QRectF visibleArea = this->visibleArea();

	if (m_loader->prepare(visibleArea, { 200, 100 })) {
		painter.setClipRect(bound);

		QRectF pixmapArea = m_loader->pixmapArea();
		QPixmap & pixmap = m_loader->pixmap();
		QRect pixmapRect = pixmap.rect();

		QRectF drawArea = tool::map(pixmapArea, visibleArea, tool::rectFlipY(bound));
		drawArea = tool::rectFlipY(drawArea);
		painter.drawPixmap(drawArea, pixmap, pixmapRect);

		painter.setClipRect(QRect());
	}
}


void WaterfallWidget::drawSelection(QPainter & painter)
{
	QRectF selection = selectArea();
	if (! selection.isEmpty()) {
		QRectF bound = this->viewport(); // 控件视口（像素，设备）
		painter.setClipRect(bound);

		QRectF drawArea = tool::map(selection, visibleArea(), tool::rectFlipY(bound));
		drawArea = drawArea.normalized();

		painter.setPen(QPen(QColor(255, 255, 255), 2, Qt::DotLine));
		painter.drawRect(drawArea);

		painter.setClipRect(QRectF());
	}
}

QRectF WaterfallWidget::selectArea()
{
	if (m_dragStartPos == m_dragEndPos) {
		return QRectF();
	}

	QRectF ret(m_dragStartPos, m_dragEndPos);
	QRectF total = totalArea();
	ret = tool::clip(ret.normalized(), total);
	return ret;
}

QRectF WaterfallWidget::viewport(int tag)
{
	QRectF bound = rect();
	bound = tool::rectExpand(bound, -1);
	
	QRectF temp;

	int rightBand = 64;
	int rightPos = bound.right() - rightBand;
	int upBand = 24;
	int bottomPos = bound.top() + upBand;

	if (tag == 0) {
		temp.setTopLeft(QPointF(bound.left(), bottomPos + 1));
		temp.setBottomRight(QPointF(rightPos, bound.bottom()));
		return temp;
	}
	else if (tag == 1) {
		temp.setTopLeft(QPointF(bound.left(), bound.top()));
		temp.setBottomRight(QPointF(rightPos, bottomPos));
		return temp;
	}
	else if (tag == 2) {
		temp.setTopLeft(QPointF(rightPos + 1, bottomPos + 1));
		temp.setBottomRight(QPointF(bound.right(), bound.bottom()));
		return temp;
	}

	return QRectF();
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
	if (m_loader)
		return m_loader->totalArea();

	return QRectF();
}



void WaterfallWidget::setVisibleArea(QRectF r)
{
	QRectF newArea = tool::clip(r.normalized(), totalArea());
	

	if (newArea != visibleArea()) {
		SignalWidget::setVisibleArea(newArea);

		update(); //repaint();

		if (m_needNotify) {
			emit viewChanged(visibleArea());
		}
	}
}

void WaterfallWidget::wheelEvent(QWheelEvent * evt)
{
	bool ctrl = evt->modifiers() & Qt::ControlModifier;

	auto pos = evt->pos();
	auto viewport = this->viewport();
	auto pxy = tool::map(pos, viewport, tool::rectFlipY(visibleArea()));
	
	auto angles = evt->angleDelta();

	//if (!angles.isNull()) {

	//	if (ctrl) {
	//		if (angles.ry() > 0) {
	//			executeCommand(FreqZoomInAt, pxy.ry());
	//		}
	//		else {
	//			executeCommand(FreqZoomOutAt, pxy.ry());
	//		}
	//	}
	//	else {
	//		if (angles.ry() > 0) {
	//			executeCommand(TimeZoomInAt, pxy.rx());
	//		}
	//		else {
	//			executeCommand(TimeZoomOutAt, pxy.rx());
	//		}
	//	}
	//}

	//evt->accept();
}

void WaterfallWidget::keyPressEvent(QKeyEvent * evt)
{
	SignalWidget::keyPressEvent(evt);
	if (evt->isAccepted()) {
		return;
	}
}

bool WaterfallWidget::executeCommand(int cmd, QVariant param)
{
	if (SignalWidget::executeCommand(cmd, param)) {
		return true;
	}

	bool dirty = false;

	switch (cmd) {
	case ColorRangeDown:
	case ColorRangeUp:
	{
		auto rng = m_loader->colorRange();
		float diff = (cmd == ColorRangeUp) ? 5 : -5;
		rng = tool::range_move<float>(rng, diff);
		m_loader->setColorRange(rng);
		dirty = true;
		break;
	}

	case ColorRangeAdd:
	case ColorRangeDec:
	{
		auto rng = m_loader->colorRange();
		float width = tool::range_length(rng);
		float diff = (cmd == ColorRangeAdd) ? width * 0.05 : width * -0.05;

		rng = tool::range_expand<float>(rng, diff);
		m_loader->setColorRange(rng);
		dirty = true;
		break;
	}

	case ColorRangeAuto:
	{
		std::pair<float, float> rng = m_loader->currentValueRange();
		if (rng != std::pair<float, float>(0.0f, 0.0f)) {
			float val = tool::range_length(rng) * param.toDouble();
			m_loader->setColorRange({ rng.first - val, rng.second + val });
			dirty = true;
		}
		break;
	}

	case ToggleAutoFft:
		m_loader->setAutoFft(!m_loader->isAutoFft());
		dirty = true;
		break;

	case ReloadSelect:
		if (! selectArea().isEmpty()) {
			setVisibleArea(selectArea());
		}
		break;

	default:
		return false;
	}

	if (dirty) {
		repaint();
	}

	return true;
}

void WaterfallWidget::mousePressEvent(QMouseEvent *evt)
{
	if (evt->buttons() & Qt::LeftButton) {

		m_mousePressed = true;

		bool hasSelection = (m_dragStartPos != m_dragEndPos);
		
		m_dragStartPoint = evt->pos();
		m_dragStartPos = tool::map(evt->pos(), tool::rectFlipY(viewport()), visibleArea());

		m_dragEndPoint = m_dragStartPoint;
		m_dragEndPos = m_dragStartPos;

		if (hasSelection) {
			repaint();
		}
	}
}

void WaterfallWidget::mouseMoveEvent(QMouseEvent *evt)
{
	auto pos = evt->pos();
	auto viewport = this->viewport(0);

	if (viewport.contains(pos)) {
		auto pos2 = tool::map(evt->pos(), tool::rectFlipY(viewport), visibleArea());
		emit positionMoved(pos2);
	}
	
	if (m_mousePressed) {
		m_dragEndPoint = evt->pos();
		m_dragEndPos = tool::map(evt->pos(), tool::rectFlipY(viewport), visibleArea());;
		repaint();
	}
}

void WaterfallWidget::mouseReleaseEvent(QMouseEvent *evt)
{
	bool hasSelection = (m_dragStartPos != m_dragEndPos);
	if (m_mousePressed && hasSelection) {
		repaint();
	}

	m_mousePressed = false;
}

QRectF WaterfallWidget::setVisible(WaterfallCommand cmd, double param)
{
	QRectF ret = visibleArea();
	QRectF totalArea = this->totalArea();

	//switch (cmd) {
	//case XY_Reset:
	//	ret = totalArea;
	//	break;

	//case ResetTime:
	//	ret.setLeft(totalArea.left());
	//	ret.setRight(totalArea.right());
	//	break;

	//case ResetFreq:
	//	ret.setTop(totalArea.top());
	//	ret.setBottom(totalArea.bottom());
	//	break;

	//case TimeForward:
	//	ret = tool::rectMoveX(m_visibleArea, m_visibleArea.width() * 0.1);
	//	break;

	//case TimeBackward:
	//	ret = tool::rectMoveX(m_visibleArea, - m_visibleArea.width() * 0.1);
	//	break;

	//case TimeZoomIn:
	//	ret = tool::rectExpandX(m_visibleArea, m_visibleArea.width() * 0.1);
	//	break;

	//case TimeZoomOut:
	//	ret = tool::rectExpandX(m_visibleArea, -m_visibleArea.width() * 0.1);
	//	break;

	//case TimeZoomInAt:
	//	if (tool::range_contain({ m_visibleArea.left(), m_visibleArea.right() }, param) > 0) {
	//		double r0 = param - m_visibleArea.left();
	//		double r1 = m_visibleArea.right() - param;
	//		ret.setLeft(param - r0 * 1.1);
	//		ret.setRight(param + r1 * 1.1);
	//	}
	//	break;

	//case TimeZoomOutAt:
	//	if (tool::range_contain({ m_visibleArea.left(), m_visibleArea.right() }, param) > 0) {
	//		double r0 = param - m_visibleArea.left();
	//		double r1 = m_visibleArea.right() - param;
	//		ret.setLeft(param - r0 * 0.9);
	//		ret.setRight(param + r1 * 0.9);
	//	}
	//	break;

	//case FreqForward:
	//	ret = tool::rectMoveY(m_visibleArea, m_visibleArea.height() * 0.1);
	//	break;

	//case FreqBackward:
	//	ret = tool::rectMoveY(m_visibleArea, -m_visibleArea.height() * 0.1);
	//	break;
	//
	//case FreqZoomIn:
	//	ret = tool::rectExpandY(m_visibleArea, m_visibleArea.height() * 0.1);
	//	break;

	//case FreqZoomOut:
	//	ret = tool::rectExpandY(m_visibleArea, -m_visibleArea.height() * 0.1);
	//	break;

	//case FreqZoomInAt:
	//	if (tool::range_contain({ m_visibleArea.top(), m_visibleArea.bottom() }, param) > 0) {
	//		double r0 = param - m_visibleArea.top();
	//		double r1 = m_visibleArea.bottom() - param;
	//		ret.setTop(param - r0 * 1.1);
	//		ret.setBottom(param + r1 * 1.1);

	//	}
	//	break;

	//case FreqZoomOutAt:
	//	if (tool::range_contain({ m_visibleArea.top(), m_visibleArea.bottom() }, param) > 0) {
	//		double r0 = param - m_visibleArea.top();
	//		double r1 = m_visibleArea.bottom() - param;
	//		ret.setTop(param - r0 * 0.9);
	//		ret.setBottom(param + r1 * 0.9);

	//	}
	//	break;

	//default:
	//	return QRectF();
	//}
	//
	//ret = tool::adjust(ret, totalArea);
	return ret;
}


void WaterfallWidget::appendShortcuts()
{
	// 1.初始化快捷键.
	m_shortcuts[ReloadSelect] = { Qt::Key_R, false, false, false };
	m_shortcuts[ColorRangeAuto] = { Qt::Key_C, false, false, false };
	//m_shortcuts[ToggleAutoFft] = { Qt::Key_F, false, false, false };

	// 2.初始化命令参数.
	m_commandParams[ReloadSelect] = QVariant(0.05);
	m_commandParams[ColorRangeAuto] = QVariant(0.2);
	
}