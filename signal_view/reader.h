#pragma once

#include <fstream>
#include <string>

// 数据类型.
enum DataType
{
	Unknown = -1,
	Int8,
	Int16,
	Int32,
	Real32,
	Real64,
	Int8_2,
	Int16_2,
	Int32_2,
	Real32_2,
	Real64_2,
};

int channelCount(DataType datatype);

// 读取设备.
class Reader
{
public:
	virtual ~Reader() {}

public:
	virtual unsigned int read(void * data, unsigned int count, unsigned int pos) { return 0; }
	virtual double sampleRate() { return 1; }
	virtual unsigned int count() { return 0; }
	virtual int type() { return DataType::Real32; }
	virtual int itemSize() { return sizeof(float); }
	virtual int channel() { return 1; }
	virtual double maxFreq() { return (channel() == 2) ? sampleRate() : sampleRate() / 2; }
	virtual double maxTime() { return count() / sampleRate(); }
};

// 文件数据源.
class FileReader : public Reader
{
public:
	FileReader();
	FileReader(const std::string & filename, int datatype, double sampleRate = 1.0);
	virtual ~FileReader();

public:
	virtual unsigned int read(void * data, unsigned int count, unsigned int pos);
	virtual double sampleRate();
	virtual unsigned int count();
	virtual int type();
	virtual int itemSize();
	virtual int channel();

public:
	bool open(const std::string & filename, int datatype, double sampleRate = 1.0);
	void close();

private:
	std::string m_filename;
	double m_sampleRate;
	int m_dataType;

	std::ifstream m_file;
	
	unsigned int m_fileSize;
};


/**
 * 从数据器中读取数据，并转换为real64数据类型.
 * @param reader 读取器
 * @param data 输出缓冲区，用户需要保证缓冲区可用（通常数量等于 count * 2）
 * @param count 读取的数据数量
 * @param pos 读取位置
 * @return 转换形成的real64数据数量，如果双通道数据，应等于 count * 2
 */
int readAsReal64(Reader * reader, double * data, unsigned int count, unsigned int pos);