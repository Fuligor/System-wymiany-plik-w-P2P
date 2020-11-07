#pragma once
#include <iostream>
#include <string>
#include <memory>

namespace bencode
{
	class ICode;
	class Int;
	class String;
	class List;
	class Dictionary;

	class Decoder
	{
		enum class type {
			INT,
			STR,
			LIST,
			DICT,
			END,
			UNKNOWN
		};

	private:
		static type decode_type(const char& value);
		static Int* int_decoder(std::istream& stream);
		static String* string_decoder(std::istream& stream);
		static const wchar_t from_utf8(std::istream& stream);
		static List* list_decoder(std::istream& stream);
		static Dictionary* dict_decoder(std::istream& stream);
	public:
		static ICode* decode(std::istream&& stream);
		static ICode* decode(const std::string& stream);
	};
}
