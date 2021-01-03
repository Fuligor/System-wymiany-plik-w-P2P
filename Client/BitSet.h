#ifndef BIT_SET_H
#define BIT_SET_H
#include <stddef.h>

class BitSet
{
	using pageType = unsigned char;

	unsigned char* data;
	size_t size;
	size_t count;
	int pages;
protected:
	size_t countBits(unsigned char i);
	void updateStatistics();
public:
	BitSet(const size_t size);
	BitSet(unsigned char* data, const size_t size);
	~BitSet();
	BitSet operator~ () const;
	BitSet operator& (const BitSet&) const;
	void set();
	void reset();
	const unsigned char* const set(size_t index);
	const unsigned char* const reset(size_t index);
	bool bit(size_t index);

	size_t getSetedBit(size_t index);
	size_t getSize() const;
	size_t getCount() const;
	size_t getDataSize() const;
	const char* const getData();
	void setData(unsigned char* data);

	static size_t getPageCount(const size_t size);
};
#endif