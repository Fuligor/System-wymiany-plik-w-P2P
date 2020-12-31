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
    uploaded.reset(new bencode::Int());
    downloaded.reset(new bencode::Int());
    length = std::dynamic_pointer_cast <bencode::Int> ((*info)["length"]);
    compact.reset(new bencode::Int());
    no_peer_id.reset(new bencode::Int());
}

TrackerRequest::~TrackerRequest()
{
    
}

void TrackerRequest::setPeer_id(std::string p_id)
{
    peer_id.reset(new bencode::String(p_id));
    return;
}
void TrackerRequest::setPort(int po)
{
    port.reset(new bencode::Int(po));
    return;
}
void TrackerRequest::setUploaded(int up)
{
    uploaded.reset(new bencode::Int(up));
    return;
}
void TrackerRequest::setDownloaded(int down)
{
    downloaded.reset(new bencode::Int(down));
    return;
}
void TrackerRequest::setCompact(bool com)
{
    compact.reset(new bencode::Int(com));
    return;
}
void TrackerRequest::setNo_peer_id(bool n_p_id)
{
    no_peer_id.reset(new bencode::Int(n_p_id));
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

void TrackerRequest::setTrackerId(std::string id)
{
    trackerId.reset(new bencode::String(id));
    return;
}

void TrackerRequest::resetTrackerId()
{
    trackerId.reset();
    return;
}

std::string TrackerRequest::getRequest()
{
    bencode::Dict request;

    request["info_hash"] = std::make_shared <bencode::String> (InfoDictHash::getHash(torrentDict));
    request["peer_id"] = peer_id;
    request["port"] = port;
    request["uploaded"] = uploaded;
    request["downloaded"] = downloaded;
    request["left"] = std::make_shared <bencode::Int> (*length - *downloaded);
    request["compact"] = compact;
    request["no_peer_id"] = no_peer_id;

    if(event != nullptr)
    {
        request["event"] = event;
    }
    if(trackerId != nullptr)
    {
        request["trackerid"] = trackerId;
    }

    return request.code();
}
