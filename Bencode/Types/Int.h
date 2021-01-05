#pragma once
#include "ICode.h"

#include <cstddef>

namespace bencode
{
	class Int
		:public ICode
	{
	protected:
		uint64_t mValue;
	public:
		Int();
		Int(const uint64_t& value);
		Int(const Int& other);
		Int(Int&& other) noexcept;

		uint64_t getValue() const;
		virtual std::string code() const;

		Int& operator = (const uint64_t& value);
		Int& operator = (const Int& other);

		Int operator + (const uint64_t& b) const;
		Int operator + (const Int& b) const;

		Int operator - (const uint64_t& b) const;
		Int operator - (const Int& b) const;
	};

	Int operator + (const uint64_t& a, const Int& b);
	Int operator - (const uint64_t& a, const Int& b);
}