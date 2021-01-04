#include "FileSize.h"

size_t powi(size_t a, size_t b)
{
	if (b == 0)
	{
		return 1;
	}

	size_t result = powi(a, b / 2);

	result *= result;

	if (b % 2)
	{
		return result * a;
	}

	return result;

}

FileSize::FileSize(size_t size, SizeUnit unit)
{
	this->size = size * powi(size, (int) unit);
}

size_t FileSize::to_int() const
{
    return size;
}

std::string FileSize::toString() const
{
	double temp = (double)size;
	size_t unit = 0;


	while(temp > 1024.0)
	{
		temp /= 1024;
		++unit;
	}

	std::string result = std::to_string(temp);

	size_t pos = result.find(".");
	
	if(pos != std::string::npos)
	{
		result = result.substr(0, pos + 2);
	}
	
	result += " ";

	switch (unit)
	{
	case 0:
		result += "B";
		break;
	case 1:
		result += "kB";
		break;
	case 2:
		result += "MB";
		break;
	case 3:
		result += "GB";
		break;
	default:
		break;
	}

    return result;
}

FileSize FileSize::operator+(const FileSize& fileSize)
{
    return FileSize(size + fileSize.size);
}

FileSize FileSize::operator+(const size_t& fileSize)
{
    return FileSize(size + fileSize);
}

FileSize FileSize::operator+=(const FileSize& fileSize)
{
	size += fileSize.size;

	return *this;
}

FileSize FileSize::operator+=(const size_t& fileSize)
{
	size += fileSize;

	return  *this;
}

FileSize FileSize::operator-(const FileSize& fileSize)
{
    return FileSize(size - fileSize.size);
}

FileSize FileSize::operator-(const size_t& fileSize)
{
    return FileSize(size - fileSize);
}

FileSize FileSize::operator-=(const FileSize& fileSize)
{
	size -= fileSize.size;

	return  *this;
}

FileSize FileSize::operator-=(const size_t& fileSize)
{
	size -= fileSize;

	return  *this;
}

FileSize FileSize::operator=(const size_t& fileSize)
{
	size = fileSize;

    return *this;
}

FileSize FileSize::operator=(const FileSize& fileSize)
{
	size = fileSize.size;

    return *this;
}
