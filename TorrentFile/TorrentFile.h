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
        TorrentFile(const std::wstring fname, const std::wstring Utracker, int pLength);
        ~TorrentFile();
        void createFile();
};
