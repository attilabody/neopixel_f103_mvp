#include <stdint.h>
#include "f1ll/strutil.h"

//////////////////////////////////////////////////////////////////////////////
size_t strcpy_ex(char *dst, char const *src)
{
	size_t ret = 0;
	do {
		*dst++ = *src;
		++ret;
	} while(*src++);
	return ret - 1;
}

//////////////////////////////////////////////////////////////////////////////
void strrev(char *first, char *last)
{
	char tmp;
	while(last > first) {
		tmp = *first;
		*first++ = *last;
		*last-- = tmp;
	}
}

//////////////////////////////////////////////////////////////////////////////
char tochr(const uint8_t in, const uint8_t upper)
{
	return in + ((in < 10) ? '0' : (upper ? 'A' : 'a') - 10);
}

//////////////////////////////////////////////////////////////////////////////
size_t uitodec(char* buffer, uint32_t data)
{
	char *b2 = buffer;
	if(!data) {
		*b2++ = '0';
		*b2 = '\0';
		return 1;
	}

	while(data) {
		*b2++ = (data % 10) + '0';
		data /= 10;
	}
	size_t ret = b2 - buffer;

	*b2-- = 0;

	strrev(buffer, b2);
    return ret;
}

//////////////////////////////////////////////////////////////////////////////
size_t uitohex(char* buffer, uint32_t data, uint8_t chars)
{
	char	*b2 = buffer;
	size_t	ret = 0;

	if(chars == 0xff || !chars)
	{
		if(!data) {
			*b2++ = '0';
			*b2 = '\0';
			return 1;
		}

		while(data) {
			uint8_t curval = data & 0x0f;
			*b2++ = tochr(curval, 1);
			data >>= 4;
		}
		ret = b2 - buffer;

	}
	else
	{
		ret = chars;
		for(uint8_t pos = 0; pos < (uint8_t)ret; ++pos) {
			*b2++ = tochr(data & 0x0f, 1);
			data >>= 4;
		}

	}
	*b2-- = 0;
	strrev(buffer, b2);
	return ret;
}


//////////////////////////////////////////////////////////////////////////////
size_t itodec(char* buffer, int data)
{
	if(data < 0) {
		*buffer++ = '-';
		return uitodec(buffer, -data) + 1;
	}

	return uitodec(buffer, data);
}

//////////////////////////////////////////////////////////////////////////////
size_t itohex(char* buffer, int data)
{
	if(data < 0) {
		*buffer++ = '-';
		return uitohex(buffer, -data, 0) + 1;
	}
	return uitohex(buffer, data, 0);
}
