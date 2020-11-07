#include "Decoder.h"

#include <sstream>
#include <stdexcept>

#include "../Types.h"

bencode::Decoder::type bencode::Decoder::decode_type(const char& value)
{
	switch (value)
	{
	case 'i':
		return type::INT;
	case 'l':
		return type::LIST;
	case 'd':
		return type::DICT;
	case 'e':
		return type::END;
	default:
		if (isdigit(value))
		{
			return type::STR;
		}
		else
		{
			return type::UNKNOWN;
		}
	}
}

bencode::Int* bencode::Decoder::int_decoder(std::istream& stream)
{
	std::string buf;

	std::getline(stream, buf, 'e');

	return new Int(std::stoi(buf));
}
bencode::String* bencode::Decoder::string_decoder(std::istream& stream)
{
	size_t size;
	std::string buf;

	stream.unget();

	getline(stream, buf, ':');

	size = std::stoi(buf);
	
	std::wstring result;

	for (int i = 0; i < size; ++i)
	{
		result.append(1, from_utf8(stream));
	}

	if (size != result.size())
	{
		throw(std::logic_error("Bencode Decoder: Wrong UTF-8 encoding!"));
	}

	return new String(result);
}

const wchar_t bencode::Decoder::from_utf8(std::istream& stream)
{
	wchar_t result = 0;
	unsigned int count = 0;

	unsigned char temp = stream.get();
	if (temp <= 0x7F)
	{
		result = temp;
		count = 0;
	}
	else if (temp <= 0xDF)
	{
		result = temp & 0x1F;
		count = 1;
	}
	else if (temp <= 0xEF)
	{
		result = temp & 0xF;
		count = 2;
	}
	else if (temp <= 0xF7)
	{
		result = temp & 0x7;
		count = 3;
	}
	else if (temp <= 0xFB)
	{
		result = temp & 0x3;
		count = 4;
	}
	else if (temp <= 0xFD)
	{
		result = temp & 0x1;
		count = 5;
	}
	else
	{
		//throw(std::logic_error("Bencode Decoder: Wrong UTF-8 encoding!"));
	}

	for (unsigned int i = 0; i < count; ++i)
	{
		temp = stream.get();

		if (temp < 0x80)
		{
			throw(std::logic_error("Bencode Decoder: Wrong UTF-8 encoding!"));
		}

		result = (result << 6) + (temp & 0x3F);
	}

	return result;
}

bencode::List* bencode::Decoder::list_decoder(std::istream& stream)
{
	List* list = new List();
	
	while (true)
	{
		std::shared_ptr <ICode> temp(decode(std::move(stream)));

		if (temp == nullptr)
		{
			break;
		}

		list->push_back(temp);
	}

	return list;
}
bencode::Dictionary* bencode::Decoder::dict_decoder(std::istream& stream)
{
	Dictionary* dict = new Dict();

	while (true)
	{
		type decoded_type = decode_type(stream.get());

		if (decoded_type == type::END)
		{
			break;
		}
		else if (decoded_type != type::STR)
		{
			throw std::domain_error("Bencode Decoder: Expected string key!");
		}

		std::shared_ptr <String> key(string_decoder(stream));
		std::shared_ptr <ICode> value(decode(std::move(stream)));

		(*dict)[*key] = value;
	}

	return dict;
}

bencode::ICode* bencode::Decoder::decode(std::istream&& stream)
{
	ICode* result = nullptr;

	type decoded_type = decode_type(stream.get());

	switch (decoded_type)
	{
	case type::INT:
		result = int_decoder(stream);
		break;
	case type::STR:
		result = string_decoder(stream);
		break;
	case type::LIST:
		result = list_decoder(stream);
		break;
	case type::DICT:
		result = dict_decoder(stream);
		break;
	case type::UNKNOWN:
		throw std::domain_error("Bencode Decoder: Wrong value type!");
	}

	return result;
}

bencode::ICode* bencode::Decoder::decode(const std::string& string)
{
	return decode(std::istringstream(string));
}