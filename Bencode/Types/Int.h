#pragma once
#include "ICode.h"

#include <cstddef>

namespace bencode
{
	class Int
		:public ICode
	{
	protected:
		size_t mValue;
	public:
		Int();
		Int(const size_t& value);
		Int(const Int& other);
		Int(Int&& other) noexcept;

		size_t getValue() const;
		virtual std::string code() const;

		Int& operator = (const size_t& value);
		Int& operator = (const Int& other);

		Int operator + (const size_t& b) const;
		Int operator + (const Int& b) const;

		Int operator - (const size_t& b) const;
		Int operator - (const Int& b) const;
	};

	Int operator + (const size_t& a, const Int& b);
	Int operator - (const size_t& a, const Int& b);
}