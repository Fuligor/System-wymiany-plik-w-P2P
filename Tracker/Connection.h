#pragma once
#include <string>

#include "Bencode.h"
#include "TrackerResponse.h"

#include "Peer.h"

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

    int socket;
    int interval;
    sockaddr_in* address;

    std::shared_ptr <std::wstring> info_hash;
    Peer peer;
    bool completed;

    std::wstring buf;

    bencode::Utf8Decoder utf8Decoder;
    bencode::Decoder decoder;
public:
    Connection(const int& socket, int interval, sockaddr_in* address);
    ~Connection();

    void closeConnection();
    Peer readPeerInfo(bencode::Dict &request);
    int getSocket();

    void addToBuffer(const char* data, uint64_t size);
    bencode::Dict* getReguest();
    bool createResponse();
    bool sendResponse(const TrackerResponse& response);
protected:
    static const char toHexEncoding(const unsigned char& number);
public:
    static std::string getReadablePeerId(std::string peerId);
};