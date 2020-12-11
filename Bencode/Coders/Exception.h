#pragma once
#include <stdexcept>
#include <string>

namespace bencode
{
	enum class Type;

	namespace Exception
	{
		class end_of_file
			:std::logic_error
		{
			Type type;
			int left;
			wchar_t data;
		public:
			end_of_file();
		};

		class utf_encoding
			:std::logic_error
		{
		public:
			utf_encoding(std::string message = "");
		};
	}
}