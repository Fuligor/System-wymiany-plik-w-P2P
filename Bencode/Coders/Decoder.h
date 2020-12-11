#pragma once
#include <iostream>
#include <string>

#include "Exception.h"

namespace bencode
{
	class ICode;
	class Int;
	class String;
	class List;
	class Dictionary;

	enum class Type;

	class Decoder
	{
	protected:
		virtual Type decode_type(const char& value);
		virtual Int* int_decoder(std::wistream& stream);
		virtual String* string_decoder(std::wistream& stream);
		virtual List* list_decoder(std::wistream& stream);
		virtual Dictionary* dict_decoder(std::wistream& stream);
	public:
		virtual ICode* decode(std::wistream& stream);
		virtual ICode* decode(const std::wstring& string);
	};
}