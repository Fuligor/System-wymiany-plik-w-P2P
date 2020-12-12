#include "Decoder.h"

#include "../Types.h"

std::wstring bencode::Decoder::getline(const std::wstring& string, const std::wstring& delim)
{
	std::wstring result;
	int find = string.find(delim, position);

	if(find == std::wstring::npos)
	{
		throw(Exception::end_of_file());
	}

	result = string.substr(position, find - position);
	position = find + 1;

	return result;
}

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

bencode::Int* bencode::Decoder::int_decoder(const std::wstring& string)
{
	std::wstring buf;

	buf = getline(string, L"e");

	return new Int(std::stoi(buf));
}

bencode::String* bencode::Decoder::string_decoder(const std::wstring& string)
{
	size_t size;
	std::wstring buf;

	buf = getline(string, L":");

	size = std::stoi(buf);

	buf = string.substr(position, size);

	if (buf.size() < size)
	{
		throw(Exception::end_of_file());
	}

	position += size;

	return new String(buf);
}

bencode::List* bencode::Decoder::list_decoder(const std::wstring& string)
{
	List* list = new List();

	while (true)
	{
		std::shared_ptr <ICode> temp(core_decoder(string));

		if (temp == nullptr)
		{
			break;
		}

		list->push_back(temp);
	}

	return list;
}

bencode::Dictionary* bencode::Decoder::dict_decoder(const std::wstring& string)
{
	Dictionary* dict = new Dict();

	while (true)
	{
		Type decoded_type = decode_type(string[position]);

		if (decoded_type == Type::END)
		{
			break;
		}
		else if (decoded_type != Type::STR)
		{
			throw std::domain_error("Bencode FileDecoder: Expected string key!");
		}

		std::shared_ptr <String> key(string_decoder(string));
		std::shared_ptr <ICode> value(core_decoder(string));

		(*dict)[*key] = value;
	}

	return dict;
}

bencode::ICode* bencode::Decoder::core_decoder(const std::wstring& string)
{
	ICode* result = nullptr;

	Type decoded_type = decode_type(string[position]);

	switch (decoded_type)
	{
	case Type::INT:
		++position;
		result = int_decoder(string);
		break;
	case Type::STR:
		result = string_decoder(string);
		break;
	case Type::LIST:
		++position;
		result = list_decoder(string);
		break;
	case Type::DICT:
		++position;
		result = dict_decoder(string);
		break;
	case Type::UNKNOWN:
		throw std::domain_error("Bencode FileDecoder: Wrong value type!");
	}

	return result;
}

bencode::ICode* bencode::Decoder::decode(std::wstring& string)
{
	ICode* result = nullptr;

	try
	{
		position = 0;

		result = core_decoder(string);

		string = string.substr(position);

		return result;
	}
	catch (Exception::end_of_file e)
	{
		if (result != nullptr)
		{
			delete[] result;

			result = nullptr;
		}

		throw(e);
	}
}

