#pragma once
#include <string>
#include <codecvt>

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

		String& operator = (const char* value);
		String& operator = (const wchar_t* value);
		String& operator = (const std::string& value);
		String& operator = (const String& other);

		virtual std::string code() const override;
	protected:
		std::string to_utf8(const wchar_t value) const;
	};
}