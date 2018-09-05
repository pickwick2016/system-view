#pragma once

#include <memory>
#include <string>

class Reader;

/**
* ÎÄ¼şÃèÊö.
*/
struct FileInfo
{
public:
	FileInfo(const std::string & filename = "", int datatype = 0, double samplerate = 1);

	bool isValid() const;

public:
	std::shared_ptr<Reader> createReader();

public:
	std::string fileName;
	double  sampleRate;
	int dataType;
};