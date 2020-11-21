#pragma once

#include <QObject>
#include <QVector>

class QFile;
class QString;

class FileFrag;

class File 
	: public QObject
{
	Q_OBJECT
	
	QFile* mFile;
	QVector <FileFrag*> mFileFragments;
	size_t mFragSize;
public:
	File(const QString& name, const size_t& fragSize,  QObject *parent);
	File(const QString& name, const size_t& fragSize);
	~File();

	const FileFrag* operator [] (const unsigned int index) const;

	const size_t getSize() const;
	const size_t getFragSize() const;
	const unsigned int getFragCount() const;
	const std::string getFragsHash() const;

	QVector <FileFrag*>::const_iterator begin() const;
	QVector <FileFrag*>::const_iterator end() const;
};
