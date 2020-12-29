#include "TrackerConnection.h"

#include <QTcpSocket>
#include <QTimer>

#include "Client.h"

TrackerConnection::TrackerConnection(const std::shared_ptr <bencode::Dict>& torrentDict, QObject* parent)
	:QObject(parent), socket(new QTcpSocket(this)), requestTimer(new QTimer(this)), mutex(new QMutex()), inActiveState(new QWaitCondition()), request(torrentDict)
{
	connect(socket, SIGNAL(connected()), this, SLOT(startRequest()));
	connect(requestTimer, SIGNAL(timeout()), this, SLOT(interval()));
	connect(socket, SIGNAL(readyRead()), this, SLOT(stopRequest()));

	requestTimer->setSingleShot(true);
	currentState = State::INIT;
	connectToTracker(torrentDict);
	initRequest();

	isUpdateSheduled = false;
	isTimerTimeouted = false;
}

TrackerConnection::~TrackerConnection()
{
	stopRequest();
	delete mutex;
	delete inActiveState;
}

void TrackerConnection::initRequest()
{
	mutex->lock();
	request.setPeer_id(Client::getInstance()->getId());
	request.setPort(0);
	request.setUploaded(0);
	request.setDownloaded(0);
	request.setCompact(false);
	request.setNo_peer_id(false);
	mutex->unlock();
}

void TrackerConnection::connectToTracker(const std::shared_ptr <bencode::Dict>& torrentDict)
{
	bencode::String* announce = dynamic_cast <bencode::String*> ((*torrentDict)["announce"].get());

	size_t pos = announce->find(L":");

	QString adress = QString::fromStdWString(announce->substr(0, pos));
	quint16 port = (quint16) QString::fromStdWString(announce->substr(pos + 1)).toUInt();

	socket->connectToHost(adress, port);
}

void TrackerConnection::sendRequest()
{
	socket->write(QByteArray::fromStdString(request.getRequest()));
	std::cerr << request.getRequest() << std::endl;
	socket->flush();

	currentState = State::AWAITING;
}

void TrackerConnection::startRequest()
{
	mutex->lock();

	if (currentState == State::INIT)
	{
		request.setEvent("started");
		sendRequest();
	}

	mutex->unlock();
}

void TrackerConnection::stopRequest()
{
	mutex->lock();

	if (currentState == State::AWAITING)
	{
		inActiveState->wait(mutex);
	}
	if (currentState == State::ACTIVE)
	{
		request.setEvent("stopped");
		requestTimer->stop();
		sendRequest();
		socket->waitForDisconnected();
		currentState = State::CLOSED;
		inActiveState->wakeAll();
	}

	mutex->unlock();
}

void TrackerConnection::completeRequest()
{
	mutex->lock();

	if (currentState == State::AWAITING)
	{
		inActiveState->wait(mutex);
	}
	if (currentState == State::ACTIVE)
	{
		request.setEvent("completed");
		sendRequest();
		socket->waitForDisconnected();
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
	mutex->lock();

	if (currentState != State::ACTIVE)
	{
		return;
	}

	request.resetEvent();
	sendRequest();
	mutex->unlock();
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

	std::wcerr << buf << std::endl;

	try
	{
		response = dynamic_cast <bencode::Dict*> (decoder.decode(buf));

		if (response == nullptr)
		{
			return;
		}

		std::cout << response->code() << std::endl;

		bencode::Int* interval = dynamic_cast <bencode::Int*> (response->at(L"interval").get());

		if (interval != nullptr)
		{
			requestTimer->setInterval(interval->getValue() * 1000);
		}

		isUpdateSheduled = false;
		isTimerTimeouted = false;
		requestTimer->start();

		if (response->at(L"peers"))
		{
			bencode::List peers = *(dynamic_cast <bencode::List*> (response->at(L"peers").get()));

			emit peerListUpdated(peers);
		}

		currentState = State::ACTIVE;
		inActiveState->wakeOne();

		std::wcerr << buf << std::endl;
	}
	catch (bencode::Exception::end_of_file())
	{
		std::cerr << "Oczekiwanie na pozosta³e fragmenty odpowiedzi..." << std::endl;
	}
	catch (std::exception e)
	{
		std::cerr << e.what() << std::endl;
	}

	return;
}