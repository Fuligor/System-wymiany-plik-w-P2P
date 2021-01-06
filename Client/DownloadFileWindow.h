//Klasa obs�uguj�ca okno pobierania w kliencie
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
	//pzeprowadza walidacj� p�l tekstowych
	void check();
private:
	Ui::DownloadFileWindow ui;
signals:
	void checked();
};
