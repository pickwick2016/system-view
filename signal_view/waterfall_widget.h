#pragma once

#include <QWidget>

class Waterfall;

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
	// ���������ļ�.
	bool load(QString filename, int type, double samplerate);
	
	// �ر������ļ�.
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
	// ���߼�����ת��Ϊ��������.
	QRectF map(QRectF world, QRectF worldR, QRectF viewR);

private:
	std::shared_ptr<Waterfall> m_waterfall;
	QRectF m_totalArea, m_visibleArea, m_dataArea;
};