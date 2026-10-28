#pragma once

#include "../../GameLib/Area.h"

class CAreaAccessor : public CArea
{
	public:
		struct SSelectObject
		{
			DWORD dwIndex;
			DWORD dwCRC32;
		};
		typedef std::list<SSelectObject> TSelectObjectList;

		struct TCollisionDataCounter
		{
			std::string stName;
			DWORD		count;
		};

		static void PrintCounter (const TCollisionDataCounter & r)
		{
			fprintf (ms_LogFile, "%2d : %s\n", r.count, r.stName.c_str());
		}

	public:
		CAreaAccessor();
		virtual ~CAreaAccessor();

		void Clear();

		bool Save (const std::string & c_rstrMapName);
		bool SaveCollisionData (const char* c_szLoadingAreaFileName, FILE * SavingFile);

		void RefreshArea();
		void UpdateObject (DWORD dwIndex, const TObjectData * c_pObjectData); // Height, Rotation

		void AddObject (const TObjectData * c_pObjectData);
		bool GetObjectPointer (DWORD dwIndex, TObjectData** ppObjectData);

		int GetPickedObjectIndex();
		BOOL IsSelectedObject (DWORD dwIndex);
		void SelectObject (DWORD dwIndex);

		int GetSelectedObjectCount();
		const CArea::TObjectData* GetLastSelectedObjectData() const;		// 가장 마지막에 선택한 오브젝트의 정보(위치, 회전 등..)를 리턴

		BOOL Picking();
		BOOL PickObject();

		void RenderSelectedObject();
		void CancelSelect();
		BOOL SelectObject (float fxStart, float fyStart, float fxEnd, float fyEnd);
		void DeleteSelectedObject();
		void MoveSelectedObject (float fx, float fy);
		void MoveSelectedObjectHeight (float fHeight);
		void AddSelectedAmbienceScale (int iAddScale);
		void AddSelectedAmbienceMaxVolumeAreaPercentage (float fPercentage);
		void AddSelectedObjectRotation (float fYaw, float fPitch, float fRoll);
		void SetSelectedObjectPortalNumber (int iID);
		void DelSelectedObjectPortalNumber (int iID);
		void CollectPortalNumber (std::set<int>* pkSet_iPortalNumber);
		BOOL IsSelected();

		static void OpenCollisionDataCountMapLog();
		static void CloseCollisionDataCountMapLog();

		//////////////////////////////////////////////////////////////////////////
		// Shadow Map
		void RenderToShadowMap();

		// 건물 텍스처 리로딩
		void ReloadBuildingTexture();

	protected:
		bool CheckInstanceIndex (DWORD dwIndex);
		bool GetInstancePointer (DWORD dwIndex, TObjectInstance** ppObjectInstance);

		bool __SaveObjects (const char* c_szFileName);
		bool __SaveAmbiences (const char* c_szOtherPathName, const char* c_szFileName);

		void __ClickObject (DWORD dwIndex);
		void __DeselectObject (DWORD dwIndex);
		void __RefreshSelectedInfo();
		void __RefreshObjectPosition (float fx, float fy, float fz);

	protected:
		TSelectObjectList							m_SelectObjectList;

	private:
		static FILE* 								ms_LogFile;
		static std::set<std::string>				ms_NonAttributeObjectSet;
		static std::vector<TCollisionDataCounter>	ms_CollisionDataCountVec;
};
