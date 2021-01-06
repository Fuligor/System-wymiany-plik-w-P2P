//Klasa obs³uguj¹ca okno pobierania w kliencie
#pragma once

#include <QWidget>
#include "ui_DownloadFileWindow.h"

class QFileDialog;

class DownloadFileWindow : public QWidget
{
	Q_OBJECT

	QWidget* prevWindow;
    QFileDialog* TorrentsDialog;
    QFileDialog* SavesDialog;
public:
	DownloadFileWindow(QWidget* prevWindow, QWidget *parent = Q_NULLPTR);
	~DownloadFileWindow();

public slots:
	void init();
    void torrentSelected();
    void saveSelected();
	void DownloadFile();
	//pzeprowadza walidacjê pól tekstowych
	void check();
private:
	Ui::DownloadFileWindow ui;
signals:
	void checked();
};
