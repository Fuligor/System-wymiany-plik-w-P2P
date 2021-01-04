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
        std::shared_ptr <bencode::Int> uploaded;
        std::shared_ptr <bencode::Int> downloaded;
        std::shared_ptr <bencode::Int> length;
        std::shared_ptr <bencode::Int> compact;
        std::shared_ptr <bencode::Int> no_peer_id;
        std::shared_ptr <bencode::String> event;
        std::shared_ptr <bencode::String> trackerId;
    public:
        TrackerRequest(std::shared_ptr <bencode::Dict> torrentDict);
        ~TrackerRequest();
        void setPeer_id(std::string p_id);
        void setPort(size_t po);
        void setLeft(size_t left);
        void setUploaded(size_t up);
        void setDownloaded(size_t down);
        void setCompact(bool com);
        void setNo_peer_id(bool n_p_id);
        void setEvent(std::string ev);
        void resetEvent();
        void setTrackerId(std::string id);
        void resetTrackerId();
        std::string getRequest();
};
