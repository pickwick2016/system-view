#pragma once

#include <memory>
#include <vector>
#include <map>

#include <QtWidgets/QMainWindow>
#include "ui_main_window.h"

class ProjectItem;
class Project;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = Q_NULLPTR);
	virtual ~MainWindow();

public slots:
	void openFile();
	void closeFile();

	void subWindowDestroyed(QObject * obj);

public:
	void onProject(unsigned int id, int action);

protected:
	void showEvent(QShowEvent * evt);

private:
	QWidget * makeSubWidget(ProjectItem * item);
	
private:
	Ui::MainWindowClass ui;

	Project * m_project;
	std::map<unsigned int, QMdiSubWindow *> m_wndOfIds;
	//std::vector<QWidget *> m_widgets;

};
