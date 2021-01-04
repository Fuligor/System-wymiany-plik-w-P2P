#include "torrentReader.h"
#include <fstream>

torrentReader::torrentReader(std::string fileName)
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
