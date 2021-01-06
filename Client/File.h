//Klasa, która zajmuje siê obs³ug¹ pobieranego lub udostêpnianego pliku
#pragma once

#include <QObject>
#include <QVector>
#include <qmutex.h>

class QFile;
class QString;

class FileFrag;

class File 
	: public QObject
{
	Q_OBJECT

	QMutex mutex;
	QFile* mFile;
	QVector <FileFrag*> mFileFragments;
	uint64_t mFragSize;
	uint64_t mFileSize;
	bool mIsReadonly = true;
public:
	File(const QString& name, const uint64_t& fragSize,  QObject *parent = nullptr);
	File(const QString& name, const uint64_t& fragSize, const uint64_t& fileSize, QObject* parent = nullptr);
	~File();

	FileFrag* operator [] (const unsigned int index);

	bool readonly() const;
	void readonly(bool isReadonly);
	const uint64_t getSize() const;
	const uint64_t getFragSize() const;
	const unsigned int getFragCount() const;
	const std::string getFragsHash() const;

	void setFrag(std::string frag, int index);

	QVector <FileFrag*>::const_iterator begin() const;
	QVector <FileFrag*>::const_iterator end() const;
};
