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

} // namespace tool