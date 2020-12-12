#include "Connection.h"

#include <sys/epoll.h>
#include <sys/timerfd.h>

#include <stdlib.h>
#include <unistd.h>

#include "Server.h"

Connection::Connection(const int& socket, const int& timer, int interval, sockaddr_in* address)
    :socket(socket), timer(timer), state(State::NEW), interval(interval)
{
    timerReset();
}

Connection::~Connection()
{
    closeConnection();
}

void Connection::closeConnection()
{
    if(state == State::CLOSED)
    {
        return;
    }

    state = State::CLOSED;

    printf("Klient rozłączony\n");

    close(socket);
    close(timer);

    epoll_ctl(Server::getInstance()->getSocketEpoll(), EPOLL_CTL_DEL, socket, NULL);
    epoll_ctl(Server::getInstance()->getTimerEpoll(), EPOLL_CTL_DEL, timer, NULL);
}

int Connection::getSocket()
{
    return socket;
}

void Connection::addToBuffer(const char *data, size_t size)
{
    std::string string(data, size);
    buf += utf8Decoder.decode(string);
}

bencode::Dict* Connection::getReguest()
{
    printf("Dekodowanie zapytania...\n");
    try 
    {
        bencode::Dict* result = dynamic_cast <bencode::Dict*> (decoder.decode(buf));
        timerReset();
        printf("%s\n", result->code().c_str());

        return result;
    }
    catch(bencode::Exception::end_of_file)
    {
        printf("Niepełne kodowanie... oczekiwanie na pozostałe fragmenty\n");

        return nullptr;
    }
}

bool Connection::createResponse()
{
    TrackerResponse response;
    bencode::Dict* request;

    try
    {
        request = getReguest();
    }
    catch(const std::exception& e)
    {
        printf("Niepoprawne kodowanie! Odsyłanie odpowiedzi...");

        response.setF_reason("Wrong bencoding!");

        sendResponse(response);
    }
    
    if(request == nullptr)
    {
        return false;
    }
    bencode::String* event = dynamic_cast <bencode::String*> ((*request)["event"].get());
    
    if(event == nullptr)
    {

    }
    else if(*event == L"started")
    {

    }
    else if(*event == L"stopped")
    {

    }
    else if(*event == L"completed")
    {

    }

    response.setInterval(interval);
    response.setTracker_id("0");
    response.setComplete(0);
    response.setIncomplete(0);

    sendResponse(response);

    return true;
}

void Connection::sendResponse(const TrackerResponse& response)
{
    std::string code = response.getResponse();

    write(socket, code.c_str(), code.size());
}

void Connection::timerReset()
{
    timespec now;
    itimerspec time;
    clock_gettime(CLOCK_REALTIME, &now);

    time.it_value.tv_nsec = now.tv_nsec;
    time.it_value.tv_sec = now.tv_sec + interval + 1;
    time.it_interval.tv_nsec = 0;
    time.it_interval.tv_sec = 0;

    timerfd_settime(timer, TFD_TIMER_ABSTIME, &time, NULL);
}