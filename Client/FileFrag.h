//Klasa obs³uguj¹ca fragment pobieranego lub udostepnianego pliku
#pragma once

#include <QObject>

#include <string>

#include "File.h"

class FileFrag : public QObject
{
	Q_OBJECT

	mutable QMutex* mutex;
	mutable QFile* mFile;
	unsigned int mPosition;
	uint64_t mSize;
	QByteArray dataBuffer;
public:
	FileFrag(QFile* file, const unsigned int position, uint64_t size, QMutex* mutex, QObject *parent = nullptr);
	~FileFrag();

	void setData(std::string frag);

	const QByteArray getHash();
	const QByteArray& getData();
	const uint64_t getSize() const;

	void free();
signals:
	void dataBuffered(FileFrag* fragment);
	void dataCleared(FileFrag* fragment);
};
