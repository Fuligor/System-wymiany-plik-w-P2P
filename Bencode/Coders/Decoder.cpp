#include "Decoder.h"

#include <sstream>
#include <stdexcept>

#include "../Types.h"

bencode::Type bencode::Decoder::decode_type(const char& value)
{
	switch (value)
	{
	case 'i':
		return Type::INT;
	case 'l':
		return Type::LIST;
	case 'd':
		return Type::DICT;
	case 'e':
		return Type::END;
	default:
		if (isdigit(value))
		{
			return Type::STR;
		}
		else
		{
			return Type::UNKNOWN;
		}
	}
}

bencode::Int* bencode::Decoder::int_decoder(std::wistream& stream)
{
	std::wstring buf;

	std::getline(stream, buf, L'e');

	return new Int(std::stoi(buf));
}

bencode::String* bencode::Decoder::string_decoder(std::wistream& stream)
{
	size_t size;
	std::wstring buf;

	stream.unget();

	std::getline(stream, buf, L':');

	size = std::stoi(buf);
	buf.resize(size);

	stream.read(&buf[0], size);

	if(stream.gcount() < size)
	{
		throw(Exception::end_of_file());
	}

	return new String(buf);
}

bencode::List* bencode::Decoder::list_decoder(std::wistream& stream)
{
	List* list = new List();

	while (true)
	{
		std::shared_ptr <ICode> temp(decode(stream));

		if (temp == nullptr)
		{
			break;
		}

		list->push_back(temp);
	}

	return list;
}

bencode::Dictionary* bencode::Decoder::dict_decoder(std::wistream& stream)
{
	Dictionary* dict = new Dict();

	while (true)
	{
		Type decoded_type = decode_type(stream.get());

		if (decoded_type == Type::END)
		{
			break;
		}
		else if (decoded_type != Type::STR)
		{
			throw std::domain_error("Bencode FileDecoder: Expected string key!");
		}

		std::shared_ptr <String> key(string_decoder(stream));
		std::shared_ptr <ICode> value(decode(stream));

		(*dict)[*key] = value;
	}

	return dict;
}

bencode::ICode* bencode::Decoder::decode(std::wistream& stream)
{
	ICode* result = nullptr;

	try
	{
		Type decoded_type = decode_type(stream.get());

		switch (decoded_type)
		{
		case Type::INT:
			result = int_decoder(stream);
			break;
		case Type::STR:
			result = string_decoder(stream);
			break;
		case Type::LIST:
			result = list_decoder(stream);
			break;
		case Type::DICT:
			result = dict_decoder(stream);
			break;
		case Type::UNKNOWN:
			throw std::domain_error("Bencode FileDecoder: Wrong value type!");
		}

		return result;
	}
	catch (Exception::end_of_file e)
	{
		if (result != nullptr)
		{
			delete[] result;
		}

		throw(e);
	}
}

bencode::ICode* bencode::Decoder::decode(const std::wstring& string)
{
	std::wistringstream stream(string);
	return decode(stream);
}
