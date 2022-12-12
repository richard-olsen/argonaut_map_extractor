#pragma once

#include <vector>

template<typename T>
inline T getFromData(const std::vector<char> &data, size_t index)
{
	return *((T *) &data[index]);
}

template<typename T>
inline void setIntoData(std::vector<char> &data, size_t index, T &value)
{
	*((T *) &data[index]) = value;
}

template<typename T>
inline T swapEndianness(T value)
{
	char *valueStr = (char *) &value;
	size_t size = sizeof(T);
	for (size_t i = 0; i < (size / 2); i++)
	{
		char temp = valueStr[i];
		valueStr[i] = valueStr[size - (i + 1)];
		valueStr[size - (i + 1)] = temp;
	}
	return value;
}

