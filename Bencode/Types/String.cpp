#include "String.h"

bencode::String::String()
	:std::string()
{}

bencode::String::String(const char* const ptr)
	:std::string(ptr)
{}

bencode::String::String(const std::string& other)
	:std::string(other)
{}

bencode::String::String(const String& other)
	:std::string(other)
{}

bencode::String::String(const String&& other) noexcept
	:std::string(other)
{}

std::string bencode::String::code() const
{
	std::string result;

	result = std::to_string(size());
	result += ":";
	result += *this;

	return result;
}
