#ifndef FILE_INFO_H
#define FILE_INFO_H

#include <memory>
#include <string>

#include "data_type.h"
class Reader;

/**
* 文件描述.
*/
struct FileInfo
{
public:
	FileInfo(const std::string & filename = "", int datatype = DataType::Int8, double samplerate = 1);

public:
	// 判断当前文件信息是否有效.
	bool isValid() const;

	// 根据文件信息，创建读取器.
	std::shared_ptr<Reader> createReader();

public:
	std::string fileName;
	double  sampleRate;
	int dataType;
};

#endif //FILE_INFO_H