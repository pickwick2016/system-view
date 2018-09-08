#ifndef PROJECT_H
#define PROJECT_H

#include <string>
#include <vector>
#include <memory>

#include <QObject>

#include "file_info.h"
#include "misc.h"

class ProjectItem;
class Reader;

/**
 * 项目管理器.
 * 1、管理项目
 * 2、通知内外部消息.
 */
class Project : public QObject
{
	Q_OBJECT

public:
	// 项目相关动作.
	enum Action
	{
		AddItem = 0, // 增加条目
		RemoveItem,  // 移除条目
		UpdateItem,  // 条目内容更新
		ShowItem,    // 要求显示条目
		SelectItem,  // 选中条目
	};

public:
	Project();

signals:
	void projectChanged(unsigned int itemId, int action);

public:
	// 增加条目.
	unsigned int add(const std::string & filename, int sampleType, double sampleRate);
	
	// 增加条目.
	unsigned int add(ProjectItem * item);

	// 移除条目.
	void remove(unsigned int itemId);

	// 显示条目.
	void show(unsigned int itemId);

	// 选择条目.
	void select(unsigned int itemId);

	// 当前选中条目.
	unsigned int selection() { return m_selection; }

	// 查找条目.
	ProjectItem * find(unsigned int itemId);

private:
	void notify(unsigned int itemId, int action);

//public:
//	boost::signals2::signal<void(unsigned int /*itemId*/, int/*action*/)> m_signal;

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
	// 条目唯一ID.
	unsigned int id();

	// 条目名称.
	virtual std::string name();

	// 条目信息.
	virtual std::string info(const std::string & key) { return ""; }

private:
	unsigned int m_id;
};

/**
 * 信号文件条目.
 * 1、保存文件信息.
 * 2、创建和保存对应的读取器.
 */
class SignalFileItem : public ProjectItem
{
public:
	SignalFileItem();
	SignalFileItem(const FileInfo & info);

public:
	virtual std::string name();
	virtual std::string info(const std::string & key);

public:
	// 获取文件信息.
	const FileInfo & fileInfo() { return m_info; }
	
	// 获取读取器.
	std::shared_ptr<Reader> reader() { return m_reader; }

private:
	FileInfo m_info;
	std::shared_ptr<Reader> m_reader;
};

#endif //PROJECT_H