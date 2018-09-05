#ifndef WATERFALL_H
#define WATERFALL_H

#include <vector>
#include <utility>
#include <memory>
#include <tuple>
#include <functional>
#include <boost/bind.hpp>

#include <QRect>
#include <QPixmap>

#include "reader.h"
#include "fft.h"

/**
 * �ٲ�ͼ����.
 *
 * 1�����ݿͻ���ָ��ʱ��ε����󣬹������ṩ�ٲ�ͼ����.
 * 2���Զ������ڲ��������.
 */
class Waterfall
{
public:
	Waterfall(unsigned int fftlen = 128);
	~Waterfall();

public:
	// ��������.
	bool load(const std::string & filename, int type, double samplerate);

	void load(std::shared_ptr<Reader> reader);

	bool isLoaded();
	
	// �ر�����.
	void close();

public:
	/**
	 * ׼���ٲ�ͼ����.
	 * 1.��鵱ǰ�����Ƿ���ã���������ã��͸���.
	 * 2.���׼����ɣ����Ե���pixmap(), datamap(), ��ȡ��ǰ���ݣ�����pixmapSize()��datamapSize()��ȡͼ��ߴ�.
	 *
	 * @param visible ��ǰ���ӷ�Χ.
	 * @param sizeHint ����ֶε�����(segmentHint, freqHint).
	 * @return �������.
	 */
	bool prepare(QRectF visible, std::pair<int, int> sizeHint = { 0, 0 });

	// ��ǰ���ݵ�ʱ���Ƶ�ʷ�Χ���߼�ֵ������ʱ�䣬����Ƶ�ף�.
	QRectF currentArea();

	// ������߼�ʱƵ��Χ.
	QRectF totalArea();

	// ��ǰͼ������.
	QPixmap & pixmap() { return m_pixmap; }

	// ��ǰƵ������.
	std::vector<float> & datamap() { return m_datamap; }

	// Ƶ�����ݴ�С (freq_count, segment_count)
	std::pair<int, int> dataSize();

	std::pair<float, float> colorRange() { return m_colorRange; }

	void setColorRange(std::pair<float, float> rng);

	std::pair<float, float> currentValueRange() { return m_valueRange; }

	void setAutoFft(bool on, int fftlen = 256);
	bool isAutoFft() { return m_isAutoFft; }
		
private:
	// ����һ��ʱ�䷶Χ������.
	bool query(QRectF visible, std::pair<int, int> sizeHint = { 0, 0 });

	// ���ݵ�ǰ״̬��������������.
	bool reloadBuffer();

	bool bufferToPixmap();

	// ���״̬.
	void clear();
	
	// ��ֵת��Ϊɫ��.
	uint32_t valueToColor(float val);

	// ������.
	unsigned int power(void * input, float * output, unsigned int fftlen, int type);

	// Ƶ�ʵ���.
	unsigned int freqCount();
	

private:
	unsigned int m_stepAlign; // ����.

	std::vector<unsigned int> m_fftHints;

	std::pair<unsigned int, unsigned int> m_currentRange; //��ʼ�㷶Χ����̬ȷ����
	unsigned int m_currentStep;	// ��������̬ȷ����.
	unsigned int m_currentFft; // fft����.

	QPixmap m_pixmap;
	std::vector<unsigned int> m_pixmapData;

	std::vector<float> m_datamap;
	
	std::shared_ptr<Reader> m_reader;	

	std::shared_ptr<Fft> m_fft; // fft ������.

	std::pair<float, float> m_colorRange;
	std::pair<float, float> m_valueRange;

	std::function<std::tuple<int, int, int>(float, float, float)> m_colormap;

	typedef std::tuple<decltype(m_currentFft), decltype(m_currentRange), decltype(m_currentStep)> BufferState;
	BufferState m_currState, m_prevState;
	std::pair<float, float> m_prevColorRange;

	bool m_isAutoFft;
	unsigned int m_fixFft;
};

#endif //WATERFALL_H


