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

    uint64_t newPeer();
    void addPeer(const Peer& peer, bool completed);
    void removePeer(const Peer& peer, bool completed);
    void setCompleted(const Peer& peer, bool completed);
    Peer getPeer(const std::string& peerId);

    const uint64_t& getCompleted();
    const uint64_t& getIncompleted();

    const std::vector <Peer> getRandomPeers(const uint64_t peerCount, Peer peer);
};

#endif