#include "File.h"

#include <cmath>

#include <QFile>
#include <QString>

#include "FileFrag.h"

File::File(const QString& name, const size_t& fragSize, QObject* parent)
	:QObject(parent), mFile(new QFile(name, this)), mFragSize(fragSize)
{
	mFile->open(QIODevice::ReadWrite);
	mFileSize = mFile->size();
	unsigned int fragCount = ceil((double) mFileSize / mFragSize);
	mFileFragments.reserve(fragCount);

	for(unsigned int position = 0; position < mFileSize; position += (unsigned int)fragSize)
	{
		mFileFragments.push_back(new FileFrag(mFile, position, mFragSize, &mutex, this));
	}
}

File::File(const QString& name, const size_t& fragSize, const size_t& fileSize, QObject* parent)
	:QObject(parent), mFile(new QFile(name, this)), mFragSize(fragSize)
{
	mFile->open(QIODevice::ReadWrite);
	mFileSize = fileSize;
	unsigned int fragCount = ceil((double)mFileSize / mFragSize);
	mFileFragments.reserve(fragCount);

	for (unsigned int position = 0; position < mFileSize; position += (unsigned int)fragSize)
	{
		mFileFragments.push_back(new FileFrag(mFile, position, mFragSize, &mutex, this));
	}
}

File::~File()
{
	mFile->close();
}

FileFrag* File::operator[](const unsigned int index)
{
	return mFileFragments[index];
}

bool File::readonly() const
{
	return mIsReadonly;
}

void File::readonly(bool isReadonly)
{
	if(mIsReadonly != isReadonly)
	{
		mIsReadonly= isReadonly;

		if(mIsReadonly == true)
		{
			mFile->close();
			mFile->open(QIODevice::ReadOnly);
		}
		else
		{
			mFile->close();
			mFile->open(QIODevice::ReadWrite);
		}
	}
}

const size_t File::getSize() const
{
	return mFileSize;
}

const size_t File::getFragSize() const
{
	return mFragSize;
}

const unsigned int File::getFragCount() const
{
	return mFileFragments.size();
}

const std::string File::getFragsHash() const
{
	std::string result;

	for(auto frag: mFileFragments)
	{
		result += frag->getHash().toStdString();
	}

	return result;
}

void File::setFrag(std::string frag, int index)
{
	mFileFragments[index]->setData(frag);
}

QVector <FileFrag*>::const_iterator File::begin() const
{
	return mFileFragments.begin();
}

QVector<FileFrag*>::const_iterator File::end() const
{
	return mFileFragments.end();
}
