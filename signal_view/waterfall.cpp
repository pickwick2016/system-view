#include <algorithm>

#include "waterfall.h"
#include "misc.h"


Waterfall::Waterfall(unsigned int fftlen)
{
	clear();

	m_fftLen = fftlen;
	m_align = m_fftLen;

	m_reader.reset(new FileReader());
	m_fft.reset(new Fft(m_fftLen));
}

std::pair<int, int> Waterfall::dataSize()
{
	std::pair<int, int> ret = { dataCount(), m_freqCount };
	return ret;
}


void Waterfall::clear()
{
	m_freqCount = 0;
	m_currentRange = {0, 0}; 
	m_currentStep = 0;	

	m_data.clear();
}

unsigned int Waterfall::power(void * input, float * output, unsigned int fftlen, int type)
{
	if (m_fft->length() != fftlen) {
		m_fft.reset(new Fft(fftlen));
	}

	switch (type) {
	case DataType::Int8:
		m_fft->power((char *)input, output);
		break;
	case DataType::Int16:
		m_fft->power((short *)input, output);
		break;
	case DataType::Int32:
		m_fft->power((int *)input, output);
		break;
	case DataType::Real32:
		m_fft->power((float *)input, output);
		break;
	case DataType::Real64:
		m_fft->power((double *)input, output);
		break;
	default:
		return 0;
	}

	// TODO : 

	// int outputCount = fftlen / 2;
	//switch (type) {
	//case DataType::Int8:
	//	std::copy((char *)input, (char *)input + outputCount, output);
	//	break;
	//case DataType::Int16:
	//	std::copy((short *)input, (short *)input + outputCount, output);
	//	break;
	//case DataType::Int32:
	//	std::copy((int *)input, (int *)input + outputCount, output);
	//	break;
	//case DataType::Real32:
	//	std::copy((float *)input, (float *)input + outputCount, output);
	//	break;
	//case DataType::Real64:
	//	std::copy((double *)input, (double *)input + outputCount, output);
	//	break;
	//default:
	//	return 0;
	//}

	return fftlen;
}

bool Waterfall::query(double start, double end, unsigned int countHint)
{
	assert(m_reader);

	double fs = sampleRate();

	unsigned int pos1 = floor(start * fs);
	pos1 = tool::round_down(std::min<unsigned int>(m_reader->count(), pos1), m_align);

	unsigned int pos2 = floor(end * fs);
	pos2 = tool::round_down(std::min<unsigned int>(m_reader->count(), pos2), m_align);

	m_currentRange = { pos1, pos2 };
	m_currentStep = m_align; // TODO: 根据countHint调整.

	return true;
}

bool Waterfall::reloadData()
{
	assert(m_reader);

	// 1.根据当前状态载入数据.
	auto mapSize = currentMapSize();

	m_data.resize(mapSize.first * mapSize.second);

	std::vector<char> buffer(m_fftLen * sizeof(double) * 2); // 临时缓冲区.

	for (int i = 0; i < mapSize.first; i++) {
		int readPos = m_currentRange.first + i * m_currentStep;
		m_reader->read((void *)buffer.data(), m_fftLen, readPos);

		auto ret = power((void *)buffer.data(), m_data.data() + i * m_freqCount, m_fftLen, m_reader->type());
		if (ret == 0) {
			return false;
		}
	}

	// 2.根据数据更新pixmap
	if (!fill(m_pixmap)) {
		return false;
	}

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

	m_freqCount = m_fftLen / (freader->channel() == 1 ? 2 : 1);

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

QRectF Waterfall::totalArea() 
{
	QRectF rect;

	auto s = totalSize();
	rect.setWidth(s.first);
	rect.setHeight(s.second);

	return rect;
}

QRectF Waterfall::currentArea()
{
	QRectF rect;

	auto s = totalSize();

	double ts = 1 / sampleRate();

	rect.setLeft(m_currentRange.first * ts);
	rect.setRight((tool::round_down(m_currentRange.second, m_align) + m_fftLen) * ts);

	rect.setHeight(s.second);

	return rect;
}

std::pair<double, double> Waterfall::totalSize()
{
	assert(m_reader);

	auto dsize = dataSize();

	double fs = sampleRate();
	double ts = 1 / fs;

	std::pair<double, double> ret;
	
	ret.first = (tool::round_down(dataCount(), m_align) + m_fftLen) * ts;
	ret.second = fs / m_fftLen * m_freqCount;

	return ret;
}


std::pair<int, int> Waterfall::currentMapSize()
{
	int width = (m_currentRange.second - m_currentRange.first) / m_currentStep + 1;
	int height = m_freqCount;

	return { width, height };
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

bool Waterfall::fill(QPixmap & pixmap)
{
	// 1.图形化.
	auto mapSize = currentMapSize();
	std::pair<int, int> dataSize = { mapSize.second, mapSize.first };

	m_buffer.resize(mapSize.first * mapSize.second);

	for (int ri = 0; ri < dataSize.second; ri++) {
		for (int ci = 0; ci < dataSize.first; ci++) {
			int idx1 = calculateIndex(ri, ci, dataSize);
			int idx2 = calculateIndex(dataSize.first - ci - 1, ri, mapSize);

			float val = m_data[idx1];
			uint32_t color = valueToColor(val);
			m_buffer[idx2] = color;
		}
	}

	// 2.转储至 Pixmap
	QImage image((uchar *)m_buffer.data(), mapSize.first, mapSize.second, QImage::Format_ARGB32);
	pixmap = QPixmap::fromImage(image);
	return true;
}

QRectF Waterfall::pixmapArea()
{
	return currentArea();
}


unsigned int Waterfall::fftLen()
{ 
	return m_fftLen; 
}

double Waterfall::sampleRate()
{
	return m_reader ? m_reader->sampleRate() : 0;
}

int Waterfall::dataType() 
{ 
	return m_reader ? m_reader->type() : DataType::Unknown;
}

unsigned int Waterfall::dataCount()
{
	return m_reader ? m_reader->count() : 0;
}

bool Waterfall::preparePixmap(QRectF visible)
{
	if (!m_reader)
		return false;

	if (!query(visible.left(), visible.right())) {
		return false;
	}

	m_currState = { m_fftLen, m_currentRange, m_currentStep };
	if (m_currState == m_oldState) {
		return true;
	}

	if (reloadData()) {
		m_oldState = m_currState;
		return true;
	}

	return false;
}