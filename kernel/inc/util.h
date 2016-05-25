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

#ifndef UTIL_H
#define UTIL_H

#include <kernel.h>

/**
 * @brief Copy memory from source to destination address.
 * @param src Source address.
 * @param dst Destination address.
 * @param size Number of bytes to copy.
 * @return Destination address.
 */
void* util_memcpy(const void* src, void* dst, size_t size);

/**
 * @brief Move memory from source to destination address (memory blocks may overlap).
 * @param src Source address.
 * @param dst Destination address.
 * @param size Number of bytes to move.
 * @return Destination address.
 */
void* util_memmove(void* src, void* dst, size_t size);

/**
 * @brief Compare two strings.
 * @param str0 Address of first string.
 * @param str1 Address of second string.
 * @return 0 if strings are equal.
 * <0 if first character that doesn't match has a lower value in str0.
 * >0 if first character that doesn't match has a greater value in str0.
 */
int util_strcmp(const char* str0, const char* str1);

/**
 * @brief Copy a string from source to destination address.
 * @param srcStr Source address.
 * @param dstStr Destination address.
 * @return Destination address.
 */
char* util_strcpy(const char* srcStr, char* dstStr);

/**
 * @brief Return length of a given string.
 * @param str String with null termination.
 * @return Length of the given string.
 */
size_t util_strlen(const char* str);

#endif // UTIL_H
