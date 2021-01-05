#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QMutex>
#include <qthread.h>
class TorrentFile;
class Client : public QObject
{
    Q_OBJECT
private:
    static Client* sigleInstance;
    static QMutex mutex;
    QThread thread;

    static const std::string configPath;
    const std::string myId;

    Client();
protected:
    const std::string createId();
public:
    static Client *getInstance();
    const std::string& getId() const;
    static const std::string& getConfigPath();
    void shareFile(const std::string& fileName, const std::string& trackerAddres, const size_t pieceSize);
    void downloadFile(const std::string& torrentPath, const std::string& downloadPath);
public slots:
    void onFileCreated(TorrentFile*);
signals:
    void torrentAdded();
    void fileShared();
    void onFileShare();
    void wrongFile(std::string);
};

#endif // CLIENT_H
