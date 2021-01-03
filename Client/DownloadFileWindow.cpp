#include "DownloadFileWindow.h"

#include <QFileDialog>
#include <QDebug>
#include <QListView>
#include <QTreeView>

#include "Client.h"
#include "TorrentManager.h"

DownloadFileWindow::DownloadFileWindow(QWidget* prevWindow, QWidget *parent)
    : QWidget(parent), prevWindow(prevWindow), TorrentsDialog(new QFileDialog(this)), SavesDialog(new QFileDialog(this))
{
    ui.setupUi(this);
    hide();
    TorrentsDialog->setModal(true);
    TorrentsDialog->setNameFilter("Torrent (*.torrent)");
    TorrentsDialog->setFileMode(QFileDialog::ExistingFile);
    SavesDialog->setModal(true);
    SavesDialog->setFileMode(QFileDialog::Directory);

	connect(ui.Cancel, SIGNAL(clicked()), prevWindow, SLOT(show()));
	connect(ui.Download, SIGNAL(clicked()), prevWindow, SLOT(show()));
    connect(ui.listTorrents, SIGNAL(clicked()), TorrentsDialog, SLOT(show()));
    connect(TorrentsDialog, SIGNAL(accepted()), this, SLOT(torrentSelected()));
    connect(ui.listSaves, SIGNAL(clicked()), SavesDialog, SLOT(show()));
    connect(SavesDialog, SIGNAL(accepted()), this, SLOT(saveSelected()));
    connect(ui.Download, SIGNAL(clicked()), this, SLOT(DownloadFile()));
}

DownloadFileWindow::~DownloadFileWindow()
{
}

void DownloadFileWindow::init()
{
    ui.torrentName->setText("");
    ui.saveName->setText("");
}

void DownloadFileWindow::getTorrents()
{
    TorrentsDialog->show();
}

void DownloadFileWindow::getSaves()
{
    SavesDialog->show();
}

void DownloadFileWindow::torrentSelected()
{
    ui.torrentName->setText(TorrentsDialog->selectedFiles()[0]);
    qDebug() << TorrentsDialog->selectedFiles()[0];
}

void DownloadFileWindow::saveSelected()
{
    ui.saveName->setText(SavesDialog->selectedFiles()[0]);
    qDebug() << SavesDialog->selectedFiles()[0];
}

void DownloadFileWindow::DownloadFile()
{
    //Client::getInstance()->shareFile(ui.fileName->text().toStdString(), ui.trackerURL->currentText().toStdString(), ui.pieceSize->value());
}

const size_t DownloadFileWindow::kB = 1024;