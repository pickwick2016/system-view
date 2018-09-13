#pragma once

#include <map>
#include <vector>

namespace tool {


	// 安全获取数据.
	template <class T1, class T2>
	T2 safe_get(std::map<T1, T2> & container, T1 key, T2 default_value)
	{
		if (container.find(key) != container.end()) {
			return container[key];
		}

		return default_value;
	}

	// 安全获取数据.
	template <class T>
	T safe_get(std::vector<T> & container, int idx, T default_value)
	{
		if (idx >= 0 && idx < container.size()) {
			return container[idx];
		}

		return default_value;
	}

	// 获取最小值/最大值.
	template <class FwdIt, class T = FwdIt::value_type>
	std::pair<T, T> minmax(FwdIt first, FwdIt last)
	{
		auto mm = std::minmax_element(first, last);
		return { *(mm.first), *(mm.second) };
	}

	// 获取最小值/最大值.
	template <class T>
	std::pair<T, T> minmax(T * first, T * last)
	{
		auto mm = std::minmax_element(first, last);
		return { *(mm.first), *(mm.second) };
	}

	// 范围规范化（ first <= second ）
	template <class T>
	std::pair<T, T> range_normalize(const std::pair<T, T> & val)
	{
		return { std::min<T>(val.first, val.second), std::max<T>(val.first, val.second) };
	}

	// 计算范围长度.
	template <class T>
	T range_length(const std::pair<T, T> & val)
	{
		return val.second - val.first;
	}

	/**
	 * 判断范围是否包含数值.
	 * @return 1:内部，0:在两端，-1:不包含.
	 */
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

	// 移动范围.
	template <class T>
	std::pair<T, T>  range_move(const std::pair<T, T> & rng, T val)
	{
		return { rng.first + val, rng.second + val };
	}

	// 增加/缩减范围.
	template <class T>
	std::pair<T, T> range_expand(const std::pair<T, T> & val, T dv)
	{
		return { val.first - dv, val.second + dv };
	}

	// 调整范围.
	template <class T>
	std::pair<T, T> range_adjust(const std::pair<T, T> & rng, const std::pair<T, T> & all, bool keepsize)
	{
		std::pair<T, T> ret = rng;

		if (keepsize) {
			if (ret.first < all.first) {
				ret = range_move(ret, all.first - ret.first);
			}

			if (ret.second > all.second) {
				ret = range_move(ret, all.second - ret.second);
			}
		}

		ret.first = std::max<T>(ret.first, all.first);
		ret.second = std::min<T>(ret.second, all.second);

		return ret;
	}

	// 限制.
	template <class T>
	T clamp(T val, const std::pair<T, T> & rng)
	{
		std::pair<T, T> rng2 = range_normalize(rng);

		if (val < rng.first)
			return rng.first;

		if (val > rng.second)
			return rng.second;

		return val;
	}

	// 限制.
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