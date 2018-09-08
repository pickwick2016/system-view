#ifndef SIGNAL_LOADER_H
#define SIGNAL_LOADER_H

#include <tuple>
#include <memory>
#include <utility>
#include <vector>
#include <array>

class Reader;
class Fft;

// ��������(ѹ��)������.
class WaveLoader
{
public:
	// �ڲ�״̬ <start, end, step> 3Ԫ��.
	typedef std::tuple<int, int, int> State;

public:
	WaveLoader();

public:
	void load(std::shared_ptr<Reader> reader);

	bool prepare(std::pair<double, double> timeRange, int countHint);

	// ���ݣ���Է�ѹ�����ݣ�
	double * value(int channel);

	// ���ݷ�Χ�飨���ѹ�����ݣ�
	std::pair<double, double> * value2(int channel);
	
	// ��ֵ��Χ
	std::pair<double, double> valueRange(int channel);
	
	// �����Ƿ�ѹ��.
	bool isZipped();

	// ����ͨ������.
	int channel();

	double timeStep();

	std::pair<double, double> timeRange();

	State state() { return m_state; }

private:
	// �����ڲ�״̬.
	void reset();
	
	// ���¼��㵱ǰ״̬.
	State calculateState(std::pair<double, double> timeRange, int countHint);

	// ���ݵ�ǰ״̬��������.
	bool reload(State state);

	// �����ѹ������.
	void reload1(State state);

	// ����ѹ������.
	void reload2(State state);

private:
	int m_align; // ѹ��ʱ������ֵ.
	int m_zipCount; // ѹ��ʱ��������.

	std::shared_ptr<Reader> m_reader;

	std::vector<double> m_values[2]; // ����ͨ���Ļ�������.
	std::pair<double, double> m_valueRange[2]; // ����ͨ�����ݵķ�Χ.
	
	State m_state; // ��ǰ״̬.
};


// Ƶ������������.
class FreqLoader
{
public:
	FreqLoader();

public:
	void load(std::shared_ptr<Reader> reader);

	int fftCount();
	void setFftCount(int fft);

	bool prepare(double time);

	// ��������.
	std::pair<float *, int> values();

	// ������С���ֵ��Χ.
	std::pair<float, float> valueRange();

	// ��ӦƵ�ʷ�Χ.
	std::pair<float, float> freqRange();
	
	// Ƶ�ʲ���.
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