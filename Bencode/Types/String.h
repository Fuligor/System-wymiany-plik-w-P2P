#pragma once
#include <string>
#include "ICode.h"

namespace bencode {
	class String
		:public ICode, public std::string
	{
	public:
		String();
		String(const char* const ptr);
		String(const std::string& other);
		String(const String& other);
		String(const String&& other) noexcept;

		virtual std::string code() const override;
	};
}