#ifndef FILE_SIZE_H
#define FILE_SIZE_H
#include <string>

enum class SizeUnit
{
	B,
	kB,
	MB,
	GB
};

class FileSize
{
private:
	uint64_t size;
public:
	FileSize(uint64_t size = 0, SizeUnit unit = SizeUnit::B);

	uint64_t to_int() const;
	std::string toString() const;

	FileSize operator+(const FileSize& fileSize);
	FileSize operator+(const uint64_t& fileSize);
	FileSize operator+=(const FileSize& fileSize);
	FileSize operator+=(const uint64_t& fileSize);
	FileSize operator-(const FileSize& fileSize);
	FileSize operator-(const uint64_t& fileSize);
	FileSize operator-=(const FileSize& fileSize);
	FileSize operator-=(const uint64_t& fileSize);

	FileSize operator=(const uint64_t& fileSize);
	FileSize operator=(const FileSize& fileSize);
};
#endif // !FILE_SIZE_H
