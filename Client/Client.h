#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QMutex>

class Client : public QObject
{
    Q_OBJECT
private:
    static Client* sigleInstance;
    static QMutex mutex;

    static const std::string configPath;
    const std::string myId;

    Client();
protected:
    const std::string createId();
public:
    static Client *getInstance();
    const std::string& getId() const;
    static const std::string& getConfigPath();
    void shareFile(const std::wstring& fileName, const std::wstring& trackerAddres, const size_t pieceSize);
signals:
    void torrentAdded();
};

#endif // CLIENT_H
