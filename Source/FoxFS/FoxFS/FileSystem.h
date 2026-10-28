#ifndef FOXFS_FILESYSTEM_H
#define FOXFS_FILESYSTEM_H

#include <string>
#include <map>

#include "Archive.h"

#include <cryptopp/md5.h>

#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
#pragma warning(disable : 4996)
#include <windows.h>
#else
#include <pthread.h>
#endif

namespace FoxFS
{

	class FileSystem
	{
	public:
		FileSystem();
		~FileSystem();

		int load(const wchar_t* filename);
		int unload(const wchar_t* filename);

		unsigned int size(const char* filename) const;
		int exists(const char* filename) const;
		int get(const char* filename, void* buffer, unsigned int maxsize, unsigned int* outsize) const;

	private:
		std::map<std::basic_string<wchar_t>, Archive*> archives;

#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
		mutable CRITICAL_SECTION mutex;
#else
		mutable pthread_mutex_t mutex;
#endif
	};

}

#endif