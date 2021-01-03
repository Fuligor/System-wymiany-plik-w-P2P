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

void FileFrag::free() const
{
	mHash.clear();
	mData.clear();
}

void FileFrag::setData(std::string frag)
{
	mutex->lock();
	mFile->seek(mPosition);
	mFile->write(frag.data(), frag.size());
	mutex->unlock();
}

const QByteArray& FileFrag::getHash() const
{
	getData();
	if(!mIsHashCalculated)
	{
		mIsHashCalculated = true;
		mHash = QCryptographicHash::hash(mData, QCryptographicHash::Algorithm::Sha1);
	}

	return mHash;
}

const QByteArray& FileFrag::getData() const
{
	if (!mIsDataReaded)
	{
		mutex->lock();
		mIsDataReaded = true;
		mFile->seek(mPosition);
		mData = mFile->read(mSize);
		mutex->unlock();
	}

	return mData;
}
