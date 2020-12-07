#include "ShareFileWindow.h"

#include <QFileDialog>
#include <QDebug>
#include <QListView>
#include <QTreeView>

ShareFileWindow::ShareFileWindow(QWidget* prevWindow, QWidget *parent)
    : QWidget(parent), prevWindow(prevWindow), fileDialog(new QFileDialog(this))
{
    ui.setupUi(this);
    hide();

	connect(ui.Cancel, SIGNAL(clicked()), prevWindow, SLOT(show()));
	connect(ui.Share, SIGNAL(clicked()), prevWindow, SLOT(show()));
    connect(fileDialog, SIGNAL(accepted()), this, SLOT(fileSelected()));
}

ShareFileWindow::~ShareFileWindow()
{
}

void ShareFileWindow::getFiles()
{
    fileDialog->show();
}

void ShareFileWindow::fileSelected()
{
    ui.lineEdit->setText(fileDialog->selectedFiles()[0]);
    qDebug() << fileDialog->selectedFiles()[0];
}
