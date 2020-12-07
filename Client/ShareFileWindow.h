#pragma once

#include <QWidget>
#include "ui_ShareFileWindow.h"

class QFileDialog;

class ShareFileWindow : public QWidget
{
	Q_OBJECT
	QWidget* prevWindow;
    QFileDialog* fileDialog;
public:
	ShareFileWindow(QWidget* prevWindow, QWidget *parent = Q_NULLPTR);
	~ShareFileWindow();

public slots:
    void getFiles();
    void fileSelected();
private:
	Ui::ShareFileWindow ui;
};
