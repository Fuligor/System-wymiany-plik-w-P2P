#include "Encoder.h"

#include "../Types/ICode.h"

std::string bencode::Encoder::encode(const ICode* object)
{
	return object->code();
}
