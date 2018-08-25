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
 * 瀑布图数据.
 */
class Waterfall
{
public:
	Waterfall(unsigned int fftlen = 256);

public:
	// 载入数据.
	bool load(const std::string & filename, int type, double samplerate);
	
	// 关闭数据.
	void close();

public:
	// 请求一定时间范围的数据.
	bool query(double start, double end, unsigned int countHint = 0);

	// 内部数据尺寸（时间 width + 频谱 height）
	std::pair<int, int> dataSize();

	// 当前图的尺寸.
	std::pair<int, int> currentMapSize();

	// 整体的逻辑时频范围.
	QRectF totalArea();

	// 当前对应的时间和频率范围.
	QRectF currentArea();

	// 整体的时间范围和频率范围.
	std::pair<double, double> totalSize();

public:
	// 填充图形.
	bool fill(QPixmap & pixmap);

	uint32_t valueToColor(float val);

public:
	// FFT 长度.
	unsigned int fftLen();
	
	// 采样率.
	double sampleRate();
	
	// 数据类型.
	int dataType();

	// 所有数据的数量.
	unsigned int dataCount();


public:
	QPixmap & pixmap() { return m_pixmap; }

	QRectF pixmapArea();

	// 判断内部数据是否有效.
	bool preparePixmap(QRectF visible);

private:
	// 根据当前状态，重新载入数据.
	bool reloadData();

	// 清空状态.
	void clear();

private:
	unsigned int power(void * input, float * output, unsigned int fftlen, int type);

private:
	unsigned int m_fftLen; // fft长度.
	unsigned int m_align; // 对齐.

	unsigned int m_freqCount; // fft 长度（载入时确定）.

	std::pair<unsigned int, unsigned int> m_currentRange; //起始点范围（动态确定）
	unsigned int m_currentStep;	// 步长（动态确定）.

	std::vector<float> m_data;
	std::shared_ptr<Reader> m_reader;	
	std::vector<unsigned int> m_buffer;

	QPixmap m_pixmap;

	typedef std::tuple<decltype(m_fftLen), decltype(m_currentRange), decltype(m_currentStep)> InnerState;
	InnerState m_currState, m_oldState;

	std::shared_ptr<Fft> m_fft;
};



