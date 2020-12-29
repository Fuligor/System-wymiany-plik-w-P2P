#include "InfoDictHash.h"

#include <memory>

#include <QCryptographicHash>

inline const char InfoDictHash::toHexEncoding(const unsigned char& number)
{
	if (number < 10)
	{
		return '0' + number;
	}
	if (number < 16)
	{
		return 'a' + (number - (char) 10);
	}

	return '0';
}

inline const std::string InfoDictHash::toHexEncoding(std::string infoHash)
{
	std::string result;

	for (size_t i = 0; i < infoHash.size(); ++i)
	{
		result += toHexEncoding((unsigned char) infoHash[i] / 16);
		result += toHexEncoding((unsigned char) infoHash[i] % 16);
	}

	return result;
}

std::string InfoDictHash::getHash(const std::shared_ptr<bencode::Dict>& dict)
{
    return QCryptographicHash::hash(QByteArray::fromStdString((*dict)["info"]->code()), QCryptographicHash::Sha1).toStdString();
}

std::string InfoDictHash::getReadableHash(const std::shared_ptr<bencode::Dict>& dict)
{
	return toHexEncoding(getHash(dict));
}
