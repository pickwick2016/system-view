#pragma once

#include <QWidget>

/**
 * ʱƵͼ���ٲ�ͼ���ؼ�.
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
