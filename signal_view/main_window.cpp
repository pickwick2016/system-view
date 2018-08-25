#include <assert.h>
#include <boost/bind.hpp>

#include <QMdiSubWindow>
#include <QTreeWidget>

#include "main_window.h"
#include "open_dialog.h"
#include "project.h"
#include "project_widget.h"
#include "application.h"
#include "waterfall_widget.h"
#include "reader.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	m_project = Application::instance()->project();

	// 连接用户消息-槽.
	connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(openFile()));
	m_project->m_signal.connect(boost::bind(&MainWindow::onProject, this, _1, _2));

	// 添加“项目”窗口
	auto projectPane = new ProjectWidget(this);
	addDockWidget(Qt::LeftDockWidgetArea, projectPane);

	connect(ui.actionProjectPane, SIGNAL(toggled(bool)), projectPane, SLOT(setVisible(bool)));
	connect(projectPane, SIGNAL(visibilityChanged(bool)), ui.actionProjectPane, SLOT(setChecked(bool)));
	Application::instance()->project()->m_signal.connect(boost::bind(& ProjectWidget::onProject, projectPane, _1, _2));
	
}

MainWindow::~MainWindow()
{
	assert(m_project);

	m_project->m_signal.disconnect_all_slots();
}

void MainWindow::showEvent(QShowEvent * evt)
{
	assert(m_project);

#ifdef _DEBUG
	m_project->add("../data/tone_f5k_s32k.bin", DataType::Real32, 1);
#endif //_DEBUG
}

void MainWindow::openFile()
{
	assert(m_project);

	OpenDialog dialog;
	if (dialog.exec()) {
		// 添加文件.
		auto desc = dialog.fileDesc();
		if (desc.isValid()) {
			m_project->add(new SignalFileItem(desc));
		}
	}
}

void MainWindow::closeFile()
{

}

void MainWindow::onProject(unsigned int id, int action)
{
	auto pitem = Application::instance()->project()->find(id);
	if (pitem == nullptr)
		return;

	if (action == Project::ShowItem) {
		if (m_wndOfIds.find(pitem->id()) == m_wndOfIds.end()) {
			auto widget = makeSubWidget(pitem);
			if (widget) {
				auto subWindow = ui.mdiArea->addSubWindow(widget);
				m_wndOfIds[pitem->id()] = subWindow;

				connect(subWindow, SIGNAL(destroyed(QObject *)), this, SLOT(subWindowDestroyed(QObject*)));

				subWindow->setAttribute(Qt::WA_DeleteOnClose);
				subWindow->setWindowTitle(QString::fromStdString(pitem->name2("simple")));
				subWindow->setToolTip(QString::fromStdString(pitem->name2("hint")));
				subWindow->show();
			}
		}
		else {
			auto subWindow = m_wndOfIds[pitem->id()];
			ui.mdiArea->setActiveSubWindow(subWindow);
		}
	}

	if (action == Project::RemoveItem) {
		if (m_wndOfIds.find(pitem->id()) != m_wndOfIds.end()) {
			QMdiSubWindow * sub = m_wndOfIds[pitem->id()];
			ui.mdiArea->removeSubWindow(sub);

			m_wndOfIds.erase(pitem->id());
		}
	}
}

QWidget * MainWindow::makeSubWidget(ProjectItem * item)
{
	assert(item);

	SignalFileItem * fileitem = dynamic_cast<SignalFileItem *>(item);
	if (fileitem) {
		std::auto_ptr<WaterfallWidget> widget(new WaterfallWidget());

		auto desc = fileitem->desc();
		if (widget->load(QString::fromStdString(desc.fileName), desc.dataType, desc.sampleRate))	{
			return widget.release();
		}
	}

	return nullptr;
}

void MainWindow::subWindowDestroyed(QObject * obj)
{
	if (obj == nullptr)
		return;

	for (auto it = m_wndOfIds.begin(); it != m_wndOfIds.end(); it++) {
		if ((void *)it->second == (void *)obj) {
			m_wndOfIds.erase(it);
			return;
		}
	}
}