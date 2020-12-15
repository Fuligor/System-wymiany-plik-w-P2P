#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <vector>
#include <unordered_map>

struct Peer;

class Server
{
private:
    static Server* server;

    sockaddr_in address;
    int socket_desc;
    int socketEpoll;
    int state;

    std::unordered_map <std::wstring, std::vector <Peer>> peers;
public:
    size_t completed = 0;
    size_t incompleted = 0;
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
    bool isRunning();

    void addPeer(const std::wstring* info_hash, const Peer& peer);
    void removePeer(const std::wstring* info_hash, const Peer& peer);
    const std::vector <Peer>& getRandomPeers(const std::wstring *info_hash); 

    void run();
};