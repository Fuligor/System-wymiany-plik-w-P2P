#pragma once

#include <QWidget>
#include "ui_DownloadFileWindow.h"

class QFileDialog;

class DownloadFileWindow : public QWidget
{
	Q_OBJECT
	static const size_t kB;

	QWidget* prevWindow;
    QFileDialog* TorrentsDialog;
    QFileDialog* SavesDialog;
public:
	DownloadFileWindow(QWidget* prevWindow, QWidget *parent = Q_NULLPTR);
	~DownloadFileWindow();

public slots:
	void init();
    void getTorrents();
    void getSaves();
    void torrentSelected();
    void saveSelected();
	void DownloadFile();
private:
	Ui::DownloadFileWindow ui;
};
