#ifndef PEER_H
#define PEER_H
#include <string>

struct Peer
{
	std::string id;
	std::string address;
	unsigned int port;

	bool operator < (const Peer other) const
	{
		return id < other.id;
	}
};
#endif