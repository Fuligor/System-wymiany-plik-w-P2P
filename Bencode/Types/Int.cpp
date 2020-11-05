#include "Int.h"

bencode::Int::Int()
	:Int(0)
{}

bencode::Int::Int(const int& value)
	: mValue(value)
{}

bencode::Int::Int(const Int& other)
	: mValue(other.mValue)
{}

bencode::Int::Int(Int&& other) noexcept
	: mValue(other.mValue)
{
}

std::string bencode::Int::code() const
{
	std::string result = "i";

	result += std::to_string(mValue);

	result += "e";

	return result;
}