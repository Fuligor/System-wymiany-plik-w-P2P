#ifndef TRACKER_RESPONSE_H
#define TRACKER_RESPONSE_H
#include <string>

#include "Bencode.h"

struct Peer;

class TrackerResponse
{
    private:
        bencode::String f_reason;
        bencode::Int interval;
        bencode::Int complete;
        bencode::Int incomplete;
        bencode::List peers;

    public:
        TrackerResponse();
        ~TrackerResponse();
        void setF_reason(std::string reason);
        void resetF_reason();
        void setInterval(int interv);
        void resetTracker_id();
        void setComplete(int com);
        void setIncomplete(int incom);
        void addPeer(const Peer& peer);

        std::string getResponse() const;
};

#endif