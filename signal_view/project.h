#pragma once

#include <string>
#include <vector>
#include <memory>
#include <boost/signals2.hpp>

#include "file_info.h"
#include "misc.h"

class ProjectItem;
class Reader;

/**
 * 项目管理器.
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
		SelectItem,
	};

public:
	Project();

public:
	unsigned int add(const std::string & filename, int sampleType, double sampleRate);
	unsigned int add(ProjectItem * item);

	void remove(unsigned int id);

	void show(unsigned int id);

	void select(unsigned int id);

	unsigned int selection() { return m_selection; }

	ProjectItem * find(unsigned int id);

private:
	void notify(unsigned int id, int action);

public:
	boost::signals2::signal<void(unsigned int /*id*/, int/*action*/)> m_signal;

private:
	std::vector<std::shared_ptr<ProjectItem>> m_items;
	unsigned int m_selection;	
};


/**
 * 项目条目.
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
 * 信号文件条目.
 */
class SignalFileItem : public ProjectItem
{
public:
	SignalFileItem();
	SignalFileItem(FileInfo info);

public:
	virtual std::string name();
	virtual std::string name2(const std::string & key);

public:
	FileInfo fileInfo() { return m_info; }
	std::shared_ptr<Reader> reader() { return m_reader; }

private:
	FileInfo m_info;
	std::shared_ptr<Reader> m_reader;
};