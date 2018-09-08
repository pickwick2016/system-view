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
 * ��Ŀ������.
 * 1��������Ŀ
 * 2��֪ͨ���ⲿ��Ϣ.
 */
class Project : public QObject
{
	Q_OBJECT

public:
	// ��Ŀ��ض���.
	enum Action
	{
		AddItem = 0, // ������Ŀ
		RemoveItem,  // �Ƴ���Ŀ
		UpdateItem,  // ��Ŀ���ݸ���
		ShowItem,    // Ҫ����ʾ��Ŀ
		SelectItem,  // ѡ����Ŀ
	};

public:
	Project();

signals:
	void projectChanged(unsigned int itemId, int action);

public:
	// ������Ŀ.
	unsigned int add(const std::string & filename, int sampleType, double sampleRate);
	
	// ������Ŀ.
	unsigned int add(ProjectItem * item);

	// �Ƴ���Ŀ.
	void remove(unsigned int itemId);

	// ��ʾ��Ŀ.
	void show(unsigned int itemId);

	// ѡ����Ŀ.
	void select(unsigned int itemId);

	// ��ǰѡ����Ŀ.
	unsigned int selection() { return m_selection; }

	// ������Ŀ.
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
 * ��Ŀ��Ŀ.
 */
class ProjectItem
{
public:
	ProjectItem();
	virtual ~ProjectItem() {}

public:
	// ��ĿΨһID.
	unsigned int id();

	// ��Ŀ����.
	virtual std::string name();

	// ��Ŀ��Ϣ.
	virtual std::string info(const std::string & key) { return ""; }

private:
	unsigned int m_id;
};

/**
 * �ź��ļ���Ŀ.
 * 1�������ļ���Ϣ.
 * 2�������ͱ����Ӧ�Ķ�ȡ��.
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
	// ��ȡ�ļ���Ϣ.
	const FileInfo & fileInfo() { return m_info; }
	
	// ��ȡ��ȡ��.
	std::shared_ptr<Reader> reader() { return m_reader; }

private:
	FileInfo m_info;
	std::shared_ptr<Reader> m_reader;
};

#endif //PROJECT_H