#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "Server.h"

int main(int argc, char *argv[])
{
    if(argc <= 1)
    {
        std::cerr << "Proszę podać numer portu jako pierwszy parametr!" << std::endl;

        return 1; 
    }

    const unsigned int port = atoi(argv[1]);
    const unsigned int queueSize = 500;

    Server::create(port, queueSize);
    Server *server = Server::getInstance();

    server->run();

    delete server;

    return (0);
}
