#ifndef READER_H
#define READER_H

#include <fstream>
#include <string>

#include "data_type.h"

// ��ȡ�豸.
class Reader
{
public:
	virtual ~Reader() {}

public:
	/** ��ȡ����.
	 * @param data �������������Ҫ�û�ά��.
	 * @param count ��Ҫ��������.
	 * @param position ��ȡλ��.
	 * @return ʵ�ʶ�ȡ����������. -1 ��ʾ����ʧ��.
	 */
	virtual unsigned int read(void * data, unsigned int count, unsigned int pos) { return 0; }

	// ������.
	virtual unsigned int count() { return 0; }

	// ����Ԫ������.
	virtual int type() { return DataType::Unknown; }

	// ������.
	virtual double sampleRate() { return 1; }
		
	// ����Ԫ�ش�С���ֽ�����.
	virtual int itemSize();
	
	// ͨ��������1:��ͨ����2:IQͨ����
	virtual int channel();

	// �����ЧƵ��ֵ.
	virtual double maxFreq() { return (channel() == 2) ? sampleRate() : sampleRate() / 2; }
	
	// ���ʱ��ֵ.
	virtual double maxTime();

public:
	/**
	 * ��ȡ���ݣ���ת��Ϊreal64��������.
	 * @param data ������������û���Ҫ��֤���������ã�ͨ���������� count * 2��
	 * @param count ��ȡ����������
	 * @param position ��ȡλ��
	 * @return ת���γɵ�real64�������������˫ͨ�����ݣ�Ӧ���� count * 2
	 */
	int readAsReal64(double * data, unsigned int count, unsigned int position);

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