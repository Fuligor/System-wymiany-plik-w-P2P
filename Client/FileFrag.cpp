#include "FileFrag.h"

#include <QFile>
#include <QCryptographicHash>

#include "BufferManager.h"

FileFrag::FileFrag(QFile* file, const unsigned int position, uint64_t size, QMutex* mutex, QObject* parent)
	: QObject(parent), mutex(mutex), mFile(file), mPosition(position), mSize(size)
{
	connect(this, SIGNAL(dataBuffered(FileFrag*)), BufferManager::getInstance(), SLOT(onDataBuffered(FileFrag*)));
	connect(this, SIGNAL(dataCleared(FileFrag*)), BufferManager::getInstance(), SLOT(onDataCleared(FileFrag*)));
}

FileFrag::~FileFrag()
{
	free();
}

void FileFrag::setData(std::string frag)
{
	mutex->lock();
	mFile->seek(mPosition);
	mFile->write(frag.data(), frag.size());
	mutex->unlock();
}

const QByteArray FileFrag::getHash()
{
	return QCryptographicHash::hash(getData(), QCryptographicHash::Algorithm::Sha1);
}

const QByteArray& FileFrag::getData()
{
	if (dataBuffer.isEmpty())
	{
		mutex->lock();
		mFile->seek(mPosition);
		dataBuffer = mFile->read(mSize);
		mutex->unlock();

		emit dataBuffered(this);
	}

	return dataBuffer;
}

const uint64_t FileFrag::getSize() const
{
	return mSize;
}

void FileFrag::free()
{
	if (!dataBuffer.isEmpty())
	{
		dataBuffer.clear();
		emit dataCleared(this);
	}
}
