/* The MIT License (MIT)
 *
 * Copyright (c) 2015-2016 Christopher Cichos
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

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
