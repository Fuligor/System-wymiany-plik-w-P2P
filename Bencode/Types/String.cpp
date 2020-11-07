#include "String.h"

bencode::String::String()
	:std::wstring()
{}

bencode::String::String(const char* const ptr)
	: std::wstring()
{
	resize(std::char_traits<char>::length(ptr));
	mbstowcs(&(*this)[0], ptr, size());
}

bencode::String::String(const wchar_t* const ptr)
	: std::wstring(ptr)
{
}

bencode::String::String(const std::string& other)
	: String(other.c_str())
{}

bencode::String::String(const std::wstring& other)
	: String(other.c_str())
{
}

bencode::String::String(const String& other)
	: std::wstring(other)
{}

bencode::String::String(const String&& other) noexcept
	: std::wstring(other)
{}

std::string bencode::String::code() const
{
	std::string result;

	result = std::to_string(size());
	result += ":";

	for (const wchar_t& i: *this)
	{
		result += to_utf8(i);
	}

	return result;
}

std::string bencode::String::to_utf8(const wchar_t value)
{
	wchar_t temp = value;
	unsigned int count = 1;
	std::string result = " ";

	if (temp <= 0x7F)
	{
		count = 1;
		result[0] = (char) 0;
	}
	else if (temp <= 0x7FF)
	{
		count = 2;
		result[0] = (char) 0xC0;
	}
	else if (temp <= 0xFFFF)
	{
		count = 3;
		result[0] = (char) 0xE0;
	}
	else if (temp <= 0x1FFFFF)
	{
		count = 4;
		result[0] = (char) 0xF0;
	}
	else if (temp <= 0x3FFFFFF)
	{
		count = 5; 
		result[0] = (char) 0xF8;
	}
	else if (temp <= 0x7FFFFFFF)
	{
		count = 6;
		result[0] = (char) 0xFC;
	}

	for (unsigned int i = count - 1; i > 0; --i)
	{
		result.append(1, 0x80 + (temp & 0x3F));
		temp = temp >> 6;
	}

	result[0] += (unsigned char)temp;

	return result;
}