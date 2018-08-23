#pragma once

#include <QWidget>

/**
 * Ê±ÆµÍ¼£¨ÆÙ²¼Í¼£©¿Ø¼þ.
 */
class WaterfallWidget : public QWidget
{
	Q_OBJECT

public:
	WaterfallWidget(QWidget *parent = nullptr);
	~WaterfallWidget();

public:
	bool load(QString filename, int type, double samplerate);
	void close();

protected:
	virtual void paintEvent(QPaintEvent *event);

};
