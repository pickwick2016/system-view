#include "misc.h"
#include "reader.h"


namespace tool {

	std::tuple<int, int, int> colormap_rainbow(float v, float vmin, float vmax)
	{
		float red = 1, blue = 1, green = 1;

		float val = tool::clamp<float>(v, vmin, vmax);
		float dv = vmax - vmin;

		if (val < (vmin + 0.25 * dv)) {
			red = 0;
			green = 4 * (val - vmin) / dv;
		}
		else if (val < (vmin + 0.5 * dv)) {
			red = 0;
			blue = 1 + 4 * (vmin + 0.25 * dv - val) / dv;
		}
		else if (val < (vmin + 0.75 * dv)) {
			red = 4 * (val - vmin - 0.5 * dv) / dv;
			blue = 0;
		}
		else {
			green = 1 + 4 * (vmin + 0.75 * dv - val) / dv;
			blue = 0;
		}

		int r = tool::clamp<int>(red * 255, 0, 255);
		int g = tool::clamp<int>(green * 255, 0, 255);
		int b = tool::clamp<int>(blue * 255, 0, 255);

		return { r, g, b };
	}

	std::tuple<int, int, int> colormap_gray(float v, float vmin, float vmax)
	{
		float val = tool::clamp<float>(v, vmin, vmax);
		float rval = (val - vmin) / (vmax - vmin); // 归一化

		return std::make_tuple<int, int, int>(rval * 255, rval * 255, rval * 255);
	}

	std::tuple<int, int, int> colormap_other(float v, float vmin, float vmax)
	{
		float val = tool::clamp<float>(v, vmin, vmax);
		float t = (val - vmin) / (vmax - vmin) * 2 - 1; // 归一化

		float red = clamp<float>(2 - abs(2 * t - 2), 0, 1);
		float green = clamp<float>(2 - abs(2 * t), 0, 1);
		float blue = clamp<float>(2 - abs(2 * t + 2), 0, 1);

		return std::make_tuple<int, int, int>(red * 255, green * 255, blue * 255);
	}

	QRectF RectClip(const QRectF & r, const QRectF & all)
	{
		QRectF ret;
		ret.setLeft(std::max<double>(r.left(), all.left()));
		ret.setRight(std::min<double>(r.right(), all.right()));
		ret.setTop(std::max<double>(r.top(), all.top()));
		ret.setBottom(std::min<double>(r.bottom(), all.bottom()));
		return ret;
	}

	QRect RectClip(const QRect & r, const QRect & all)
	{
		QRect ret;
		ret.setLeft(std::min<int>(r.left(), all.left()));
		ret.setRight(std::max<int>(r.right(), all.right()));
		ret.setTop(std::min<int>(r.top(), all.top()));
		ret.setBottom(std::max<int>(r.bottom(), all.bottom()));
		return ret;
	}

	QRectF RectAdjust(const QRectF & r, const QRectF & all, bool keepsize)
	{
		QRectF ret = RectAdjustX(r, all, keepsize);
		ret = RectAdjustY(ret, all, keepsize);
		return ret;
	}

	QRectF RectAdjustX(const QRectF & r, const QRectF & all, bool keepsize)
	{
		auto x1x2 = range_adjust<double>(RectRangeX(r), RectRangeX(all), keepsize);
		return QRectF(QPointF(x1x2.first, r.top()), QPointF(x1x2.second, r.bottom()));
	}

	QRectF RectAdjustY(const QRectF & r, const QRectF & all, bool keepsize)
	{
		auto y1y2 = range_adjust<double>(RectRangeY(r), RectRangeY(all), keepsize);
		return QRectF(QPointF(r.left(), y1y2.first), QPointF(r.right(), y1y2.second));
	}

	QRectF RectExpand(const QRectF & r, double v)
	{
		QRectF ret = r;
		ret.setLeft(r.left() - v);
		ret.setRight(r.right() + v);
		ret.setTop(r.top() - v);
		ret.setBottom(r.bottom() + v);
		return ret;
	}

	QRectF RectExpandX(const QRectF & r, double v)
	{
		QRectF ret = r;
		ret.setLeft(r.left() - v);
		ret.setRight(r.right() + v);
		return ret;
	}

	QRectF RectExpandY(const QRectF & r, double v)
	{
		QRectF ret = r;
		ret.setTop(r.top() - v);
		ret.setBottom(r.bottom() + v);
		return ret;
	}

	QRectF RectMoveX(const QRectF & r, double v)
	{
		QRectF ret = r;
		ret.setLeft(r.left() + v);
		ret.setRight(r.right() + v);
		return ret;
	}

	QRectF RectMoveY(const QRectF & r, double v)
	{
		QRectF ret = r;
		ret.setTop(r.top() + v);
		ret.setBottom(r.bottom() + v);
		return ret;
	}

	double DivideRange(const std::pair<double, double> & rng, int countHint, std::vector<double> & ret)
	{
		ret.clear();
		
		if (tool::range_length(rng) <= 0) {
			return 0;
		}

		double len = range_length(rng);
		double delta = range_length(rng) / countHint;
		int k = std::floorf(std::log10f(delta));
		double step = std::powf(10, k);

		std::vector<double> steps = { step, step * 2, step * 5 };
		std::vector<int> counts;
		for (auto v : steps) {
			counts.push_back((int) std::abs(len / v - countHint));
		}

		int minIdx = std::distance(counts.begin(), std::min_element(counts.begin(), counts.end()));
		step = steps[minIdx];

		double current = round_down(rng.first, step);
		while (true) {
			if (current > rng.second)
				break;

			if (tool::range_contain(rng, current) >= 0) {
				ret.push_back(current);
			}
			current += step;
		}

		return step;
	}

	QRectF RectFlipY(const QRectF & r)
	{
		QRectF ret = r;
		ret.setTop(r.bottom());
		ret.setBottom(r.top());
		return ret;
	}

	QRectF RectFlipX(const QRectF & r)
	{
		QRectF ret = r;
		ret.setLeft(r.right());
		ret.setRight(r.left());
		return ret;
	}

	QPointF Map(QPointF pa, QRectF ra, QRectF rb)
	{
		double x = Map(pa.rx(), RectRangeX(ra), RectRangeX(rb));
		double y = Map(pa.ry(), RectRangeY(ra), RectRangeY(rb));

		return QPointF(x, y);
	}

	QRectF Map(QRectF pa, QRectF ra, QRectF rb)
	{
		QPointF tl = Map(pa.topLeft(), ra, rb);
		QPointF br = Map(pa.bottomRight(), ra, rb);

		return QRectF(tl, br);
	}

	double round_pow2(double value)
	{
		double v = (int) std::round(std::log2(value));
		return std::pow(2, v);
	}

} // namespace tool