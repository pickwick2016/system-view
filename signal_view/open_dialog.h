#pragma once

#include <QDialog>

#include "ui_open_dialog.h"
#include "misc.h"

class OpenDialog : public QDialog
{
	Q_OBJECT

public:
	OpenDialog(QWidget *parent = Q_NULLPTR);
	~OpenDialog();

public:
	FileDescription fileDesc();

public slots:
	void openFile();

private:
	Ui::OpenDialog ui;

	FileDescription m_fileDesc;
};
