#pragma once

#include <vector>
#include <utility>
#include <memory>
#include <QRect>

#include "reader.h"


/**
 * 瀑布图数据.
 */
class Waterfall
{
public:
	Waterfall();

public:
	// 请求一定时间范围的数据.
	bool query(double start, double end, unsigned int countHint = 0);
	
	const float * data() const;
	
	std::pair<unsigned int, unsigned int> range() const;
	
	QRectF area() const;

private:
	bool reloadData();

private:
	// 绘制状态.
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


		unsigned int fftLen; // fft 长度.
		std::pair<unsigned int, unsigned int> range; //起始点范围
		unsigned int step; // 步长.
	};

	State m_state, m_oldState;

	std::vector<float> m_data;
	unsigned int m_fftLen;
	unsigned int m_align;

	std::shared_ptr<Reader> m_reader;	
};

