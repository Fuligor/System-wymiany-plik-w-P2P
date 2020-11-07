// Bencode.cpp: definiuje punkt wejścia dla aplikacji.
//

#include "Bencode.h"

using namespace std;

int main()
{
	bencode::Dict dict;
	std::shared_ptr <bencode::String> string(new bencode::String("adam"));
	std::shared_ptr <bencode::Int> integer(new bencode::Int(-42));
	bencode::List list;

	dict["adam"] = integer;
	dict["ewa"] = std::make_shared <bencode::Int> (6);
	dict["justyna"] = std::make_shared <bencode::Int> (60);

	list.push_back(integer);
	list.push_back(string);

	cout << string->code() << endl;
	cout << dict.code() << endl;
	cout << integer->code() << endl;
	cout << list.code() << endl;

	cout << bencode::Encoder::encode(&dict) << std::endl;
	std::shared_ptr <bencode::ICode> decoded;
	decoded.reset(bencode::Decoder::decode(dict.code()));

	cout << decoded->code() << std::endl;

	decoded.reset(bencode::Decoder::decode(string->code()));
	cout << decoded->code() << std::endl;

	return 0;
}
