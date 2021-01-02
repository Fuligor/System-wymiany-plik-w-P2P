#include "TorrentInstanceInfo.h"

#include <algorithm>

TorrentInstanceInfo::TorrentInstanceInfo()
    :num_completed(0), num_incompleted(0)
{
}

TorrentInstanceInfo::~TorrentInstanceInfo()
{

}

void TorrentInstanceInfo::addPeer(const Peer& peer, bool completed)
{
    for(size_t i = 0; i < peers.size(); ++i)
    {
        if(peers[i].id == peer.id)
        {
            return;
        }
    }

    peers.push_back(peer);

    if(completed)
    {
        ++num_completed;
    }
    else
    {
        ++num_incompleted;
    }
}

void TorrentInstanceInfo::removePeer(const Peer& peer, bool completed)
{
    std::vector <Peer>::iterator iterator = peers.end();
    
    for(size_t i = 0; i < peers.size(); ++i)
    {
        if(peers[i].id == peer.id)
        {
            iterator = peers.begin() + i;

            break;
        }
    }

    if(iterator != peers.end())
    {
        peers.erase(iterator);
    }

    if(completed)
    {
        --num_completed;
    }
    else
    {
        --num_incompleted;
    }
}

Peer* TorrentInstanceInfo::getPeer(const std::string& peerId)
{
    for(size_t i = 0; i < peers.size(); ++i)
    {
        if(peers[i].id == peerId)
        {
            return &peers[i];
        }
    }

    return nullptr;
}

void TorrentInstanceInfo::setCompleted(const Peer& peer, bool completed)
{
    if(completed)
    {
        --num_incompleted;
        ++num_completed;
    }
    else
    {
        ++num_incompleted;
        --num_completed;
    }

    return;
}

const size_t& TorrentInstanceInfo::getCompleted()
{
    return num_completed;
}

const size_t& TorrentInstanceInfo::getIncompleted()
{
    return num_incompleted;
}

const std::vector <Peer> TorrentInstanceInfo::getRandomPeers(const size_t peerCount)
{
    std::random_shuffle(peers.begin(), peers.end());

    size_t count;

    if(peerCount < peers.size())
    {
        count = peerCount;
    }
    else
    {
        count = peers.size();
    }

    std::random_shuffle(peers.begin(), peers.end());

    std::vector <Peer> result(peers.begin(), peers.begin() + count);

    return result;
}