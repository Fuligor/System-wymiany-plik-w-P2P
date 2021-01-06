//Klasa tworz¹ca zapytanie do trackera
#pragma once
#include <string>
#include "Bencode.h"
#include "torrentReader.h"

class TrackerRequest
{
    private:
        std::shared_ptr <bencode::Dict> torrentDict;
        std::shared_ptr <bencode::Dict> info;
        std::shared_ptr <bencode::String> peer_id;
        std::shared_ptr <bencode::Int> port;
        std::shared_ptr <bencode::Int> left;
        std::shared_ptr <bencode::Int> length;
        std::shared_ptr <bencode::String> event;
    public:
        TrackerRequest(std::shared_ptr <bencode::Dict> torrentDict);
        ~TrackerRequest();
        void setPeer_id(std::string p_id);
        void setPort(uint64_t po);
        void setLeft(uint64_t left);
        void setEvent(std::string ev);
        void resetEvent();
        std::string getRequest();
};
