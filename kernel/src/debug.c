#include <debug.h>
#include <drivers/uart.h>

#ifdef DEBUG

#include <stdarg.h>

static char strBuf[32];

static size_t debug_strlen(const char* str)
{
	const uint8_t* ptr = str;
	size_t len = 0;

	while (*ptr != '\0')
	{
		len++;
		ptr++;
	}

	return len;
}

static void swap(char* c0, char* c1)
{
	char c = *c0;
	*c0 = *c1;
	*c1 = c;
}

/* A utility function to reverse a string  */
static void reverse(char str[], int length)
{
	int start = 0;
	int end = length -1;
	while (start < end)
	{
		swap(str+start, str+end);
		start++;
		end--;
	}
}

// Implementation of itoa()
static char* itoa(int num, char* str, int base)
{
	int i = 0;
	bool isNegative = false;

	/* Handle 0 explicitely, otherwise empty string is printed for 0 */
	if (num == 0)
	{
		str[i++] = '0';
		str[i] = '\0';
		return str;
	}

	// In standard itoa(), negative numbers are handled only with
	// base 10. Otherwise numbers are considered unsigned.
	if (num < 0 && base == 10)
	{
		isNegative = true;
		num = -num;
	}

	// Process individual digits
	while (num != 0)
	{
		int rem = num % base;
		str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
		num = num/base;
	}

	// If number is negative, append '-'
	if (isNegative)
		str[i++] = '-';

	str[i] = '\0'; // Append string terminator

	// Reverse the string
	reverse(str, i);

	return str;
}

void debug_printf(const char* fmt, ...)
{
	va_list list;
	int i;
	const char* str;

	if (!uart_isInit())
		return;

	va_start(list, fmt);

	for (const char* ptr = fmt; *ptr != '\0'; ptr++)
	{
		if (*ptr != '%')
		{
			uart_send(ptr, 1);
		}
		else
		{
			ptr++;
			switch (*ptr)
			{
				case 0:
					return;
				case 'i':
					i = va_arg(list, int);
					itoa(i, strBuf, 10);
					uart_send(strBuf, debug_strlen(strBuf));
					break;
				case 'x':
					i = va_arg(list, int);
					itoa(i, strBuf, 16);
					uart_send(strBuf, debug_strlen(strBuf));
					break;
				case 's':
					str = va_arg(list, const char*);
					uart_send(str, debug_strlen(str));
					break;
			}
		}
	}

	va_end(list);
}

#endif
