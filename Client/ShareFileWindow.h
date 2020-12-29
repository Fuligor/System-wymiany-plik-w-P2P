#pragma once

#include <QWidget>
#include "ui_ShareFileWindow.h"

#include <map>

class QFileDialog;

class ShareFileWindow : public QWidget
{
	Q_OBJECT
	static const size_t kB;

	QWidget* prevWindow;
    QFileDialog* fileDialog;
	std::map <std::string, size_t> idToRow;
public:
	ShareFileWindow(QWidget* prevWindow, QWidget *parent = Q_NULLPTR);
	~ShareFileWindow();

public slots:
	void init();
    void getFiles();
    void fileSelected();
	void shareFile();
private:
	Ui::ShareFileWindow ui;
};
