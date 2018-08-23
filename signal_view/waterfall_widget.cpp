#include <QPainter>

#include "waterfall_widget.h"

WaterfallWidget::WaterfallWidget(QWidget *parent)
	: QWidget(parent)
{
}

WaterfallWidget::~WaterfallWidget()
{
}

bool WaterfallWidget::load(QString filename, int type, double samplerate)
{
	return true;
}

void WaterfallWidget::close()
{

}

void WaterfallWidget::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);

	QRect r = this->rect();
	painter.fillRect(r, QColor::fromRgb(30, 130, 230));

}