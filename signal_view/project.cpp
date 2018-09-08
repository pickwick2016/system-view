#include <algorithm>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

#include <QFileInfo>

#include "project.h"

static unsigned int PROJECT_ITEM_ID_COUNTER = 1;

ProjectItem::ProjectItem() : m_id(PROJECT_ITEM_ID_COUNTER++)
{
}

unsigned int ProjectItem::id()
{
	return m_id;
}

Project::Project()
{
	m_selection = 0;
}

unsigned int Project::add(ProjectItem * item)
{
	if (item != nullptr && find(item->id()) == nullptr) {
		m_items.push_back(std::shared_ptr<ProjectItem>(item));

		notify(item->id(), (int) Action::AddItem);
		return item->id();
	}

	return 0;
}

unsigned int Project::add(const std::string & filename, int sampleType, double sampleRate)
{
	FileInfo desc;
	desc.fileName = filename;
	desc.dataType = sampleType;
	desc.sampleRate = sampleRate;

	if (desc.isValid()) {
		ProjectItem * item = new SignalFileItem(desc);
		return add(item);
	}
	
	return 0;
}

void Project::remove(unsigned int id)
{
	if (find(id) != nullptr) {
		notify(id, (int) Action::RemoveItem);

		auto fit = std::remove_if(m_items.begin(), m_items.end(),
			[=](auto & it) { return it->id() == id; });
		m_items.erase(fit, m_items.end());
	}
}

void Project::show(unsigned int id)
{
	if (find(id) != nullptr) {
		notify(id, (int)Action::ShowItem);
	}
}

ProjectItem * Project::find(unsigned int id)
{
	auto fit = std::find_if(m_items.begin(), m_items.end(), 
		[=](auto & it) { return it->id() == id; });

	if (fit != m_items.end()) {
		return (* fit).get();
	}

	return nullptr;
}

std::string ProjectItem::name()
{
	std::string ret = (boost::format("project_item_%d") % id()).str();
	return ret;
}

void Project::select(unsigned int id)
{
	unsigned int old = m_selection;
	if (id == 0 || find(id) != nullptr) {
		m_selection = id;
	}

	if (old != m_selection) {
		notify(m_selection, SelectItem);
	}
}

void Project::notify(unsigned int id, int action)
{
	//m_signal(id, action);

	emit projectChanged(id, action);
}

SignalFileItem::SignalFileItem()
{
}

SignalFileItem::SignalFileItem(const FileInfo & info)
	: m_info(info)
{
	m_reader = m_info.createReader();
}

std::string SignalFileItem::name()
{
	assert(m_info.isValid());

	return m_info.fileName;
}

std::string SignalFileItem::info(const std::string & key)
{
	if (boost::algorithm::iequals(key, "hint")) {
		std::string ret;
		ret = (boost::format("path : %s\nsps : %f") % m_info.fileName % m_info.sampleRate).str();
		return ret;
	}

	if (boost::algorithm::iequals(key, "simple")) {
		QFileInfo info(QString::fromStdString(m_info.fileName));
		return info.fileName().toStdString();
	}

	return "";
}

