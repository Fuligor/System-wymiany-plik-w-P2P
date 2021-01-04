#pragma once
#include <string>

#include <QString>

class TorrentFile
{
    private:
        QString fileName;
        QString URLtracker;
        int pieceLength;

    public:
        TorrentFile(const std::string fname, const std::string Utracker, size_t pLength);
        ~TorrentFile();
        void createFile();
};
