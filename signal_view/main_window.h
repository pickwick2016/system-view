#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <memory>
#include <vector>
#include <map>

#include <QLabel>
#include <QtWidgets/QMainWindow>
#include "ui_main_window.h"

class ProjectItem;
class SignalFileItem;
class Project;

// ������.
class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = Q_NULLPTR);
	virtual ~MainWindow();

public slots:
	// ���ļ�(���޽����ļ�����).
	void openFile(QString filename = "");
	
	// �رյ�ǰ�ļ�.
	void closeFile();

	// ����MDI�Ӵ��ڹر��ź�.
	void subWindowDestroyed(QObject * obj);

	// ִ�нű�����.
	void doScript1();

	// ����ؼ��е�ǰλ�øı��ź�.
	void positionMoved(QPointF pos);

	void toggleFreqView();
	void toggleWaveView();
	void toggleWaterfallView();

public:
	// ���� Project ��Ŀ��Ϣ.
	void onProject(unsigned int id, int action);

protected:
	void showEvent(QShowEvent * evt);
	void dragEnterEvent(QDragEnterEvent *evt);
	void dropEvent(QDropEvent *evt);

private:
	QWidget * makeSubWidget(ProjectItem * item);
	QWidget * makeSubWidgetForSignalFile(SignalFileItem * fitem);
	
private:
	Ui::MainWindowClass ui;

	Project * m_project;
	std::map<unsigned int, QMdiSubWindow *> m_wndOfIds;
	//std::vector<QWidget *> m_widgets;
	QLabel * m_statusLabel;

};

#endif //MAIN_WINDOW_H
