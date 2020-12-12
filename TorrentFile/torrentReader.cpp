#include "torrentReader.h"
#include <fstream>

#include <iostream>

torrentReader::torrentReader(std::string fileName)
{
    std::ifstream inFile(fileName, std::ios::binary);

    bencode::Utf8Decoder utfDecoder;
    std::wstring string = utfDecoder.decode(inFile);

    bencode::Decoder decoder;
    readDict = dynamic_cast <bencode::Dict*> (decoder.decode(string));

    std::cerr << readDict->code() << std::endl;
}

torrentReader::~torrentReader(){}


bencode::Dict *torrentReader::getDict()
{
    return readDict;
}

bencode::Dict* torrentReader::getInfoDict()
{
    return dynamic_cast <bencode::Dict*> ((*readDict)["info"].get());
}
