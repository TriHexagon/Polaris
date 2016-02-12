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
