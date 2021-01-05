#pragma once
#include <stdexcept>
#include <string>

namespace bencode
{
	enum class Type;

	namespace Exception
	{
		class end_of_file
			:public std::logic_error
		{
		public:
			end_of_file();
		};

		class utf_encoding
			:public std::logic_error
		{
		public:
			utf_encoding(std::string message = "");
		};
	}
}