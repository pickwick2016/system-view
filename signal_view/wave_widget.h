#ifndef WAVE_WIDGET_H
#define WAVE_WIDGET_H

#include <memory>

#include "paint_panel.h"

#include "signal_loader.h"
#include "signal_widget.h"


// 频谱控件.
class WaveWidget : public SignalWidget
{
	Q_OBJECT

public:
	WaveWidget(QWidget *parent = nullptr);
	~WaveWidget();
	
public slots:
	// 处理外部提示的可视区变化（不再对外发变动信号）
	void syncView(QRectF);

public:
	// 载入数据.
	void load(std::shared_ptr<Reader> reader);

protected:
	virtual void paintEvent(QPaintEvent *event);

protected:
	virtual void setVisibleArea(QRectF);
	virtual QRectF totalArea();

private:
	// 重新载入指定时刻的内容（只准备数据，不更新显示内容）.
	bool reload(double t1, double t2);

	// 根据当前loader状态，更新线条数据（不更新显示内容）.
	void updateChannel(int idx);

	void drawTimeMarker(QPainter * painter);

	void drawChannel(QPainter * painter, int idx);

	// 获取不同区域视口. 0-时间标尺；10-通道1数据；11-通道1标尺；20-通道2数据；21-通道2标尺.
	QRectF viewport(int idx);

private:
	LinePanel m_lines[2];
	MarkerPanel m_timeMarker;
	MarkerPanel m_valueMarker[2];
	
	WaveLoader m_loader;

	bool m_needNotify;

};

#endif //WAVE_WIDGET_H
