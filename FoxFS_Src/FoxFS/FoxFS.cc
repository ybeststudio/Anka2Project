#include <FoxFS.h>

#include <cstdlib>
#include <cstring>

#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
#include <windows.h>
#else
#include <limits.h>
#endif

#include "FileSystem.h"

extern "C"
{

	PFoxFS FOXFS_EXPORT FOXFS_API FoxFS_Create() { return reinterpret_cast<PFoxFS>(new FoxFS::FileSystem()); }
	void FOXFS_EXPORT FOXFS_API FoxFS_Destroy(PFoxFS manager) { delete reinterpret_cast<FoxFS::FileSystem*>(manager); }

	int FOXFS_EXPORT FOXFS_API FoxFS_LoadA(PFoxFS manager, const char* filename)
	{
		wchar_t file[
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
			MAX_PATH + 1
#else
			PATH_MAX + 1
#endif
		] = { 0 };
			mbstowcs(file, filename, strlen(filename));
			return FoxFS_LoadW(manager, file);
	}
	int FOXFS_EXPORT FOXFS_API FoxFS_LoadW(PFoxFS manager, const wchar_t* filename) { return reinterpret_cast<FoxFS::FileSystem*>(manager)->load(filename); }
	int FOXFS_EXPORT FOXFS_API FoxFS_UnloadA(PFoxFS manager, const char* filename)
	{
		wchar_t file[
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
			MAX_PATH + 1
#else
			PATH_MAX + 1
#endif
		] = { 0 };
			mbstowcs(file, filename, strlen(filename));
			return FoxFS_LoadW(manager, file);
	}
	int FOXFS_EXPORT FOXFS_API FoxFS_UnloadW(PFoxFS manager, const wchar_t* filename) { return reinterpret_cast<FoxFS::FileSystem*>(manager)->unload(filename); }

	unsigned int FOXFS_EXPORT FOXFS_API FoxFS_SizeA(PFoxFS manager, const char* filename) { return reinterpret_cast<FoxFS::FileSystem*>(manager)->size(filename); }
	unsigned int FOXFS_EXPORT FOXFS_API FoxFS_SizeW(PFoxFS manager, const wchar_t* filename)
	{
		char file[
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
			MAX_PATH + 1
#else
			PATH_MAX + 1
#endif
		] = { 0 };
			wcstombs(file, filename, wcslen(filename));
			return FoxFS_SizeA(manager, file);
	}
	int FOXFS_EXPORT FOXFS_API FoxFS_ExistsA(PFoxFS manager, const char* filename) { return reinterpret_cast<FoxFS::FileSystem*>(manager)->exists(filename); }
	int FOXFS_EXPORT FOXFS_API FoxFS_ExistsW(PFoxFS manager, const wchar_t* filename)
	{
		char file[
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
			MAX_PATH + 1
#else
			PATH_MAX + 1
#endif
		] = { 0 };
			wcstombs(file, filename, wcslen(filename));
			return FoxFS_ExistsA(manager, file);
	}
	int FOXFS_EXPORT FOXFS_API FoxFS_GetA(PFoxFS manager, const char* filename, void* buffer, unsigned int maxsize, unsigned int* outsize) { return reinterpret_cast<FoxFS::FileSystem*>(manager)->get(filename, buffer, maxsize, outsize); }
	int FOXFS_EXPORT FOXFS_API FoxFS_GetW(PFoxFS manager, const wchar_t* filename, void* buffer, unsigned int maxsize, unsigned int* outsize)
	{
		char file[
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
			MAX_PATH + 1
#else
			PATH_MAX + 1
#endif
		] = { 0 };
			wcstombs(file, filename, wcslen(filename));
			return FoxFS_GetA(manager, file, buffer, maxsize, outsize);
	}

}