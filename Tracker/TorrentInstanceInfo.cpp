#include "TorrentInstanceInfo.h"

#include <algorithm>

#include <iostream>

#include "Connection.h"

TorrentInstanceInfo::TorrentInstanceInfo()
    :num_completed(0), num_incompleted(0)
{
}

TorrentInstanceInfo::~TorrentInstanceInfo()
{

}

void TorrentInstanceInfo::addPeer(const Peer& peer, bool completed)
{
    for(uint64_t i = 0; i < peers.size(); ++i)
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
    
    for(uint64_t i = 0; i < peers.size(); ++i)
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

        std::cout << "Usuwanie peera " + Connection::getReadablePeerId(peer.id) << std::endl;

        if(completed)
        {
            --num_completed;
        }
        else
        {
            --num_incompleted;
        }
    }
    else
    {
        std::cout << "Nie znaleziono peera " + Connection::getReadablePeerId(peer.id) << std::endl;
    }
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

const uint64_t& TorrentInstanceInfo::getCompleted()
{
    return num_completed;
}

const uint64_t& TorrentInstanceInfo::getIncompleted()
{
    return num_incompleted;
}

const std::vector <Peer> TorrentInstanceInfo::getRandomPeers(const uint64_t peerCount, Peer peer)
{
    uint64_t count;

    std::vector <Peer> result(peers.begin(), peers.end());

    std::vector <Peer>::iterator iterator = result.end();
    
    for(uint64_t i = 0; i < result.size(); ++i)
    {
        if(result[i].id == peer.id)
        {
            iterator = result.begin() + i;

            break;
        }
    }

    if(iterator != result.end())
    {
        result.erase(iterator);
    }

    if(peerCount < result.size())
    {
        count = peerCount;
    }
    else
    {
        count = result.size();
    }

    std::random_shuffle(result.begin(), result.end());

    return std::vector <Peer> (result.begin(), result.begin() + count);
}