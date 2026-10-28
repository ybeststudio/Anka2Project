#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

inline const char *encode_byte(char ind)
{
	static char a[8];
	*((char *) a) = ind;
	return (a);
}

inline const char *encode_2bytes(sh_int ind)
{
	static char a[8];
	*((sh_int *) a) = ind;
	return (a);
}

inline const char *encode_4bytes(int ind)
{
	static char a[8];
	*((int *) a) = ind;
	return (a);
}

inline BYTE decode_byte(const void * a)
{
	return (*(BYTE *) a);
}

inline WORD decode_2bytes(const void * a)
{
	return (*((WORD *) a));
}

inline INT decode_4bytes(const void *a)
{
	return (*((INT *) a));
}

#define packet_encode(buf, data, len) __packet_encode(buf, data, len, __FILE__, __LINE__)

inline bool __packet_encode(LPBUFFER pbuf, const void * data, int length, const char * file, int line)
{
	assert(NULL != pbuf);
	assert(NULL != data);

	if (buffer_has_space(pbuf) < length)
	{
		return false;
	}

	buffer_write(pbuf, data, length);
	return true;
}

#endif
