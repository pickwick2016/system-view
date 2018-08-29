#include <algorithm>

#include "waterfall.h"
#include "misc.h"


Waterfall::Waterfall(unsigned int fftlen)
{
	clear();

	m_fftHints = { 256, 512, 1024, 2048 };
	m_currentFft = fftlen;
	m_stepAlign = m_currentFft;


	m_reader.reset(new FileReader());
	m_fft.reset(new Fft(m_currentFft));

	m_colorRange = { -40, 80 };

	m_colormap = boost::bind(tool::colormap_other, _1, _2, _3);
}

Waterfall::~Waterfall()
{
	m_reader.reset();
	m_fft.reset();
}

std::pair<int, int> Waterfall::dataSize()
{
	int dataCount = ((m_currentRange.second - m_currentRange.first) / m_currentStep) + 1;
	int freqCount = this->freqCount();
	return { freqCount, dataCount };
}


void Waterfall::clear()
{
	m_currentRange = {0, 0}; 
	m_currentStep = 0;	

	m_datamap.clear();
}

unsigned int Waterfall::power(void * input, float * output, unsigned int fftlen, int type)
{
	switch (type) {
	case DataType::Int8:
		m_fft->power((char *)input, output, fftlen);
		break;
	case DataType::Int16:
		m_fft->power((short *)input, output, fftlen);
		break;
	case DataType::Int32:
		m_fft->power((int *)input, output, fftlen);
		break;
	case DataType::Real32:
		m_fft->power((float *)input, output, fftlen);
		break;
	case DataType::Real64:
		m_fft->power((double *)input, output, fftlen);
		break;
	default:
		return 0;
	}
	return fftlen;
}

bool Waterfall::query(QRectF visible, std::pair<int, int> sizeHint)
{
	assert(m_reader);

	double fs = m_reader->sampleRate();

	// 1.调整频率分段信息.
	int freqHint = sizeHint.second;
	if (freqHint > 0) {
		double freqStep = visible.height() / freqHint;

		double fs2 = fs / (m_reader->channel() == 1 ? 2 : 1);

		m_currentFft = m_fftHints.back();
		for (int i = 0; i < m_fftHints.size(); i++) {
			double step = fs2 / m_fftHints[i];
			if (step < freqStep) {
				m_currentFft = m_fftHints[i];
				break;
			}
		}
	}

	
	// 2.调整时间分段信息.
	unsigned int pos1 = floor(visible.left() * fs);
	unsigned int pos2 = floor(visible.right() * fs);

	int timeHint = sizeHint.first;
	if (timeHint > 0) {
		int temp = tool::round_down((pos2 - pos1) / timeHint, m_stepAlign);
		int step = std::max<int>(temp, m_stepAlign);
		m_currentStep = step;
	}
	else {
		m_currentStep = m_stepAlign;
	}

	int align = m_currentStep;

	pos1 = tool::round_down(pos1, align);
	pos2 = tool::round_down(pos2, align);
	
	if (pos1 >= m_reader->count()) {
		return false;
	}

	m_currentRange = { pos1, pos2 };
	
	return true;
}
bool Waterfall::reloadBuffer()
{
	// 1.根据当前状态载入数据.
	auto dataSize = this->dataSize();

	int segments = dataSize.second; // 分段数量.
	int freqs = dataSize.first; // 频点数量.

	if (m_datamap.size() != segments * freqs) {
		m_datamap.resize(segments * freqs);
	}

	std::vector<char> tempBuffer(m_currentFft * sizeof(double) * 2); // 临时缓冲区.
	for (int si = 0; si < segments; si++) {

		memset(tempBuffer.data(), 0, tempBuffer.size());

		int readPos = m_currentRange.first + si * m_currentStep;
		m_reader->read((void *)tempBuffer.data(), m_currentFft, readPos);

		void * input = (void *)tempBuffer.data();
		float * output = m_datamap.data() + si * freqs;
		auto ret = power(input, output, m_currentFft, m_reader->type());
		if (ret == 0) {
			return false;
		}
	}

	return true;
}

bool Waterfall::bufferToPixmap()
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

	return true;
}



bool Waterfall::load(const std::string & filename, int type, double samplerate)
{
	assert(m_reader);

	close();

	FileReader * freader = dynamic_cast<FileReader *>(m_reader.get());
	if (! freader || ! freader->open(filename, type, samplerate)) {
		return false;
	}

	return true;
}

void Waterfall::close()
{
	assert(m_reader);

	FileReader * freader = dynamic_cast<FileReader *>(m_reader.get());
	if (freader) {
		freader->close();
	}

	clear();
}

unsigned int Waterfall::freqCount()
{
	return m_currentFft / (m_reader->channel() == 1 ? 2 : 1);
}

QRectF Waterfall::totalArea() 
{
	QRectF rect;
	if (m_reader) {
		double fs = m_reader->sampleRate();
		int step = (m_currentStep <= 0) ? m_currentFft : m_currentStep;

		double maxTime = (tool::round_down(m_reader->count() - 1, step) + std::max<int>(m_currentFft, step)) / fs;
		//double maxTime = (tool::round_down(m_reader->count() - 1, step) + m_fftLen) * fs;
		//double maxTime = (m_reader->count() - 1) * fs;
		double maxFreq = (m_reader->channel() == 2) ? fs : fs / 2;

		rect.setRight(maxTime);
		rect.setBottom(maxFreq);
	}

	return rect;
}

QRectF Waterfall::currentArea()
{
	QRectF rect;
	
	if (m_reader && m_reader->sampleRate() > 0.0f) {
		double fs = m_reader->sampleRate();
		double ts = 1 / fs;

		double timeStart = m_currentRange.first * ts;
		double timeEnd = (tool::round_down(m_currentRange.second, m_currentStep)
			+ std::max<int>(m_currentFft, m_currentStep)) * ts; // TODO :CHECK

		double freqEnd = (m_reader->channel() == 2) ? fs : fs / 2;

		rect.setLeft(timeStart);
		rect.setRight(timeEnd);
		rect.setBottom(freqEnd);
	}

	return rect;
}


uint32_t Waterfall::valueToColor(float val)
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


bool Waterfall::prepare(QRectF visible, std::pair<int, int> sizeHint)
{
	if (!m_reader)
		return false;

	if (!query(visible, sizeHint)) {
		return false;
	}

	m_currState = { m_currentFft, m_currentRange, m_currentStep };

	bool bufferDirty = (m_currState != m_prevState);
	bool colorDirty = (m_prevColorRange != m_colorRange);

	if (bufferDirty) {
		if (! reloadBuffer()) {
			return false;
		}
		m_prevState = m_currState;
	}

	if (bufferDirty || colorDirty) {
		if (! bufferToPixmap()) {
			return false;
		}
		m_prevColorRange = m_colorRange;
	}
		
	return true;
}

void Waterfall::setColorRange(std::pair<float, float> rng)
{
	m_colorRange = tool::normalize(rng);
}