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
}

bool WaterfallWidget::load(QString filename, int type, double samplerate)
{
	assert(m_loader);

	m_loader->load(filename.toStdString(), type, samplerate);

	executeCommand(SignalCommand::XY_Reset);
	return  m_loader->isLoaded();
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
	auto bound = this->viewport(0);
	if (bound.isEmpty())
		return;

	MarkerPanel marker;
	marker.setRect(bound);
	marker.setRange(tool::RectRangeX(visibleArea()));
	marker.paint(&painter);
}

void WaterfallWidget::drawFreqBar(QPainter & painter)
{
	auto bound = this->viewport(11);
	if (bound.isEmpty())
		return;

	QRectF visibleArea = this->visibleArea();

	MarkerPanel marker;
	marker.setRect(bound);
	marker.setDirection(1);
	marker.setRange(tool::RectRangeY(visibleArea));
	marker.paint(&painter);
}

void WaterfallWidget::drawData(QPainter & painter)
{
	QRectF bound = this->viewport(10); // 控件视口（像素，设备）

	QRectF visibleArea = this->visibleArea();

	if (m_loader->prepare(visibleArea, { 200, 100 })) {
		painter.setClipRect(bound);

		QRectF pixmapArea = m_loader->pixmapArea();
		QPixmap & pixmap = m_loader->pixmap();
		QRect pixmapRect = pixmap.rect();

		QRectF drawArea = tool::Map(pixmapArea, visibleArea, tool::RectFlipY(bound));
		drawArea = tool::RectFlipY(drawArea);
		painter.drawPixmap(drawArea, pixmap, pixmapRect);

		painter.setClipRect(QRect());
	}
}


void WaterfallWidget::drawSelection(QPainter & painter)
{
	QRectF selection = selectArea();
	if (! selection.isEmpty()) {
		QRectF bound = this->viewport(10); // 控件视口（像素，设备）
		painter.setClipRect(bound);

		QRectF drawArea = tool::Map(selection, visibleArea(), tool::RectFlipY(bound));
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
	ret = tool::RectClip(ret.normalized(), total);
	return ret;
}

QRectF WaterfallWidget::viewport(int tag)
{
	assert(tag >= 0);

	QRectF bound = rect();

	int border = 2;
	double rightBand = 64;
	double upBand = 24;

	double w[2] = { bound.width() - 2 * border - rightBand, rightBand };
	double h[2] = { upBand, bound.height() - upBand - 2 * border };

	int col = tag % 10;
	int row = tag / 10;

	if (tag == 0 || tag == 10 || tag == 11) {
		double offsetx = std::accumulate(w, w + col, 0) + (col + 1) * border;
		double offsety = std::accumulate(h, h + row, 0) + (row + 1) * border;

		QPointF pt = bound.topLeft() + QPointF(offsetx, offsety);
		return QRectF(pt, QSizeF(w[col], h[row]));
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
	QRectF newArea = tool::RectClip(r.normalized(), totalArea());
	

	if (newArea != visibleArea()) {
		SignalWidget::setVisibleArea(newArea);

		update(); //repaint();

		if (m_needNotify) {
			emit viewChanged(visibleArea());
		}
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
		m_dragStartPos = tool::Map(evt->pos(), tool::RectFlipY(viewport(10)), visibleArea());

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
	auto viewport = this->viewport(10);

	if (viewport.contains(pos)) {
		auto pos2 = tool::Map(evt->pos(), tool::RectFlipY(viewport), visibleArea());
		emit positionMoved(pos2);
	}
	
	if (m_mousePressed) {
		m_dragEndPoint = evt->pos();
		m_dragEndPos = tool::Map(evt->pos(), tool::RectFlipY(viewport), visibleArea());;
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