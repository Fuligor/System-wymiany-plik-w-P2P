#include "TrackerRequest.h"


TrackerRequest::TrackerRequest(std::string fileName)
{
    bencode::Dict *tmp;
    tmp = torrentReader(fileName).getDict();
    info = (*tmp)["info"];
    peer_id = "";
    port = 0;
    uploaded = 0;
    downloaded = 0;
    length = dynamic_cast <bencode::Int*> ((*tmp)["length"].get());
    compact = false;
    no_peer_id = false;
    event = "started";
    
}

TrackerRequest::~TrackerRequest(){}

void TrackerRequest::setPeer_id(std::string p_id)
{
    peer_id = p_id;
    return;
}
void TrackerRequest::setPort(int po)
{
    port = po;
    return;
}
void TrackerRequest::setUploaded(int up)
{
    uploaded = up;
    return;
}
void TrackerRequest::setDownloaded(int down)
{
    downloaded = down;
    return;
}
void TrackerRequest::setCompact(bool com)
{
    compact = com;
    return;
}
void TrackerRequest::setNo_peer_id(bool n_p_id)
{
    no_peer_id = n_p_id;
    return;
}
void TrackerRequest::setEvent(std::string ev)
{
    event = ev;
    return;
}

std::string TrackerRequest::getRequest()
{
    request["info_hash"] = std::make_shared <bencode::String> (QCryptographicHash::hash(QByteArray::fromStdString(info.get()->code()), QCryptographicHash::Sha1));
    request["peer_id"] = std::make_shared <bencode::String> (peer_id);
    request["port"] = std::make_shared <bencode::Int> (port);
    request["uploaded"] = std::make_shared <bencode::Int> (uploaded);
    request["downloaded"] = std::make_shared <bencode::Int> (downloaded);
    request["left"] = std::make_shared <bencode::Int> (*length - downloaded);
    request["compact"] = std::make_shared <bencode::Int> (compact);
    request["no_peer_id"] = std::make_shared <bencode::Int> (no_peer_id);
    request["event"] = std::make_shared <bencode::String> (event);
    return request.code();
}
