#ifndef WAVE_WIDGET_H
#define WAVE_WIDGET_H

#include <memory>

#include "paint_panel.h"

#include "signal_loader.h"
#include "signal_widget.h"


// Ƶ�׿ؼ�.
class WaveWidget : public SignalWidget
{
	Q_OBJECT

public:
	WaveWidget(QWidget *parent = nullptr);
	~WaveWidget();
	
public slots:
	// �����ⲿ��ʾ�Ŀ������仯�����ٶ��ⷢ�䶯�źţ�
	void syncView(QRectF);

public:
	// ��������.
	void load(std::shared_ptr<Reader> reader);

protected:
	virtual void paintEvent(QPaintEvent *event);

protected:
	virtual void setVisibleArea(QRectF);
	virtual QRectF totalArea();

private:
	// ��������ָ��ʱ�̵����ݣ�ֻ׼�����ݣ���������ʾ���ݣ�.
	bool reload(double t1, double t2);

	// ���ݵ�ǰloader״̬�������������ݣ���������ʾ���ݣ�.
	void updateChannel(int idx);

	void drawTimeMarker(QPainter * painter);

	void drawChannel(QPainter * painter, int idx);

	// ��ȡ��ͬ�����ӿ�. 0-ʱ���ߣ�10-ͨ��1���ݣ�11-ͨ��1��ߣ�20-ͨ��2���ݣ�21-ͨ��2���.
	QRectF viewport(int idx);

private:
	LinePanel m_lines[2];
	MarkerPanel m_timeMarker;
	MarkerPanel m_valueMarker[2];
	
	WaveLoader m_loader;

	bool m_needNotify;

};

#endif //WAVE_WIDGET_H
