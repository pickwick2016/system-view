#pragma once

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
	Waterfall(unsigned int fftlen = 256);
	~Waterfall();

public:
	// ��������.
	bool load(const std::string & filename, int type, double samplerate);
	
	// �ر�����.
	void close();

public:
	/**
	 * ׼���ٲ�ͼ����.
	 * 1.��鵱ǰ�����Ƿ���ã���������ã��͸���.
	 * 2.���׼����ɣ����Ե���pixmap(), datamap(), ��ȡ��ǰ���ݣ�����pixmapSize()��datamapSize()��ȡͼ��ߴ�.
	 *
	 * @param visible ��ǰ���ӷ�Χ.
	 * @param segmentHint ����ֶε�����.
	 * @return �������.
	 */
	bool prepare(QRectF visible, int segmentHint = 0);

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

		
private:
	// ����һ��ʱ�䷶Χ������.
	bool query(double start, double end, unsigned int countHint = 0);

	// ���ݵ�ǰ״̬��������������.
	bool update();

	// ���״̬.
	void clear();

	// ���ͼ��.
	bool fill(QPixmap & pixmap);

	// ��ֵת��Ϊɫ��.
	uint32_t valueToColor(float val);

	// ������.
	unsigned int power(void * input, float * output, unsigned int fftlen, int type);
	

private:
	unsigned int m_fftLen; // fft����.
	unsigned int m_align; // ����.

	unsigned int m_freqCount; // fft ���ȣ�����ʱȷ����.

	std::pair<unsigned int, unsigned int> m_currentRange; //��ʼ�㷶Χ����̬ȷ����
	unsigned int m_currentStep;	// ��������̬ȷ����.

	QPixmap m_pixmap;
	std::vector<unsigned int> m_pixmapData;

	std::vector<float> m_datamap;
	
	std::shared_ptr<Reader> m_reader;	

	std::shared_ptr<Fft> m_fft; // fft ������.

	std::pair<float, float> m_colorRange;

	std::function<std::tuple<int, int, int>(float, float, float)> m_colormap;

	typedef std::tuple<decltype(m_fftLen), decltype(m_currentRange), decltype(m_currentStep)> InnerState;
	InnerState m_currState, m_prevState;
};



