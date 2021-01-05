﻿#include "Window.h"

#include "TorrentManager.h"
#include "TorrentDownloadStatus.h"
#include <QMessageBox>

Window::Window(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    client = Client::getInstance();
    shareFileWindow = new ShareFileWindow(ui.MainWidget, nullptr);
    downloadFileWindow = new DownloadFileWindow(ui.MainWidget, nullptr);
    ui.DownloadedFiles->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui.BottomBar->hide();

    QStringList headerLabel;

    headerLabel.append(QString("Nazwa"));
    headerLabel.append(QString("Rozmiar"));
    headerLabel.append(QString("Status"));
    headerLabel.append(QString("Id"));

    ui.DownloadedFiles->setHorizontalHeaderLabels(headerLabel);
    ui.DownloadedFiles->setColumnHidden(3, true);

    connect(ui.ShareNewFile, SIGNAL(clicked()), shareFileWindow, SLOT(init()));
    connect(ui.ShareNewFile, SIGNAL(clicked()), shareFileWindow, SLOT(show()));
    connect(ui.DownloadNewFIles, SIGNAL(clicked()), downloadFileWindow, SLOT(init()));
    connect(ui.DownloadNewFIles, SIGNAL(clicked()), downloadFileWindow, SLOT(show()));
    connect(ui.DownloadedFiles, SIGNAL(currentCellChanged(int, int, int, int)), this, SLOT(updateBottomBar(int, int, int, int)));
    connect(ui.DeleteConfig, SIGNAL(clicked()), this, SLOT(deleteFile()));
    connect(&TorrentManager::getInstance(), SIGNAL(torrentStatusUpdated(const std::string, const TorrentDownloadStatus*)), this, SLOT(torrentStatusUpdated(const std::string, const TorrentDownloadStatus*)));
    connect(&TorrentManager::getInstance(), SIGNAL(wrongConfigFile(std::string)), this, SLOT(showWarning(std::string)));
    TorrentManager::getInstance().updateDownloadList();
}

void Window::torrentStatusUpdated(const std::string torrentId, const TorrentDownloadStatus* status)
{
    int rowIndex;
    QString connectionState;

    if(idToRow.find(torrentId) == idToRow.end())
    {
        rowIndex = rowStatus.size();
        ui.DownloadedFiles->insertRow(rowIndex);
        rowStatus.push_back(status);

        idToRow[torrentId] = rowIndex;
    }
    else
    {
        rowIndex = idToRow.at(torrentId);
        rowStatus[rowIndex] = status;
    }

    if(ui.DownloadedFiles->currentRow() == rowIndex)
    {
        updateBottomBar(rowIndex, 0, 0, 0);
    }

    switch (status->connectionState)
    {
    case TorrentDownloadStatus::State::CONNECTING_TO_TRACKER:
        connectionState = QString::fromStdWString(L"Łączenie z trackerem...");
        break;
    case TorrentDownloadStatus::State::TRACKER_CONNECTION_REFUSED:
        connectionState = QString::fromStdWString(L"Połączenie odrzucone...");
        break;
    case TorrentDownloadStatus::State::LEECHING:
        connectionState = QString::fromStdWString(L"Leeching...");
        break;
    case TorrentDownloadStatus::State::SEEDING:
        connectionState = QString::fromStdWString(L"Seeding...");
        break;
    case TorrentDownloadStatus::State::CLOSED:
        connectionState = QString::fromStdWString(L"Closed...");
        break;
    }

    ui.DownloadedFiles->setItem((int)rowIndex, 0, new QTableWidgetItem(QString::fromStdWString(status->fileName)));
    ui.DownloadedFiles->setItem((int)rowIndex, 1, new QTableWidgetItem(QString::fromStdString(status->fileSize.toString())));
    ui.DownloadedFiles->setItem((int)rowIndex, 2, new QTableWidgetItem(connectionState));
    ui.DownloadedFiles->setItem((int)rowIndex, 3, new QTableWidgetItem(QString::fromStdString(torrentId)));
}

void Window::updateBottomBar(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
    if(currentRow > (int) rowStatus.size())
    {
        return;
    }

    const TorrentDownloadStatus* status = rowStatus[currentRow];

    ui.progressBar->setValue((int)(100 * status->downloadedSinceStart.to_int() / status->fileSize.to_int()));

    ui.ETA->setText(status->estimatedEndTime.toString());
    ui.downloadTime->setText(QString::number(status->startTime.secsTo(QDateTime::currentDateTime())));
    ui.downloaded->setText(QString::fromStdString(status->downloaded.toString()));
    ui.uploaded->setText(QString::fromStdString(status->uploaded.toString()));
    ui.downloadSpeed->setText(QString::fromStdString(status->downloadSpeed.toString() + "/s"));
    ui.uploadSpeed->setText(QString::fromStdString(status->uploadSpeed.toString() + "/s"));
    ui.downloadedSinceStart->setText(QString::fromStdString(status->downloadedSinceStart.toString()));
    ui.connectionCount->setText(QString::number(status->connectionCount));

    ui.DeleteConfig->setEnabled(true);

    ui.BottomBar->show();
}

void Window::showWarning(std::string info)
{
    QMessageBox msgBox;
    msgBox.setText(QString::fromStdString(info));
    msgBox.exec();
}

void Window::deleteFile()
{
    std::string torrentHash;
    int rowIndex = 0;

    for(auto i: idToRow)
    {
        if(i.second == ui.DownloadedFiles->currentRow())
        {
            torrentHash = i.first;
            rowIndex = i.second;
            break;
        }
    }

    if (!torrentHash.empty())
    {
        idToRow.erase(torrentHash);

        for(auto i: idToRow)
        {
            if(i.second > rowIndex)
            {
                --i.second;
            }
        }

        rowStatus.erase(rowStatus.begin() + rowIndex);
        ui.DownloadedFiles->removeRow(rowIndex);

        uint64_t pos = torrentHash.rfind('/');

        TorrentManager::getInstance().remove(torrentHash.substr(pos + 1));
    }

    ui.DownloadedFiles->clearSelection();
    ui.BottomBar->hide();
}
