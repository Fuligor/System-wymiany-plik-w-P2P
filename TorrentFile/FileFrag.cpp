#include "FileFrag.h"

#include <QFile>
#include <QCryptographicHash>

FileFrag::FileFrag(QFile* file, const unsigned int position, size_t size, QObject* parent)
	: QObject(parent), mFile(file), mPosition(position), mSize(size)
{
}

FileFrag::FileFrag(QFile* file, const unsigned int position, size_t size)
	: FileFrag(file, position, size, NULL)
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
		mIsDataReaded = true;
		mFile->seek(mPosition);
		mData = mFile->read(mSize);
	}

	return mData;
}
