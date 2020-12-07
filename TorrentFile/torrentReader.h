#pragma once
#include "Bencode.h"



class torrentReader{
    private:
        bencode::Dict *readDict;
    public:
        torrentReader(std::string fileName);
        ~torrentReader();
        bencode::Dict *getDict();
        bencode::Dict *getInfoDict();
};
