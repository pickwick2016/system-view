#pragma once

#include <functional>
#include <QWidget>

class Waterfall;

/**
 * 时频图（瀑布图）控件.
 */
class WaterfallWidget : public QWidget
{
	Q_OBJECT

public:
	// 预定义操作类型.
	enum Command
	{
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
		ColorRangeUp,
		ColorRangeDown,
		ColorRangeAdd,
		ColorRangeDec,
		Reset,
	};

public:
	WaterfallWidget(QWidget *parent = nullptr);
	~WaterfallWidget();

public:
	// 载入数据文件.
	bool load(QString filename, int type, double samplerate);
	
	// 关闭数据文件.
	void close();

public:
	// 数据空间（逻辑）大小.
	QRectF totalArea();

	// 可视空间（逻辑）大小.
	QRectF visibleArea();

	// 设置可视空间（逻辑）
	void setVisibleArea(QRectF r);

protected:
	virtual void paintEvent(QPaintEvent *event);
	virtual void resizeEvent(QResizeEvent *event);
	virtual void showEvent(QShowEvent * evt);
	virtual void keyPressEvent(QKeyEvent * evt);
	virtual void wheelEvent(QWheelEvent * evt);

private:
	// 将逻辑坐标转换为物理坐标.
	QRectF map(QRectF world, QRectF worldR, QRectF viewR);

private:
	// 执行预定义操作.
	void executeCommand(Command type, double param = 0);

	QRectF limitArea(QRectF vis, QRectF total, bool keepsize = true);

private:
	std::shared_ptr<Waterfall> m_waterfall;
	QRectF m_visibleArea;

	typedef std::tuple<int, bool, bool, bool> KeyState;
	std::map<Command, KeyState> m_shortcuts;
	std::map<Command, std::function<void()>> m_commands;
};
