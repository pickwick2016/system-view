#include <assert.h>
#include <boost/bind.hpp>

#include <QMdiSubWindow>
#include <QTreeWidget>
#include <QDropEvent> 
#include <QDragEnterEvent>
#include <QMimeData>
#include <QFileInfo>
#include <QLabel>

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

	setAcceptDrops(true);

	m_statusLabel = new QLabel();
	m_statusLabel->setMinimumWidth(180);
	m_statusLabel->setAlignment(Qt::AlignRight);
	ui.statusBar->addPermanentWidget(m_statusLabel);


	m_project = Application::instance()->project();

	// 连接用户消息-槽.
	connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(openFile()));
	connect(ui.actionClose, SIGNAL(triggered()), this, SLOT(closeFile()));
	m_project->m_signal.connect(boost::bind(&MainWindow::onProject, this, _1, _2));

	// 添加“项目”窗口
	auto projectPane = new ProjectWidget(this);
	addDockWidget(Qt::LeftDockWidgetArea, projectPane);

	connect(ui.actionProjectPane, SIGNAL(toggled(bool)), projectPane, SLOT(setVisible(bool)));
	connect(projectPane, SIGNAL(visibilityChanged(bool)), ui.actionProjectPane, SLOT(setChecked(bool)));
	Application::instance()->project()->m_signal.connect(boost::bind(&ProjectWidget::onProject, projectPane, _1, _2));

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
	m_project->add("c:/data/bingle.dat", DataType::Real32, 1);
#endif //_DEBUG
}

void MainWindow::openFile(QString filename)
{
	assert(m_project);

	OpenDialog dialog;
	dialog.initFile(filename);
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
	unsigned int sel = m_project->selection();
	m_project->remove(sel);
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
				widget->setFocusPolicy(Qt::StrongFocus);

				m_wndOfIds[pitem->id()] = subWindow;

				connect(subWindow, SIGNAL(destroyed(QObject *)), this, SLOT(subWindowDestroyed(QObject*)));

				subWindow->setAttribute(Qt::WA_DeleteOnClose);
				subWindow->setWindowTitle(QString::fromStdString(pitem->name2("simple")));
				//subWindow->setToolTip(QString::fromStdString(pitem->name2("hint")));
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

		connect(widget.get(), SIGNAL(positionMoved(QPointF)), this, SLOT(positionMoved(QPointF)));

		auto desc = fileitem->desc();
		if (widget->load(QString::fromStdString(desc.fileName), desc.dataType, desc.sampleRate)) {
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

void MainWindow::dragEnterEvent(QDragEnterEvent *evt)
{
	if (evt->mimeData()->hasFormat("text/uri-list")) {
		evt->acceptProposedAction();
	}
	else {
		evt->ignore();
	}
}

void MainWindow::dropEvent(QDropEvent *evt)
{
	QList<QUrl> urls = evt->mimeData()->urls();
	if (!urls.empty()) {
		QString fileName = urls.at(0).toLocalFile();
		QFileInfo info(fileName);
		if (info.isFile() && info.exists()) {
			openFile(fileName);
		}
	}

	//foreach(QUrl url, urls) {
	//	QString file_name = url.toLocalFile();
	//	//textEdit->append(file_name);
	//}
}

void MainWindow::positionMoved(QPointF pos)
{
	QString str = QString::asprintf("%f : %f", pos.rx(), pos.ry());
	m_statusLabel->setText(str);
}