#pragma once

#include "../../GameLib/ActorInstance.h"

class CActorInstanceAccessor : public CActorInstance
{
	public:
		CActorInstanceAccessor();
		virtual ~CActorInstanceAccessor();

		void ClearModel();
		void ClearMotion();
		void ClearAttachingEffect();

		BOOL IsModelThing();
		BOOL IsMotionThing();

		BOOL SetAccessorModel (CGraphicThing * pThing);
		BOOL SetAccessorMotion (CGraphicThing * pThing);

		void SetMotion();
		void SetMotionData (CRaceMotionData * pMotionData);
		float GetMotionDuration();

		DWORD GetBoneCount();
		void SetLocalTime (float fLocalTime);
		BOOL GetBoneName (DWORD dwIndex, std::string * pstrBoneName);

		BOOL GetBoneMatrix (DWORD dwBoneIndex, D3DXMATRIX** ppMatrix);
		BOOL GetBoneIndexByName (const char* c_szBoneName, int* pBoneIndex) const;

		void ClearAttachingObject();
		void AttachObject (const char* c_szFileName, const char* c_szBoneName);
		void UpdateAttachingObject();
		void RenderAttachingObject();

	protected:
		CGraphicThing* m_pModelThing;
		CGraphicThing* m_pMotionThing;

		struct SAttachingModelInstance
		{
			std::string strBoneName;
			CGraphicThing* pThing;
			CGrannyModelInstance* pModelInstance;
		};
		std::list<SAttachingModelInstance> m_AttachingObjectList;
};