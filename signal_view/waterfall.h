#pragma once

#include <vector>
#include <utility>
#include <memory>
#include <QRect>

#include "reader.h"


/**
 * �ٲ�ͼ����.
 */
class Waterfall
{
public:
	Waterfall();

public:
	// ����һ��ʱ�䷶Χ������.
	bool query(double start, double end, unsigned int countHint = 0);
	
	const float * data() const;
	
	std::pair<unsigned int, unsigned int> range() const;
	
	QRectF area() const;

private:
	bool reloadData();

private:
	// ����״̬.
	struct State
	{
		bool operator != (const State & rhs) const
		{
			return fftLen != rhs.fftLen || step != rhs.step || range != rhs.range;
		}

		int count()
		{
			return (range.second - range.first) / step + 1;
		}


		unsigned int fftLen; // fft ����.
		std::pair<unsigned int, unsigned int> range; //��ʼ�㷶Χ
		unsigned int step; // ����.
	};

	State m_state, m_oldState;

	std::vector<float> m_data;
	unsigned int m_fftLen;
	unsigned int m_align;

	std::shared_ptr<Reader> m_reader;	
};

