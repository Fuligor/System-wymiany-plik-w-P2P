#pragma once
#include <string>

#include "../Bencode/Bencode.h"
#include "TrackerResponse.h"

#include "../TorrentFile/Peer.h"

struct sockaddr_in;

class Connection
{
protected:
    enum class State
    {
        NEW,
        CONNECTED,
        CLOSED
    } state;

    int interval;
    sockaddr_in* address;

    std::shared_ptr <std::wstring> info_hash;
    Peer* peer;
    bool completed;

    int socket;

    std::wstring buf;

    bencode::Utf8Decoder utf8Decoder;
    bencode::Decoder decoder;
public:
    Connection(const int& socket, int interval, sockaddr_in* address);
    ~Connection();

    void closeConnection();
    Peer readPeerInfo(bencode::Dict &request);
    int getSocket();

    void addToBuffer(const char* data, size_t size);
    bencode::Dict* getReguest();
    bool createResponse();
    void sendResponse(const TrackerResponse& response);
};