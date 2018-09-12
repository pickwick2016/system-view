#pragma once

#include <map>
#include <vector>

namespace tool {

	template <class T1, class T2>
	T2 safe_get(std::map<T1, T2> & container, T1 key, T2 default_value)
	{
		if (container.find(key) != container.end()) {
			return container[key];
		}

		return default_value;
	}

	template <class T>
	T safe_get(std::vector<T> & container, int idx, T default_value)
	{
		if (idx >= 0 && idx < container.size()) {
			return container[idx];
		}

		return default_value;
	}

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


} // namespace tool