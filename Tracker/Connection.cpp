#include "Connection.h"

#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <vector>

#include "Server.h"

int min(int a, int b)
{
    return a < b? a : b;
}

Connection::Connection(const int &socket, int interval, sockaddr_in *address)
    : socket(socket), state(State::NEW), interval(interval), address(address)
{
}

Connection::~Connection()
{
    closeConnection();
}

void Connection::closeConnection()
{
    if (state == State::CLOSED)
    {
        return;
    }

    state = State::CLOSED;

    printf("Klient rozłączony\n");

    close(socket);
}

int Connection::getSocket()
{
    return socket;
}

void Connection::addToBuffer(const char *data, size_t size)
{
    std::wcout << buf << std::endl;
    std::string string(data, size);
    printf("Readed %d\n", string.size());
    buf += utf8Decoder.decode(string);
}

bencode::Dict *Connection::getReguest()
{
    printf("Dekodowanie zapytania...\n");
    try
    {
        bencode::Dict *result = dynamic_cast<bencode::Dict *>(decoder.decode(buf));

        return result;
    }
    catch (bencode::Exception::end_of_file)
    {
        printf("Niepełne kodowanie... oczekiwanie na pozostałe fragmenty\n");

        return nullptr;
    }
}

bool Connection::createResponse()
{
    TrackerResponse response;
    bencode::Dict *request = nullptr;
    Server *server = Server::getInstance();

    try
    {
        request = getReguest();
    }
    catch (const std::exception &e)
    {
        printf("Niepoprawne kodowanie! Odsyłanie odpowiedzi...");

        response.setF_reason("Wrong bencoding!");

        sendResponse(response);

        return false;
    }

    if (request == nullptr)
    {
        return false;
    }

    bencode::String *info_hash = dynamic_cast<bencode::String *>((*request)["info_hash"].get());
    bencode::String *id = dynamic_cast<bencode::String *>((*request)["peer_id"].get());
    bencode::String *event = dynamic_cast<bencode::String *>((*request)["event"].get());
    bencode::Int *port = dynamic_cast<bencode::Int *>((*request)["port"].get());
    bencode::Int *left = dynamic_cast<bencode::Int *>((*request)["left"].get());

    std::cout << (*request)["event"]->code() << std::endl;
    std::cout << (*request)["info_hash"]->code() << std::endl;
    std::cout << (*request)["peer_id"]->code() << std::endl;
    std::cout << (*request)["port"]->code() << std::endl;

    Peer peer;

    peer.id = std::string(id->begin(), id->end());

    std::cout << std::endl;

    char ip_address[20];
    inet_ntop(AF_INET, &(address->sin_addr), ip_address, sizeof(ip_address));
    peer.address = std::string(ip_address);
    peer.port = port->getValue();

    if (*event == L"started")
    {
        state = State::CONNECTED;

        server->addPeer(info_hash, peer);
        if (left->getValue() == 0)
        {
            ++(server->completed);
        }
        else
        {
            ++(server->incompleted);
        }
    }
    else if (*event == L"stopped")
    {
        server->removePeer(info_hash, peer);
        if (left == 0)
        {
            --(server->completed);
        }
        else
        {
            --(server->incompleted);
        }

        closeConnection();

        return true;
    }
    else if (*event == L"completed")
    {
        --(server->incompleted);
        ++(server->completed);
    }

    const std::vector<Peer>& peers = server->getRandomPeers(info_hash);
    std::vector <Peer> randomPeer = std::vector <Peer> (peers.begin(), peers.end());

    for(int i = 0; i < min(randomPeer.size(), 50); ++i)
    {
        if(peer.id != randomPeer[i].id)
        {
            response.addPeer(randomPeer[i]);
        }
    }

    response.setInterval(interval);
    response.setTracker_id("0");
    response.setComplete(server->completed);
    response.setIncomplete(server->incompleted);

    sendResponse(response);

    return true;
}

void Connection::sendResponse(const TrackerResponse &response)
{
    std::string code = response.getResponse();

    write(socket, code.c_str(), code.size());
}