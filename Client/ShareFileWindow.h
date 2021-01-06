// Klasa obs³uguj¹ca okno udostepniania pliku
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
    void fileSelected();
	void shareFile();
	//walidacja pól tekstowych
	void check();
	void onFileShared();
	void showWarning(std::string info);
private:
	Ui::ShareFileWindow ui;
signals:
	void checked();
};
