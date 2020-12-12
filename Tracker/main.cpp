#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "Server.h"

int main(int argc, char *argv[])
{
    const unsigned int port = 1234;
    const unsigned int queueSize = 500;

    Server::create(port, queueSize);
    Server *server = Server::getInstance();

    server->run();

    delete server;

    return (0);
}
