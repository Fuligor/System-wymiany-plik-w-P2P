#ifndef BUFFER_MANAGER_H
#define BUFFER_MANAGER_H

#include <QObject>
#include <QMutex>

#include <list>

class FileFrag;

class BufferManager 
    : public QObject
{
    Q_OBJECT
private:
    static BufferManager* sigleInstance;
    static QMutex mutex;
    int64_t bufferSpace = 512 * 1024 * 1024;

    std::list <FileFrag*> bufferedFrags;

    BufferManager();
public:
    static BufferManager* getInstance();

public slots:
    void onDataBuffered(FileFrag* fragment);
    void onDataCleared(FileFrag* fragment);
};

#endif // BUFFER_MANAGER_H
