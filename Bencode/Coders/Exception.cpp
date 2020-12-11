#include "Exception.h"

#include "../Types.h"

bencode::Exception::end_of_file::end_of_file()
	:std::logic_error("End of file")
{
}

bencode::Exception::utf_encoding::utf_encoding(std::string message)
	:std::logic_error(message)
{
}
