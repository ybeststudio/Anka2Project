#pragma once

#include "Packet.h"

#ifdef ENABLE_STATUS_EFFECT_SHOWER
class CPythonStatusEffectManager : public CSingleton<CPythonStatusEffectManager>
{
public:
	enum EStatusEffectType
	{
		STATUS_EFFECT_TYPE_AFFECT_TYPE = 0,
		STATUS_EFFECT_TYPE_AFFECT_FLAG = 1,
	};

	CPythonStatusEffectManager();
	virtual ~CPythonStatusEffectManager();

	void SetStatusEffectShowerHandler(PyObject* poHandler);
	void ClearStatusEffects();
	void RefreshStatusEffects();
	void RemoveAffect(DWORD dwAffect, bool is_buff = false);
	bool IsUsingAffectFlagBit(DWORD dwAffect);

	bool RecvStatusEffectPacket();

private:
	PyObject* m_poHandler;
};

void initStatusEffectManager();
#endif // ENABLE_STATUS_EFFECT_SHOWER
