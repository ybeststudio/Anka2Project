#ifndef FOXFS_CONFIG_H
#define FOXFS_CONFIG_H

#define FOXFS_MAGIC (0x000FED0C)
#define FOXFS_LZ4_BLOCKSIZE (0x00001000)

namespace FoxFS
{

#pragma pack(push, 1)
	typedef struct SArchiveHeader
	{
		unsigned int magic;
		unsigned char key[32];
		unsigned char iv[32];
	} TArchiveHeader;
	typedef struct SArchiveEntry
	{
		unsigned int decompressed;
		unsigned int hash;
		unsigned int offset;
		unsigned int size;
		unsigned int name;
	} TArchiveEntry;
#pragma pack(pop)

}

#endif