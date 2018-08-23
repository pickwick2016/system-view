#include <algorithm>

#include "waterfall.h"

unsigned int round_down(unsigned int count, unsigned int seg)
{
	return (count / seg) * count;
}

Waterfall::Waterfall()
{
	m_fftLen = 1024;
	m_align = 1024;
}

bool Waterfall::query(double start, double end, unsigned int countHint)
{
	if (!m_reader)
		return false;

	double rate = m_reader->sampleRate();

	unsigned int pos1 = floor(start * rate);
	pos1 = std::min<unsigned int>(m_reader->count(), round_down(pos1, m_align));

	unsigned int pos2 = floor(end * rate);
	pos2 = std::min<unsigned int>(m_reader->count(), round_down(pos2, m_align));

	m_state.range = { pos1, pos2 };
	m_state.step = m_align; // TODO: 根据countHint调整.
	m_state.fftLen = m_fftLen;

	if (m_state != m_oldState) {
		if (reloadData()) {
			m_oldState = m_state;
			return true;
		}
	}

	return false;
}

unsigned int fft(void * input, float * output, unsigned int fftlen, int type)
{
	// TODO : 

	return 0;
}

bool Waterfall::reloadData()
{
	if (!m_reader)
		return false;

	unsigned int fftLen = m_state.fftLen;
	std::vector<char> buffer(fftLen * 8);

	int count = m_state.count();
	m_data.resize(fftLen * count);

	for (int i = 0; i < count; i++) {
		m_reader->read((void *)buffer.data(), fftLen);
		fft((void *)buffer.data(), m_data.data() + i * fftLen, fftLen, m_reader->type());
	}

	return false;
}

const float * Waterfall::data() const
{
	return m_data.data();
}

std::pair<unsigned int, unsigned int> Waterfall::range() const
{
	return m_range;
}

QRectF Waterfall::area() const
{
	QRectF rect;
	return rect;
}