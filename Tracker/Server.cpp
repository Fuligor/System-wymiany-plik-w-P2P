#include "Server.h"

#include <sys/epoll.h>

#include <pthread.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <stdexcept>

#include <ctime>
#include <cstdlib>
#include <algorithm>

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
        fprintf(stderr, "Błąd przy próbie utworzenia gniazda..\n");
        exit(1);
    }
    setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse_addr_val, sizeof(reuse_addr_val));

    bind_result = bind(socket_desc, (sockaddr *)&address, sizeof(address));

    if (bind_result < 0)
    {
        fprintf(stderr, "Błąd przy próbie dowiązania adresu IP i numeru portu do gniazda.\n");
        exit(1);
    }
}

void Server::initListen(unsigned int queueSize)
{
    int listen_result = listen(socket_desc, queueSize);
    if (listen_result < 0)
    {
        fprintf(stderr, "Błąd przy próbie ustawienia wielkości kolejki.\n");
        exit(1);
    }
}

void Server::initEpoll()
{
    socketEpoll = epoll_create1(0);

    if (socketEpoll < 0)
    {
        fprintf(stderr, "Błąd przy próbie utworzenie epoll");
        exit(-1);
    }
}

Server::Server()
    : state(0), configPath("config")
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
        throw std::invalid_argument("Server is already created!");
    }

    server = new Server();

    server->bindSocket(port);
    server->initListen(queueSize);
    server->initEpoll();

    return;
}

Server *Server::getInstance()
{
    if (server == nullptr)
    {
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

    int connection_socket_descriptor;
    int connection_timer;
    int create_result = 0;

    epoll_event event;
    memset(&event, 0, sizeof(event));
    event.events = EPOLLIN;

    pthread_t thread1;
    create_result = pthread_create(&thread1, NULL, readData, NULL);
    if (create_result)
    {
        fprintf(stderr, "Błąd przy próbie utworzenia wątku, kod błędu: %d\n", create_result);
        exit(-1);
    }

    sockaddr_in clientAddress;
    memset(&clientAddress, 0, sizeof(sockaddr));
    socklen_t clientAddressSize = sizeof(clientAddress);

    while (isRunning())
    {
        connection_socket_descriptor = accept(socket_desc, (sockaddr *)&clientAddress, &clientAddressSize);
        if (connection_socket_descriptor < 0)
        {
            fprintf(stderr, "%s: Błąd przy próbie utworzenia gniazda dla połączenia.\n");
            exit(1);
        }

        fcntl(connection_socket_descriptor, F_SETFL, O_NONBLOCK, 1);

        event.data.ptr = new Connection(connection_socket_descriptor, 120, &clientAddress);

        epoll_ctl(socketEpoll, EPOLL_CTL_ADD, connection_socket_descriptor, &event);

        printf("Połączono z klientem\n");
    }

    close(socket_desc);
}

TorrentInstanceInfo& Server::getTorrentInfo(const std::wstring *info_hash)
{
    return TorrentInstances[*info_hash];
}

Server *Server::server = nullptr;