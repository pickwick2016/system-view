#include <QFile>

#include "misc.h"
#include "reader.h"

FileDescription::FileDescription(const std::string & filename, int datatype, double samplerate)
	: fileName(filename), dataType(datatype), sampleRate(samplerate)
{
}

bool FileDescription::isValid() const
{
	QFile file(QString::fromStdString(fileName));
	return file.exists() && (sampleRate > 0) && (dataType != DataType::Unknown);
}

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

		return { rval * 255, rval * 255, rval * 255 };
	}

	std::tuple<int, int, int> colormap_other(float v, float vmin, float vmax)
	{
		float val = tool::clamp<float>(v, vmin, vmax);
		float t = (val - vmin) / (vmax - vmin) * 2 - 1; // 归一化

		float red = clamp<float>(2 - abs(2 * t - 2), 0, 1);
		float green = clamp<float>(2 - abs(2 * t), 0, 1);
		float blue = clamp<float>(2 - abs(2 * t + 2), 0, 1);

		return { red * 255, green * 255, blue * 255 };
	}

	QRectF clip(QRectF r, QRectF all)
	{
		QRectF ret;
		ret.setLeft(std::max<double>(r.left(), all.left()));
		ret.setRight(std::min<double>(r.right(), all.right()));
		ret.setTop(std::max<double>(r.top(), all.top()));
		ret.setBottom(std::min<double>(r.bottom(), all.bottom()));
		return ret;
	}

	QRect clip(QRect r, QRect all)
	{
		QRect ret;
		ret.setLeft(std::min<int>(r.left(), all.left()));
		ret.setRight(std::max<int>(r.right(), all.right()));
		ret.setTop(std::min<int>(r.top(), all.top()));
		ret.setBottom(std::max<int>(r.bottom(), all.bottom()));
		return ret;
	}

	QRectF adjust(QRectF r, QRectF all, bool keepsize)
	{
		auto old = r;

		auto x1x2 = range_adjust<double>({ r.left(), r.right() }, { all.left(), all.right() }, keepsize);
		auto y1y2 = range_adjust<double>({ r.top(), r.bottom() }, { all.top(), all.bottom() }, keepsize);

		return QRectF(QPointF(x1x2.first, y1y2.first), QPointF(x1x2.second, y1y2.second));
	}

	QRectF rectExpand(QRectF r, double v)
	{
		QRectF ret = r;
		ret.setLeft(r.left() - v);
		ret.setRight(r.right() + v);
		ret.setTop(r.top() - v);
		ret.setBottom(r.bottom() + v);
		return ret;
	}

	QRectF rectExpandX(QRectF r, double v)
	{
		QRectF ret = r;
		ret.setLeft(r.left() - v);
		ret.setRight(r.right() + v);
		return ret;
	}

	QRectF rectExpandY(QRectF r, double v)
	{
		QRectF ret = r;
		ret.setTop(r.top() - v);
		ret.setBottom(r.bottom() + v);
		return ret;
	}

	QRectF rectMoveX(QRectF r, double v)
	{
		QRectF ret = r;
		ret.setLeft(r.left() + v);
		ret.setRight(r.right() + v);
		return ret;
	}

	QRectF rectMoveY(QRectF r, double v)
	{
		QRectF ret = r;
		ret.setTop(r.top() + v);
		ret.setBottom(r.bottom() + v);
		return ret;
	}
	
	double range_split(std::pair<double, double> rng, int countHint, std::vector<double> & ret)
	{
		ret.clear();
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

	QRectF rectFlipY(QRectF r)
	{
		QRectF ret = r;
		ret.setTop(r.bottom());
		ret.setBottom(r.top());
		return ret;
	}

	QPointF map(QPointF pa, QRectF ra, QRectF rb)
	{
		double x = map(pa.rx(), { ra.left(), ra.right() }, { rb.left(), rb.right() });
		double y = map(pa.ry(), { ra.top(), ra.bottom() }, { rb.top(), rb.bottom() });

		return QPointF(x, y);
	}

	QRectF map(QRectF pa, QRectF ra, QRectF rb)
	{
		QPointF tl = map(pa.topLeft(), ra, rb);
		QPointF br = map(pa.bottomRight(), ra, rb);

		return QRectF(tl, br);
	}

} // namespace tool