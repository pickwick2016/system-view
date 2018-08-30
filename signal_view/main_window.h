#pragma once

#include <memory>
#include <vector>
#include <map>

#include <QLabel>
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
	void openFile(QString filename = "");
	void closeFile();

	void subWindowDestroyed(QObject * obj);

	void positionMoved(QPointF pos);

public:
	void onProject(unsigned int id, int action);

protected:
	void showEvent(QShowEvent * evt);
	void dragEnterEvent(QDragEnterEvent *evt);
	void dropEvent(QDropEvent *evt);

private:
	QWidget * makeSubWidget(ProjectItem * item);
	
private:
	Ui::MainWindowClass ui;

	Project * m_project;
	std::map<unsigned int, QMdiSubWindow *> m_wndOfIds;
	//std::vector<QWidget *> m_widgets;
	QLabel * m_statusLabel;

};
