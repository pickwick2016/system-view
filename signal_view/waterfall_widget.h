#pragma once

#include <QWidget>

class Waterfall;

/**
 * 时频图（瀑布图）控件.
 */
class WaterfallWidget : public QWidget
{
	Q_OBJECT

public:
	WaterfallWidget(QWidget *parent = nullptr);
	~WaterfallWidget();

public:
	// 载入数据文件.
	bool load(QString filename, int type, double samplerate);
	
	// 关闭数据文件.
	void close();

public:
	QRectF totalArea();

	QRectF visibleArea();
	void setVisibleArea(QRectF r);
	
	QRectF dataArea();

protected:
	virtual void paintEvent(QPaintEvent *event);
	virtual void resizeEvent(QResizeEvent *event);
	virtual void showEvent(QShowEvent * evt);

private:
	// 将逻辑坐标转换为物理坐标.
	QRectF map(QRectF world, QRectF worldR, QRectF viewR);

private:
	std::shared_ptr<Waterfall> m_waterfall;
	QRectF m_totalArea, m_visibleArea, m_dataArea;
};
