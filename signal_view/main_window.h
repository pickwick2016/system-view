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

// 主窗口.
class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = Q_NULLPTR);
	virtual ~MainWindow();

public slots:
	// 打开文件(有无建议文件参数).
	void openFile(QString filename = "");
	
	// 关闭当前文件.
	void closeFile();

	// 处理MDI子窗口关闭信号.
	void subWindowDestroyed(QObject * obj);

	// 执行脚本命令.
	void doScript1();

	// 处理控件中当前位置改变信号.
	void positionMoved(QPointF pos);

	void toggleFreqView();
	void toggleWaveView();
	void toggleWaterfallView();

public:
	// 处理 Project 项目消息.
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
