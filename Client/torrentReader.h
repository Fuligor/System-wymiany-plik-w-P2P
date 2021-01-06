//Klasa odpowiedzialna za odczytania s³ownika z pliku .torrent
#pragma once
#include "Bencode.h"

class torrentReader{
    private:
        std::shared_ptr <bencode::Dict> readDict;
    public:
        torrentReader(std::string fileName);
        ~torrentReader();
        std::shared_ptr <bencode::Dict> getDict();
};
