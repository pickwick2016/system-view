#include <QPainter>
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

		double fs = m_waterfall->sampleRate();
		double ts = 1 / fs;

		auto s = m_waterfall->totalSize();

		m_totalArea = QRectF(0, 0, s.first, s.second);
		m_visibleArea = m_totalArea;

		m_dataArea = m_totalArea;		
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

	painter.setRenderHint(QPainter::Antialiasing);

	QRect bound = rect();
	painter.fillRect(bound, QColor::fromRgb(0, 0, 0));

	// 绘制频谱数据.
	QRect viewport = rect(); // 控件视口（像素，设备）
	QRectF visibleArea = m_visibleArea;

	if (m_waterfall->preparePixmap(visibleArea)) {
		painter.setClipRect(viewport);

		QRectF pixmapArea = m_waterfall->pixmapArea();
		QRectF drawArea = map(pixmapArea, visibleArea, viewport);
		QPixmap & pixmap = m_waterfall->pixmap();
		QRect pixmapRect = pixmap.rect();
		
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
	return m_totalArea;
}

QRectF WaterfallWidget::visibleArea()
{
	return m_visibleArea;
}

void WaterfallWidget::setVisibleArea(QRectF r)
{
	m_visibleArea = r.normalized();
}

QRectF WaterfallWidget::dataArea()
{
	return m_dataArea;
}

QRectF WaterfallWidget::map(QRectF worldB, QRectF worldA, QRectF viewA2)
{
	QRectF viewA = viewA2;

	double ratioX = viewA.width() / worldA.width();
	double ratioY = viewA.height() / worldA.height();

	double startX = viewA.left() + (worldB.left() - worldA.left()) * ratioX;
	double startY = viewA.top() + (worldB.top() - worldA.top()) * ratioY;

	QRectF ret(QPointF(startX, startY), QSize(worldB.width() * ratioX, worldB.height() * ratioY));

	return ret;
}