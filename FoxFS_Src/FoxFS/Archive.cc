#include <FoxFS.h>
#include "Archive.h"

#include <cryptopp/modes.h>
#include <cryptopp/aes.h>

#include "../xxhash/xxhash.h"

#include "../lz4/lz4.h"
#include "../lz4/lz4hc.h"

namespace FoxFS
{

#ifdef MIN
#undef MIN
#endif

	template <typename typeA, typename typeB> typeA MIN(typeA a, typeB b)
	{
		return a < b ? a : b;
	}

#ifdef MAX
#undef MAX
#endif

	template <typename typeA, typename typeB> typeA MAX(typeA a, typeB b)
	{
		return a > b ? a : b;
	}

#ifdef MINMAX
#undef MINMAX
#endif

	template <typename typeMin, typename typeValue, typename typeMax> typeValue MINMAX(typeMin min, typeValue value, typeMax max)
	{
		return ((value) < (min) ? (min) : ((max) < (value) ? (max) : (value)));
	}

	Archive::Archive() :
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
		file(INVALID_HANDLE_VALUE)
#else
		file(-1)
#endif
	{}
	Archive::~Archive() { unload(); }

	const wchar_t* Archive::getFilename() const { return filename; }

	int Archive::exists(const char* filename) const
	{
		unsigned int index = generateFilenameIndex(filename);

		int r = ERROR_FILE_WAS_NOT_FOUND;
		std::map<unsigned int, FileListEntry>::const_iterator iter = files.find(index);
		if (iter != files.end())
		{
			r = ERROR_OK;
		}
		return r;
	}
	unsigned int Archive::size(const char* filename) const
	{
		unsigned int index = generateFilenameIndex(filename);

		unsigned int r = 0;
		std::map<unsigned int, FileListEntry>::const_iterator iter = files.find(index);
		if (iter != files.end())
		{
			r = iter->second.decompressed;
		}
		return r;
	}
	int Archive::get(const char* filename, void* buffer, unsigned int maxsize, unsigned int* outsize) const
	{
		unsigned int index = generateFilenameIndex(filename);

		int r = ERROR_FILE_WAS_NOT_FOUND;
		std::map<unsigned int, FileListEntry>::const_iterator iter = files.find(index);
		if (iter != files.end())
		{
			if (iter->second.size == iter->second.decompressed)
			{
				unsigned int len = MIN(maxsize, iter->second.size);
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
				DWORD dwRead;
				LARGE_INTEGER m;
				m.QuadPart = iter->second.offset;
				SetFilePointerEx(file, m, 0, FILE_BEGIN);
				ReadFile(file, buffer, len, &dwRead, 0);
#else
				lseek(file, iter->second.offset, SEEK_SET);
				read(file, buffer, len);
#endif
				try
				{
					CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption decoder(key, 32, iv);
					decoder.ProcessData(reinterpret_cast<unsigned char*>(buffer), reinterpret_cast<unsigned char*>(buffer), len);
				}
				catch (...)
				{
					return ERROR_DECRYPTION_FAILED;
				}
				if (len == iter->second.size && XXH32(reinterpret_cast<char*>(buffer), len, FOXFS_MAGIC) != iter->second.hash)
				{
					return ERROR_CORRUPTED_FILE;
				}
				//TODO: use other buffschlampe 
				//memcpy(buffer, &tmp[0], MIN(maxsize, iter->second.size));
				if (outsize)
				{
					*outsize = len;
				}
			}
			else
			{
				std::vector<unsigned char> tmp(iter->second.size + iter->second.decompressed);
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
				DWORD dwRead;
				LARGE_INTEGER m;
				m.QuadPart = iter->second.offset;
				SetFilePointerEx(file, m, 0, FILE_BEGIN);
				ReadFile(file, &tmp[0], iter->second.size, &dwRead, 0);
#else
				lseek(file, iter->second.offset, SEEK_SET);
				read(file, &tmp[0], iter->second.size);
#endif
				try
				{
					CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption decoder(key, 32, iv);
					decoder.ProcessData(&tmp[0], &tmp[0], iter->second.size);
				}
				catch (...)
				{
					return ERROR_DECRYPTION_FAILED;
				}
				if (LZ4_decompress_fast(reinterpret_cast<char*>(&tmp[0]), reinterpret_cast<char*>(&tmp[iter->second.size]), iter->second.decompressed) != iter->second.size)
				{
					return ERROR_DECOMPRESSION_FAILED;
				}
				if (XXH32(reinterpret_cast<char*>(&tmp[iter->second.size]), iter->second.decompressed, FOXFS_MAGIC) != iter->second.hash)
				{
					return ERROR_CORRUPTED_FILE;
				}
				// TODO: buffer
				memcpy(buffer, &tmp[iter->second.size], MIN(maxsize, iter->second.decompressed));
				if (outsize)
				{
					*outsize = MIN(maxsize, iter->second.decompressed);
				}
			}
			r = ERROR_OK;
		}
		return r;
	}

