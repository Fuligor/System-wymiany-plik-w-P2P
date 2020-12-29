#include "torrentReader.h"
#include <fstream>

#include <iostream>

torrentReader::torrentReader(std::wstring fileName)
{
    std::ifstream inFile(fileName, std::ios::binary);

    bencode::Utf8Decoder utfDecoder;
    std::wstring string = utfDecoder.decode(inFile);

    bencode::Decoder decoder;
    readDict.reset(dynamic_cast <bencode::Dict*> (decoder.decode(string)));
}

torrentReader::~torrentReader()
{
}


std::shared_ptr <bencode::Dict> torrentReader::getDict()
{
    return readDict;
}
