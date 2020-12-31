#ifndef CONNECTION_STATUS_H
#define CONNECTION_STATUS_H

enum class ConnectionStatus
{
    INIT,
    REFUSED,
    ACTIVE,
    AWAITING,
    CLOSED
};
#endif