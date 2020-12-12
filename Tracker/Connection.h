#pragma once
#include <signal.h>
#include <time.h>

#include <string>

#include "../Bencode/Bencode.h"
#include "TrackerResponse.h"

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

    static int timerEpoll;
    int interval;

    int timer;
    int socket;

    std::wstring buf;

    bencode::Utf8Decoder utf8Decoder;
    bencode::Decoder decoder;
public:
    Connection(const int& socket, const int& timer, int interval, sockaddr_in* address);
    ~Connection();

    void closeConnection();
    int getSocket();

    void addToBuffer(const char* data, size_t size);
    bencode::Dict* getReguest();
    bool createResponse();
    void sendResponse(const TrackerResponse& response);

    void timerReset();
};