#pragma once
#include <map>
#include "ICode.h"

#include <memory>

namespace bencode
{
	class String;

	class Dictionary
		:public ICode, public std::map < String, std::shared_ptr <ICode> >
	{
	public:
		virtual std::string code() const override;
	};

	using Dict = Dictionary;
}