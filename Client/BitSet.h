#ifndef BIT_SET_H
#define BIT_SET_H

class BitSet
{
	unsigned char* data;
	size_t size;
	size_t count;
	size_t pages;
protected:
	void updateStatistics();
public:
	BitSet(const size_t size);
	BitSet(unsigned char* data, const size_t size);
	~BitSet();

	void set();
	void reset();
	const unsigned char* const set(size_t index);
	const unsigned char* const reset(size_t index);
	bool bit(size_t index);

	size_t getSize() const;
	size_t getCount() const;
	const char* const getData();
};
#endif