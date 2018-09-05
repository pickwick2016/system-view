#include <QFile>

#include "file_info.h"
#include "reader.h"

FileInfo::FileInfo(const std::string & filename, int datatype, double samplerate)
	: fileName(filename), dataType(datatype), sampleRate(samplerate)
{
}

bool FileInfo::isValid() const
{
	QFile file(QString::fromStdString(fileName));
	return file.exists() && (sampleRate > 0) && (dataType != DataType::Unknown);
}

std::shared_ptr<Reader> FileInfo::createReader()
{
	std::shared_ptr<Reader> ret;

	if (isValid()) {
		std::auto_ptr<FileReader> freader(new FileReader());
		if (freader->open(fileName, dataType, sampleRate)) {
			ret.reset(freader.release());
		}
	}

	return ret;
}