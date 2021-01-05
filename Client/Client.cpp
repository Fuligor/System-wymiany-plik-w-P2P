#include "Client.h"

#include <QApplication>
#include <QDateTime>
#include <QDir>

#include <cstdlib>
#include <ctime>

#include "Torrent.h"
#include "TorrentFile.h"
#include "TorrentManager.h"

void toString(quint64 value, char* string)
{
    for(int i = 0; i < 8; i++)
    {
        string[i] = value & (0xFF);
        value = value >> 8;
    }
}

Client::Client()
    :QObject(nullptr), myId(createId())
{
    std::srand(std::time(NULL));
    QDir config(configPath.c_str());
    if(!config.exists())
    {
        std::cout << config.absolutePath().toStdString() << std::endl;

        QDir().mkpath(config.absolutePath());
    }
    thread.start();
}

const std::string Client::createId()
{
    char id[20] = "-TA-";

    toString(QApplication::instance()->applicationPid(), id + 4);
    toString(QDateTime::currentDateTime().toMSecsSinceEpoch(), id + 12);

    return std::string(id, 20);
}

Client *Client::getInstance()
{
    mutex.lock();

    if(sigleInstance == nullptr)
    {
        sigleInstance = new Client();
    }

    mutex.unlock();

    return sigleInstance;
}

const std::string& Client::getId() const
{
    return myId;
}

const std::string& Client::getConfigPath()
{
    return Client::configPath;
}

void Client::shareFile(const std::string& fileName, const std::string& trackerAddres, const size_t pieceSize)
{
    TorrentFile* file = new TorrentFile(fileName, trackerAddres, pieceSize);
    connect(file, SIGNAL(torrentCreated(TorrentFile*)), this, SLOT(onFileCreated(TorrentFile*)));
    file->moveToThread(&thread);
    file->createFile();
}

void Client::downloadFile(const std::string& torrentPath, const std::string& downloadPath)
{
    new Torrent(torrentPath, downloadPath);
}

void Client::onFileCreated(TorrentFile* file)
{
    new Torrent(file->getFileName().toStdString() + ".torrent", file->getFileName().toStdString(), true);
    file->deleteLater();
    emit fileShared();
}

Client* Client::sigleInstance = nullptr;
QMutex Client::mutex;
const std::string Client::configPath = "config";