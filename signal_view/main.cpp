#include "main_window.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;

#ifdef _DEBUG
	w.show();
#else
	w.showMaximized();
#endif //_DEBUG
	
	return a.exec();
}
