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
 * 瀑布图数据.
 *
 * 1、根据客户对指定时间段的需求，构建和提供瀑布图数据.
 * 2、自动调整内部各项参数.
 */
class Waterfall
{
public:
	Waterfall(unsigned int fftlen = 256);
	~Waterfall();

public:
	// 载入数据.
	bool load(const std::string & filename, int type, double samplerate);
	
	// 关闭数据.
	void close();

public:
	/**
	 * 准备瀑布图数据.
	 * 1.检查当前数据是否可用，如果不可用，就更新.
	 * 2.如果准备完成，可以调用pixmap(), datamap(), 获取当前数据；调用pixmapSize()、datamapSize()获取图像尺寸.
	 *
	 * @param visible 当前可视范围.
	 * @param segmentHint 建议分段的数量.
	 * @return 操作结果.
	 */
	bool prepare(QRectF visible, int segmentHint = 0);

	// 当前数据的时间和频率范围（逻辑值，横向时间，纵向频谱）.
	QRectF currentArea();

	// 整体的逻辑时频范围.
	QRectF totalArea();

	// 当前图像数据.
	QPixmap & pixmap() { return m_pixmap; }

	// 当前频谱数据.
	std::vector<float> & datamap() { return m_datamap; }

	// 频谱数据大小 (freq_count, segment_count)
	std::pair<int, int> dataSize();

		
private:
	// 请求一定时间范围的数据.
	bool query(double start, double end, unsigned int countHint = 0);

	// 根据当前状态，重新载入数据.
	bool update();

	// 清空状态.
	void clear();

	// 填充图形.
	bool fill(QPixmap & pixmap);

	// 数值转化为色彩.
	uint32_t valueToColor(float val);

	// 求功率谱.
	unsigned int power(void * input, float * output, unsigned int fftlen, int type);
	

private:
	unsigned int m_fftLen; // fft长度.
	unsigned int m_align; // 对齐.

	unsigned int m_freqCount; // fft 长度（载入时确定）.

	std::pair<unsigned int, unsigned int> m_currentRange; //起始点范围（动态确定）
	unsigned int m_currentStep;	// 步长（动态确定）.

	QPixmap m_pixmap;
	std::vector<unsigned int> m_pixmapData;

	std::vector<float> m_datamap;
	
	std::shared_ptr<Reader> m_reader;	

	std::shared_ptr<Fft> m_fft; // fft 计算器.

	std::pair<float, float> m_colorRange;

	std::function<std::tuple<int, int, int>(float, float, float)> m_colormap;

	typedef std::tuple<decltype(m_fftLen), decltype(m_currentRange), decltype(m_currentStep)> InnerState;
	InnerState m_currState, m_prevState;
};



