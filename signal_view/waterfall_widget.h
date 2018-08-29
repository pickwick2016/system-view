#pragma once

#include <functional>
#include <QWidget>

class Waterfall;

/**
 * ʱƵͼ���ٲ�ͼ���ؼ�.
 */
class WaterfallWidget : public QWidget
{
	Q_OBJECT

public:
	// Ԥ�����������.
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
	// ���������ļ�.
	bool load(QString filename, int type, double samplerate);
	
	// �ر������ļ�.
	void close();

public:
	// ���ݿռ䣨�߼�����С.
	QRectF totalArea();

	// ���ӿռ䣨�߼�����С.
	QRectF visibleArea();

	// ���ÿ��ӿռ䣨�߼���
	void setVisibleArea(QRectF r);

protected:
	virtual void paintEvent(QPaintEvent *event);
	virtual void resizeEvent(QResizeEvent *event);
	virtual void showEvent(QShowEvent * evt);
	virtual void keyPressEvent(QKeyEvent * evt);
	virtual void wheelEvent(QWheelEvent * evt);

private:
	// ���߼�����ת��Ϊ��������.
	QRectF map(QRectF world, QRectF worldR, QRectF viewR);

private:
	// ִ��Ԥ�������.
	void executeCommand(Command type, double param = 0);

	QRectF limitArea(QRectF vis, QRectF total, bool keepsize = true);

private:
	std::shared_ptr<Waterfall> m_waterfall;
	QRectF m_visibleArea;

	typedef std::tuple<int, bool, bool, bool> KeyState;
	std::map<Command, KeyState> m_shortcuts;
	std::map<Command, std::function<void()>> m_commands;
};
