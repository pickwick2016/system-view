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
	m_needNotify = true;
	m_mousePressed = false;
	m_loader.reset(new WaterfallLoader());

	setMouseTracking(true);
	setMinimumSize(QSize(100, 75));

	initShortcuts();
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

	QRectF newArea = m_visibleArea;
	if (area.width() > 0) {
		newArea.setLeft(area.left());
		newArea.setRight(area.right());
	}

	if (area.height() > 0) {
		newArea.setTop(area.top());
		newArea.setBottom(area.bottom());
	}

	if (newArea != m_visibleArea) {
		setVisibleArea(newArea);
	}
}

void WaterfallWidget::reload(QRectF area)
{

}

void WaterfallWidget::load(std::shared_ptr<Reader> reader)
{
	m_loader->load(reader);

	bool loaded = m_loader->isLoaded();
	if (loaded) {
		m_visibleArea = m_loader->totalArea();
	}
	else {
		m_visibleArea = QRectF();
	}
}

bool WaterfallWidget::load(QString filename, int type, double samplerate)
{
	assert(m_loader);

	m_loader->load(filename.toStdString(), type, samplerate);
	
	bool loaded = m_loader->isLoaded();
	if (loaded) {
		m_visibleArea = m_loader->totalArea();
	}
	else {
		m_visibleArea = QRectF();
	}

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
}

void WaterfallWidget::drawTimeBar(QPainter & painter)
{
	auto bound = this->viewport(1);
	if (bound.isEmpty())
		return;

	std::vector<double> times;
	double step = tool::range_split({ m_visibleArea.left(), m_visibleArea.right() }, 10, times);
	if (times.empty())
		return;

	int e = std::floorf(std::log10f(step));
	QString fmtStr = "%.0f";
	if (e < 0) {
		fmtStr = QString::asprintf("%.%df", -e);
	}

	QPointF boundCenter = bound.center();
	QRectF textRect(bound.topLeft(), QSizeF(bound.width() / 10, bound.height()));
	painter.setClipRect(bound);
	painter.setPen(QPen(QColor(255, 255, 255)));
	for (auto time : times) {
		double ptx = tool::map<double>(time,
			{ m_visibleArea.left(), m_visibleArea.right() }, { bound.left(), bound.right() });
		
		painter.drawLine(QPointF(ptx, bound.bottom()), QPointF(ptx, bound.bottom() - 5));

		QString text = QString::asprintf(fmtStr.toStdString().c_str(), time);
		textRect.moveCenter({ ptx, boundCenter.ry() });
		painter.drawText(textRect, Qt::AlignCenter, text);
	}
	painter.setClipRect(QRectF());
}

void WaterfallWidget::drawFreqBar(QPainter & painter)
{
	auto bound = this->viewport(2);
	if (bound.isEmpty())
		return;

	std::vector<double> freqs;
	double step = tool::range_split({ m_visibleArea.top(), m_visibleArea.bottom() }, 10, freqs);
	if (freqs.empty())
		return;

	int e = std::floorf(std::log10f(step));
	QString formatText = "%.0f";
	if (e < 0) {
		formatText = QString::asprintf("%.%df", -e);
	}

	QPointF boundCenter = bound.center();
	QRectF textRect(bound.topLeft(), QSizeF(bound.width(), bound.height() / 10));
	painter.setClipRect(bound);
	painter.setPen(QPen(QColor(255, 255, 255)));
	for (auto freq : freqs) {
		double ptx = tool::map<double>(freq,
			{ m_visibleArea.top(), m_visibleArea.bottom() }, { bound.bottom(), bound.top() });

		painter.drawLine(QPointF(bound.left(), ptx), QPointF(bound.left() + 4, ptx));

		QString text = QString::asprintf(formatText.toStdString().c_str(), freq);
		textRect.moveCenter({ boundCenter.rx(), ptx });
		painter.drawText(textRect, Qt::AlignCenter, text);
	}
	painter.setClipRect(QRectF());
}

void WaterfallWidget::drawData(QPainter & painter)
{
	QRectF bound = this->viewport(); // 控件视口（像素，设备）

	QRectF visibleArea = m_visibleArea;

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
	return m_loader->totalArea();
}

QRectF WaterfallWidget::visibleArea()
{
	return m_visibleArea;
}

void WaterfallWidget::setVisibleArea(QRectF r, bool redraw)
{
	QRectF newArea = tool::clip(r.normalized(), totalArea());
	
	if (newArea != m_visibleArea) {
		m_visibleArea = newArea;

		if (redraw) {
			update(); //repaint();
		}

		if (m_needNotify) {
			emit viewChanged(m_visibleArea);
		}
	}
}

