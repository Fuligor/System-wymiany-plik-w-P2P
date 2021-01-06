// Klasa przechowuj¹ca informacji o danym torrencie (pzechowuje listê peer'ów którzy s¹ po³¹czeni do trackera i sa zainteresowani danym torrentem, liczbê peer'ów posiadaj¹cych kompletny i niekompletny plik)
#ifndef TORRENT_INSTANCE_INFO_H
#define TORRENT_INSTANCE_INFO_H

#include <fstream>
#include <vector>

#include "Peer.h"

#include "../Bencode/Bencode.h"

class TorrentInstanceInfo
{
    std::vector <Peer> peers;

    uint64_t num_completed;
    uint64_t num_incompleted;
public:
    TorrentInstanceInfo();
    ~TorrentInstanceInfo();

    void addPeer(const Peer& peer, bool completed);
    void removePeer(const Peer& peer, bool completed);
    void setCompleted(const Peer& peer, bool completed);

    const uint64_t& getCompleted();
    const uint64_t& getIncompleted();

    const std::vector <Peer> getRandomPeers(const uint64_t peerCount, Peer peer);
};

#endif