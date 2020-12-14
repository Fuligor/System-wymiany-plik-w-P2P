#pragma once
#include <list>
#include "ICode.h"

#include <memory>

namespace bencode
{
	class List
		:public ICode, public std::list < std::shared_ptr <ICode> >
	{
	public:
		virtual std::string code() const override;
	};
}