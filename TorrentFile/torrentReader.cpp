#include "torrentReader.h"
#include <fstream>
torrentReader::torrentReader(std::string fileName)
{
    std::ifstream inFile(fileName, std::ios::binary);

    bencode::Utf8Decoder utfDecoder;
    std::wstring string = utfDecoder.decode(inFile);

    bencode::Decoder decoder;
    readDict = dynamic_cast <bencode::Dict*> (decoder.decode(string));
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
