#pragma once

#include <QWidget>
#include <QVariant>

// ��������.
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

	User, // �û��Զ�����Ϣ�Ŀ�ʼ.
};

// �ź���ʾ�ؼ�.
class SignalWidget : public QWidget
{
	Q_OBJECT

public:
	SignalWidget(QWidget * parent = nullptr);

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

public:
	typedef std::tuple<int, bool, bool, bool> KeyState; // ����״̬.
	//typedef std::pair<WaterfallCommand, double> CommandState; // ����״̬.

private:
	QRectF m_visibleArea;

	std::map<int, KeyState> m_shortcuts; // ��ݼ�.
	std::map<int, QVariant> m_commandParams; // �������.
};
