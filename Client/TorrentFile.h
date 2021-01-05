#pragma once
#include <string>

#include <QString>
#include <qobject.h>

class TorrentFile 
    :public QObject
{
    Q_OBJECT
    private:
        QString fileName;
        QString URLtracker;
        int pieceLength;

    public:
        TorrentFile(const std::string fname, const std::string Utracker, size_t pLength);
        ~TorrentFile();
        void createFile();
        QString getFileName();
signals:
    void torrentCreated(TorrentFile*);
};
