#pragma once

#include <fstream>
#include <string>


enum DataType
{
	Unknown = 0,
	Int8,
	Int16,
	Int32,
	Real32,
	Real64,
};


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
};

// 文件数据源.
class FileReader : public Reader
{
public:
	FileReader(const std::string & filename, int datatype, double sampleRate = 1.0);
	virtual ~FileReader();

public:
	virtual unsigned int read(void * data, unsigned int count, unsigned int pos);
	virtual double sampleRate();
	virtual unsigned int count();
	virtual int type();
	virtual int itemSize();

private:
	std::string m_filename;
	double m_sampleRate;
	int m_dataType;

	std::ifstream m_file;
	
	unsigned int m_fileSize;
};