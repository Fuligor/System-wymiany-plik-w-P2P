#include "Int.h"

bencode::Int::Int()
	:Int(0)
{}

bencode::Int::Int(const size_t& value)
	: mValue(value)
{}

bencode::Int::Int(const Int& other)
	: mValue(other.mValue)
{}

bencode::Int::Int(Int&& other) noexcept
	: mValue(other.mValue)
{
}

size_t bencode::Int::getValue() const
{
	return mValue;
}

std::string bencode::Int::code() const
{
	std::string result = "i";

	result += std::to_string(mValue);

	result += "e";

	return result;
}

bencode::Int& bencode::Int::operator=(const size_t& value)
{
	mValue = value;

	return *this;
}

bencode::Int& bencode::Int::operator=(const Int& other)
{
	mValue = other.mValue;

	return *this;
}

bencode::Int bencode::Int::operator+(const size_t& b) const
{
	return Int(mValue + b);
}

bencode::Int bencode::Int::operator+(const Int& b) const
{
	return Int(mValue + b.mValue);
}

bencode::Int bencode::Int::operator-(const size_t& b) const
{
	return Int(mValue - b);
}

bencode::Int bencode::Int::operator-(const Int& b) const
{
	return Int(mValue - b.mValue);
}

bencode::Int operator+(const size_t& a, const bencode::Int& b)
{
	return b + a;
}

bencode::Int operator-(const size_t& a, const bencode::Int& b)
{
	return b - a;
}