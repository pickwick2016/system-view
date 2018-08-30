#pragma once

#include <functional>
#include <QWidget>

class Waterfall;

// Ԥ�����������.
enum WaterfallCommand
{
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
 * ʱƵͼ���ٲ�ͼ���ؼ�.
 */
class WaterfallWidget : public QWidget
{
	Q_OBJECT

public:
	WaterfallWidget(QWidget *parent = nullptr);
	virtual ~WaterfallWidget();

signals:
	
	// ���ָ��λ�÷����仯.
	void positionMoved(QPointF pos);
	

public:
	// ���������ļ�.
	bool load(QString filename, int type, double samplerate);
	
	// �ر������ļ�.
	void close();

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
	virtual void mousePressEvent(QMouseEvent *evt);
	virtual void mouseMoveEvent(QMouseEvent *evt);
	virtual void mouseReleaseEvent(QMouseEvent *evt);

private:
	void drawTimeBar(QPainter & painter);
	void drawFreqBar(QPainter & painter);
	void drawData(QPainter & painter);
	void drawSelection(QPainter & painter);

private:
	// ִ��Ԥ�������.
	void executeCommand(WaterfallCommand type, double param = 0);

	// ��ȡ��ͬ���ݵ��ӿ� 0:main 1:time_bar 2:freq_bar
	QRectF viewport(int tag = 0);

	QRectF setVisible(WaterfallCommand cmd, double param);

	QRectF selectArea();

private:
	std::shared_ptr<Waterfall> m_waterfall;
	QRectF m_visibleArea;
	bool m_mousePressed;
	QPointF m_startPoint, m_endPoint;
	QPointF m_startPos, m_endPos;

	typedef std::tuple<int, bool, bool, bool> KeyState;
	std::map<WaterfallCommand, KeyState> m_shortcuts;
	std::map<WaterfallCommand, std::function<void()>> m_commands;
};


