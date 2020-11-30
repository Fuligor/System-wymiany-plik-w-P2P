#include "torrentReader.h"
#include <fstream>
torrentReader::torrentReader(std::string fileName)
{
    std::ifstream inFile(fileName);

    readDict = dynamic_cast <bencode::Dict*> (bencode::Decoder::decode(inFile));

}

torrentReader::~torrentReader(){}


bencode::Dict *torrentReader::getDict()
{
    return readDict;
}