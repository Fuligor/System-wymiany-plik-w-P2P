#pragma once
#include <string>
#include "../Bencode/Bencode.h"
class TrackerResponse
{
    private:
        bencode::Dict response;
        bencode::String f_reason;
        bencode::Int interval;
        bencode::String tracker_id;
        bencode::Int complete;
        bencode::Int incomplete;
        bencode::List peers;

    public:
        TrackerResponse();
        ~TrackerResponse();
        void setF_reason(std::string reason);
        void setInterval(int interv);
        void setTracker_id(std::string tracker);
        void setComplete(int com);
        void setIncomplete(int incom);
        void addPeer(std::string peer_id, std::string ip, int port);

        std::string getResponse();

};