#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class Server
{
private:
    static Server* server;

    sockaddr_in address;
    int socket_desc;
    int socketEpoll;
    int timerEpoll;
    int state;
protected:
    void bindSocket(int port);
    void initListen(unsigned int queueSize);
    void initEpoll();

    Server();
public:
    ~Server();
    
    static void create(int port, int queueSize);
    static Server* getInstance();

    int getSocketEpoll();
    int getTimerEpoll();
    bool isRunning();

    void run();
};