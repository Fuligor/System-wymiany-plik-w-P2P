#pragma once
#include <string>
#include <QCryptographicHash>
#include "../Bencode/Bencode.h"
#include "torrentReader.h"

class TrackerRequest
{
    private: 
        bencode::Dict request;
        std::shared_ptr <bencode::ICode> info;
        bencode::String peer_id;
        bencode::Int port;
        bencode::Int uploaded;
        bencode::Int downloaded;
        bencode::Int* length;
        bool compact;
        bool no_peer_id;
        bencode::String event;
    public:
        TrackerRequest(std::string fileName);
        ~TrackerRequest();
        void setPeer_id(std::string p_id);
        void setPort(int po);
        void setUploaded(int up);
        void setDownloaded(int down);
        void setCompact(bool com);
        void setNo_peer_id(bool n_p_id);
        void setEvent(std::string ev);
        std::string getRequest();

};