void WaterfallWidget::wheelEvent(QWheelEvent * evt)
{
	bool ctrl = evt->modifiers() & Qt::ControlModifier;

	auto pos = evt->pos();
	auto viewport = this->viewport();
	auto pxy = tool::map(pos, viewport, tool::rectFlipY(m_visibleArea));
	
	auto angles = evt->angleDelta();

	if (!angles.isNull()) {

		if (ctrl) {
			if (angles.ry() > 0) {
				executeCommand(FreqZoomInAt, pxy.ry());
			}
			else {
				executeCommand(FreqZoomOutAt, pxy.ry());
			}
		}
		else {
			if (angles.ry() > 0) {
				executeCommand(TimeZoomInAt, pxy.rx());
			}
			else {
				executeCommand(TimeZoomOutAt, pxy.rx());
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

	KeyState ks = { evt->key(), ctrl, shift, alt };
	bool processed = false;

	for (auto kv : m_shortcuts) {
		if (kv.second == ks) {
			CommandState cs = kv.first;
			processed = executeCommand(cs.first, cs.second);
			break;
		}
	}

	if (processed) {
		evt->accept();
	}
}

bool WaterfallWidget::executeCommand(WaterfallCommand type, double param)
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
		setVisibleArea(visible);
		break;
	}

	case ColorRangeDown:
	case ColorRangeUp:
	{
		auto rng = m_loader->colorRange();
		float diff = (type == ColorRangeUp) ? 5 : -5;
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
		float diff = (type == ColorRangeAdd) ? width * 0.05 : width * -0.05;

		rng = tool::range_expand<float>(rng, diff);
		m_loader->setColorRange(rng);
		dirty = true;
		break;
	}

	case ColorRangeAuto:
	{
		std::pair<float, float> rng = m_loader->currentValueRange();
		if (rng != std::pair<float, float>(0, 0)) {
			float val = tool::range_length(rng) * param;
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
		ret = tool::rectMoveX(m_visibleArea, m_visibleArea.width() * 0.1);
		break;

	case TimeBackward:
		ret = tool::rectMoveX(m_visibleArea, - m_visibleArea.width() * 0.1);
		break;

	case TimeZoomIn:
		ret = tool::rectExpandX(m_visibleArea, m_visibleArea.width() * 0.1);
		break;

	case TimeZoomOut:
		ret = tool::rectExpandX(m_visibleArea, -m_visibleArea.width() * 0.1);
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
		ret = tool::rectMoveY(m_visibleArea, m_visibleArea.height() * 0.1);
		break;

	case FreqBackward:
		ret = tool::rectMoveY(m_visibleArea, -m_visibleArea.height() * 0.1);
		break;
	
	case FreqZoomIn:
		ret = tool::rectExpandY(m_visibleArea, m_visibleArea.height() * 0.1);
		break;

	case FreqZoomOut:
		ret = tool::rectExpandY(m_visibleArea, -m_visibleArea.height() * 0.1);
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


void WaterfallWidget::initShortcuts()
{
	m_shortcuts.clear();

	// 1.初始化快捷键.
	m_shortcuts[{Reset, 0}] = { Qt::Key_Q, false, false, false };
	m_shortcuts[{ResetTime, 0}] = { Qt::Key_Q, false, true, false };
	m_shortcuts[{ResetFreq, 0}] = { Qt::Key_Q, true, false, false };

	m_shortcuts[{TimeBackward, 0.05}] = { Qt::Key_Left, false, false, false };
	m_shortcuts[{TimeForward, 0.051}] = { Qt::Key_Right, false, false, false };
	m_shortcuts[{TimeZoomIn, 0.1}] = { Qt::Key_Left, true, false, false };
	m_shortcuts[{TimeZoomOut, 0.1}] = { Qt::Key_Right, true, false, false };


	m_shortcuts[{FreqBackward, 0.05}] = { Qt::Key_Down, false, false, false };
	m_shortcuts[{FreqForward, 0.05}] = { Qt::Key_Up, false, false, false };
	m_shortcuts[{FreqZoomIn, 0.1}] = { Qt::Key_Down, true, false, false };
	m_shortcuts[{FreqZoomOut, 0.1}] = { Qt::Key_Up, true, false, false };

	
	m_shortcuts[{ReloadSelect, 0.05}] = { Qt::Key_R, false, false, false };

	m_shortcuts[{ColorRangeAuto, 0.2}] = { Qt::Key_C, false, false, false };

	m_shortcuts[{ToggleAutoFft, 0}] = { Qt::Key_F, false, false, false };
}