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
	private:
		uint64_t position;
	protected:
		std::wstring getline(const std::wstring& string, const std::wstring& delim = L"\n");
		virtual Type decode_type(const char& value);
		virtual Int* int_decoder(const std::wstring& string);
		virtual String* string_decoder(const std::wstring& string);
		virtual List* list_decoder(const std::wstring& string);
		virtual Dictionary* dict_decoder(const std::wstring& string);
		virtual ICode* core_decoder(const std::wstring& string);
	public:
		virtual ICode* decode(std::wstring& string);
	};
}