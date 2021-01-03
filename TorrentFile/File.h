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
	size_t mFragSize;
	size_t mFileSize;
public:
	File(const QString& name, const size_t& fragSize,  QObject *parent = nullptr);
	File(const QString& name, const size_t& fragSize, const size_t& fileSize, QObject* parent = nullptr);
	~File();

	FileFrag* operator [] (const unsigned int index);

	const size_t getSize() const;
	const size_t getFragSize() const;
	const unsigned int getFragCount() const;
	const std::string getFragsHash() const;

	void setFrag(std::string frag, int index);

	QVector <FileFrag*>::const_iterator begin() const;
	QVector <FileFrag*>::const_iterator end() const;
};
