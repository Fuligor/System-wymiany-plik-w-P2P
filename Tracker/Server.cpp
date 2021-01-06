#include "Server.h"

#include <sys/epoll.h>

#include <pthread.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <stdexcept>

#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <iostream>

#include "Connection.h"
#include "Peer.h"

void *readData(void *data)
{
    pthread_detach(pthread_self());
    epoll_event event;
    int epoll = Server::getInstance()->getSocketEpoll();

    Connection *connection;

    char buf[2000];
    int size;

    while (Server::getInstance()->isRunning())
    {
        epoll_wait(epoll, &event, 1, -1); //1 = maksymalnie jedno zdarzenie, -1 = bez timeout
        if (event.events & EPOLLIN)
        {
            connection = (Connection *)event.data.ptr;
            size = read(connection->getSocket(), buf, sizeof(buf));

            if (size <= 0)
            {
                perror("Socket");

                delete connection;

                continue;
            }

            connection->addToBuffer(buf, size);
            connection->createResponse();
        }
    }

    pthread_exit(NULL);
}

struct acceptConnection_data_t
{
    int socketEpoll;
    int socket_desc;
    sockaddr_in clientAddress;
    socklen_t clientAddressSize;
    epoll_event event;
};

void *acceptConnection(void *data)
{
    pthread_detach(pthread_self());

    int connection_socket_descriptor;

    acceptConnection_data_t* acceptConnection_data = (acceptConnection_data_t*) data;
    int socketEpoll = acceptConnection_data->socketEpoll;
    int socket_desc = acceptConnection_data->socket_desc;
    sockaddr_in clientAddress = acceptConnection_data->clientAddress;
    socklen_t clientAddressSize = acceptConnection_data->clientAddressSize;
    epoll_event event = acceptConnection_data->event;

    while (Server::getInstance()->isRunning())
    {
        connection_socket_descriptor = accept(socket_desc, (sockaddr *)&clientAddress, &clientAddressSize);
        if (connection_socket_descriptor < 0)
        {
            std::cerr << "Błąd przy próbie utworzenia gniazda dla połączenia.\n";
            exit(1);
        }

        fcntl(connection_socket_descriptor, F_SETFL, O_NONBLOCK, 1);

        event.data.ptr = new Connection(connection_socket_descriptor, 120, &clientAddress);

        epoll_ctl(socketEpoll, EPOLL_CTL_ADD, connection_socket_descriptor, &event);

        std::cout << "Połączono z klientem\n";
    }

    pthread_exit(NULL);
}

void Server::bindSocket(int port)
{
    static const char reuse_addr_val = 1;
    int bind_result;

    memset(&address, 0, sizeof(struct sockaddr));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port);

    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc < 0)
    {
        std::cerr << "Błąd przy próbie utworzenia gniazda..\n";
        exit(1);
    }
    setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse_addr_val, sizeof(reuse_addr_val));

    bind_result = bind(socket_desc, (sockaddr *)&address, sizeof(address));

    if (bind_result < 0)
    {
        std::cerr << "Błąd przy próbie dowiązania adresu IP i numeru portu do gniazda.\n";
        exit(1);
    }
}

void Server::initListen(unsigned int queueSize)
{
    int listen_result = listen(socket_desc, queueSize);
    if (listen_result < 0)
    {
        std::cerr << "Błąd przy próbie ustawienia wielkości kolejki.\n";
        exit(1);
    }
}

void Server::initEpoll()
{
    socketEpoll = epoll_create1(0);

    if (socketEpoll < 0)
    {
        std::cerr << "Błąd przy próbie utworzenie epoll\n";
        exit(-1);
    }
}

Server::Server()
    : configPath("config"), state(0)
{
    std::srand(time(NULL));
}

Server::~Server()
{
    server = nullptr;

    close(socketEpoll);
}

void Server::create(int port, int queueSize)
{
    if (server != nullptr)
    {
        std::cerr << "Próba ponownego utworzenia instancji serwera! Zamykanie aplikacji.\n";
        throw std::invalid_argument("Server is already created!");
    }

    server = new Server();

    server->bindSocket(port);
    server->initListen(queueSize);
    server->initEpoll();

    std::cout << "Nasłuchowanie na porcie " << port << "..." << std::endl;

    return;
}

Server *Server::getInstance()
{
    if (server == nullptr)
    {
        std::cerr << "Nie utworzono instancji serwera! Zamykanie aplikacji.\n";
        throw std::invalid_argument("Server wasn't created!");
    }

    return server;
}

int Server::getSocketEpoll()
{
    return socketEpoll;
}

bool Server::isRunning()
{
    return state & 1;
}

void Server::run()
{
    state = state | 1;

    int create_result = 0;

    epoll_event event;
    memset(&event, 0, sizeof(event));
    event.events = EPOLLIN;

    sockaddr_in clientAddress;
    memset(&clientAddress, 0, sizeof(sockaddr));
    socklen_t clientAddressSize = sizeof(clientAddress);

    acceptConnection_data_t* acceptConnection_data = new acceptConnection_data_t();

    acceptConnection_data->socketEpoll = socketEpoll;
    acceptConnection_data->socket_desc = socket_desc;
    acceptConnection_data->clientAddress = clientAddress;
    acceptConnection_data->clientAddressSize = clientAddressSize;
    acceptConnection_data->event = event;

    pthread_t thread1;
    create_result = pthread_create(&thread1, NULL, readData, NULL);
    if (create_result)
    {
        std::cerr << "Błąd przy próbie utworzenia wątku, kod błędu: %d\n";
        exit(-1);
    }
    
    pthread_t thread2;
    create_result = pthread_create(&thread2, NULL, acceptConnection, (void *) acceptConnection_data);
    if (create_result)
    {
        std::cerr << "Błąd przy próbie utworzenia wątku, kod błędu: %d\n";
        exit(-1);
    }

    std::cout << "Serwer uruchomiony\n";

    while (isRunning())
    {
        std::string buf;

        std::cin >> buf;

        if(buf == "exit")
        {
            Server::getInstance()->stop();
        }
    }

    close(socket_desc);
}

void Server::stop()
{
    if(state & 1)
    {
        state = state ^ 1;
    }

    return;
}

TorrentInstanceInfo& Server::getTorrentInfo(const std::wstring *info_hash)
{
    return TorrentInstances[*info_hash];
}

Server *Server::server = nullptr;