#include "StdAfx.h"
#ifndef _WIN64
#include "stdafx.h"
#include "SoundManager3D.h"
#include "../eterBase/Timer.h"

CSoundInstance3D::CSoundInstance3D() : m_sample(nullptr), m_pSoundData(nullptr)
{
}

CSoundInstance3D::~CSoundInstance3D()
{
	Destroy();
}

void CSoundInstance3D::Destroy()
{
	SAFE_RELEASE(m_pSoundData);

	if (m_sample)
	{
		AIL_release_3D_sample_handle(m_sample);
		m_sample = nullptr;
	}
}

bool CSoundInstance3D::Initialize()
{
	if (m_sample)
		return true;

	m_sample = AIL_allocate_3D_sample_handle(ms_pProviderDefault->hProvider);
	return m_sample ? true : false;
}

bool CSoundInstance3D::SetSound(CSoundData* pSoundData)
{
	assert(m_sample != nullptr && pSoundData != nullptr);

	LPVOID lpData = pSoundData->Get();

	if (m_pSoundData != nullptr)
	{
		m_pSoundData->Release();
		m_pSoundData = nullptr;
	}

	if (AIL_set_3D_sample_file(m_sample, lpData) == NULL)
	{
		TraceError("%s: %s", AIL_last_error(), pSoundData->GetFileName());
		pSoundData->Release();
		return false;
	}

	m_pSoundData = pSoundData;

	AIL_set_3D_position(m_sample, 0.0F, 0.0F, 0.0F);
	AIL_auto_update_3D_position(m_sample, 0);
	return true;
}

bool CSoundInstance3D::IsDone() const
{
	return AIL_3D_sample_status(m_sample) == SMP_DONE;
}

void CSoundInstance3D::Play(int iLoopCount, DWORD dwPlayCycleTimeLimit) const
{
	if (!m_pSoundData)
		return;

	DWORD dwCurTime = ELTimer_GetMSec();

	if (dwCurTime - m_pSoundData->GetPlayTime() < dwPlayCycleTimeLimit)
		return;

	m_pSoundData->SetPlayTime(dwCurTime);

	AIL_set_3D_sample_loop_count(m_sample, iLoopCount);
	AIL_start_3D_sample(m_sample);
}

void CSoundInstance3D::Pause() const
{
	AIL_stop_3D_sample(m_sample);
}

void CSoundInstance3D::Resume() const
{
	AIL_resume_3D_sample(m_sample);
}

void CSoundInstance3D::Stop()
{
	AIL_end_3D_sample(m_sample);
}

void CSoundInstance3D::GetVolume(float& rfVolume) const
{
	rfVolume = AIL_3D_sample_volume(m_sample);
}

void CSoundInstance3D::SetVolume(float volume) const
{
	volume = max(0.0f, min(1.0f, volume));
	AIL_set_3D_sample_volume(m_sample, volume);
}

void CSoundInstance3D::SetPosition(float x, float y, float z) const
{
	AIL_set_3D_position(m_sample, x, y, -z);
}

void CSoundInstance3D::SetOrientation(float x_face, float y_face, float z_face,
									  float x_normal, float y_normal, float z_normal) const
{
	assert(!" CSoundInstance3D::SetOrientation - Don't use this function");
}

void CSoundInstance3D::SetVelocity(float fDistanceX, float fDistanceY, float fDistanceZ, float fNagnitude) const
{
	AIL_set_3D_velocity(m_sample, fDistanceX, fDistanceY, fDistanceZ, fNagnitude);
	AIL_auto_update_3D_position(m_sample, 1);
}

void CSoundInstance3D::UpdatePosition(float fElapsedTime)
{
	AIL_update_3D_position(m_sample, fElapsedTime);
}
#endif // _WIN64
