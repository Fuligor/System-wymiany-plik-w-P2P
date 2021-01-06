// Klasakt�ra implementuje bitset (tablic� bitow�) 
#ifndef BIT_SET_H
#define BIT_SET_H
#include <stddef.h>
#include <memory>

class BitSet
{
	using pageType = unsigned char;
	//dane struktury
	std::shared_ptr <unsigned char> data;
	//liczba bit�w, kt�re zajmuje struktura
	size_t size;
	//liczba jedynek w bitsecie
	size_t count;
	// liczba bajt�w, kt�re zajmuje struktura
	size_t pages;
protected:
	size_t countBits(unsigned char i);
	void updateStatistics();
public:
	BitSet(const size_t size);
	BitSet(unsigned char* data, const size_t size);
	~BitSet();
	//dope�nienie zbioru
	BitSet operator~ () const;
	//cz�� wsp�lna dw�ch zbior�w
	BitSet operator& (const BitSet&) const;
	void set();
	void reset();
	const unsigned char* const set(size_t index);
	const unsigned char* const reset(size_t index);
	//zwraca czy warto�c bitu pod danym indexem jest ustawiona na 0 czy 1
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