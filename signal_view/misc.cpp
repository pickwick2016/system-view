#include <QFile>

#include "misc.h"

FileDescription::FileDescription(const std::string & filename, int datatype, double samplerate)
	: fileName(filename), dataType(datatype), sampleRate(samplerate)
{
}

bool FileDescription::isValid() const
{
	QFile file(QString::fromStdString(fileName));
	return file.exists() && (sampleRate > 0);
}