#ifndef INFO_DICT_HASH
#define INFO_DICT_HASH
#include <QObject>

#include "Bencode.h"

class InfoDictHash
	:public QObject
{
protected:
	static const char toHexEncoding(const unsigned char& number);
	static const std::string toHexEncoding(std::string infoHash);
public:
	static std::string getHash(const std::shared_ptr <bencode::Dict>& dict);
	static std::string getReadableHash(const std::shared_ptr <bencode::Dict>& dict);
};
#endif