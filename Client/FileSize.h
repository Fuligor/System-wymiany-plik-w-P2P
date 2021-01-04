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
	size_t size;
public:
	FileSize(size_t size = 0, SizeUnit unit = SizeUnit::B);

	size_t to_int() const;
	std::string toString() const;

	FileSize operator+(const FileSize& fileSize);
	FileSize operator+(const size_t& fileSize);
	FileSize operator+=(const FileSize& fileSize);
	FileSize operator+=(const size_t& fileSize);
	FileSize operator-(const FileSize& fileSize);
	FileSize operator-(const size_t& fileSize);
	FileSize operator-=(const FileSize& fileSize);
	FileSize operator-=(const size_t& fileSize);

	FileSize operator=(const size_t& fileSize);
	FileSize operator=(const FileSize& fileSize);
};
#endif // !FILE_SIZE_H
