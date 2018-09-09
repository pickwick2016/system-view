#pragma once

#include <string>
#include <tuple>
#include <vector>
#include <algorithm>
#include <memory>
#include <utility>

#include <QRect>


namespace tool {

	// 数值守护.
	template <class T>
	class ValueGuard
	{
	public:
		ValueGuard(T & obj, T newValue) : m_reference(obj)
		{
			m_oldValue = obj;
			obj = newValue;
		}

		~ValueGuard()
		{
			m_reference = m_oldValue;
		}

	private:
		T & m_reference;
		T m_oldValue;
	};

	template <class FwdIt, class T = FwdIt::value_type>
	std::pair<T, T> minmax(FwdIt first, FwdIt last)
	{
		auto mm = std::minmax_element(first, last);
		return { *(mm.first), *(mm.second) };
	}

	template <class T>
	std::pair<T, T> minmax(T * first, T * last)
	{
		auto mm = std::minmax_element(first, last);
		return { *(mm.first), *(mm.second) };
	}

	int convert(void * input, double * output, int count, int datatype);

	// 剪切矩形.
	QRectF clip(QRectF r, QRectF all);
	QRect clip(QRect r, QRect all);

	// 调整窗口.
	QRectF adjust(QRectF r, QRectF all, bool keepsize = true);

	QRectF rectExpand(QRectF r, double v);
	QRectF rectExpandX(QRectF r, double v);
	QRectF rectExpandY(QRectF r, double v);


	QRectF rectMoveX(QRectF r, double v);
	QRectF rectMoveY(QRectF r, double v);

	QRectF rectFlipY(QRectF r);


	template <class T>
	T map(T valFrom, std::pair<T, T> rngFrom, std::pair<T, T> rngTo)
	{
		return rngTo.first + (valFrom - rngFrom.first) * range_length(rngTo) / range_length(rngFrom);
	}

	QPointF map(QPointF pa, QRectF ra, QRectF rb);
	QRectF map(QRectF pa, QRectF ra, QRectF rb);

	template <class T>
	std::pair<T, T> range_normalize(const std::pair<T, T> & val)
	{
		return { std::min<T>(val.first, val.second), std::max<T>(val.first, val.second) };
	}

	template <class T>
	T range_length(const std::pair<T, T> & val)
	{
		return val.second - val.first;
	}

	template <class T>
	int range_contain(const std::pair<T, T> & rng, T val)
	{
		if (val > rng.first && val < rng.second) {
			return 1;
		}
		else if (val < rng.first || val > rng.second) {
			return -1;
		}
			
		return 0;
	}

	double range_split(std::pair<double, double> rng, int countHint, std::vector<double> & ret);

	template <class T>
	std::pair<T, T> range_expand(const std::pair<T, T> & val, T dv)
	{
		return { val.first - dv, val.second + dv };
	}

	template <class T>
	std::pair<T, T> range_move(const std::pair<T, T> & val, T dv)
	{
		return { val.first + dv, val.second + dv };
	}
	
	inline int round_down(int count, int seg)
	{
		return (count / seg) * seg;
	}

	inline double round_down(double val, double step)
	{
		return (int)(val / step) * step;
	}

	inline unsigned int round_up(unsigned int count, unsigned int seg)
	{
		return ((count + seg - 1) / seg) * seg;
	}

	double round_pow2(double value);

	std::tuple<int, int, int> colormap_rainbow(float val, float vmin, float vmax);

	std::tuple<int, int, int> colormap_gray(float val, float vmin, float vmax);

	std::tuple<int, int, int> colormap_other(float val, float vmin, float vmax);

	template <class T>
	T clamp(T val, T min, T max)
	{
		if (val < min)
			return min;

		if (val > max)
			return max;

		return val;
	}

	template <class T>
	std::pair<T, T> range_adjust(std::pair<T, T> rng, std::pair<T, T> all, bool keepsize)
	{
		auto ret = rng;

		auto old = rng;
		if (rng.first < all.first) {
			ret.first = all.first;
			if (keepsize) {
				ret.second = ret.first + range_length(old);
			}
		}

		if (rng.second > all.second) {
			ret.second = all.second;
			if (keepsize) {
				ret.first = ret.second - range_length(old);
			}
		}
		
		ret.first = clamp(ret.first, all.first, all.second);
		ret.second = clamp(ret.second, all.first, all.second);

		return ret;
	}



} // namespace tool

