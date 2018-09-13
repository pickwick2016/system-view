#ifndef WATERFALL_WIDGET_H
#define WATERFALL_WIDGET_H

#include <functional>
#include <memory>

#include "signal_widget.h"

class Reader;
class WaterfallLoader;

// Ԥ�����������.
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
 * ʱƵͼ���ٲ�ͼ���ؼ�.
 */
class WaterfallWidget : public SignalWidget
{
	Q_OBJECT

public:
	WaterfallWidget(QWidget *parent = nullptr);
	virtual ~WaterfallWidget();

signals:
	// ���ָ��λ�÷����仯.
	void positionMoved(QPointF pos);

public slots:
	void syncView(QRectF area);

public:
	// ���������ļ�.
	bool load(QString filename, int type, double samplerate);

	// ���ö�ȡ��.
	void load(std::shared_ptr<Reader> reader);
	
	// �ر������ļ�.
	void close();

public:
	// ���ݿռ䣨�߼�����С.
	virtual QRectF totalArea();

	// ���ÿ��ӿռ䣨�߼���
	virtual void setVisibleArea(QRectF r);

	// ��ǰѡ��.
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
	// ִ��Ԥ�������.
	virtual bool executeCommand(int cmd, QVariant param = QVariant());

	// ��ȡ��ͬ���ݵ��ӿ� 0:main 1:time_bar 2:freq_bar
	QRectF viewport(int tag = 0);

	// �޸Ŀ�����.
	QRectF setVisible(WaterfallCommand cmd, double param);

	// ��ʼ�������б�.
	void appendShortcuts();

private:
	std::shared_ptr<WaterfallLoader> m_loader;
	
	//QRectF m_visibleArea; // ��ǰ������.
	
	bool m_mousePressed; // ����Ƿ���ק.
	QPointF m_dragStartPoint, m_dragEndPoint; // ��ק�����ֹ��
	QPointF m_dragStartPos, m_dragEndPos; // ��ק�߼���ֹ��

	bool m_needNotify;
};

#endif //WATERFALL_WIDGET_H


