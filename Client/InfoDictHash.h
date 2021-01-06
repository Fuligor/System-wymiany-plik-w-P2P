//Klasa, kt�ra oblicza hash s�ownika info z pliku .torrent
//Hash ten jest u�ywany do identyfikacji plik�w w systemie
#ifndef INFO_DICT_HASH
#define INFO_DICT_HASH
#include <QObject>

#include "Bencode.h"

class InfoDictHash
	:public QObject
{
protected:
	//Zapisuje bajt w postaci hexadecymalnej
	static const char toHexEncoding(const unsigned char& number);
	//Zapisuje stringa w postaci hexadecymalnej
	static const std::string toHexEncoding(std::string infoHash);
public:
	//podaje hash w postaci bajt�w
	static std::string getHash(const std::shared_ptr <bencode::Dict>& dict);
	//podaje hash w postaci liczb hexadecymalnych
	static std::string getReadableHash(const std::shared_ptr <bencode::Dict>& dict);
};
#endif