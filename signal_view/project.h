#pragma once

#include <string>
#include <vector>
#include <memory>
#include <boost/signals2.hpp>

#include "misc.h"

class ProjectItem;

/**
 * ��Ŀ������.
 */
class Project
{
public:
	enum Action
	{
		AddItem = 0,
		RemoveItem,
		UpdateItem,
		ShowItem,
	};

public:
	unsigned int add(const std::string & filename, int sampleType, double sampleRate);
	unsigned int add(ProjectItem * item);

	void remove(unsigned int id);

	void show(unsigned int id);

	ProjectItem * find(unsigned int id);

private:
	void notify(unsigned int id, int action);


public:
	boost::signals2::signal<void(unsigned int /*id*/, int/*action*/)> m_signal;

private:
	std::vector<std::shared_ptr<ProjectItem>> m_items;
	
};


/**
 * ��Ŀ��Ŀ.
 */
class ProjectItem
{
public:
	ProjectItem();
	virtual ~ProjectItem() {}

public:
	unsigned int id();

	virtual std::string name();
	virtual std::string name2(const std::string & key) { return ""; }

private:
	unsigned int m_id;
};

/**
 * �ź��ļ���Ŀ.
 */
class SignalFileItem : public ProjectItem
{
public:
	SignalFileItem();
	SignalFileItem(FileDescription desc);

public:
	virtual std::string name();
	virtual std::string name2(const std::string & key);

public:
	FileDescription desc() { return m_desc; }

private:
	FileDescription m_desc;
};