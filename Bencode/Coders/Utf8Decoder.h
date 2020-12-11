#pragma once
#include <istream>
#include <string>

#include "Exception.h"

#include "../Types.h"

#include <istream>
#include <string>

namespace bencode
{
	class Utf8Decoder
	{
	protected:
		wchar_t utf8Char = 0;
		unsigned int count = 0;
	public:
		void clear();
		std::wstring decode(std::istream& stream);
		std::wstring decode(std::string string);
	protected:
		virtual void decodeUtf8Char(std::istream& stream);
		virtual void decodeFirstChar(std::istream& stream);
		virtual void decodeOtherChar(std::istream& stream);
	};
}