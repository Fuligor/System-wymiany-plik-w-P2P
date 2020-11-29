#include "File.h"

#include <cmath>

#include <QFile>
#include <QString>

#include "FileFrag.h"

File::File(const QString& name, const size_t& fragSize, QObject* parent)
	:mFile(new QFile(name, this)), QObject(parent), mFragSize(fragSize)
{
	mFile->open(QIODevice::ReadOnly);

	unsigned int fragCount = ceil((double) mFile->size() / mFragSize);
	mFileFragments.reserve(fragCount);

	for(unsigned int position = 0; position < mFile->size(); position += fragSize)
	{
		mFileFragments.push_back(new FileFrag(mFile, position, mFragSize, this));
	}
}

File::File(const QString& name, const size_t& fragSize)
	: File(name, fragSize, NULL)
{
}

File::~File()
{
	mFile->close();
}

const FileFrag* File::operator[](const unsigned int index) const
{
	return mFileFragments[index];
}

const size_t File::getSize() const
{
	return mFile->size();
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

QVector <FileFrag*>::const_iterator File::begin() const
{
	return mFileFragments.begin();
}

QVector<FileFrag*>::const_iterator File::end() const
{
	return mFileFragments.end();
}
