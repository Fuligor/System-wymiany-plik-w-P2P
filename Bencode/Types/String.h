#pragma once
#include <string>
#include "ICode.h"

namespace bencode {
	class String
		:public ICode, public std::wstring
	{
	public:
		String();
		String(const char* const ptr);
		String(const wchar_t* const ptr);
		String(const std::string& other);
		String(const std::wstring& other);
		String(const String& other);
		String(const String&& other) noexcept;

		virtual std::string code() const override;

	private:
		static std::string to_utf8(wchar_t value);
	};
}