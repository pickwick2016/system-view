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
	m_mousePressed = false;
	m_waterfall.reset(new Waterfall());

	setMouseTracking(true);
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

	if (m_waterfall->prepare(visibleArea, { 200, 100 })) {
		painter.setClipRect(bound);

		QRectF pixmapArea = m_waterfall->currentArea();
		QPixmap & pixmap = m_waterfall->pixmap();
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
	if (m_startPos == m_endPos) {
		return QRectF();
	}

	QRectF ret(m_startPos, m_endPos);
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

	switch (evt->key()) {
	case Qt::Key_R:
		executeCommand(ReloadSelect);
		break;

	case Qt::Key_F:
		executeCommand(ToggleAutoFft);
		break;

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

void WaterfallWidget::executeCommand(WaterfallCommand type, double param)
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

	case ToggleAutoFft:
		m_waterfall->setAutoFft(!m_waterfall->isAutoFft());
		dirty = true;
		break;

	case ReloadSelect:
	{
		if (! selectArea().isEmpty()) {
			if (m_visibleArea != selectArea()) {
				m_visibleArea = selectArea();
				dirty = true;
			}
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

void WaterfallWidget::mousePressEvent(QMouseEvent *evt)
{
	if (evt->buttons() & Qt::LeftButton) {

		m_mousePressed = true;

		bool hasSelection = (m_startPos != m_endPos);
		
		m_startPoint = evt->pos();
		m_startPos = tool::map(evt->pos(), tool::rectFlipY(viewport()), visibleArea());

		m_endPoint = m_startPoint;
		m_endPos = m_startPos;

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
		m_endPoint = evt->pos();
		m_endPos = tool::map(evt->pos(), tool::rectFlipY(viewport), visibleArea());;
		repaint();
	}
}

void WaterfallWidget::mouseReleaseEvent(QMouseEvent *evt)
{
	bool hasSelection = (m_startPos != m_endPos);
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