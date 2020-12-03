#include "TrackerResponse.h"

TrackerResponse::TrackerResponse(){}
TrackerResponse::~TrackerResponse(){}

void TrackerResponse::setF_reason(std::string reason)
{
    f_reason = reason;
    return;
}
void TrackerResponse::setInterval(int interv)
{
    interval = interv;
    return;
}
void TrackerResponse::setTracker_id(std::string tracker)
{
    tracker_id = tracker;
    return;
}
void TrackerResponse::setComplete(int com)
{
    complete = com;
    return;
}
void TrackerResponse::setIncomplete(int incom)
{
    incomplete = incom;
    return;
}
void TrackerResponse::addPeer(std::string peer_id, std::string ip, int port)
{
    bencode::Dict dic;
    dic["peer id"] = std::make_shared <bencode::String> (peer_id);
    dic["ip"] = std::make_shared <bencode::String> (ip);
    dic["port"] = std::make_shared <bencode::Int> (port);
    peers.push_back(std::make_shared <bencode::Dict> (dic));
    return;
}

std::string TrackerResponse::getResponse()
{
    response["failure reason"] = std::make_shared <bencode::String> (f_reason);
    response["interval"] = std::make_shared <bencode::Int> (interval);
    response["tracker id"] = std::make_shared <bencode::String> (tracker_id);
    response["complete"] = std::make_shared <bencode::Int> (complete);
    response["incomplete"] = std::make_shared <bencode::Int> (incomplete);
    response["peers"] = std::make_shared <bencode::List> (peers);

    return response.code();
}