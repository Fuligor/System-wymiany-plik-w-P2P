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
        TorrentFile(std::string fname, std::string Utracker, int pLength);
        ~TorrentFile();
        void createFile();

};
