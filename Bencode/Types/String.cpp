#include "String.h"

bencode::String::String()
	:std::wstring()
{}

bencode::String::String(const char* const ptr)
	: String(std::string(ptr))
{}

bencode::String::String(const wchar_t* const ptr)
	: std::wstring(ptr)
{
}

bencode::String::String(const std::string& other)
{
	for(unsigned char i: other)
	{
		push_back(i);
	}
}

bencode::String::String(const std::wstring& other)
	: std::wstring(other)
{
}

bencode::String::String(const String& other)
	: std::wstring(other)
{}

bencode::String::String(const String&& other) noexcept
	: std::wstring(other)
{}

bencode::String& bencode::String::operator=(const char* value)
{
	*this = String(value);

	return *this;
}

bencode::String& bencode::String::operator=(const wchar_t* value)
{
	*this = String(value);

	return *this;
}

bencode::String& bencode::String::operator=(const std::string& value)
{
	*this = String(value);

	return *this;
}

bencode::String& bencode::String::operator=(const String& other)
{
	assign(other);

	return *this;
}

std::string bencode::String::code() const
{
	std::string result;

	result = std::to_string(size());
	result += ":";

	for (size_t i = 0; i < size(); ++i)
	{
		result += to_utf8((*this)[i]);
	}

	return result;
}

std::string bencode::String::to_utf8(const wchar_t value) const
{
	wchar_t temp = value;
	unsigned int count = 1;
	std::string result;
	unsigned char mask = 0;

	if (temp <= 0x7F)
	{
		count = 1;
		mask = (unsigned char) 0;
	}
	else if (temp <= 0x7FF)
	{
		count = 2;
		mask = (unsigned char) 0xC0;
	}
	else if (temp <= 0xFFFF)
	{
		count = 3;
		mask = (unsigned char) 0xE0;
	}
	else if (temp <= 0x1FFFFF)
	{
		count = 4;
		mask = (unsigned char) 0xF0;
	}
	else if (temp <= 0x3FFFFFF)
	{
		count = 5;
		mask = (unsigned char) 0xF8;
	}
	else if (temp <= 0x7FFFFFFF)
	{
		count = 6;
		mask = (unsigned char) 0xFC;
	}

	for (unsigned int i = count - 1; i > 0; --i)
	{
		result.push_back(0x80 + (temp & 0x3F));
		temp = temp >> 6;
	}

	result.push_back((unsigned char) temp + mask);

	result = std::string(result.rbegin(), result.rend());

	return result;
}