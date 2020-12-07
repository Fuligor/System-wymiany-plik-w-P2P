#include "TorrentFile.h"

#include <QFileInfo>
#include <QDateTime>
#include <QDate>
#include <QTextStream>

#include <Bencode.h>

#include "File.h"

TorrentFile::TorrentFile(const std::string fname, const std::string Utracker, int pLength)
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
    std::string pieces;
    bencode::String bpieces;

    if(file.isFile())
    {
        infoDict["piece length"] = std::make_shared <bencode::Int> (pieceLength);
        pieces = file2.getFragsHash();
        bpieces = file2.getFragsHash();
        infoDict["pieces"] = std::make_shared <bencode::String> (file2.getFragsHash());
        infoDict["name"] = std::make_shared <bencode::String> (file.fileName().toStdString());
        infoDict["length"] = std::make_shared <bencode::Int> (file.size());
    }
    bencode::String bpieces2(pieces);
    std::wstring wpieces(pieces.begin(), pieces.end());

    metainfoDict["info"] = std::make_shared <bencode::Dict> (infoDict);
    std::string urlAddres = URLtracker.toStdString();
    metainfoDict["announce"] = std::make_shared <bencode::String> (urlAddres);
    QDateTime creation_date = QDateTime::currentDateTime();
    metainfoDict["creation date"] = std::make_shared <bencode::Int> (creation_date.toTime_t());

    QFile outFile(fileName + ".torrent");
    if(outFile.open(QIODevice::WriteOnly))
    {
        QTextStream out(&outFile);
        std::string code = metainfoDict.code();
        QByteArray buf = QByteArray::fromStdString(code);
        outFile.write(buf);
        outFile.close();
    }

    return;
}