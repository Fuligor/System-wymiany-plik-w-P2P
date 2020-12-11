#include "trackerconnection.h"

#include <QTcpSocket>
#include <QTimer>

TrackerConnection::TrackerConnection(const std::string& fileName)
	:socket(new QTcpSocket(this)), requestTimer(new QTimer(this)), torrent(fileName), request(torrent.getDict())
{
	connect(socket, SIGNAL(connected()), this, SLOT(start()));
	connect(requestTimer, SIGNAL(timeout()), this, SLOT(interval()));

	connectToTracker();
	initRequest();
}

TrackerConnection::~TrackerConnection()
{
	stop();
	socket->disconnect();
}

void TrackerConnection::initRequest()
{
	request.setPeer_id("0");
	request.setPort(0);
	request.setUploaded(0);
	request.setDownloaded(0);
	request.setCompact(false);
	request.setNo_peer_id(false);
}

void TrackerConnection::connectToTracker()
{
	bencode::Dict& torrentDict = (*torrent.getDict());

	bencode::String* announce = dynamic_cast <bencode::String*> (torrentDict["announce"].get());

	size_t pos = announce->find(L":");

	QString adress = QString::fromStdWString(announce->substr(0, pos));
	quint16 port = (quint16) QString::fromStdWString(announce->substr(pos + 1)).toUInt();

	socket->connectToHost(adress, port);
}

void TrackerConnection::sendRequest()
{
	QByteArray data = QByteArray::fromStdString(request.getRequest());
	std::cout << data.size() << " " << request.getRequest().size() << std::endl;

	socket->write(data);
	socket->flush();
}

void TrackerConnection::start()
{
	if (!isActive)
	{
		isActive = true;
		request.setEvent("started");
		sendRequest();
	}
}

void TrackerConnection::stop()
{
	if (isActive)
	{
		isActive = false;
		request.setEvent("stopped");
		requestTimer->stop();
		sendRequest();
	}
}

void TrackerConnection::completed()
{
	request.setEvent("completed");
	sendRequest();
}

void TrackerConnection::interval()
{
	request.resetEvent();
	sendRequest();
	requestTimer->start();
}

void TrackerConnection::read()
{

}
