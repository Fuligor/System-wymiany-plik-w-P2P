#pragma once

#include <QWidget>
#include <qprogressdialog.h>
#include "ui_ShareFileWindow.h"

class QFileDialog;
class QProgressBar;

class ShareFileWindow : public QWidget
{
	Q_OBJECT

	QWidget* prevWindow;
    QFileDialog* fileDialog;
	QProgressDialog* prog;
	QProgressBar* bar;
public:
	ShareFileWindow(QWidget* prevWindow, QWidget *parent = Q_NULLPTR);
	~ShareFileWindow();

public slots:
	void init();
   // void getFiles();
    void fileSelected();
	void shareFile();
	void check();
	void onFileShared();
private:
	Ui::ShareFileWindow ui;
signals:
	void checked();
};
