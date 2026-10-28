#include "StdAfx.h"
#include <math.h>
#include "SoundManager.h"
#include "../EterBase/Timer.h"

#ifndef _WIN64
CSoundManager2D CSoundManager::ms_SoundManager2D;
CSoundManager3D CSoundManager::ms_SoundManager3D;
CSoundManagerStream CSoundManager::ms_SoundManagerStream;
#endif

#ifdef _WIN64
static SoundEngine s_SoundEngineInstance;

CSoundManager::CSoundManager()
{
	m_bInitialized = FALSE;
	m_isSoundDisable = FALSE;
	m_fxPosition = 0.0f;
	m_fyPosition = 0.0f;
	m_fzPosition = 0.0f;
	m_fSoundScale = 200.0f;
	m_fAmbienceSoundScale = 1000.0f;
	m_fSoundVolume = 1.0f;
	m_fMusicVolume = 1.0f;
	m_fBackupMusicVolume = 0.0f;
	m_fBackupSoundVolume = 0.0f;
}

CSoundManager::~CSoundManager() {}

BOOL CSoundManager::Create()
{
	m_bInitialized = SoundEngine::Instance().Initialize();
	return m_bInitialized ? TRUE : FALSE;
}

void CSoundManager::Destroy() {}

void CSoundManager::SetPosition(float fx, float fy, float fz)
{
	m_fxPosition = fx; m_fyPosition = fy; m_fzPosition = fz;
	SoundEngine::Instance().SetListenerPosition(fx, fy, fz);
}

void CSoundManager::SetDirection(float fxDir, float fyDir, float fzDir, float fxUp, float fyUp, float fzUp)
{
	SoundEngine::Instance().SetListenerOrientation(fxDir, fyDir, fzDir, fxUp, fyUp, fzUp);
}

void CSoundManager::Update()
{
	SoundEngine::Instance().Update();
}

float CSoundManager::GetSoundScale() { return m_fSoundScale; }
void CSoundManager::SetSoundScale(float fScale) { m_fSoundScale = fScale; }
void CSoundManager::SetAmbienceSoundScale(float fScale) { m_fAmbienceSoundScale = fScale; }
void CSoundManager::SetSoundVolume(float fVolume)
{
	m_fSoundVolume = fVolume;
	SoundEngine::Instance().SetSoundVolume(fVolume);
}
void CSoundManager::SetSoundVolumeRatio(float fRatio) { SetSoundVolume(fRatio); }
void CSoundManager::SetMusicVolume(float fVolume)
{
	m_fMusicVolume = fVolume;
	SoundEngine::Instance().SetMusicVolume(fVolume);
}
void CSoundManager::SetMusicVolumeRatio(float fRatio) { SetMusicVolume(fRatio); }
void CSoundManager::SetSoundVolumeGrade(int iGrade) { SetSoundVolume(float(iGrade) / 5.0f); }
void CSoundManager::SetMusicVolumeGrade(int iGrade) { SetMusicVolume(float(iGrade) / 5.0f); }
void CSoundManager::SaveVolume() { SoundEngine::Instance().SaveVolume(false); }
void CSoundManager::RestoreVolume() { SoundEngine::Instance().RestoreVolume(); }
float CSoundManager::GetSoundVolume() { return m_fSoundVolume; }
float CSoundManager::GetMusicVolume() { return SoundEngine::Instance().GetMusicVolume(); }

void CSoundManager::PlaySound2D(const char * c_szFileName)
{
	if (!c_szFileName || !*c_szFileName) return;
	SoundEngine::Instance().PlaySound2D(c_szFileName);
}
void CSoundManager::PlaySound2DLoop(const char * c_szFileName)
{
	if (!c_szFileName || !*c_szFileName) return;
	SoundEngine::Instance().PlaySound2DLoop(c_szFileName);
}
void CSoundManager::StopSound2D(const char * c_szFileName)
{
	if (!c_szFileName || !*c_szFileName) return;
	SoundEngine::Instance().StopSound2D(c_szFileName);
}
void CSoundManager::PlaySound3D(float fx, float fy, float fz, const char * c_szFileName, int iPlayCount)
{
	if (!c_szFileName || !*c_szFileName) return;
	SoundEngine::Instance().PlaySound3D(c_szFileName, fx, fy, fz);
}
void CSoundManager::StopSound3D(int iIndex) { SoundEngine::Instance().StopAmbience(iIndex); }
int CSoundManager::PlayAmbienceSound3D(float fx, float fy, float fz, const char * c_szFileName, int iPlayCount)
{
	if (!c_szFileName || !*c_szFileName) return -1;
	auto instance = SoundEngine::Instance().PlayAmbienceSound3D(fx, fy, fz, c_szFileName, iPlayCount);
	return SoundEngine::Instance().GetAmbienceHandle(instance);
}
void CSoundManager::PlayCharacterSound3D(float fx, float fy, float fz, const char * c_szFileName, BOOL bCheckFrequency)
{
	if (!c_szFileName || !*c_szFileName) return;
	SoundEngine::Instance().PlaySound3D(c_szFileName, fx, fy, fz);
}
void CSoundManager::SetSoundVolume3D(int iIndex, float fVolume) { SoundEngine::Instance().SetAmbienceVolume(iIndex, fVolume); }
void CSoundManager::StopAllSound3D()
{
	SoundEngine::Instance().StopAllSound3D();
}

void CSoundManager::PlayMusic(const char * c_szFileName)
{
	if (!c_szFileName || !*c_szFileName) return;
	SoundEngine::Instance().FadeInMusic(c_szFileName, m_fMusicVolume, 0.1f);
}
void CSoundManager::FadeInMusic(const char * c_szFileName, float fVolumeSpeed)
{
	if (!c_szFileName || !*c_szFileName) return;
	SoundEngine::Instance().FadeInMusic(c_szFileName, m_fMusicVolume, 1.5f);
}
void CSoundManager::FadeOutMusic(const char * c_szFileName, float fVolumeSpeed)
{
	if (!c_szFileName || !*c_szFileName) return;
	SoundEngine::Instance().FadeOutMusic(c_szFileName, 0.0f, 1.5f);
}
void CSoundManager::FadeLimitOutMusic(const char * c_szFileName, float fLimitVolume, float fVolumeSpeed)
{
	if (!c_szFileName || !*c_szFileName) return;
	SoundEngine::Instance().FadeOutMusic(c_szFileName, fLimitVolume, 1.5f);
}
void CSoundManager::FadeOutAllMusic()
{
	SoundEngine::Instance().FadeOutAllMusic();
}
void CSoundManager::FadeAll()
{
	SoundEngine::Instance().FadeOutAllMusic();
}

void CSoundManager::UpdateSoundData(DWORD dwcurFrame, const NSound::TSoundDataVector * c_pSoundDataVector) {}
void CSoundManager::UpdateSoundData(float fx, float fy, float fz, DWORD dwcurFrame, const NSound::TSoundDataVector * c_pSoundDataVector) {}
void CSoundManager::UpdateSoundInstance(float fx, float fy, float fz, DWORD dwcurFrame, const NSound::TSoundInstanceVector * c_pSoundInstanceVector, BOOL bCheckFrequency)
{
	if (c_pSoundInstanceVector)
		SoundEngine::Instance().UpdateSoundInstance(fx, fy, fz, dwcurFrame, c_pSoundInstanceVector, bCheckFrequency != FALSE);
}
void CSoundManager::UpdateSoundInstance(DWORD dwcurFrame, const NSound::TSoundInstanceVector * c_pSoundInstanceVector) {}
#endif
