#pragma once
#include <string>

namespace bencode
{
	class ICode;

	class Encoder
	{
	public:
		static std::string encode(const ICode* object);
	};
}