#include <FoxFS.h>
#include "FileSystem.h"

#include <algorithm>

// Splash screen window procedure kaldýrýldý - antivirüs uyarýsýný önlemek için
// #if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
// LRESULT CALLBACK SplashScreenProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
// {
// 	return DefWindowProcW(hWnd, uMsg, wParam, lParam);
// }
// #endif

namespace FoxFS
{

	FileSystem::FileSystem()
	{
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
		InitializeCriticalSection(&mutex);
#else
		pthread_mutex_init(&mutex, 0);
#endif
	}
	FileSystem::~FileSystem()
	{
		for (std::map<std::basic_string<wchar_t>, Archive*>::iterator iter = archives.begin(); iter != archives.end(); ++iter)
		{
			delete iter->second;
		}
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
		DeleteCriticalSection(&mutex);
#else
		pthread_mutex_destroy(&mutex);
#endif
	}

	int FileSystem::load(const wchar_t* filename)
	{
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
		EnterCriticalSection(&mutex);
#else
		pthread_mutex_lock(&mutex);
#endif
		int r = Archive::ERROR_OK;
		std::basic_string<wchar_t> fn = filename;
		transform(fn.begin(), fn.end(), fn.begin(), tolower);

		std::map<std::basic_string<wchar_t>, Archive*>::iterator iter = archives.find(fn);
		if (iter == archives.end())
		{
			Archive* a = new Archive();
			if (a)
			{
					if ((r = a->load(filename, 0, 0)) == Archive::ERROR_OK)
					{
						archives.insert(std::map<std::basic_string<wchar_t>, Archive*>::value_type(fn, a));
					}
					else
					{
						delete a;
					}
			}
		}
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
		LeaveCriticalSection(&mutex);
#else
		pthread_mutex_unlock(&mutex);
#endif
		return r;
	}
	int FileSystem::unload(const wchar_t* filename)
	{
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
		EnterCriticalSection(&mutex);
#else
		pthread_mutex_lock(&mutex);
#endif
		std::basic_string<wchar_t> fn = filename;
		transform(fn.begin(), fn.end(), fn.begin(), tolower);

		std::map<std::basic_string<wchar_t>, Archive*>::iterator iter = archives.find(fn);

		int r = Archive::ERROR_ARCHIVE_NOT_FOUND;
		if (iter != archives.end())
		{
			delete iter->second;
			archives.erase(iter);
			r = Archive::ERROR_OK;
		}
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
		LeaveCriticalSection(&mutex);
#else
		pthread_mutex_unlock(&mutex);
#endif
		return Archive::ERROR_ARCHIVE_NOT_FOUND;
	}

	unsigned int FileSystem::size(const char* filename) const
	{
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
		EnterCriticalSection(&mutex);
#else
		pthread_mutex_lock(&mutex);
#endif
		unsigned int r = 0;
		for (std::map<std::basic_string<wchar_t>, Archive*>::const_iterator iter = archives.begin(); iter != archives.end(); ++iter)
		{
			if ((r = iter->second->size(filename)) > 0)
			{
				break;
			}
		}
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
		LeaveCriticalSection(&mutex);
#else
		pthread_mutex_unlock(&mutex);
#endif
		return r;
	}
	int FileSystem::exists(const char* filename) const
	{
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
		EnterCriticalSection(&mutex);
#else
		pthread_mutex_lock(&mutex);
#endif
		int r = Archive::ERROR_FILE_WAS_NOT_FOUND;
		for (std::map<std::basic_string<wchar_t>, Archive*>::const_iterator iter = archives.begin(); iter != archives.end(); ++iter)
		{
			if ((r = iter->second->exists(filename)) == Archive::ERROR_OK)
			{
				break;
			}
		}
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
		LeaveCriticalSection(&mutex);
#else
		pthread_mutex_unlock(&mutex);
#endif
		return r;
	}
	int FileSystem::get(const char* filename, void* buffer, unsigned int maxsize, unsigned int* outsize) const
	{
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
		EnterCriticalSection(&mutex);
#else
		pthread_mutex_lock(&mutex);
#endif
		int r = Archive::ERROR_FILE_WAS_NOT_FOUND;
		for (std::map<std::basic_string<wchar_t>, Archive*>::const_iterator iter = archives.begin(); iter != archives.end(); ++iter)
		{
			if ((r = iter->second->get(filename, buffer, maxsize, outsize)) == Archive::ERROR_OK)
			{
				break;
			}
		}
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
		LeaveCriticalSection(&mutex);
#else
		pthread_mutex_unlock(&mutex);
#endif
		return r;
	}

}