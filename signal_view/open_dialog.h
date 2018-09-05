#ifndef OPEN_DIALOG_H
#define OPEN_DIALOG_H

#include <QDialog>

#include "ui_open_dialog.h"
#include "misc.h"
#include "file_info.h"

class OpenDialog : public QDialog
{
	Q_OBJECT

public:
	OpenDialog(QWidget *parent = Q_NULLPTR);
	~OpenDialog();

public:
	// ��ʼ���Ի�������.
	void initContent(QString filename);

	// ��ȡ��ǰ���ļ���Ϣ.
	FileInfo fileInfo();

public slots:
	void openFile();

private:
	Ui::OpenDialog ui;

	FileInfo m_fileInfo;
};

#endif //OPEN_DIALOG_H