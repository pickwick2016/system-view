#include <algorithm>
//#include <boost/bind.hpp>

#include "waterfall.h"
#include "misc.h"
#include "reader.h"
#include "fft.h"

WaterfallLoader::WaterfallLoader(unsigned int fftlen)
{
	clear();

	m_fftHints = { 256, 512, 1024, 2048 };
	std::get<0>(m_state) = m_fftHints[0];
	m_stepAlign = 128;

	m_fft.reset(new Fft(std::get<0>(m_state)));

	m_colorRange = { -40.0f, 80.0f };

	m_colormap = std::bind(tool::colormap_other, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

	m_isAutoFft = false;
	m_fixFft = 256;
}

WaterfallLoader::~WaterfallLoader()
{
	m_reader.reset();
	m_fft.reset();
}

void WaterfallLoader::setAutoFft(bool on, int fftlen)
{
	m_isAutoFft = on;

	if (!m_isAutoFft && fftlen > 0) {
		m_fixFft = fftlen;
	}
}

std::pair<int, int> WaterfallLoader::dataSize()
{
	int currentFft = std::get<0>(m_state);
	std::pair<int, int> currentRange = std::get<1>(m_state);
	int currentStep = std::get<2>(m_state);

	int dataCount = ((currentRange.second - currentRange.first) / currentStep) + 1;
	
	int freqCount = currentFft / (m_reader->channel() == 1 ? 2 : 1);
	return { freqCount, dataCount };
}


void WaterfallLoader::clear()
{
	std::get<1>(m_state) = {0, 0}; 
	std::get<2>(m_state) = 0;

	m_datamap.clear();
}

WaterfallLoader::State WaterfallLoader::calculateState(QRectF visible, std::pair<int, int> sizeHint)
{
	assert(m_reader);

	unsigned int currentFft = 0;
	unsigned int currentStep = 0;
	std::pair<unsigned int, unsigned int > currentRange;

	double fs = m_reader->sampleRate();

	// 1.调整频率分段信息.
	if (m_isAutoFft) {
		int freqHint = sizeHint.second;
		if (freqHint > 0) {
			double freqStep = visible.height() / freqHint;

			double fs2 = fs / (m_reader->channel() == 1 ? 2 : 1);

			currentFft = m_fftHints.back();
			for (int i = 0; i < m_fftHints.size(); i++) {
				double step = fs2 / m_fftHints[i];
				if (step < freqStep) {
					currentFft = m_fftHints[i];
					break;
				}
			}
		}
	}
	else {
		currentFft = m_fixFft;
	}
	

	// 2.调整时间分段信息.
	unsigned int pos1 = floor(visible.left() * fs);
	unsigned int pos2 = floor(visible.right() * fs);

	int timeHint = sizeHint.first;
	if (timeHint > 0) {
		int temp = tool::round_down((pos2 - pos1) / timeHint, (int) m_stepAlign);
		int step = std::max<int>(temp, m_stepAlign);
		currentStep = step;
	}
	else {
		currentStep = m_stepAlign;
	}

	int align = currentStep;

	pos1 = tool::round_down(pos1, align);
	pos2 = tool::round_down(pos2, align);
	
	if (pos1 < m_reader->count()) {
		currentRange = { pos1, pos2 };
	}
	
	return {currentFft, currentRange, currentStep};
}

bool WaterfallLoader::reloadValues()
{
	auto currentFft = std::get<0>(m_state);
	auto currentRange = std::get<1>(m_state);
	auto currentStep = std::get<2>(m_state);

	// 1.根据当前状态载入数据.
	auto dataSize = this->dataSize();

	int segments = dataSize.second; // 分段数量.
	int freqs = dataSize.first; // 频点数量.

	if (m_datamap.size() != segments * freqs) {
		m_datamap.resize(segments * freqs);
	}

	std::vector<char> tempBuffer(currentFft * sizeof(double) * 2); // 临时缓冲区.
	for (int si = 0; si < segments; si++) {

		memset(tempBuffer.data(), 0, tempBuffer.size());

		int readPos = currentRange.first + si * currentStep;
		int getCount = m_reader->read((void *)tempBuffer.data(), currentFft, readPos);
		
		void * input = (void *)tempBuffer.data();
		float * output = m_datamap.data() + si * freqs;
		auto ret = m_fft->power(input, output, currentFft, m_reader->type());
		if (ret == 0) {
			return false;
		}
	}

	if (!m_datamap.empty()) {
		auto mm = std::minmax_element(m_datamap.begin(), m_datamap.end());
		m_valueRange.first =  *mm.first;
		m_valueRange.second = *mm.second;
	}
	else {
		m_valueRange = { 0.f, 0.f };
	}
	
	return true;
}

void WaterfallLoader::reloadPixmap()
{
	assert(m_colormap);

	// 1.图形化.
	auto dataSize = this->dataSize();

	int segments = dataSize.second; // 分段数量.
	int freqs = dataSize.first; // 频点数量.

	if (m_pixmapData.size() != segments * freqs) {
		m_pixmapData.resize(segments * freqs);
	}

	// 1.a 行列转化 + 数值/色彩转换
	for (int ti = 0; ti < segments; ti++) {
		for (int fi = 0; fi < freqs; fi++) {

			int idx1 = ti * freqs + fi;
			int idx2 = (freqs - fi - 1) * segments + ti;

			float val = m_datamap[idx1];
			auto rgb = m_colormap(val, m_colorRange.first, m_colorRange.second);
			m_pixmapData[idx2] = QColor::fromRgb(std::get<0>(rgb), std::get<1>(rgb), std::get<2>(rgb)).rgb();
		}
	}

	// 2.转储至 Pixmap
	QImage image((uchar *)m_pixmapData.data(), segments, freqs, QImage::Format_ARGB32);
	m_pixmap = QPixmap::fromImage(image);
}

void WaterfallLoader::load(std::shared_ptr<Reader> reader)
{
	m_reader = reader;
}

bool WaterfallLoader::load(const std::string & filename, int type, double samplerate)
{
	close();

	std::auto_ptr<FileReader> freader(new FileReader());
	if (freader->open(filename, type, samplerate)) {
		m_reader.reset(freader.release());
		return true;
	}

	return false;
}

void WaterfallLoader::close()
{
	m_reader.reset();

	clear();
}

bool WaterfallLoader::isLoaded()
{
	return m_reader.get() != nullptr;
}

QRectF WaterfallLoader::totalArea() 
{
	assert(m_reader);

	QRectF ret;
	ret.setWidth(m_reader->maxTime());
	ret.setHeight(m_reader->maxFreq());

	return ret;
}

QRectF WaterfallLoader::pixmapArea()
{
	QRectF rect;

	int currentFft = std::get<0>(m_state);
	std::pair<int, int> currentRange = std::get<1>(m_state);
	int currentStep = std::get<2>(m_state);
	
	if (m_reader && m_reader->sampleRate() > 0.0f) {
		double fs = m_reader->sampleRate();
		double ts = 1 / fs;

		double timeStart = currentRange.first * ts;
		double timeEnd = (tool::round_down(currentRange.second, (int)currentStep)
			+ std::max<int>(currentFft, currentStep)) * ts; // TODO :CHECK

		rect.setLeft(timeStart);
		rect.setRight(timeEnd);
		
		rect.setBottom(m_reader->maxFreq());
	}

	return rect;
}


uint32_t WaterfallLoader::valueToColor(float val)
{
	float val2 = tool::clamp<float>(val, -127, 128);
	int v = (int) (val2 + 127);
	
	QColor color = QColor::fromRgb(v, v, v);
	return color.rgb();
}

inline int calculateIndex(int row, int col, std::pair<int, int> & s)
{
	return row * s.first + col;
}

bool WaterfallLoader::reload(WaterfallLoader::State state)
{
	State oldState = m_state;
	m_state = state;
	
	bool bufferDirty = (m_state != oldState);
	if (!bufferDirty) {
		return true;
	}

	if (reloadValues()) {
		reloadPixmap();
		return true;
	}

	m_state = oldState;
	return false;
}

bool WaterfallLoader::prepare(QRectF visible, std::pair<int, int> sizeHint)
{
	if (!m_reader) {
		return false;
	}

	State newState = calculateState(visible, sizeHint);
	if (newState == m_state) {
		return true;
	}

	bool ret = reload(newState);
	return ret;
}

void WaterfallLoader::setColorRange(std::pair<float, float> rng)
{
	auto rng2 = tool::range_normalize(rng);
	if (m_colorRange != rng2) {
		m_colorRange = rng2;
		reloadPixmap();
	}
}