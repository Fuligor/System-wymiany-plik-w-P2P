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
	};
}