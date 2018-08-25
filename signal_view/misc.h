#pragma once

#include <string>

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

} // namespace tool

