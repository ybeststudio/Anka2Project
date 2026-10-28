#ifndef FOXFS_ARCHIVE_H
#define FOXFS_ARCHIVE_H

#include <algorithm>
#include <map>
#include <cstdlib>

#include <cryptopp/crc.h>

#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
#pragma warning(disable : 4996)
#include <windows.h>
#else
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#endif

#include "config.h"

namespace FoxFS
{

	class Archive
	{
	public:
		enum
		{
			ERROR_OK = 0,
			ERROR_BASE_CODE = 0,
			ERROR_FILE_WAS_NOT_FOUND = ERROR_BASE_CODE + 1,
			ERROR_CORRUPTED_FILE = ERROR_BASE_CODE + 2,
			ERROR_MISSING_KEY = ERROR_BASE_CODE + 3,
			ERROR_MISSING_IV = ERROR_BASE_CODE + 4,
			ERROR_DECRYPTION_HAS_FAILED = ERROR_BASE_CODE + 5,
			ERROR_DECOMPRESSION_FAILED = ERROR_BASE_CODE + 6,
			ERROR_ARCHIVE_NOT_FOUND = ERROR_BASE_CODE + 7,
			ERROR_ARCHIVE_NOT_READABLE = ERROR_BASE_CODE + 8,
			ERROR_ARCHIVE_INVALID = ERROR_BASE_CODE + 9,
			ERROR_ARCHIVE_ACCESS_DENIED = ERROR_BASE_CODE + 10,
			ERROR_KEYSERVER_SOCKET = ERROR_BASE_CODE + 11,
			ERROR_KEYSERVER_CONNECTION = ERROR_BASE_CODE + 12,
			ERROR_KEYSERVER_RESPONSE = ERROR_BASE_CODE + 13,
			ERROR_KEYSERVER_TIMEOUT = ERROR_BASE_CODE + 14,
			ERROR_UNKNOWN = ERROR_BASE_CODE + 15
		};

		struct FileListEntry
		{
			unsigned long long offset;
			unsigned int size;
			unsigned int decompressed;
			unsigned int hash;
			unsigned int name;
		};

	public:
		Archive();
		~Archive();

		const wchar_t* getFilename() const;

		int exists(const char* filename) const;
		unsigned int size(const char* filename) const;
		int get(const char* filename, void* buffer, unsigned int maxsize, unsigned int* outsize) const;

		int load(const wchar_t* filename, const void* key, const void* iv);
		void unload();

		static inline unsigned int generateFilenameIndex(const char* filename)
		{
			std::string fn = filename;
			transform(fn.begin(), fn.end(), fn.begin(), tolower);
			for (int i = 0; i < fn.length(); ++i)
			{
				if (fn[i] == '\\')
				{
					fn[i] = '/';
				}
			}
			unsigned int index = 0;
			CryptoPP::CRC32 crc;
			crc.CalculateDigest(reinterpret_cast<unsigned char*>(&index), reinterpret_cast<const unsigned char*>(fn.c_str()), fn.length());
			return index;
		}

	private:
		std::map<unsigned int, FileListEntry> files;

		unsigned char key[32];
		unsigned char iv[32];

		TArchiveHeader header;

#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
		HANDLE file;
#else
		int file;
#endif
		wchar_t filename[
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
			MAX_PATH + 1
#else
			PATH_MAX + 1
#endif
		];
	};

}

#endif