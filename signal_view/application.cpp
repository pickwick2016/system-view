#include <assert.h>

#include "application.h"
#include "project.h"


Application * Application::instance()
{
	static std::shared_ptr<Application> theApp;

	if (!theApp.get()) {
		theApp.reset(new Application());
	}
	assert(theApp.get());

	return theApp.get();
}

Application::Application()
{
	m_project.reset(new Project());
}

bool Application::initialize()
{
	return true;
}


void Application::uninitialize()
{

}

Project * Application::project()
{
	assert(m_project);
	
	return m_project.get();
}