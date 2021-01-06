#include "TrackerRequest.h"

#include <iostream>

#include "InfoDictHash.h"

TrackerRequest::TrackerRequest(std::shared_ptr <bencode::Dict> torrentDict)
    :torrentDict(torrentDict)
{
    bencode::Dict &tmp = *torrentDict;
    info = std::dynamic_pointer_cast <bencode::Dict> (tmp["info"]);
    peer_id.reset(new bencode::String());
    port.reset(new bencode::Int());
    left.reset(new bencode::Int());
    length = std::dynamic_pointer_cast <bencode::Int> ((*info)["length"]);
}

TrackerRequest::~TrackerRequest()
{
    
}

void TrackerRequest::setPeer_id(std::string p_id)
{
    peer_id.reset(new bencode::String(p_id));
    return;
}
void TrackerRequest::setPort(uint64_t po)
{
    port.reset(new bencode::Int(po));
    return;
}
void TrackerRequest::setLeft(uint64_t left)
{
    this->left.reset(new bencode::Int(left));
    return;
}

void TrackerRequest::setEvent(std::string ev)
{
    event.reset(new bencode::String(ev));
    return;
}

void TrackerRequest::resetEvent()
{
    event.reset();

    return;
}

std::string TrackerRequest::getRequest()
{
    bencode::Dict request;

    request["info_hash"] = std::make_shared <bencode::String> (InfoDictHash::getHash(torrentDict));
    request["peer_id"] = peer_id;
    request["port"] = port;
    request["left"] = left;

    if(event != nullptr)
    {
        request["event"] = event;
    }

    return request.code();
}
