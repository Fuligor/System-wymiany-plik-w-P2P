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
		count += countBits(data.get()[i]);
	}
}

BitSet::BitSet(const size_t size)
	:size(size), count(0)
{
	pages = getPageCount(size);

	data.reset(new unsigned char[pages]);

	for (size_t i = 0; i < pages; ++i)
	{
		data.get()[i] = 0;
	}
}

BitSet::BitSet(unsigned char* bits, const size_t size)
	:size(size)
{
	pages = getPageCount(size);

	data.reset(new unsigned char[pages]);

	setData(bits);
}

BitSet::~BitSet()
{
}

BitSet BitSet::operator~() const
{

	unsigned char* result = new unsigned char[pages];
	for (int i = 0; i < pages; i++)
	{
		result[i] = ~data.get()[i];
	}
	return BitSet(result, pages);
}

BitSet BitSet::operator&(const BitSet& bitset) const
{
	unsigned char* result = new unsigned char[pages];
	for (int i = 0; i < pages; i++)
	{
		result[i] = data.get()[i] & bitset.data.get()[i];
	}
	return BitSet(result, pages);
}

void BitSet::set()
{
	if(pages == 0)
	{
		return;
	}

	for (size_t i = 0; i < pages; ++i)
	{
		data.get()[i] = -1;
	}

	data.get()[pages - 1] = powi(2, size % 8) - 1;

	count = size;
}

void BitSet::reset()
{
	for (size_t i = 0; i < pages; ++i)
	{
		data.get()[i] = 0;
	}

	count = 0;
}

const unsigned char* const BitSet::set(size_t index)
{
	unsigned char mask = 1 << (index % 8);

	data.get()[index / 8] |= mask;

	++count;

	return data.get() + index / 8;
}

const unsigned char* const BitSet::reset(size_t index)
{
	unsigned char mask = -1 ^ (1 << (index % 8));

	data.get()[index / 8] &= mask;

	--count;

	return data.get() + index / 8;
}

bool BitSet::bit(size_t index)
{
	unsigned char mask = 1 << (index % 8);

	return data.get()[index / 8] & mask;
}

size_t BitSet::getSetedBit(size_t index)
{
	size_t count = 0;
	for (int i = 0; i < pages; i++)
	{
		count += countBits(data.get()[i]);
		if (count > index)
		{
			for (int j = 7; j >= 0; j--)
			{
				count -= bit(i * (size_t)8 + j);
				if (count == index)
				{
					return count;
				}
				
			}
		}
		else if (count == index)
		{
			return count;
		}
	}
	return count;
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
	return (char*) data.get();
}

void BitSet::setData(unsigned char* bits)
{
	
	for (size_t i = 0; i < pages; ++i)
	{
		data.get()[i] = bits[i];
	}

	updateStatistics();
}

size_t BitSet::getPageCount(const size_t size)
{
	return (size_t) ceil(size / 8.0);;
}
