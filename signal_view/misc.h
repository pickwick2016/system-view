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

	// ��ֵ�ػ�.
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

	// ���η�Χ.
	inline std::pair<double, double> RectRangeX(const QRectF & r) { return { r.left(), r.right() }; }
	inline std::pair<int, int> RectRangeX(const QRect & r) { return { r.left(), r.right() }; }
	inline std::pair<double, double> RectRangeY(const QRectF & r) { return { r.top(), r.bottom() }; }
	inline std::pair<int, int> RectRangeY(const QRect & r) { return { r.top(), r.bottom() }; }

	// ���о���.
	QRectF RectClip(const QRectF & r, const QRectF & all);
	QRect RectClip(const QRect & r, const QRect & all);

	// �������ڣ����ƾ��Σ�.
	QRectF RectAdjust(const QRectF & r, const QRectF & all, bool keepsize = true);
	QRectF RectAdjustX(const QRectF & r, const QRectF & all, bool keepsize = true);
	QRectF RectAdjustY(const QRectF & r, const QRectF & all, bool keepsize = true);

	// ��չ���η�Χ
	QRectF RectExpand(const QRectF & r, double v);
	QRectF RectExpandX(const QRectF & r, double v);
	QRectF RectExpandY(const QRectF & r, double v);

	// �ƶ�����.
	QRectF RectMoveX(const QRectF & r, double v);
	QRectF RectMoveY(const QRectF & r, double v);

	// ��ת����.
	QRectF RectFlipX(const QRectF & r);
	QRectF RectFlipY(const QRectF & r);

	/**
	 * ������ֵ��Χ.
	 * @param rng �ָ����ݷ�Χ.
	 * @param countHint �ָ���������
	 * @param ret �ָ��Ľ��.
	 * @return �ָ��� step. ����ָ�ʧ�� step <= 0 && ret.empty() == true;
	 */
	double DivideRange(const std::pair<double, double> & rng, int countHint, std::vector<double> & ret);


	// ӳ��
	template <class T>
	T Map(T valFrom, const std::pair<T, T> & rngFrom, const std::pair<T, T> & rngTo)
	{
		return rngTo.first + (valFrom - rngFrom.first) * range_length(rngTo) / range_length(rngFrom);
	}

	// ӳ��
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

