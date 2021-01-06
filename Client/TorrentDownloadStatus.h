//struktura przechowuj¹ca stan pobierana i udostêpniania pliku
#ifndef TORRENT_DOWNLOAD_STATUS
#define TORRENT_DOWNLOAD_STATUS
#include <stddef.h>

#include <QElapsedTimer>
#include <QTime>

#include "FileSize.h"

struct TorrentDownloadStatus
{
	FileSize downloaded;
	FileSize uploaded;
	FileSize downloadedSinceStart;
	FileSize downloadSpeed;
	FileSize uploadSpeed;
	QTime estimatedEndTime;
	QElapsedTimer startTime;
	std::wstring fileName;
	FileSize fileSize;
	uint64_t connectionCount;
	mutable bool isActualVisable = false;
	enum class State
	{
		CONNECTING_TO_TRACKER,
		TRACKER_CONNECTION_REFUSED,
		LEECHING,
		SEEDING,
		CLOSED
	} connectionState;
};
#endif // !TORRENT_DOWNLOAD_STATUS
