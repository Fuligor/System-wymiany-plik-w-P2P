#include "DownloadFileWindow.h"

#include <QFileDialog>
#include <QDebug>
#include <QListView>
#include <QTreeView>
#include <QMessageBox>

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

	connect(ui.Cancel, SIGNAL(clicked()), this, SLOT(hide()));
	connect(ui.Download, SIGNAL(clicked()), this, SLOT(check()));
	connect(this, SIGNAL(checked()), this, SLOT(hide()));
    connect(ui.listTorrents, SIGNAL(clicked()), TorrentsDialog, SLOT(show()));
    connect(TorrentsDialog, SIGNAL(accepted()), this, SLOT(torrentSelected()));
    connect(ui.listSaves, SIGNAL(clicked()), SavesDialog, SLOT(show()));
    connect(SavesDialog, SIGNAL(accepted()), this, SLOT(saveSelected()));
    connect(this, SIGNAL(checked()), this, SLOT(DownloadFile()));
}

DownloadFileWindow::~DownloadFileWindow()
{
}

void DownloadFileWindow::init()
{
    ui.torrentName->setText("");
    ui.saveName->setText("");
}

//void DownloadFileWindow::getTorrents()
//{
//    TorrentsDialog->show();
//}
//
//void DownloadFileWindow::getSaves()
//{
//    SavesDialog->show();
//}

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
    std::string saveName = ui.saveName->text().toStdString();

    if(saveName[saveName.size() - 1] != '/' || saveName[saveName.size() - 1] != '\\')
    {
        saveName += '/';
    }

    Client::getInstance()->downloadFile(ui.torrentName->text().toStdString(), saveName);
}

void DownloadFileWindow::check()
{
    if (ui.saveName->text().isEmpty() || ui.torrentName->text().isEmpty())
    {

        QMessageBox msgBox;
        msgBox.setText(QString::fromStdWString(L"Przynajmniej jedno pole jest puste."));
        msgBox.setInformativeText(QString::fromStdWString(L"Uzupełnij puste pola, by kontynuować."));
        msgBox.exec();
    }
    else
    {
        emit checked();
    }
}
