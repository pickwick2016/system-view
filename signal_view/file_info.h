#ifndef FILE_INFO_H
#define FILE_INFO_H

#include <memory>
#include <string>

#include "data_type.h"
class Reader;

/**
* �ļ�����.
*/
struct FileInfo
{
public:
	FileInfo(const std::string & filename = "", int datatype = DataType::Int8, double samplerate = 1);

public:
	// �жϵ�ǰ�ļ���Ϣ�Ƿ���Ч.
	bool isValid() const;

	// �����ļ���Ϣ��������ȡ��.
	std::shared_ptr<Reader> createReader();

public:
	std::string fileName;
	double  sampleRate;
	int dataType;
};

#endif //FILE_INFO_H