#include <QFileInfo>

#include "reader.h"

FileReader::FileReader(const std::string & filename, int datatype, double sampleRate)
{
	m_filename = filename;
	m_dataType = datatype;
	m_sampleRate = sampleRate;

	QFileInfo info(QString::fromStdString(filename));
	m_fileSize = info.size();
	
	m_file.open(m_filename.c_str(), std::ios::binary | std::ios::in);
	if (!m_file.is_open()) {
		m_fileSize = 0;
	}
}

FileReader::~FileReader()
{
	m_file.close();
}

unsigned int FileReader::read(void * data, unsigned int count, unsigned int pos)
{ 
	if (!m_file.is_open()) {
		return 0;
	}

	unsigned int totalCount = this->count();
	unsigned int itemSize = this->itemSize();

	int readCount = std::min<int>(count, totalCount - pos);
	if (readCount <= 0 || data == nullptr || itemSize == 0) {
		return 0;
	}

	int readPos = pos * itemSize;
	m_file.seekg(readPos);
	if (readPos != m_file.tellg()) {
		return 0;
	}

	m_file.read((char *) data, readCount * itemSize);

	unsigned int gcount = m_file.gcount() / itemSize * itemSize;
	return gcount;
}

double FileReader::sampleRate() 
{ 
	return m_sampleRate;
}

unsigned int FileReader::count()
{ 
	unsigned int itemsize = itemSize();
	return itemsize > 0 ? m_fileSize / itemSize() : 0;
}

int FileReader::type() 
{ 
	return m_dataType;
}

int FileReader::itemSize() 
{ 
	switch (m_dataType) {
	case Int8: 
		return sizeof(char);
	case Int16:
		return sizeof(short);
	case Int32: 
		return sizeof(int);
	case Real32:
		return sizeof(float);
	case Real64:
		return sizeof(double);
	default:
		return 0;
	}
}

