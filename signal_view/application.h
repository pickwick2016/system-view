#pragma once

#include <memory>

class Project;

/**
 * ȫ��Ӧ�ù���.
 */
class Application
{
public:
	static Application * instance();

protected:
	Application();

public:
	bool initialize();
	void uninitialize();

	Project * project();

private:
	std::shared_ptr<Project> m_project;
};