#include "TrackerConnection.h"

#include <QTcpSocket>
#include <QTimer>

#include "Client.h"
#include "TorrentDownloader.h"

TrackerConnection::TrackerConnection(const std::shared_ptr <bencode::Dict>& torrentDict, quint16 listenerPort, TorrentDownloader* parent)
	:QObject(parent), mutex(new QMutex()), inActiveState(new QWaitCondition()), socket(new QTcpSocket(this)), requestTimer(new QTimer(this)), connectTimer(new QTimer(this)),
	request(torrentDict), listenerPort(listenerPort)
{
	connect(socket, SIGNAL(connected()), this, SLOT(onConnection()));
	connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(retryConnect()));
	connect(connectTimer, SIGNAL(timeout()), this, SLOT(retryConnect()));
	connect(socket, SIGNAL(readyRead()), this, SLOT(read()));
	connect(socket, SIGNAL(disconnected()), this, SLOT(reconnect()));
	connect(this, SIGNAL(statusChanged(const ConnectionStatus&)), parent, SLOT(onTrackerStatusChanged(const ConnectionStatus&)));
	connect(this, SIGNAL(peerListUpdated(bencode::List)), parent, SLOT(updatePeerList(bencode::List)));

	requestTimer->setSingleShot(true);
	connectTimer->setSingleShot(true);

	initConnection(torrentDict);
	initRequest();

	isUpdateSheduled = true;
	isTimerTimeouted = false;
}

TrackerConnection::~TrackerConnection()
{
	stopRequest();
	delete mutex;
	delete inActiveState;
	requestTimer->deleteLater();
	connectTimer->deleteLater();
}

void TrackerConnection::setLeft(size_t left)
{
	mutex->lock();
	if (left == 0)
	{
		isUpdateSheduled = false;
	}

	request.setLeft(left);
	mutex->unlock();
}

void TrackerConnection::setDownloaded(size_t downloaded)
{
	mutex->lock();
	request.setDownloaded((int) downloaded);
	mutex->unlock();
}

void TrackerConnection::setUploaded(size_t uploaded)
{
	mutex->lock();
	request.setUploaded((int) uploaded);
	mutex->unlock();
}

void TrackerConnection::initRequest()
{
	mutex->lock();
	request.setPeer_id(Client::getInstance()->getId());
	request.setPort(listenerPort);
	request.setUploaded(0);
	request.setDownloaded(0);
	request.setCompact(false);
	request.setNo_peer_id(false);
	mutex->unlock();
}

void TrackerConnection::initConnection(const std::shared_ptr <bencode::Dict>& torrentDict)
{
	bencode::String* announce = dynamic_cast <bencode::String*> ((*torrentDict)["announce"].get());

	size_t pos = announce->find(L":");

	adress = QString::fromStdWString(announce->substr(0, pos));
	port = (quint16) QString::fromStdWString(announce->substr(pos + 1)).toUInt();
}

void TrackerConnection::connectToTracker()
{
	setState(ConnectionStatus::INIT);

	socket->connectToHost(adress, port);
	connectTimer->start(10000);
}

void TrackerConnection::onConnection()
{
	connectTimer->stop();

	if (!firstConnectionTry)
	{
		disconnect(requestTimer, SIGNAL(timeout()), this, SLOT(connectToTracker()));
		requestTimer->stop();
	}

	connect(requestTimer, SIGNAL(timeout()), this, SLOT(interval()));

	startRequest();
}

void TrackerConnection::retryConnect()
{
	socket->abort();

	setState(ConnectionStatus::REFUSED);

	if (firstConnectionTry)
	{
		connect(requestTimer, SIGNAL(timeout()), this, SLOT(connectToTracker()));
	}

	requestTimer->start(30000);

	firstConnectionTry = false;
}

