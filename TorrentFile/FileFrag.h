#pragma once

#include <QObject>

#include "File.h"

class FileFrag : public QObject
{
	Q_OBJECT

	mutable QFile* mFile;
	unsigned int mPosition;
	size_t mSize;
	mutable bool mIsHashCalculated = false;
	mutable QByteArray mHash;
	mutable bool mIsDataReaded = false;
	mutable QByteArray mData;
public:
	FileFrag(QFile* file, const unsigned int position, size_t size, QObject *parent);
	FileFrag(QFile* file, const unsigned int position, size_t size);
	~FileFrag();

	void free() const;

	const QByteArray& getHash() const;
	const QByteArray& getData() const;
};
