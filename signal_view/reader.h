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