#include "signal_loader.h"
#include "fft.h"
#include "reader.h"
#include "misc.h"

WaveLoader::WaveLoader()
{
	m_align = 10;
	m_zipCount = 10;
	
	reset();
}

void WaveLoader::reset()
{
	m_state = { 0, 0, 0 };
}

void WaveLoader::load(std::shared_ptr<Reader> reader)
{
	m_reader = reader;
	
	reset();
}

WaveLoader::State WaveLoader::calculateState(std::pair<double, double> timeRange, int countHint)
{
	double fs = m_reader->sampleRate();

	int start = std::max<int>(0, std::floor(timeRange.first * fs));
	int end = std::min<int>(m_reader->count(), std::floor(timeRange.second * fs));

	int step = 1;
	if (countHint > 0) {
		int len = end - start;
		int step2 = len / countHint;
		step2 = tool::round_down(step2, m_align);
		if (step2 > 0) {
			step = step2;
		}
	}

	if (step > 1) {
		start = tool::round_down(start, step);
		end = tool::round_down(end, step);
	}

	return { start, end, step };
}

bool WaveLoader::prepare(std::pair<double, double> timeRange, int countHint)
{
	if (!m_reader) {
		return false;
	}

	State newState = calculateState(timeRange, countHint);
	if (newState == m_state) {
		return true;
	}
	
	bool ret = reload(newState);
	if (ret) {
		m_state = newState;
	}

	return ret;
}

bool WaveLoader::reload(State state)
{
	int start = std::get<0>(state);
	int end = std::get<1>(state);
	int step = std::get<2>(state);

	int count = (end - start) / step + 1;
	bool zip = (step > 1);

	if (! zip) {
		reload1(state);
	}
	else {
		reload2(state);
	}
	
	return true;
}

void WaveLoader::reload1(State state)
{
	assert(m_reader);

	int start = std::get<0>(state);
	int end = std::get<1>(state);
	int count = end - start + 1;

	std::vector<double> temp(count * 2);
	int readCount = m_reader->readAsReal64(temp.data(), count, start);
	
	if (m_reader->channel() == 1) {
		m_values[0].resize(readCount);
		std::copy(temp.begin(), temp.begin() + readCount, m_values[0].begin());
		m_valueRange[0] = tool::minmax(m_values[0].begin(), m_values[0].begin() + readCount);
	}
	else if (m_reader->channel() == 2) {
		int count2 = readCount / 2;

		m_values[0].resize(count2);
		m_values[1].resize(count2);

		m_valueRange[0] = { -1, 1 };
		m_valueRange[1] = { -1, 1 };

		std::complex<double> * temp2 = (std::complex<double> *) temp.data();
		for (int i = 0; i < count2; i++) {
			m_values[0][i] = temp2[i].real();
			m_values[1][i] = temp2[i].imag();

			m_valueRange[0].first = std::min<double>(m_valueRange[0].first, temp2[i].real());
			m_valueRange[0].second = std::max<double>(m_valueRange[0].second, temp2[i].real());

			m_valueRange[1].first = std::min<double>(m_valueRange[1].first, temp2[i].imag());
			m_valueRange[1].second = std::max<double>(m_valueRange[1].second, temp2[i].imag());
		}
	}
}

void WaveLoader::reload2(State state)
{
	int start = std::get<0>(state);
	int end = std::get<1>(state);
	int step = std::get<2>(state);
	int chunk = std::min<int>(step, 10);
	int count = (end - start) / step + 1;

	std::vector<double> temp(chunk * 2);

	m_values[0].resize(count * 2);
	m_values[1].resize(count * 2);

	m_valueRange[0] = { -1, 1 };
	m_valueRange[1] = { -1, 1 };

	for (int pos = start, i = 0; pos <= end; pos += step, i++) {
		int readCount = m_reader->readAsReal64(temp.data(), chunk, pos);
		
		if (m_reader->channel() == 1) {
			auto mm = tool::minmax(temp.begin(), temp.begin() + readCount);
			((std::pair<double, double> *) m_values[0].data())[i] = mm;

			m_valueRange[0].first = std::min<double>(m_valueRange[0].first, mm.first);
			m_valueRange[0].second = std::max<double>(m_valueRange[0].second, mm.second);
		}
		else if (m_reader->channel() == 2) {
			int count2 = readCount / 2;
			
			std::vector<double> reals(count2);
			std::vector<double> imags(count2);

			std::complex<double> * temp2 = (std::complex<double> *) temp.data();
			for (int i = 0; i < count2; i++) {
				reals[i] = temp2[i].real();
				imags[i] = temp2[i].imag();
			}
			
			auto mm1 = tool::minmax(reals.begin(), reals.end());
			((std::pair<double, double> *) m_values[0].data())[i] = mm1;

			auto mm2 = tool::minmax(imags.begin(), imags.end());
			((std::pair<double, double> *) m_values[1].data())[i] = mm2;

			m_valueRange[0].first = std::min<double>(m_valueRange[0].first, mm1.first);
			m_valueRange[0].second = std::max<double>(m_valueRange[0].second, mm1.second);

			m_valueRange[1].first = std::min<double>(m_valueRange[1].first, mm2.first);
			m_valueRange[1].second = std::max<double>(m_valueRange[1].second, mm2.second);
		}
	}
}

