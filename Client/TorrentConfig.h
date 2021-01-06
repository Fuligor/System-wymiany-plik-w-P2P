//struktura przechowuj¹ca stan torrenta wykorzystywana przez klasê Torrent
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

	TorrentConfig(const uint64_t& pieces)
		:pieceStatus(pieces)
	{}
};
#endif