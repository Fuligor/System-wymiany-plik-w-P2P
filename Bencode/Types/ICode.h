#pragma once
#include <string>

namespace bencode
{
	class ICode {
	public:
		virtual std::string code() const = 0;
	};
}