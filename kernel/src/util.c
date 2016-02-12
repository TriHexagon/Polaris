#include <util.h>

void* util_memcpy(const void* src, void* dst, size_t size)
{
	if ((size % 4) == 0)
	{
		const uint32_t* srcPtr = src;
		uint32_t* dstPtr = dst;
		while (size != 0)
		{
			*dstPtr++ = *srcPtr++;
			size -= sizeof(uint32_t);
		}
	}
	else
	{
		const uint8_t* srcPtr = src;
		uint8_t* dstPtr = dst;
		while (size != 0)
		{
			*dstPtr++ = *srcPtr++;
			size -= sizeof(uint8_t);
		}
	}

	return dst;
}

void* util_memmove(void* src, void* dst, size_t size)
{
	uint8_t* srcPtr = src, * dstPtr = dst;
	srcPtr += size; dstPtr += size;

	while (size != 0)
	{
		*dstPtr-- = *srcPtr--;
		size -= sizeof(uint8_t);
	}

	return dst;
}

int util_strcmp(const char* str0, const char* str1)
{
	while (*str0 == *str1)
	{
		if (*str0 == '\0')
			break;

		str0++;
		str1++;
	}

	return *str0 - *str1;
}

char* util_strcpy(const char* srcStr, char* dstStr)
{
	const char* srcStrPtr = srcStr;
	char* dstStrPtr = dstStr;

	while (*srcStrPtr != '\0')
	{
		*dstStrPtr++ = *srcStrPtr++;
	}

	return dstStr;
}

size_t util_strlen(const char* str)
{
	size_t size = 0;
	while (*str != '\0')
	{
		str++;
		size++;
	}

	return size;
}
