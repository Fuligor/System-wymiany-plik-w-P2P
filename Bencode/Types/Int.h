#pragma once
#include "ICode.h"

namespace bencode
{
	class Int
		:public ICode
	{
	protected:
		int mValue;
	public:
		Int();
		Int(const int& value);
		Int(const Int& other);
		Int(Int&& other) noexcept;

		virtual std::string code() const;

		Int& operator = (const int& value);
		Int& operator = (const Int& other);

		Int operator + (const int& b) const;
		Int operator + (const Int& b) const;

		Int operator - () const;
		Int operator - (const int& b) const;
		Int operator - (const Int& b) const;
	};

	Int operator + (const int& a, const Int& b);
	Int operator - (const int& a, const Int& b);
}