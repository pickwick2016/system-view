#ifndef PYTHON_SCRIPT_H
#define PYTHON_SCRIPT_H

#include <string>

/**
 * Python�ű�(�ļ�)ִ����.
 */
class Python
{
public:
	Python(const std::string & dir = "./script/");
	~Python();

public:
	bool run(const std::string & moduleName, const std::string & functionName);

private:
	bool m_initialized;

	std::string m_dir;

};

#endif //PYTHON_SCRIPT_H