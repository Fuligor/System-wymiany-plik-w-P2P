#include "ShareFileWindow.h"

ShareFileWindow::ShareFileWindow(QWidget* prevWindow, QWidget *parent)
	: QWidget(parent), prevWindow(prevWindow)
{
	ui.setupUi(this);
	connect(ui.Cancel, SIGNAL(clicked()), prevWindow, SLOT(show()));
	connect(ui.Share, SIGNAL(clicked()), prevWindow, SLOT(show()));
}

ShareFileWindow::~ShareFileWindow()
{
}
