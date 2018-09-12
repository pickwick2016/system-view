#include <assert.h>
#include <complex>

#include <QMdiSubWindow>
#include <QTreeWidget>
#include <QDropEvent> 
#include <QDragEnterEvent>
#include <QMimeData>
#include <QFileInfo>
#include <QLabel>
#include <QSplitter>
#include <QTextEdit>
#include <QStackedWidget>

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

	// 创建和设置项目.
	m_project = Application::instance()->project();
	connect(m_project, SIGNAL(projectChanged(unsigned int, int)), this, SLOT(projectChanged(unsigned int, int)));

	// 设置主窗口状态.
	setAcceptDrops(true);
	setDockNestingEnabled(true);

	// 设置状态栏.
	m_statusLabel = new QLabel();
	m_statusLabel->setMinimumWidth(180);
	m_statusLabel->setAlignment(Qt::AlignRight);
	ui.statusBar->addPermanentWidget(m_statusLabel);
	
	// 连接用户消息-槽.
	connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(openFile()));
	connect(ui.actionClose, SIGNAL(triggered()), this, SLOT(closeFile()));
	connect(ui.actionFreq, SIGNAL(triggered()), this, SLOT(toggleFreqView()));
	connect(ui.actionWave, SIGNAL(triggered()), this, SLOT(toggleWaveView()));
	connect(ui.actionWaterfall, SIGNAL(triggered()), this, SLOT(toggleWaterfallView()));
	connect(ui.actionScript1, SIGNAL(triggered()), this, SLOT(doScript1()));
	
	// 添加和设置“项目”窗口
	auto projectPane = new ProjectWidget(this);
	addDockWidget(Qt::LeftDockWidgetArea, projectPane);
	connect(ui.actionProjectPane, SIGNAL(triggered(bool)), projectPane, SLOT(setVisible(bool)));
	connect(projectPane, SIGNAL(visibilityChanged(bool)), ui.actionProjectPane, SLOT(setChecked(bool)));
	connect(m_project, SIGNAL(projectChanged(unsigned int, int)), projectPane, SLOT(projectChanged(unsigned int, int)));

	// 测试部分代码.
#ifdef _DEBUG
	m_project->add("c:/data/bingle.dat", DataType::Real32_2, 100);
#endif //_DEBUG
}

MainWindow::~MainWindow()
{
	assert(m_project);
}

void MainWindow::openFile(QString filename)
{
	assert(m_project);

	OpenDialog dialog;
	dialog.initContent(filename);
	if (dialog.exec()) {
		auto fileInfo = dialog.fileInfo();
		if (fileInfo.isValid()) {
			m_project->add(new SignalFileItem(fileInfo));
		}
	}
}

void MainWindow::closeFile()
{
	assert(m_project);

	unsigned int selectItem = m_project->selection();
	m_project->remove(selectItem);
}

void MainWindow::projectChanged(unsigned int itemId, int action)
{
	auto pitem = Application::instance()->project()->find(itemId);
	if (pitem == nullptr) {
		return;
	}
	
	if (action == Project::ShowItem) {
		if (m_wndOfIds.find(pitem->id()) == m_wndOfIds.end()) {
			auto widget = makeSubWidget(pitem);
			if (widget) {
				auto subWindow = ui.mdiArea->addSubWindow(widget);
				m_wndOfIds[pitem->id()] = subWindow;
				
				widget->setFocusPolicy(Qt::StrongFocus);
				subWindow->setAttribute(Qt::WA_DeleteOnClose);
				subWindow->setWindowTitle(QString::fromStdString(pitem->info("simple")));

				connect(subWindow, SIGNAL(destroyed(QObject *)), this, SLOT(subWindowDestroyed(QObject*)));
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
	if (fileitem != nullptr) {
		return makeSubWidgetForSignalFile(fileitem);
	}

	return nullptr;
}


QWidget * MainWindow::makeSubWidgetForSignalFile(SignalFileItem * fitem)
{
	assert(fitem != nullptr);

	// 整体分割结构.
	// 上半部，层叠结构 a) waterfall  b） wave
	// 下半部 freq

	QSplitter * splitterWnd = new QSplitter(Qt::Vertical, this);

	// 创建子窗口.
	QStackedWidget * stackWnd = new QStackedWidget();

	WaterfallWidget * waterfallWnd = new WaterfallWidget();
	waterfallWnd->setFocusPolicy(Qt::StrongFocus);
	waterfallWnd->load(fitem->reader());
	connect(waterfallWnd, SIGNAL(positionMoved(QPointF)), this, SLOT(positionMoved(QPointF)));
	connect(waterfallWnd, SIGNAL(viewChanged(QRectF)), fitem, SLOT(viewChanged(QRectF)));
	connect(fitem, SIGNAL(syncView(QRectF)), waterfallWnd, SLOT(syncView(QRectF)));

	WaveWidget * waveWnd = new WaveWidget();
	waveWnd->setFocusPolicy(Qt::StrongFocus);
	waveWnd->load(fitem->reader());
	connect(waveWnd, SIGNAL(viewChanged(QRectF)), fitem, SLOT(viewChanged(QRectF)));
	connect(fitem, SIGNAL(syncView(QRectF)), waveWnd, SLOT(syncView(QRectF)));

	FreqWidget * freqWnd = new FreqWidget();
	freqWnd->load(fitem->reader());
	//connect(freqWnd, SIGNAL(viewChanged(QRectF)), fitem, SLOT(viewChanged(QRectF)));
	connect(fitem, SIGNAL(syncView(QRectF)), freqWnd, SLOT(syncView(QRectF)));

	// 配置分割窗口.
	stackWnd->addWidget(waveWnd);
	stackWnd->addWidget(waterfallWnd);
	splitterWnd->insertWidget(0, stackWnd);
	splitterWnd->setStretchFactor(0, 2);

	splitterWnd->insertWidget(1, freqWnd);
	splitterWnd->setStretchFactor(1, 1);

	splitterWnd->setSizes(QList<int>() << 2 << 1);

	return splitterWnd;
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
	auto subWnd = ui.mdiArea->activeSubWindow();
	if (subWnd) {
		QSplitter * splitter = dynamic_cast<QSplitter *>(subWnd->widget());
		if (splitter && splitter->widget(1)) {
			bool vis = splitter->widget(1)->isVisible();
			splitter->widget(1)->setVisible(! vis);
		}
	}
}

void MainWindow::toggleWaveView()
{
	auto subWnd = ui.mdiArea->activeSubWindow();
	if (subWnd) {
		QSplitter * splitter = dynamic_cast<QSplitter *>(subWnd->widget());
		if (splitter && splitter->widget(0)) {
			QStackedWidget * stackWnd = dynamic_cast<QStackedWidget *>(splitter->widget(0));
			if (stackWnd) {
				stackWnd->setCurrentIndex(1);
			}
		}
	}
}

void MainWindow::toggleWaterfallView()
{
	auto subWnd = ui.mdiArea->activeSubWindow();
	if (subWnd) {
		QSplitter * splitter = dynamic_cast<QSplitter *>(subWnd->widget());
		if (splitter && splitter->widget(0)) {
			QStackedWidget * stackWnd = dynamic_cast<QStackedWidget *>(splitter->widget(0));
			if (stackWnd) {
				stackWnd->setCurrentIndex(0);
			}
		}
	}
}