	int Archive::load(const wchar_t* filename, const void* key, const void* iv)
	{
		unload();
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
		file = CreateFileW(filename, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (file == INVALID_HANDLE_VALUE)
		{
			switch (GetLastError())
			{
			case ERROR_ACCESS_DENIED: return ERROR_ARCHIVE_ACCESS_DENIED;
			case ERROR_FILE_NOT_FOUND: return ERROR_ARCHIVE_NOT_FOUND;
			case ERROR_TOO_MANY_OPEN_FILES: return ERROR_ARCHIVE_NOT_READABLE;
			default: ERROR_ARCHIVE_ACCESS_DENIED;
			}
		}
		wcscpy(this->filename, filename);
		DWORD dwRead = 0;
		ReadFile(file, &header, sizeof(header), &dwRead, 0);
		DWORD size = GetFileSize(file, 0);
#else
		char tmpname[PATH_MAX + 1] = { 0 };
		wcstombs(tmpname, filename, wcslen(filename));
		file = ::open(tmpname, O_RDONLY | O_LARGEFILE);
		if (file == -1)
		{
			return ERROR_ARCHIVE_NOT_FOUND;
		}
		wcscpy(this->filename, filename);
		read(file, &header, sizeof(header));
		struct stat st;
		fstat(file, &st);
		unsigned int size = st.st_size;
#endif
		if (header.magic != FOXFS_MAGIC)
		{
			unload();
			return ERROR_ARCHIVE_INVALID;
		}
		memcpy(this->key, header.key, 32);
		memcpy(this->iv, header.iv, 32);
		if (key)
		{
			for (int i = 0; i < 4; ++i)
			{
				reinterpret_cast<unsigned long long*>(this->key)[i] ^= reinterpret_cast<const unsigned long long*>(key)[i];
			}
		}
		if (iv)
		{
			for (int i = 0; i < 4; ++i)
			{
				reinterpret_cast<unsigned long long*>(this->iv)[i] ^= reinterpret_cast<const unsigned long long*>(iv)[i];
			}
		}
		unsigned int offset = sizeof(header);
		while (offset < size)
		{
			TArchiveEntry entry;
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
			LARGE_INTEGER m;
			m.QuadPart = offset;
			SetFilePointerEx(file, m, 0, FILE_BEGIN);
			ReadFile(file, &entry, sizeof(entry), &dwRead, 0);
#else
			lseek(file, offset, SEEK_SET);
			read(file, &entry, sizeof(entry));
#endif
			offset = entry.offset + entry.size;
			FileListEntry listentry;
			listentry.offset = entry.offset;
			listentry.size = entry.size;
			listentry.decompressed = entry.decompressed;
			listentry.hash = entry.hash;
			listentry.name = entry.name;
			files.insert(std::map<unsigned int, FileListEntry>::value_type(listentry.name, listentry));
		}
		return ERROR_OK;
	}
	void Archive::unload()
	{
		files.clear();
		memset(filename, 0, sizeof(filename));
		memset(&header, 0, sizeof(header));
		memset(key, 0, sizeof(key));
		memset(iv, 0, sizeof(iv));

#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
		if (file != INVALID_HANDLE_VALUE)
		{
			CloseHandle(file);
			file = INVALID_HANDLE_VALUE;
		}
#else
		if (file != -1)
		{
			close(file);
			file = -1;
		}
#endif
	}

}