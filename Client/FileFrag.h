#pragma once

#include <QObject>

#include "File.h"

class FileFrag : public QObject
{
	Q_OBJECT

	mutable QMutex* mutex;
	mutable QFile* mFile;
	unsigned int mPosition;
	size_t mSize;
public:
	FileFrag(QFile* file, const unsigned int position, size_t size, QMutex* mutex, QObject *parent = nullptr);
	~FileFrag();

	void setData(std::string frag);

	const QByteArray getHash() const;
	const QByteArray getData() const;
};
