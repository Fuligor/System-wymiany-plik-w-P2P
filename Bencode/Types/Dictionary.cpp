#include "Dictionary.h"

#include "String.h"

std::string bencode::Dictionary::code() const
{
	std::string result = "d";

	for (auto i : *this)
	{
		result += i.first.code() + i.second->code();
	}

	result += "e";

	return result;
}
