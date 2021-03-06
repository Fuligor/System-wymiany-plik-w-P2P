// Klasa typu singleton odpowiedzialna za uruchomienie serwera i przyjmowanie nowych po��cze�
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <cstdint>

#include <unordered_map>

#include "TorrentInstanceInfo.h"

struct Peer;

class Server
{
private:
    static Server* server;
  

    sockaddr_in address;
    int socket_desc;
    int socketEpoll;
    int state;

    std::unordered_map <std::wstring, TorrentInstanceInfo> TorrentInstances;
protected:
    void bindSocket(int port);
    void initListen(unsigned int queueSize);
    void initEpoll();

    Server();
public:
    ~Server();
    // Metoda tworz�ca instancj� serwera
    static void create(int port, int queueSize);
    static Server* getInstance();

    int getSocketEpoll();
    bool isRunning();

    TorrentInstanceInfo& getTorrentInfo(const std::wstring *info_hash); 

    void run();
    void stop();

};