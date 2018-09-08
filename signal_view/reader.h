#pragma once

#include <fstream>
#include <string>

// ��������.
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

// ��ȡ�豸.
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

// �ļ�����Դ.
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
 * ���������ж�ȡ���ݣ���ת��Ϊreal64��������.
 * @param reader ��ȡ��
 * @param data ������������û���Ҫ��֤���������ã�ͨ���������� count * 2��
 * @param count ��ȡ����������
 * @param pos ��ȡλ��
 * @return ת���γɵ�real64�������������˫ͨ�����ݣ�Ӧ���� count * 2
 */
int readAsReal64(Reader * reader, double * data, unsigned int count, unsigned int pos);