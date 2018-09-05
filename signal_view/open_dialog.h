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
	// 初始化对话框内容.
	void initContent(QString filename);

	// 获取当前的文件信息.
	FileInfo fileInfo();

public slots:
	void openFile();

private:
	Ui::OpenDialog ui;

	FileInfo m_fileInfo;
};

#endif //OPEN_DIALOG_H