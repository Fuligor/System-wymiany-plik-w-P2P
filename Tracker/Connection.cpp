#include "Connection.h"

#include <sys/epoll.h>

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

    std::cout << "Klient rozłączony\n";
    epoll_ctl(Server::getInstance()->getSocketEpoll(), EPOLL_CTL_DEL, socket, NULL);

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
    buf += utf8Decoder.decode(string);
}

bencode::Dict *Connection::getRequest()
{
    char ip_address[60];
    inet_ntop(AF_INET, &(address->sin_addr), ip_address, sizeof(ip_address));
    std::cout << ip_address << ": Dekodowanie zapytania...\n";

    try
    {
        bencode::Dict *result = dynamic_cast<bencode::Dict *>(decoder.decode(buf));

        return result;
    }
    catch (bencode::Exception::end_of_file& e)
    {

        std::cerr << ip_address << ": Niepełne kodowanie... oczekiwanie na pozostałe fragmenty\n";

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
        request = getRequest();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Niepoprawne kodowanie! Odsyłanie odpowiedzi...\n";

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
    }

    std::shared_ptr <bencode::Int> left = std::dynamic_pointer_cast<bencode::Int>((*request)["left"]);

    Peer temp_peer = readPeerInfo(*request);

    if(info_hash.get() == nullptr)
    {
        info_hash = temp_info_hash;
    }
    else if (*info_hash != *temp_info_hash)
    {
        std::cerr << "Niepoprawny hash pliku...\n";

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
            peer = temp_peer;
        }
        else if (*event == L"stopped")
        {
            std::cout << "Odebrano wiadomość " << event->code() << " od klienta o id " << getReadablePeerId(peer.id) << std::endl;
            
            closeConnection();

            return true;
        }
        else if (*event == L"completed")
        {
            completed = true;

            server->getTorrentInfo(info_hash.get()).setCompleted(peer, completed);
        }
        else
        {
            std::cout << "Odebrano niepoprawny typ wiadomości od klienta o id " << getReadablePeerId(peer.id) << std::endl;

            response.setF_reason("Wrong message type!");

            sendResponse(response);
            
            return false;
        }

        std::cout << "Odebrano wiadomość " << event->code() << " od klienta o id " << getReadablePeerId(peer.id) << std::endl;
    }
    else
    {
        std::cout << "Odebrano regularną wiadomość od klienta o id " << getReadablePeerId(peer.id) << std::endl;
    }

    if(peer.id != temp_peer.id || peer.address != temp_peer.address || peer.port != temp_peer.port)
    {
        peer.id = temp_peer.id;
        peer.address = temp_peer.address;
        peer.port = temp_peer.port;
    }


    if(peer.id.empty())
    {
        std::cerr << "Oczekiwano wysłania wiadomości started...\n";

        response.setF_reason("Expecting started!");

        sendResponse(response);

        return false;
    }

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

    std::cout << "Odsyłanie odpowiedzi:" << std::endl;
    std::cout << response.getResponse() << std::endl;

    return sendResponse(response);
}


Peer Connection::readPeerInfo(bencode::Dict &request)
{
    std::shared_ptr <bencode::Int> port = std::dynamic_pointer_cast<bencode::Int>(request["port"]);

    Peer temp_peer;

    temp_peer.id = request["peer_id"]->code().substr(3);

    char ip_address[60];
    inet_ntop(AF_INET, &(address->sin_addr), ip_address, sizeof(ip_address));
    temp_peer.address = std::string(ip_address);
    temp_peer.port = port->getValue();

    return temp_peer;
}

bool Connection::sendResponse(const TrackerResponse &response)
{
    std::string code = response.getResponse();

    int writen = write(socket, code.c_str(), code.size());

    if(writen < 0)
    {
        std::cout << "Uszkodzony socket, zamykanie połączenia\n";

        closeConnection();

        return false;
    }

    while(writen < (int) code.size())
    {
        int temp = write(socket, code.substr(writen).c_str(), code.size() - writen);
        
        if(temp < 0)
        {
            std::cout << "Uszkodzony socket, zamykanie połączenia\n";

            closeConnection();

            return false;
        }

        writen += temp;
    }

    return true;
}

const char Connection::toHexEncoding(const unsigned char& number)
{
	if (number < 10)
	{
		return '0' + number;
	}
	if (number < 16)
	{
		return 'a' + (number - (char) 10);
	}

	return '0';
}

std::string Connection::getReadablePeerId(std::string peerId)
{
    if(peerId.size() < 4)
    {
        return "";
    }

    std::string result = peerId.substr(0, 4);

	for (uint64_t i = 4; i < peerId.size(); ++i)
	{
		result += toHexEncoding((unsigned char) peerId[i] / 16);
		result += toHexEncoding((unsigned char) peerId[i] % 16);
	}

	return result;
}
