#pragma once

#include <memory>
#include <string>

class Project;

/**
 * 全局应用管理.
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