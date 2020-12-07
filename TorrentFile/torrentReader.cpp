#include "torrentReader.h"
#include <fstream>
torrentReader::torrentReader(std::string fileName)
{
    std::ifstream inFile(fileName, std::ios::binary);

    readDict = dynamic_cast <bencode::Dict*> (bencode::Decoder::decode(inFile));
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
