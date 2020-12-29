#include <iostream>

#include "Window.h"

#include "TorrentManager.h"
#include "TorrentDownloadStatus.h"

Window::Window(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    client = Client::getInstance();
    shareFileWindow = new ShareFileWindow(ui.MainWidget, nullptr);
    ui.DownloadedFiles->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QStringList headerLabel;

    headerLabel.append(QString("Nazwa"));
    headerLabel.append(QString("Rozmiar"));
    headerLabel.append(QString("Status"));
    headerLabel.append(QString("Id"));

    ui.DownloadedFiles->setHorizontalHeaderLabels(headerLabel);
    ui.DownloadedFiles->setColumnHidden(3, true);

    connect(ui.ShareNewFile, SIGNAL(clicked()), shareFileWindow, SLOT(init()));
    connect(ui.ShareNewFile, SIGNAL(clicked()), shareFileWindow, SLOT(show()));
    connect(&TorrentManager::getInstance(), SIGNAL(torrentStatusUpdated(const std::string, const TorrentDownloadStatus*)), this, SLOT(torrentStatusUpdated(const std::string, const TorrentDownloadStatus*)));

    TorrentManager::getInstance().updateDownloadList();
}

void Window::torrentStatusUpdated(const std::string torrentId, const TorrentDownloadStatus* status)
{
    std::cout << torrentId << std::endl;

    size_t rowIndex = ui.DownloadedFiles->rowCount();

    ui.DownloadedFiles->insertRow(rowIndex);
    ui.DownloadedFiles->setItem(rowIndex, 0, new QTableWidgetItem(QString::fromStdWString(status->fileName)));
    ui.DownloadedFiles->setItem(rowIndex, 1, new QTableWidgetItem(QString::number(status->fileSize)));
    ui.DownloadedFiles->setItem(rowIndex, 2, new QTableWidgetItem(QString::fromStdWString(status->fileName)));
    ui.DownloadedFiles->setItem(rowIndex, 3, new QTableWidgetItem(QString::fromStdString(torrentId)));
}
