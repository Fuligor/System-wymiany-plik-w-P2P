//Typ wyliczeniowy przechowujący stan połączenia z trackerem
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