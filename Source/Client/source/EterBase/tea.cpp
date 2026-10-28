
#include "StdAfx.h"
#include "tea.h"
#include <memory.h>



#define TEA_ROUND		32
#define DELTA			0x9E3779B9

void tea_code(const unsigned long sz, const unsigned long sy, const unsigned long *key, unsigned long *dest)
{
	unsigned long y = sy, z = sz, sum = 0;
	unsigned long		n = TEA_ROUND;

	while (n-- > 0)
	{
		y	+= ((z << 4 ^ z >> 5) + z) ^ (sum + key[sum & 3]);
		sum	+= DELTA;
		z	+= ((y << 4 ^ y >> 5) + y) ^ (sum + key[sum >> 11 & 3]);
	}

	*(dest++)	= y;
	*dest	= z;
}

void tea_decode(const unsigned long sz, const unsigned long sy, const unsigned long *key, unsigned long *dest)
{
#pragma warning(disable:4307)
	unsigned long y = sy, z = sz, sum = DELTA * TEA_ROUND;
#pragma warning(default:4307)

	unsigned long		n = TEA_ROUND;

	while (n-- > 0)
	{
		z -= ((y << 4 ^ y >> 5) + y) ^ (sum + key[sum >> 11 & 3]);
		sum -= DELTA;
		y -= ((z << 4 ^ z >> 5) + z) ^ (sum + key[sum & 3]);
	}

	*(dest++)	= y;
	*dest	= z;
}

int tea_encrypt(unsigned long *dest, const unsigned long *src, const unsigned long * key, int size)
{
	int		i;
	int		resize;

	if (size % 8 != 0)
	{
		resize = size + 8 - (size % 8);
		memset((char *) src + size, 0, resize - size);
	}
	else
		resize = size;

	for (i = 0; i < resize >> 3; i++, dest += 2, src += 2)
		tea_code(*(src + 1), *src, key, dest);

	return (resize);
}

int tea_decrypt(unsigned long *dest, const unsigned long *src, const unsigned long * key, int size)
{
	int		i;
	int		resize;

	if (size % 8 != 0)
		resize = size + 8 - (size % 8);
	else
		resize = size;

	for (i = 0; i < resize >> 3; i++, dest += 2, src += 2)
		tea_decode(*(src + 1), *src, key, dest);

	return (resize);
}

