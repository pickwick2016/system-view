#pragma once

#include <string>
#include <tuple>
#include <vector>
#include <map>
#include <algorithm>
#include <memory>
#include <utility>

#include <QRect>

#include "std_ext.h"

namespace tool {

	// 数值守护.
	template <class T>
	class ValueGuard
	{
	public:
		ValueGuard(T & obj, T newValue) : m_reference(obj) {
			m_oldValue = obj;
			obj = newValue;
		}

		~ValueGuard() {
			m_reference = m_oldValue;
		}

	private:
		T & m_reference;
		T m_oldValue;
	};

	// 矩形范围.
	inline std::pair<double, double> RectRangeX(const QRectF & r) { return { r.left(), r.right() }; }
	inline std::pair<int, int> RectRangeX(const QRect & r) { return { r.left(), r.right() }; }
	inline std::pair<double, double> RectRangeY(const QRectF & r) { return { r.top(), r.bottom() }; }
	inline std::pair<int, int> RectRangeY(const QRect & r) { return { r.top(), r.bottom() }; }

	// 剪切矩形.
	QRectF RectClip(const QRectF & r, const QRectF & all);
	QRect RectClip(const QRect & r, const QRect & all);

	// 调整窗口（限制矩形）.
	QRectF RectAdjust(const QRectF & r, const QRectF & all, bool keepsize = true);
	QRectF RectAdjustX(const QRectF & r, const QRectF & all, bool keepsize = true);
	QRectF RectAdjustY(const QRectF & r, const QRectF & all, bool keepsize = true);

	// 扩展矩形范围
	QRectF RectExpand(const QRectF & r, double v);
	QRectF RectExpandX(const QRectF & r, double v);
	QRectF RectExpandY(const QRectF & r, double v);

	// 移动矩形.
	QRectF RectMoveX(const QRectF & r, double v);
	QRectF RectMoveY(const QRectF & r, double v);

	// 反转矩形.
	QRectF RectFlipX(const QRectF & r);
	QRectF RectFlipY(const QRectF & r);

	/**
	 * 均分数值范围.
	 * @param rng 分割数据范围.
	 * @param countHint 分割数量建议
	 * @param ret 分割后的结果.
	 * @return 分割间距 step. 如果分割失败 step <= 0 && ret.empty() == true;
	 */
	double DivideRange(const std::pair<double, double> & rng, int countHint, std::vector<double> & ret);


	// 映射
	template <class T>
	T Map(T valFrom, const std::pair<T, T> & rngFrom, const std::pair<T, T> & rngTo)
	{
		return rngTo.first + (valFrom - rngFrom.first) * range_length(rngTo) / range_length(rngFrom);
	}

	// 映射
	QPointF Map(QPointF pa, QRectF ra, QRectF rb);
	QRectF Map(QRectF pa, QRectF ra, QRectF rb);
	
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

	
} // namespace tool

