#include "BitSet.h"

#include <cmath>

void BitSet::updateStatistics()
{
	count = 0;

	for(size_t i = 0; i < size; ++i)
	{
		count += bit(i);
	}
}

BitSet::BitSet(const size_t size)
	:size(size), count(0)
{
	pages = ceil(size / 8.0);

	data = new unsigned char[pages];

	for (size_t i = 0; i < pages; ++i)
	{
		data[i] = 0;
	}
}

BitSet::BitSet(unsigned char* bits, const size_t size)
	:size(size)
{
	size_t pages = ceil(size / 8.0);

	data = new unsigned char[pages];

	for(size_t i = 0; i < pages; ++i)
	{
		data[i] = bits[i];
	}

	updateStatistics();
}

BitSet::~BitSet()
{
	delete[] data;
}

void BitSet::set()
{
	for (size_t i = 0; i < pages; ++i)
	{
		data[i] = -1;
	}
}

void BitSet::reset()
{
	for (size_t i = 0; i < pages; ++i)
	{
		data[i] = 0;
	}
}

const unsigned char* const BitSet::set(size_t index)
{
	unsigned char mask = 1 << (index % 8);

	data[index / 8] |= mask;

	return data + index / 8;
}

const unsigned char* const BitSet::reset(size_t index)
{
	unsigned char mask = -1 ^ (1 << (index % 8));

	data[index / 8] &= mask;

	return data + index / 8;
}

bool BitSet::bit(size_t index)
{
	unsigned char mask = 1 << (index % 8);

	return data[index / 8] & mask;
}

size_t BitSet::getSize() const
{
	return size;
}

size_t BitSet::getCount() const
{
	return count;
}

const char* const BitSet::getData()
{
	return (char*) data;
}
