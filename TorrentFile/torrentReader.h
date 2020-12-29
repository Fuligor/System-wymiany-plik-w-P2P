#pragma once
#include "Bencode.h"

class torrentReader{
    private:
        std::shared_ptr <bencode::Dict> readDict;
    public:
        torrentReader(std::wstring fileName);
        ~torrentReader();
        std::shared_ptr <bencode::Dict> getDict();
};
