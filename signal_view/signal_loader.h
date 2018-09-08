#ifndef SIGNAL_LOADER_H
#define SIGNAL_LOADER_H

#include <tuple>
#include <memory>
#include <utility>
#include <vector>
#include <array>

class Reader;
class Fft;

// 波形数据(压缩)载入器.
class WaveLoader
{
public:
	// 内部状态 <start, end, step> 3元组.
	typedef std::tuple<int, int, int> State;

public:
	WaveLoader();

public:
	void load(std::shared_ptr<Reader> reader);

	bool prepare(std::pair<double, double> timeRange, int countHint);

	// 数据（针对非压缩数据）
	double * value(int channel);

	// 数据范围组（针对压缩数据）
	std::pair<double, double> * value2(int channel);
	
	// 数值范围
	std::pair<double, double> valueRange(int channel);
	
	// 数据是否压缩.
	bool isZipped();

	// 数据通道数量.
	int channel();

	double timeStep();

	std::pair<double, double> timeRange();

	State state() { return m_state; }

private:
	// 重置内部状态.
	void reset();
	
	// 重新计算当前状态.
	State calculateState(std::pair<double, double> timeRange, int countHint);

	// 根据当前状态重载数据.
	bool reload(State state);

	// 载入非压缩数据.
	void reload1(State state);

	// 载入压缩数据.
	void reload2(State state);

private:
	int m_align; // 压缩时对齐数值.
	int m_zipCount; // 压缩时抽样数量.

	std::shared_ptr<Reader> m_reader;

	std::vector<double> m_values[2]; // 两个通道的缓存数据.
	std::pair<double, double> m_valueRange[2]; // 两个通道数据的范围.
	
	State m_state; // 当前状态.
};


// 频谱数据载入器.
class FreqLoader
{
public:
	FreqLoader();

public:
	void load(std::shared_ptr<Reader> reader);

	int fftCount();
	void setFftCount(int fft);

	bool prepare(double time);

	// 幅度数据.
	std::pair<float *, int> values();

	// 幅度最小最大值范围.
	std::pair<float, float> valueRange();

	// 对应频率范围.
	std::pair<float, float> freqRange();
	
	// 频率步进.
	double freqStep();

private:
	void reset();

	typedef std::tuple<int /*position*/> State;

	State calculateState(double time);

	bool reload(State state);

private:
	std::shared_ptr<Reader> m_reader;
	std::shared_ptr<Fft> m_fft;

	std::vector<float> m_values;

	State m_state;
	//int m_position;
	int m_valueCount;
};

#endif //SIGNAL_LOADER_H