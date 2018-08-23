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

void OpenDialog::openFile()
{
	QString filename = QFileDialog::getOpenFileName(this);
	if (!filename.isEmpty()) {
		ui.editFilename->setText(filename);
		ui.editFilename->setToolTip(filename);
	}
}

FileDescription OpenDialog::fileDesc()
{
	try {
		FileDescription desc;
		desc.fileName = ui.editFilename->text().toStdString();
		desc.sampleRate = ui.editRate->text().toDouble();
		desc.dataType = ui.comboType->currentIndex();

		if (desc.isValid()) {
			return desc;
		}
		else {
			return FileDescription();
		}
	}
	catch (...) {
		return FileDescription();
	}
}
