#include <QFileDialog> 

#include "open_dialog.h"

OpenDialog::OpenDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	ui.editRate->setValidator(new QDoubleValidator(ui.editRate));

	// Á¬½ÓÐÅºÅ-²Û
	connect(ui.buttonOpen, SIGNAL(clicked()), this, SLOT(openFile()));
}

OpenDialog::~OpenDialog()
{
}

void OpenDialog::initContent(QString filename)
{
	if (filename.isEmpty())
		return;

	QFileInfo info(filename);
	if (info.exists() && info.isFile()) {
		ui.editFilename->setText(filename);
	}
}

void OpenDialog::openFile()
{
	QString filename = QFileDialog::getOpenFileName(this);
	if (!filename.isEmpty()) {
		ui.editFilename->setText(filename);
		ui.editFilename->setToolTip(filename);
	}
}

FileInfo OpenDialog::fileInfo()
{
	try {
		FileInfo info;
		info.fileName = ui.editFilename->text().toStdString();
		info.sampleRate = ui.editRate->text().toDouble();
		info.dataType = ui.comboType->currentIndex();

		if (info.isValid()) {
			return info;
		}
		else {
			return FileInfo();
		}
	}
	catch (...) {
		return FileInfo();
	}
}
