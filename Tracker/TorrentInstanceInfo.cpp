#include "TorrentInstanceInfo.h"

#include <algorithm>

#include <iostream>

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

        std::cout << "Usuwanie peera " + peer.id << std::endl;

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
        std::cout << "Nie znaleziono peera " + peer.id << std::endl;
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

const size_t& TorrentInstanceInfo::getCompleted()
{
    return num_completed;
}

const size_t& TorrentInstanceInfo::getIncompleted()
{
    return num_incompleted;
}

const std::vector <Peer> TorrentInstanceInfo::getRandomPeers(const size_t peerCount, Peer peer)
{
    size_t count;

    std::vector <Peer> result(peers.begin(), peers.end());

    std::vector <Peer>::iterator iterator = result.end();
    
    for(size_t i = 0; i < result.size(); ++i)
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