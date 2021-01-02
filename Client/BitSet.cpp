#include "BitSet.h"

#include <cmath>

int powi(int a, unsigned int b)
{
	if(b == 0)
	{
		return 1;
	}

	int result = powi(a, b / 2);

	result *= result;

	if(b % 2)
	{
		return result * a;
	}

	return result;
}

size_t BitSet::countBits(unsigned char i)
{
	i = i - ((i >> 1) & 0x55);
	i = (i & 0x33) + ((i >> 2) & 0x33);
	return (i + (i >> 4)) & 0x0F;
}

void BitSet::updateStatistics()
{
	count = 0;

	for(size_t i = 0; i < pages; ++i)
	{
		count += countBits(data[i]);
	}
}

BitSet::BitSet(const size_t size)
	:size(size), count(0)
{
	pages = getPageCount(size);

	data = new unsigned char[pages];

	for (size_t i = 0; i < pages; ++i)
	{
		data[i] = 0;
	}
}

BitSet::BitSet(unsigned char* bits, const size_t size)
{
	this->size = size;
	pages = getPageCount(size);

	data = new unsigned char[pages];
	setData(bits);
}

BitSet::~BitSet()
{
	delete[] data;
}

void BitSet::set()
{
	if(pages == 0)
	{
		return;
	}

	for (size_t i = 0; i < pages; ++i)
	{
		data[i] = -1;
	}

	data[pages - 1] = -1 - (powi(2, (8 - size) % 8) - 1);

	count = size;
}

void BitSet::reset()
{
	for (size_t i = 0; i < pages; ++i)
	{
		data[i] = 0;
	}

	count = 0;
}

const unsigned char* const BitSet::set(size_t index)
{
	unsigned char mask = 1 << (index % 8);

	data[index / 8] |= mask;

	++count;

	return data + index / 8;
}

const unsigned char* const BitSet::reset(size_t index)
{
	unsigned char mask = -1 ^ (1 << (index % 8));

	data[index / 8] &= mask;

	--count;

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

size_t BitSet::getDataSize() const
{
	return pages;
}

const char* const BitSet::getData()
{
	return (char*) data;
}

void BitSet::setData(unsigned char* bits)
{
	
	for (size_t i = 0; i < pages; ++i)
	{
		data[i] = bits[i];
	}

	updateStatistics();
}

size_t BitSet::getPageCount(const size_t size)
{
	return ceil(size / 8.0);;
}
