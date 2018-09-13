#pragma once

#include <QWidget>
#include <QVariant>

// 常用命令.
enum SignalCommand
{
	None = -1,
	XY_Reset = 0,
	X_Reset,
	Y_Reset,

	X_Backward,
	X_Forward,
	X_ZoomIn,
	X_ZoomOut,
	X_ZoomInAt,
	X_ZoomOutAt,
	Y_Forward,
	Y_Backward,
	Y_ZoomIn,
	Y_ZoomOut,
	Y_ZoomInAt,
	Y_ZoomOutAt,

	User, // 用户自定义消息的开始.
};

// 信号显示控件.
class SignalWidget : public QWidget
{
	Q_OBJECT

public:
	typedef std::tuple<int, bool, bool, bool> KeyState; // 键盘状态.

public:
	SignalWidget(QWidget * parent = nullptr);

signals:
	// 当前可视区域发生了变化.
	void viewChanged(QRectF area);

protected:
	virtual void keyPressEvent(QKeyEvent * evt);
	virtual void wheelEvent(QWheelEvent * evt);
	//virtual void mousePressEvent(QMouseEvent *evt);
	//virtual void mouseMoveEvent(QMouseEvent *evt);
	//virtual void mouseReleaseEvent(QMouseEvent *evt);

protected:
	void initShortcuts();
	
	virtual bool executeCommand(int, QVariant param = QVariant());

	virtual QRectF visibleArea();
	virtual void setVisibleArea(QRectF );

	virtual QRectF totalArea();

	KeyState makeKeyState(QKeyEvent * evt);
	QRectF checkPointBound(QWheelEvent * evt);

	virtual QRectF viewport(int tag) { return QRectF(); }
	
protected:
	std::map<int, KeyState> m_shortcuts; // 快捷键.
	std::map<int, QVariant> m_commandParams; // 命令参数.

private:
	QRectF m_visibleArea;

};
