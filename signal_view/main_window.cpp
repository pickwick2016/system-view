#include <assert.h>
#include <boost/bind.hpp>

#include <QMdiSubWindow>
#include <QTreeWidget>
#include <QDropEvent> 
#include <QDragEnterEvent>
#include <QMimeData>
#include <QFileInfo>
#include <QLabel>
#include <QSplitter>
#include <QTextEdit>
#include <complex>

#include "main_window.h"
#include "open_dialog.h"
#include "project.h"
#include "project_widget.h"
#include "application.h"
#include "waterfall_widget.h"
#include "reader.h"
#include "python_script.h"
#include "freq_widget.h"
#include "wave_widget.h"
#include "misc.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	setAcceptDrops(true);

	m_statusLabel = new QLabel();
	m_statusLabel->setMinimumWidth(180);
	m_statusLabel->setAlignment(Qt::AlignRight);
	ui.statusBar->addPermanentWidget(m_statusLabel);

	//setDockNestingEnabled(true);
	//setDockOptions(QMainWindow::ForceTabbedDocks);

	m_project = Application::instance()->project();

	// 连接用户消息-槽.
	connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(openFile()));
	connect(ui.actionClose, SIGNAL(triggered()), this, SLOT(closeFile()));

	connect(ui.actionFreq, SIGNAL(triggered()), this, SLOT(toggleFreqView()));
	connect(ui.actionWave, SIGNAL(triggered()), this, SLOT(toggleWaveView()));
	connect(ui.actionWaterfall, SIGNAL(triggered()), this, SLOT(toggleWaterfallView()));
	
	connect(ui.actionScript1, SIGNAL(triggered()), this, SLOT(doScript1()));

	m_project->m_signal.connect(boost::bind(&MainWindow::onProject, this, _1, _2));

	//// 添加“项目”窗口
	auto projectPane = new ProjectWidget(this);
	addDockWidget(Qt::LeftDockWidgetArea, projectPane);
	//
	connect(ui.actionProjectPane, SIGNAL(triggered(bool)), projectPane, SLOT(setVisible(bool)));
	connect(projectPane, SIGNAL(visibilityChanged(bool)), ui.actionProjectPane, SLOT(setChecked(bool)));

	Application::instance()->project()->m_signal.connect(boost::bind(&ProjectWidget::onProject, projectPane, _1, _2));


#ifdef _DEBUG
	m_project->add("c:/data/bingle.dat", DataType::Real32, 1);
#endif //_DEBUG
}

MainWindow::~MainWindow()
{
	assert(m_project);

	m_project->m_signal.disconnect_all_slots();
}

void MainWindow::showEvent(QShowEvent * evt)
{

}

void MainWindow::openFile(QString filename)
{
	assert(m_project);

	OpenDialog dialog;
	dialog.initContent(filename);
	if (dialog.exec()) {
		// 添加文件.
		auto desc = dialog.fileInfo();
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
				subWindow->setWindowTitle(QString::fromStdString(pitem->info("simple")));
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
		return makeSubWidgetForSignalFile(fileitem);
	}

	return nullptr;
}


QWidget * MainWindow::makeSubWidgetForSignalFile(SignalFileItem * fitem)
{
	assert(fitem != nullptr);

	QSplitter * splitter = new QSplitter(Qt::Vertical, this);

	WaterfallWidget * widget = new WaterfallWidget();
	connect(widget, SIGNAL(positionMoved(QPointF)), this, SLOT(positionMoved(QPointF)));
	widget->load(fitem->reader());

	splitter->insertWidget(0, widget);
	splitter->setStretchFactor(0, 2);

	//FreqWidget * widget2 = new FreqWidget();
	WaveWidget * widget2 = new WaveWidget();
	widget2->load(fitem->reader());
	splitter->insertWidget(1, widget2);
	splitter->setStretchFactor(1, 1);

	widget->setFocusPolicy(Qt::StrongFocus);
	connect(widget, SIGNAL(visibleChanged(QRectF)), widget2, SLOT(visibleChanged(QRectF)));

	splitter->setSizes(QList<int>() << 2 << 1);

	return splitter;
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
}

void MainWindow::positionMoved(QPointF pos)
{
	QString str = QString::asprintf("%f : %f", pos.rx(), pos.ry());
	m_statusLabel->setText(str);
}

void MainWindow::doScript1()
{
	Python python;
	bool result = python.run("test_plugin_01", "process");
	if (result) {

	}
 }

void MainWindow::toggleFreqView()
{

}

void MainWindow::toggleWaveView()
{

}

void MainWindow::toggleWaterfallView()
{

}
