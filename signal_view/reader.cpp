#include <QFileInfo>

#include "reader.h"
#include "misc.h"

int channelCount(DataType datatype)
{
	switch (datatype) {
	case Int8:
	case Int16:
	case Int32:
	case Real32:
	case Real64:
		return 1;

	case Int8_2:
	case Int16_2:
	case Int32_2:
	case Real32_2:
	case Real64_2:
		return 2;
	
	default:
		return 0;
	}
}

int convertToReal64(void * input, double * output, unsigned int count, int type)
{
	int count2 = count * channelCount((DataType) type);

	switch (type) {
	case DataType::Int8:
	case DataType::Int8_2:
		std::copy((char *)input, (char *)input + count2, output);
		break;

	case DataType::Int16:
	case DataType::Int16_2:
		std::copy((short *)input, (short *)input + count2, output);
		break;

	case DataType::Int32:
	case DataType::Int32_2:
		std::copy((int *)input, (int *)input + count2, output);
		break;

	case DataType::Real32:
	case DataType::Real32_2:
		std::copy((float *)input, (float *)input + count2, output);
		break;

	case DataType::Real64:
	case DataType::Real64_2:
		std::copy((double *)input, (double *)input + count2, output);
		break;

	default:
		return -1;
	}

	return count2;
}

int readAsReal64(Reader * reader, double * data, unsigned int count, unsigned int pos)
{
	if (!reader || !data) {
		return -1;
	}
	
	std::vector<char> buffer(sizeof(double) * count * 2);
	int readCount = reader->read(buffer.data(), count, pos);
	if (readCount <= 0) {
		return 0;
	}

	int ret = convertToReal64(buffer.data(), data, readCount, reader->type());
	return ret;
}

FileReader::FileReader()
{
	m_filename = "";
	m_dataType = DataType::Unknown;
	m_sampleRate = 1;

	m_fileSize = 0;
}

FileReader::FileReader(const std::string & filename, int datatype, double sampleRate)
{
	open(filename, datatype, sampleRate);
}

FileReader::~FileReader()
{
	m_file.close();
}

bool FileReader::open(const std::string & filename, int datatype, double sampleRate)
{
	m_filename = filename;
	m_dataType = datatype;
	m_sampleRate = sampleRate;

	QFileInfo info(QString::fromStdString(filename));
	m_fileSize = info.size();

	if (m_file.is_open()) {
		m_file.close();
	}

	m_file.open(m_filename.c_str(), std::ios::binary | std::ios::in);

	if (!m_file.is_open()) {
		m_fileSize = 0;
	}

	return m_file.is_open();
}

void FileReader::close()
{
	m_filename = "";
	m_dataType = DataType::Unknown;
	m_sampleRate = 1;

	m_file.close();
	m_fileSize = 0;
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

	int getCount = tool::round_down((int)m_file.gcount(), (int) itemSize);
	return getCount / itemSize;
}

double FileReader::sampleRate() 
{ 
	return m_sampleRate;
}

unsigned int FileReader::count()
{ 
	unsigned int itemsize = itemSize();
	return (itemsize > 0) ? m_fileSize / itemSize() : 0;
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
	case Int8_2:
		return sizeof(char) * 2;
	case Int16_2:
		return sizeof(short) * 2;
	case Int32_2:
		return sizeof(int) * 2;
	case Real32_2:
		return sizeof(float) * 2;
	case Real64_2:
		return sizeof(double) * 2;
	default:
		return 0;
	}
}

int FileReader::channel()
{
	switch (m_dataType) {
	case Int8:
	case Int16:
	case Int32:
	case Real32:
	case Real64:
		return 1;

	case Int8_2:
	case Int16_2:
	case Int32_2:
	case Real32_2:
	case Real64_2:
		return 2;

	default:
		return 0;
	}
}

