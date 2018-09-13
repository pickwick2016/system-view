#ifndef WATERFALL_WIDGET_H
#define WATERFALL_WIDGET_H

#include <functional>
#include <memory>

#include "signal_widget.h"

class Reader;
class WaterfallLoader;

// 预定义操作类型.
enum WaterfallCommand
{
	ReloadSelect = SignalCommand::User + 1,

	ColorRangeUp,
	ColorRangeDown,
	ColorRangeAdd,
	ColorRangeDec,
	ColorRangeAuto,

	ToggleAutoFft,
};

/**
 * 时频图（瀑布图）控件.
 */
class WaterfallWidget : public SignalWidget
{
	Q_OBJECT

public:
	WaterfallWidget(QWidget *parent = nullptr);
	virtual ~WaterfallWidget();

signals:
	// 鼠标指向位置发生变化.
	void positionMoved(QPointF pos);

public slots:
	void syncView(QRectF area);

public:
	// 载入数据文件.
	bool load(QString filename, int type, double samplerate);

	// 设置读取器.
	void load(std::shared_ptr<Reader> reader);
	
	// 关闭数据文件.
	void close();

public:
	// 数据空间（逻辑）大小.
	virtual QRectF totalArea();

	// 设置可视空间（逻辑）
	virtual void setVisibleArea(QRectF r);

	// 当前选区.
	QRectF selectArea();

protected:
	virtual void paintEvent(QPaintEvent *event);
	virtual void resizeEvent(QResizeEvent *event);
	virtual void showEvent(QShowEvent * evt);
	virtual void mousePressEvent(QMouseEvent *evt);
	virtual void mouseMoveEvent(QMouseEvent *evt);
	virtual void mouseReleaseEvent(QMouseEvent *evt);

private:
	void reload(QRectF area);

	void drawTimeBar(QPainter & painter);
	void drawFreqBar(QPainter & painter);
	void drawData(QPainter & painter);
	void drawSelection(QPainter & painter);

protected:
	// 执行预定义操作.
	virtual bool executeCommand(int cmd, QVariant param = QVariant());

	// 获取不同数据的视口
	virtual QRectF viewport(int tag);

	// 初始化命令列表.
	void appendShortcuts();

private:
	std::shared_ptr<WaterfallLoader> m_loader;
	
	//QRectF m_visibleArea; // 当前可视区.
	
	bool m_mousePressed; // 鼠标是否拖拽.
	QPointF m_dragStartPoint, m_dragEndPoint; // 拖拽鼠标起止点
	QPointF m_dragStartPos, m_dragEndPos; // 拖拽逻辑起止点

	bool m_needNotify;
};

#endif //WATERFALL_WIDGET_H


