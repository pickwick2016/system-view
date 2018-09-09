#ifndef WATERFALL_WIDGET_H
#define WATERFALL_WIDGET_H

#include <functional>
#include <memory>
#include <QWidget>

class Reader;
class WaterfallLoader;

// 预定义操作类型.
enum WaterfallCommand
{
	None,
	Reset,
	ResetTime,
	ResetFreq,

	TimeBackward,
	TimeForward,
	TimeZoomIn,
	TimeZoomOut,
	TimeZoomInAt,
	TimeZoomOutAt,
	FreqForward,
	FreqBackward,
	FreqZoomIn,
	FreqZoomOut,
	FreqZoomInAt,
	FreqZoomOutAt,

	ReloadSelect,

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
class WaterfallWidget : public QWidget
{
	Q_OBJECT

public:
	WaterfallWidget(QWidget *parent = nullptr);
	virtual ~WaterfallWidget();

signals:
	// 鼠标指向位置发生变化.
	void positionMoved(QPointF pos);
	
	// 当前可视区域发生了变化.
	void viewChanged(QRectF area);

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
	QRectF totalArea();

	// 可视空间（逻辑）大小.
	QRectF visibleArea();

	// 设置可视空间（逻辑）
	void setVisibleArea(QRectF r, bool redraw = true);

	// 当前选区.
	QRectF selectArea();

protected:
	virtual void paintEvent(QPaintEvent *event);
	virtual void resizeEvent(QResizeEvent *event);
	virtual void showEvent(QShowEvent * evt);
	virtual void keyPressEvent(QKeyEvent * evt);
	virtual void wheelEvent(QWheelEvent * evt);
	virtual void mousePressEvent(QMouseEvent *evt);
	virtual void mouseMoveEvent(QMouseEvent *evt);
	virtual void mouseReleaseEvent(QMouseEvent *evt);

private:
	void reload(QRectF area);

	void drawTimeBar(QPainter & painter);
	void drawFreqBar(QPainter & painter);
	void drawData(QPainter & painter);
	void drawSelection(QPainter & painter);

private:
	// 执行预定义操作.
	bool executeCommand(WaterfallCommand type, double param = 0);

	// 获取不同数据的视口 0:main 1:time_bar 2:freq_bar
	QRectF viewport(int tag = 0);

	// 修改可视区.
	QRectF setVisible(WaterfallCommand cmd, double param);

	// 初始化命令列表.
	void initShortcuts();

private:
	std::shared_ptr<WaterfallLoader> m_loader;
	
	QRectF m_visibleArea; // 当前可视区.
	
	bool m_mousePressed; // 鼠标是否拖拽.
	QPointF m_dragStartPoint, m_dragEndPoint; // 拖拽鼠标起止点
	QPointF m_dragStartPos, m_dragEndPos; // 拖拽逻辑起止点

	typedef std::tuple<int, bool, bool, bool> KeyState; // 键盘状态.
	typedef std::pair<WaterfallCommand, double> CommandState; // 命令状态.

	std::map<CommandState, KeyState> m_shortcuts; // 命令快捷键表.

	bool m_needNotify;
};

#endif //WATERFALL_WIDGET_H


