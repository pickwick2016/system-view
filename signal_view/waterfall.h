#ifndef WATERFALL_H
#define WATERFALL_H

#include <vector>
#include <utility>
#include <memory>
#include <tuple>
#include <functional>

#include <QRect>
#include <QPixmap>

class Fft;
class Reader;

/**
 * 瀑布图数据载入器.
 * 1、根据客户对指定时间段的需求，构建和提供瀑布图数据（准备一个满足用户需求的数据即可）.
 * 2、自动调整内部各项参数.
 */
class WaterfallLoader
{
	// 内部状态3元组<fft-len, pos-range, pos-step>
	typedef std::tuple<unsigned int, std::pair<unsigned int, unsigned>, unsigned int> State;

public:
	WaterfallLoader(unsigned int fftlen = 128);
	~WaterfallLoader();

public:
	// 载入数据.
	bool load(const std::string & filename, int type, double samplerate);

	void load(std::shared_ptr<Reader> reader);

	bool isLoaded();
	
	// 关闭数据.
	void close();

public:
	/**
	 * 准备瀑布图数据.
	 * 1.检查当前数据是否可用，如果不可用，就更新.
	 * 2.如果准备完成，可以调用pixmap(), datamap(), 获取当前数据；调用pixmapSize()、datamapSize()获取图像尺寸.
	 *
	 * @param visible 当前可视范围.
	 * @param sizeHint 建议分段的数量(segmentHint, freqHint).
	 * @return 操作结果.
	 */
	bool prepare(QRectF visible, std::pair<int, int> sizeHint = { 0, 0 });

	// 整体的逻辑时频范围.
	QRectF totalArea();

	// 当前频谱数据.
	std::vector<float> & datamap() { return m_datamap; }

	// 频谱数据大小 (freq_count, segment_count)
	std::pair<int, int> dataSize();

	// 当前频谱数据数值范围.
	std::pair<float, float> currentValueRange() { return m_valueRange; }

	// 当前图像数据.
	QPixmap & pixmap() { return m_pixmap; }

	// 当前数据的时间和频率范围（逻辑值，横向时间，纵向频谱）.
	QRectF pixmapArea();

	// 当前色彩范围.
	std::pair<float, float> colorRange() { return m_colorRange; }
	
	// 设置色彩范围.
	void setColorRange(std::pair<float, float> rng);


	void setAutoFft(bool on, int fftlen = 256);
	bool isAutoFft() { return m_isAutoFft; }
		
private:
	// 根据需求，计算当前状态.
	State calculateState(QRectF visible, std::pair<int, int> sizeHint = { 0, 0 });

	// 按指定状态载入数据.（如果成功，同时改变内部状态；失败，则不改变）
	bool reload(State state);

	// 根据当前状态，重新载入数据.
	bool reloadValues();

	// 根据当前数值，重构位图.
	void reloadPixmap();
	
	// 清空状态.
	void clear();
	
	// 数值转化为色彩.
	uint32_t valueToColor(float val);

private:
	std::shared_ptr<Reader> m_reader;	// 读取器.

	std::shared_ptr<Fft> m_fft; // fft 计算器.
	std::vector<unsigned int> m_fftHints;

	unsigned int m_stepAlign; // 对齐.

	QPixmap m_pixmap;
	std::vector<unsigned int> m_pixmapData; // 图像数据（元素为 uint32 的 RGBA 数据）
	
	std::vector<float> m_datamap;
	std::pair<float, float> m_valueRange;

	std::pair<float, float> m_colorRange;
	std::pair<float, float> m_prevColorRange;

	std::function<std::tuple<int, int, int>(float, float, float)> m_colormap; // 色彩转换函数.

	State m_state;
	
	bool m_isAutoFft;
	unsigned int m_fixFft;
};

#endif //WATERFALL_H


