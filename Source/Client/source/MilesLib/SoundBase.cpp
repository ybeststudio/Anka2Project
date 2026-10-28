#include "Stdafx.h"
#include "SoundBase.h"

#ifndef _WIN64
HDIGDRIVER				CSoundBase::ms_DIGDriver = nullptr;
TProvider *				CSoundBase::ms_pProviderDefault = nullptr;
std::vector<TProvider>	CSoundBase::ms_ProviderVector;
#endif

TSoundDataMap			CSoundBase::ms_dataMap;
bool					CSoundBase::ms_bInitialized = false;
int						CSoundBase::ms_iRefCount = 0;

CSoundBase::CSoundBase() {}
CSoundBase::~CSoundBase() {}

void CSoundBase::Destroy()
{
#ifndef _WIN64
	if (ms_iRefCount > 1) { --ms_iRefCount; return; }
	ms_iRefCount = 0;
	if (!ms_dataMap.empty())
	{
		for (auto i = ms_dataMap.begin(); i != ms_dataMap.end(); ++i)
			delete i->second;
		ms_dataMap.clear();
	}
	AIL_shutdown();
#endif
}

void CSoundBase::Initialize()
{
#ifndef _WIN64
	++ms_iRefCount;
	if (ms_iRefCount > 1) return;
	AIL_set_redist_directory("miles");
	AIL_startup();
	ms_ProviderVector.clear();
	ms_dataMap.clear();
#endif
}

DWORD CSoundBase::GetFileCRC(const char * filename)
{
	return GetCRC32(filename, strlen(filename));
}

CSoundData * CSoundBase::AddFile(DWORD dwFileCRC, const char* filename)
{
	CSoundData * pSoundData = new CSoundData;
	pSoundData->Assign(filename);
	ms_dataMap.insert(TSoundDataMap::value_type(dwFileCRC, pSoundData));
	return pSoundData;
}
