#include "project_widget.h"
#include "project.h"
#include "application.h"

ProjectWidget::ProjectWidget(QWidget *parent)
	: QDockWidget(parent)
{
	ui.setupUi(this);

	connect(ui.listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem *)), 
		this, SLOT(itemDoubleClicked(QListWidgetItem *)));

	connect(ui.listWidget, SIGNAL(itemChanged(QListWidgetItem *)),
		this, SLOT(itemChanged(QListWidgetItem *)));
}

ProjectWidget::~ProjectWidget()
{

}

void ProjectWidget::projectChanged(unsigned int id, int action)
{
	auto pitem = Application::instance()->project()->find(id);
	if (pitem == nullptr) 
		return;

	if (action == Project::AddItem) {
		QString name = QString::fromStdString(pitem->name());
		QListWidgetItem * litem = new QListWidgetItem(name, ui.listWidget);
		ui.listWidget->addItem(litem);

		m_itemIds[litem] = pitem->id();

		QString hint = QString::fromStdString(pitem->info("hint"));
		if (!hint.isEmpty()) {
			litem->setToolTip(hint);
		}
	}

	if (action == Project::RemoveItem) {
		QListWidgetItem * litem = find(id);
		if (litem) {
			int r = ui.listWidget->row(litem);
			delete ui.listWidget->takeItem(r);

			m_itemIds.erase(litem);
		}
	}
}

void ProjectWidget::itemDoubleClicked(QListWidgetItem *item)
{
	if (item == nullptr)
		return;

	if (m_itemIds.find(item) != m_itemIds.end()) {
		Application::instance()->project()->show(m_itemIds[item]);
	}
}

void ProjectWidget::itemChanged(QListWidgetItem * item)
{
	if (m_itemIds.find(item) != m_itemIds.end()) {
		Application::instance()->project()->select(m_itemIds[item]);
	}
}

QListWidgetItem * ProjectWidget::find(unsigned int id)
{
	for (auto it : m_itemIds) {
		if (it.second == id) {
			return it.first;
		}
	}

	return nullptr;
}