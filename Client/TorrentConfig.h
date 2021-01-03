#ifndef TORRENT_CONFIG_H
#define TORRENT_CONFIG_H

#include <string>

#include "Bencode.h"

#include "BitSet.h"

struct TorrentConfig
{
	BitSet pieceStatus;
	std::string torrentPath;
	std::string downloadPath;

	TorrentConfig(const size_t& pieces)
		:pieceStatus(pieces)
	{}
};
#endif