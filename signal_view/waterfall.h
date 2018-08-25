#pragma once

#include <vector>
#include <utility>
#include <memory>
#include <tuple>
#include <QRect>
#include <QPixmap>

#include "reader.h"
#include "fft.h"

/**
 * �ٲ�ͼ����.
 */
class Waterfall
{
public:
	Waterfall(unsigned int fftlen = 256);

public:
	// ��������.
	bool load(const std::string & filename, int type, double samplerate);
	
	// �ر�����.
	void close();

public:
	// ����һ��ʱ�䷶Χ������.
	bool query(double start, double end, unsigned int countHint = 0);

	// �ڲ����ݳߴ磨ʱ�� width + Ƶ�� height��
	std::pair<int, int> dataSize();

	// ��ǰͼ�ĳߴ�.
	std::pair<int, int> currentMapSize();

	// ������߼�ʱƵ��Χ.
	QRectF totalArea();

	// ��ǰ��Ӧ��ʱ���Ƶ�ʷ�Χ.
	QRectF currentArea();

	// �����ʱ�䷶Χ��Ƶ�ʷ�Χ.
	std::pair<double, double> totalSize();

public:
	// ���ͼ��.
	bool fill(QPixmap & pixmap);

	uint32_t valueToColor(float val);

public:
	// FFT ����.
	unsigned int fftLen();
	
	// ������.
	double sampleRate();
	
	// ��������.
	int dataType();

	// �������ݵ�����.
	unsigned int dataCount();


public:
	QPixmap & pixmap() { return m_pixmap; }

	QRectF pixmapArea();

	// �ж��ڲ������Ƿ���Ч.
	bool preparePixmap(QRectF visible);

private:
	// ���ݵ�ǰ״̬��������������.
	bool reloadData();

	// ���״̬.
	void clear();

private:
	unsigned int power(void * input, float * output, unsigned int fftlen, int type);

private:
	unsigned int m_fftLen; // fft����.
	unsigned int m_align; // ����.

	unsigned int m_freqCount; // fft ���ȣ�����ʱȷ����.

	std::pair<unsigned int, unsigned int> m_currentRange; //��ʼ�㷶Χ����̬ȷ����
	unsigned int m_currentStep;	// ��������̬ȷ����.

	std::vector<float> m_data;
	std::shared_ptr<Reader> m_reader;	
	std::vector<unsigned int> m_buffer;

	QPixmap m_pixmap;

	typedef std::tuple<decltype(m_fftLen), decltype(m_currentRange), decltype(m_currentStep)> InnerState;
	InnerState m_currState, m_oldState;

	std::shared_ptr<Fft> m_fft;
};



