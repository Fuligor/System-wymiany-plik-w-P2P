#include "Connection.h"

#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <vector>

#include <memory>

#include "Server.h"

int min(int a, int b)
{
    return a < b? a : b;
}

Connection::Connection(const int &socket, int interval, sockaddr_in *address)
    : state(State::NEW), socket(socket), interval(interval), address(address), info_hash(nullptr), peer(), completed(false)
{
}

Connection::~Connection()
{
    closeConnection();
}

void Connection::closeConnection()
{
    if(info_hash != nullptr)
    {
        Server::getInstance()->getTorrentInfo(info_hash.get()).removePeer(peer, completed);
    }
    else
    {
        std::cout << "Pusty info hash" << std::endl;
    }
    
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

void Connection::addToBuffer(const char *data, uint64_t size)
{
    std::wcout << buf << std::endl;
    std::string string(data, size);
    printf("Readed %lu\n", string.size());
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
    catch (bencode::Exception::end_of_file& e)
    {
        printf("Niepełne kodowanie... oczekiwanie na pozostałe fragmenty\n");

        return nullptr;
    }
}

bool Connection::createResponse()
{
    TrackerResponse response;
    response.setInterval(interval);

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

    std::shared_ptr <bencode::String> temp_info_hash = std::dynamic_pointer_cast<bencode::String> ((*request)["info_hash"]);
    std::shared_ptr <bencode::String> event;

    if((*request)["event"] != nullptr)
    {
        event = std::dynamic_pointer_cast<bencode::String> ((*request)["event"]);
        std::cout << event->code() << std::endl;
    }

    std::shared_ptr <bencode::Int> left = std::dynamic_pointer_cast<bencode::Int>((*request)["left"]);

    Peer temp_peer = readPeerInfo(*request);

    if(info_hash.get() == nullptr)
    {
        info_hash = temp_info_hash;
    }
    else if (*info_hash != *temp_info_hash)
    {
        printf("Niepoprawny hash pliku...");

        response.setF_reason("Wrong info hash!");

        sendResponse(response);

        return false;
    }

    if (event != nullptr)
    {
        if (*event == L"started")
        {
            state = State::CONNECTED;

            completed = (left->getValue() == 0);

            server->getTorrentInfo(info_hash.get()).addPeer(temp_peer, completed);
        }
        else if (*event == L"stopped")
        {
            closeConnection();

            return true;
        }
        else if (*event == L"completed")
        {
            completed = true;

            server->getTorrentInfo(info_hash.get()).setCompleted(peer, completed);
        }
    }

    if(peer.id != temp_peer.id || peer.address != temp_peer.address || peer.port != temp_peer.port)
    {
        peer.id = temp_peer.id;
        peer.address = temp_peer.address;
        peer.port = temp_peer.port;
    }


    if(peer.id.empty())
    {
        printf("Oczekiwano wysłania wiadomości started...");

        response.setF_reason("Expecting started!");

        sendResponse(response);

        return false;
    }

    std::cout << completed << std::endl;

    if(!completed)
    {
        const std::vector<Peer> peers = server->getTorrentInfo(info_hash.get()).getRandomPeers(50, peer);

        for(uint64_t i = 0; i < peers.size(); ++i)
        {
            response.addPeer(peers[i]);
        }
    }

    response.setComplete(server->getTorrentInfo(info_hash.get()).getCompleted());
    response.setIncomplete(server->getTorrentInfo(info_hash.get()).getIncompleted());

    std::cout << response.getResponse() << std::endl;

    sendResponse(response);

    return true;
}


Peer Connection::readPeerInfo(bencode::Dict &request)
{
    std::shared_ptr <bencode::Int> port = std::dynamic_pointer_cast<bencode::Int>(request["port"]);

    std::cout << request["info_hash"]->code() << std::endl;
    std::cout << request["peer_id"]->code() << std::endl;
    std::cout << request["port"]->code() << std::endl;
    std::cout << request["left"]->code() << std::endl;

    Peer temp_peer;

    temp_peer.id = request["peer_id"]->code().substr(3);

    std::cout << "Peer id: " << temp_peer.id << std::endl;

    char ip_address[60];
    inet_ntop(AF_INET, &(address->sin_addr), ip_address, sizeof(ip_address));
    temp_peer.address = std::string(ip_address);
    temp_peer.port = port->getValue();

    return temp_peer;
}

void Connection::sendResponse(const TrackerResponse &response)
{
    std::string code = response.getResponse();

    uint64_t writen = write(socket, code.c_str(), code.size());

    while(writen < code.size())
    {
        writen += write(socket, code.substr(writen).c_str(), code.size() - writen);
    }
}