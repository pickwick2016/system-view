#pragma once

#include <string>

/**
 * �ļ�����.
 */
struct FileDescription
{
public:
	FileDescription(const std::string & filename = "", int datatype = 0, double samplerate = 1);

	bool isValid() const;

public:
	std::string fileName;
	double  sampleRate;
	int dataType;
};