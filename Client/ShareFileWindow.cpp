#include "ShareFileWindow.h"

#include <QFileDialog>
#include <QDebug>
#include <QListView>
#include <QTreeView>

#include "Client.h"
#include "TorrentManager.h"

ShareFileWindow::ShareFileWindow(QWidget* prevWindow, QWidget *parent)
    : QWidget(parent), prevWindow(prevWindow), fileDialog(new QFileDialog(this))
{
    ui.setupUi(this);
    hide();
    fileDialog->setModal(true);

	connect(ui.Cancel, SIGNAL(clicked()), prevWindow, SLOT(show()));
	connect(ui.Share, SIGNAL(clicked()), prevWindow, SLOT(show()));
    connect(ui.listFiles, SIGNAL(clicked()), fileDialog, SLOT(show()));
    connect(fileDialog, SIGNAL(accepted()), this, SLOT(fileSelected()));
    connect(ui.Share, SIGNAL(clicked()), this, SLOT(shareFile()));
}

ShareFileWindow::~ShareFileWindow()
{
}

void ShareFileWindow::init()
{
    ui.fileName->setText("");
}

void ShareFileWindow::getFiles()
{
    fileDialog->show();
}

void ShareFileWindow::fileSelected()
{
    ui.fileName->setText(fileDialog->selectedFiles()[0]);
    qDebug() << fileDialog->selectedFiles()[0];
}

void ShareFileWindow::shareFile()
{
    Client::getInstance()->shareFile(ui.fileName->text().toStdString(), ui.trackerURL->currentText().toStdString(), ui.pieceSize->value());
}

const size_t ShareFileWindow::kB = 1024;