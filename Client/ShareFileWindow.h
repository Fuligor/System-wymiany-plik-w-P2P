#pragma once

#include <QWidget>
#include "ui_ShareFileWindow.h"

class ShareFileWindow : public QWidget
{
	Q_OBJECT
	QWidget* prevWindow;
public:
	ShareFileWindow(QWidget* prevWindow, QWidget *parent = Q_NULLPTR);
	~ShareFileWindow();

private:
	Ui::ShareFileWindow ui;
};
