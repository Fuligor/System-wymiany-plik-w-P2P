#ifndef TORRENT_FILE_STATUS
#define TORRENT_FILE_STATUS

#include <string>

#include "Bencode.h"

#include "BitSet.h"

struct TorrentFileStatus
{
	BitSet pieceStatus;
	std::wstring torrentPath;
	std::wstring downloadPath;

	TorrentFileStatus(const size_t& pieces)
		:pieceStatus(pieces)
	{}
};
#endif