std::pair<double, double> WaveLoader::timeRange()
{
	double ts = 1 / m_reader->sampleRate();

	int start = std::get<0>(m_state);
	int end = std::get<1>(m_state);

	return { start * ts, end * ts };
}

double WaveLoader::timeStep() 
{
	return 1 / m_reader->sampleRate(); 
}

double * WaveLoader::value(int channel)
{
	if (channel >= 0 && channel < m_reader->channel()) {
		return m_values[channel].data();
	}

	return nullptr;
}

std::pair<double, double> * WaveLoader::value2(int channel)
{
	if (channel >= 0 && channel < m_reader->channel()) {
		return (std::pair<double, double> *) (m_values[channel].data());
	}

	return nullptr;
}

std::pair<double, double> WaveLoader::valueRange(int channel)
{
	if (channel >= 0 && channel < m_reader->channel()) {
		return m_valueRange[channel];
	}

	return m_valueRange[0];
}

bool WaveLoader::isZipped()
{
	int step = std::get<2>(m_state);
	return step > 1;
}

int WaveLoader::channel()
{
	if (!m_reader)
		return 0;

	return m_reader->channel();
}


///////////////////////////////////////////////////////////////////////////////////

FreqLoader::FreqLoader()
{
	m_fft.reset(new Fft());

	reset();
}

void FreqLoader::reset()
{
	m_state = { -1 };
	m_valueCount = 0;
}

void FreqLoader::load(std::shared_ptr<Reader> reader)
{
	m_reader = reader;

	reset();
}

int FreqLoader::fftCount()
{
	return m_fft->length();
}

void FreqLoader::setFftCount(int fft)
{
}

FreqLoader::State FreqLoader::calculateState(double time)
{
	double fs = m_reader->sampleRate();
	int pos = std::floor(time * fs);

	if (tool::range_contain({ 0, m_reader->count() - 1 }, pos) < 0) {
		return m_state;
	}

	return { pos };
}

bool FreqLoader::reload(FreqLoader::State state)
{
	int pos = std::get<0>(state);

	int fftlen = m_fft->length();
	m_values.resize(fftlen);
	std::vector<char> buffer(sizeof(double) * 2 * fftlen);

	int readCount = m_reader->read(buffer.data(), fftlen, pos);
	if (readCount <= 0) {
		return false;
	}

	m_valueCount = m_fft->power(buffer.data(), m_values.data(), fftlen, m_reader->type());
	if (m_valueCount <= 0) {
		return false;
	}

	return true;
}

bool FreqLoader::prepare(double time)
{
	if (!m_reader) {
		return false;
	}

	State newState = calculateState(time);
	if (newState == m_state) {
		return true;
	}

	bool ret = reload(newState);
	if (ret) {
		m_state = newState;
	}

	return true;
}

std::pair<float *, int>  FreqLoader::values()
{
	return { m_values.data(), m_valueCount };
}

std::pair<float, float> FreqLoader::valueRange()
{
	if (m_valueCount > 0) {
		auto mm = std::minmax_element(m_values.begin(), m_values.begin() + m_valueCount);
		return { *(mm.first), *(mm.second) };
	}

	return { 0.0f, 1.0f };
}

std::pair<double, double> FreqLoader::freqRange()
{
	return { 0, m_reader->maxFreq() };
}

double FreqLoader::freqStep()
{
	assert(m_fft);

	if (! m_reader) {
		return 0;
	}

	return m_reader->sampleRate() / m_fft->length();
}