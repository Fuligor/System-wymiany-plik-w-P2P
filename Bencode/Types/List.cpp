#include "List.h"

std::string bencode::List::code() const
{
	std::string result = "l";

	for (auto i : *this)
	{
		result += i->code();
	}

	result += "e";

	return result;
}
