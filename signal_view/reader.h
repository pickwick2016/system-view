#ifndef READER_H
#define READER_H

#include <fstream>
#include <string>

#include "data_type.h"

// 读取设备.
class Reader
{
public:
	virtual ~Reader() {}

public:
	/** 读取数据.
	 * @param data 输出缓冲区，需要用户维护.
	 * @param count 需要数据数量.
	 * @param position 读取位置.
	 * @return 实际读取的数据数量. -1 表示操作失败.
	 */
	virtual unsigned int read(void * data, unsigned int count, unsigned int pos) { return 0; }

	// 数据量.
	virtual unsigned int count() { return 0; }

	// 数据元素类型.
	virtual int type() { return DataType::Unknown; }

	// 采样率.
	virtual double sampleRate() { return 1; }
		
	// 数据元素大小（字节数）.
	virtual int itemSize();
	
	// 通道数量（1:单通道，2:IQ通道）
	virtual int channel();

	// 最大有效频率值.
	virtual double maxFreq() { return (channel() == 2) ? sampleRate() : sampleRate() / 2; }
	
	// 最大时间值.
	virtual double maxTime();

public:
	/**
	 * 读取数据，并转换为real64数据类型.
	 * @param data 输出缓冲区，用户需要保证缓冲区可用（通常数量等于 count * 2）
	 * @param count 读取的数据数量
	 * @param position 读取位置
	 * @return 转换形成的real64数据数量，如果双通道数据，应等于 count * 2
	 */
	int readAsReal64(double * data, unsigned int count, unsigned int position);

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

#endif //READER_H