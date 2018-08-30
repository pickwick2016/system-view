#pragma once

#include <QDockWidget>
#include "ui_project_widget.h"

class ProjectWidget : public QDockWidget
{
	Q_OBJECT

public:
	ProjectWidget(QWidget *parent = Q_NULLPTR);
	~ProjectWidget();

public slots:
	void itemDoubleClicked(QListWidgetItem * item);
	void itemChanged(QListWidgetItem * item);

public:
	void onProject(unsigned int id, int action);

private:
	QListWidgetItem * find(unsigned int id);

private:
	Ui::ProjectWidget ui;

	std::map<QListWidgetItem *, unsigned int> m_itemIds;
};
