#pragma once

#include <string>
#include <tuple>

/**
 * ÎÄ¼şÃèÊö.
 */
struct FileDescription
{
public:
	FileDescription(const std::string & filename = "", int datatype = 0, double samplerate = 1);

	bool isValid() const;

public:
	std::string fileName;
	double  sampleRate;
	int dataType;
};

namespace tool {

	template <class T>
	std::pair<T, T> normalize(const std::pair<T, T> & val)
	{
		return { std::min<T>(val.first, val.second), std::max<T>(val.first, val.second) };
	}

	template <class T>
	T length(const std::pair<T, T> & val)
	{
		return val.second - val.first;
	}
	
	inline unsigned int round_down(unsigned int count, unsigned int seg)
	{
		return (count / seg) * seg;
	}

	inline unsigned int round_up(unsigned int count, unsigned int seg)
	{
		return ((count + seg - 1) / seg) * seg;
	}


	template <class T>
	T clamp(T val, T min, T max)
	{
		if (val < min)
			return min;

		if (val > max)
			return max;

		return val;
	}


	std::tuple<int, int, int> colormap_rainbow(float val, float vmin, float vmax);


	std::tuple<int, int, int> colormap_gray(float val, float vmin, float vmax);


	std::tuple<int, int, int> colormap_other(float val, float vmin, float vmax);


} // namespace tool

