#ifndef TORRENT_DOWNLOAD_STATUS
#define TORRENT_DOWNLOAD_STATUS
#include <stddef.h>

#include <QDateTime>
#include <QTime>

#include "ConnectionStatus.h"

struct TorrentDownloadStatus
{
	size_t downloaded;
	size_t uploaded;
	size_t downloadedSinceStart;
	double downloadSpeed;
	double uploadSpeed;
	QTime estimatedEndTime;
	QDateTime startTime;
	std::wstring fileName;
	size_t fileSize;
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
