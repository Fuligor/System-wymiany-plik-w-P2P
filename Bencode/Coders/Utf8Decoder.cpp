#include "Utf8Decoder.h"

#include <sstream>

void bencode::Utf8Decoder::decodeUtf8Char(std::istream& stream)
{
	if(count == 0)
	{
		decodeFirstChar(stream);
	}
	while(count > 0)
	{
		decodeOtherChar(stream);
	}
}

void bencode::Utf8Decoder::decodeFirstChar(std::istream& stream)
{
	const unsigned char temp = stream.get();

	if (stream.eof())
	{
		utf8Char = -1;

		return;
	}

	if (temp <= 0x7F)
	{
		utf8Char = temp;
		count = 0;
	}
	else if (temp <= 0xDF)
	{
		utf8Char = temp & 0x1F;
		count = 1;
	}
	else if (temp <= 0xEF)
	{
		utf8Char = temp & 0xF;
		count = 2;
	}
	else if (temp <= 0xF7)
	{
		utf8Char = temp & 0x7;
		count = 3;
	}
	else if (temp <= 0xFB)
	{
		utf8Char = temp & 0x3;
		count = 4;
	}
	else if (temp <= 0xFD)
	{
		utf8Char = temp & 0x1;
		count = 5;
	}
	else
	{
		throw(Exception::utf_encoding("Bencode FileDecoder: Wrong UTF-8 encoding!"));
	}
}

void bencode::Utf8Decoder::decodeOtherChar(std::istream& stream)
{
	const unsigned char temp = stream.get();

	if (stream.eof())
	{
		throw(Exception::end_of_file());
	}

	if (temp < 0x80)
	{
		throw(Exception::utf_encoding("Bencode FileDecoder: Wrong UTF-8 encoding!"));
	}

	utf8Char = (utf8Char << 6) + (temp & 0x3F);

	--count;
}

void bencode::Utf8Decoder::clear()
{
	count = 0;
	utf8Char = 0;
}

std::wstring bencode::Utf8Decoder::decode(std::istream& stream)
{
	std::wstring result;

	while(true)
	{
		decodeUtf8Char(stream);

		if(utf8Char == (wchar_t) -1)
		{
			break;
		}

		result += utf8Char;
	}

	return result;
}

std::wstring bencode::Utf8Decoder::decode(std::string string)
{
	return decode(std::istringstream(string));
}
