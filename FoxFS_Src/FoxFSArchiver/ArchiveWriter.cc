#include "ArchiveWriter.h"
#include "../FoxFS/config.h"

#include <cryptopp/modes.h>
#include <cryptopp/aes.h>
#include <cryptopp/crc.h>
#include <cryptopp/osrng.h>

ArchiveWriter::ArchiveWriter()
{
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
	keys = INVALID_HANDLE_VALUE;
	file = INVALID_HANDLE_VALUE;
#else
	keys = -1;
	file = -1;
#endif
}
ArchiveWriter::~ArchiveWriter() { close(); }

bool ArchiveWriter::create(const char* filename, const char* keyfile)
{
	close();
	CryptoPP::AutoSeededRandomPool rng;
	unsigned char key[32], iv[32];
	FoxFS::TArchiveHeader header;
	header.magic = FOXFS_MAGIC;
	rng.GenerateBlock(header.key, 32);
	rng.GenerateBlock(header.iv, 32);
	rng.GenerateBlock(key, 32);
	rng.GenerateBlock(iv, 32);
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
	if (keyfile)
	{
		keys = CreateFileA(keyfile, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		if (keys == INVALID_HANDLE_VALUE)
		{
			return false;
		}
	}
	file = CreateFileA(filename, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (file == INVALID_HANDLE_VALUE)
	{
		if (keys != INVALID_HANDLE_VALUE)
		{
			CloseHandle(keys);
			keys = INVALID_HANDLE_VALUE;
		}
		return false;
	}

	DWORD dwWritten;
	WriteFile(file, &header, sizeof(header), &dwWritten, 0);
	if (keys != INVALID_HANDLE_VALUE)
	{
		// write magic number?
		WriteFile(keys, key, 32, &dwWritten, 0);
		WriteFile(keys, iv, 32, &dwWritten, 0);

		for (int i = 0; i < 4; ++i)
		{
			reinterpret_cast<unsigned long long*>(this->key)[i] = reinterpret_cast<unsigned long long*>(key)[i] ^ reinterpret_cast<unsigned long long*>(header.key)[i];
			reinterpret_cast<unsigned long long*>(this->iv)[i] = reinterpret_cast<unsigned long long*>(iv)[i] ^ reinterpret_cast<unsigned long long*>(header.iv)[i];
		}
	}
	else
	{
		memcpy(this->key, header.key, 32);
		memcpy(this->iv, header.iv, 32);
	}
#else
	if (keyfile)
	{
		keys = ::open(keyfile, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
		if (keys == -1)
		{
			return false;
		}
	}
	file = ::open(filename, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
	if (file == -1)
	{
		if (keys != -1)
		{
			::close(keys);
			keys = -1;
		}
		return false;
	}

	write(file, &header, sizeof(header));
	if (keys != -1)
	{
		// write magic number?
		write(keys, key, 32);
		write(keys, iv, 32);

		for (int i = 0; i < 4; ++i)
		{
			reinterpret_cast<unsigned long long*>(this->key)[i] = reinterpret_cast<unsigned long long*>(key)[i] ^ reinterpret_cast<unsigned long long*>(header.key)[i];
			reinterpret_cast<unsigned long long*>(this->iv)[i] = reinterpret_cast<unsigned long long*>(iv)[i] ^ reinterpret_cast<unsigned long long*>(header.iv)[i];
		}
	}
	else
	{
		memcpy(this->key, header.key, 32);
		memcpy(this->iv, header.iv, 32);
	}
#endif
	return true;
}
void ArchiveWriter::close()
{
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
	if (keys != INVALID_HANDLE_VALUE)
	{
		CloseHandle(keys);
		keys = INVALID_HANDLE_VALUE;
	}
	if (file != INVALID_HANDLE_VALUE)
	{
		CloseHandle(file);
		file = INVALID_HANDLE_VALUE;
	}
#else
	if (keys != -1)
	{
		::close(keys);
		keys = -1;
	}
	if (file != -1)
	{
		::close(file);
		file = -1;
	}
#endif
}

bool ArchiveWriter::add(const char* filename, unsigned int decompressed, unsigned int compressed, unsigned int hash, const void* data)
{
	unsigned int index = 0;
	std::string fname = filename;
	transform(fname.begin(), fname.end(), fname.begin(), tolower);
	for (unsigned int i = 0; i < fname.length(); ++i)
	{
		if (fname[i] == '\\')
		{
			fname[i] = '/';
		}
	}

	CryptoPP::CRC32 crc;
	crc.CalculateDigest(reinterpret_cast<unsigned char*>(&index), reinterpret_cast<const unsigned char*>(fname.c_str()), fname.length());

	unsigned short namelen = strlen(filename);
	FoxFS::TArchiveEntry entry;
	entry.decompressed = decompressed;
	entry.hash = hash;
	entry.offset = sizeof(entry);
	entry.size = compressed;
	entry.name = index;

	std::string fn = filename;
	transform(fn.begin(), fn.end(), fn.begin(), tolower);
	for (unsigned int i = 0; i < fn.length(); ++i)
	{
		if (fn[i] == '\\')
		{
			fn[i] = '/';
		}
	}

	CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption encoder(this->key, 32, this->iv);
	encoder.ProcessData(const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>(data)), reinterpret_cast<const unsigned char*>(data), compressed);

#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
	DWORD dwWritten;
	WriteFile(keys, &namelen, sizeof(namelen), &dwWritten, 0);
	WriteFile(keys, filename, namelen, &dwWritten, 0);
	WriteFile(keys, &hash, sizeof(hash), &dwWritten, 0);

	LARGE_INTEGER m, p;
	m.QuadPart = 0;
	SetFilePointerEx(file, m, &p, FILE_CURRENT);

	entry.offset += p.QuadPart;

	WriteFile(file, &entry, sizeof(entry), &dwWritten, 0);
	WriteFile(file, data, compressed, &dwWritten, 0);
#else
	write(keys, &namelen, sizeof(namelen));
	write(keys, filename, namelen);
	write(keys, &hash, sizeof(hash));

	entry.offset += lseek(file, 0, SEEK_CUR);

	write(file, &entry, sizeof(entry));
	write(file, data, compressed);
#endif
	return true;
}