void TrackerConnection::reconnect()
{
	connect(socket, SIGNAL(disconnected()), this, SLOT(reconnect()));
	disconnect(requestTimer, SIGNAL(timeout()), this, SLOT(interval()));
	firstConnectionTry = true;

	connectToTracker();
}

void TrackerConnection::sendRequest()
{
	if (socket->state() == QAbstractSocket::ConnectedState)
	{
		socket->write(QByteArray::fromStdString(request.getRequest()));
		socket->flush();
	}

	setState(ConnectionStatus::AWAITING);
}

void TrackerConnection::setState(ConnectionStatus state)
{
	currentState = state;

	emit statusChanged(currentState);
}

void TrackerConnection::startRequest()
{
	mutex->lock();

	if (currentState == ConnectionStatus::INIT)
	{
		request.setEvent("started");
		sendRequest();
	}

	mutex->unlock();
}

void TrackerConnection::stopRequest()
{
	mutex->lock();

	disconnect(socket, SIGNAL(disconnected()), this, SLOT(reconnect()));

	if (currentState == ConnectionStatus::AWAITING)
	{
		inActiveState->wait(mutex);
	}
	if (currentState == ConnectionStatus::ACTIVE)
	{
		request.setEvent("stopped");
		requestTimer->stop();
		sendRequest();

		socket->disconnectFromHost();

		if (socket->state() == QAbstractSocket::ConnectedState)
		{
			if (!socket->waitForDisconnected(5000))
			{
				socket->abort();
			}
		}

		setState(ConnectionStatus::CLOSED);
		inActiveState->wakeAll();
	}

	mutex->unlock();
}

void TrackerConnection::completeRequest()
{
	mutex->lock();

	isUpdateSheduled = false;

	if (currentState == ConnectionStatus::AWAITING)
	{
		inActiveState->wait(mutex);
	}
	if (currentState == ConnectionStatus::ACTIVE)
	{
		request.setEvent("completed");
		sendRequest();
	}

	mutex->unlock();
}

void TrackerConnection::updatePeerList()
{
	mutex->lock();

	if (isTimerTimeouted)
	{
		regularRequest();
	}
	else
	{
		isUpdateSheduled = true;
	}

	mutex->unlock();
}

void TrackerConnection::regularRequest()
{
	if (currentState != ConnectionStatus::ACTIVE)
	{
		return;
	}

	request.resetEvent();
	sendRequest();
}

void TrackerConnection::interval()
{
	mutex->lock();

	if (isUpdateSheduled)
	{
		regularRequest();
	}
	else
	{
		isTimerTimeouted = true;
	}

	mutex->unlock();
}

void TrackerConnection::read()
{
	QByteArray toRead;
	toRead = socket->readAll();

	buf += utf8decoder.decode(toRead.toStdString());

	decodeResponse();
}

void TrackerConnection::decodeResponse()
{
	bencode::Dict* response;

	try
	{
		response = dynamic_cast <bencode::Dict*> (decoder.decode(buf));

		if (response == nullptr)
		{
			return;
		}

		if ((*response)["failture reason"])
		{
			std::cerr << (*response)["failture reason"]->code() << std::endl;

			return;
		}

		bencode::Int* interval = dynamic_cast <bencode::Int*> (response->at(L"interval").get());

		if (interval != nullptr)
		{
			requestTimer->setInterval(interval->getValue() * 1000);
		}

		isTimerTimeouted = false;
		requestTimer->start();

		if (response->at(L"peers"))
		{
			bencode::List peers = *(dynamic_cast <bencode::List*> (response->at(L"peers").get()));

			if (peers.size())
			{
				isUpdateSheduled = false;
				emit peerListUpdated(peers);
			}
		}

		setState(ConnectionStatus::ACTIVE);
		inActiveState->wakeOne();
	}
	catch (bencode::Exception::end_of_file())
	{
		std::cerr << "Oczekiwanie na pozosta�e fragmenty odpowiedzi..." << std::endl;
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return;
}