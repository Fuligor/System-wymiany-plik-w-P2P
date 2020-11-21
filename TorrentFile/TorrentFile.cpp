#include "TorrentFile.h"
#include <QFileInfo>
#include <QDateTime>
#include <QDate>
#include <QTextStream>
#include "File.h"
#include "../Bencode/Bencode.h"

TorrentFile::TorrentFile(std::string fname, std::string Utracker, int pLength)
{
    fileName = QString::fromStdString(fname);
    URLtracker = QString::fromStdString(Utracker);
    pieceLength = pLength;
}
TorrentFile::~TorrentFile(){}
void TorrentFile::createFile()
{
    QFileInfo file(fileName);

    File file2(fileName, pieceLength);
    bencode::Dict metainfoDict;
    bencode::Dict infoDict;

    if(file.isFile())
    {
        infoDict["piece length"] = std::make_shared <bencode::Int> (pieceLength);
        infoDict["pieces"] = std::make_shared <bencode::String> (file2.getFragsHash());
        infoDict["name"] = std::make_shared <bencode::String> (file.fileName());
        infoDict["length"] = std::make_shared <bencode::Int> (file.size());
    }

    metainfoDict["info"] = std::make_shared <bencode::Dict> (infoDict);
    metainfoDict["announce"] = std::make_shared <bencode::String> (URLtracker);
    QDateTime creation_date = QDateTime::currentDateTime();
    metainfoDict["creation date"] = std::make_shared <bencode::Int> (creation_date.toTime_t());

    
    QFile outFile(fileName + ".torrent");
    if(outFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&outFile);
        out << QString::fromStdString(metainfoDict.code());
        outFile.close();

    }
    return;
}