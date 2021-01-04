#include "FileFrag.h"

#include <QFile>
#include <QCryptographicHash>

FileFrag::FileFrag(QFile* file, const unsigned int position, size_t size, QMutex* mutex, QObject* parent)
	: QObject(parent), mFile(file), mPosition(position), mSize(size), mutex(mutex)
{
}

FileFrag::~FileFrag()
{
}

void FileFrag::setData(std::string frag)
{
	mutex->lock();
	mFile->seek(mPosition);
	mFile->write(frag.data(), frag.size());
	mutex->unlock();
}

const QByteArray FileFrag::getHash() const
{
	return QCryptographicHash::hash(getData(), QCryptographicHash::Algorithm::Sha1);
}

const QByteArray FileFrag::getData() const
{
	mutex->lock();
	mFile->seek(mPosition);
	QByteArray mData = mFile->read(mSize);
	mutex->unlock();

	return mData;
}
