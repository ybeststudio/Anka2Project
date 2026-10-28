#pragma once

#include "StdAfx.h"

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
#include "Packet.h"

class CPythonAcce : public CSingleton<CPythonAcce>
{
	public:
#ifdef ENABLE_GOLD_LIMIT
		long long dwPrice;
#else
		DWORD dwPrice;
#endif
		typedef std::vector<TAcceMaterial> TAcceMaterials;

	public:
		CPythonAcce();
		virtual ~CPythonAcce();

		void Clear();
#ifdef ENABLE_GOLD_LIMIT
		void AddMaterial(long long dwRefPrice, BYTE bPos, TItemPos tPos);
#else
		void AddMaterial(DWORD dwRefPrice, BYTE bPos, TItemPos tPos);
#endif
		void AddResult(DWORD dwItemVnum, DWORD dwMinAbs, DWORD dwMaxAbs);
#ifdef ENABLE_GOLD_LIMIT
		void RemoveMaterial(long long dwRefPrice, BYTE bPos);
#else
		void RemoveMaterial(DWORD dwRefPrice, BYTE bPos);
#endif
#ifdef ENABLE_GOLD_LIMIT
		long long GetPrice() {return dwPrice;}
#else
		DWORD GetPrice() {return dwPrice;}
#endif
		bool GetAttachedItem(BYTE bPos, BYTE & bHere, WORD & wCell);
		void GetResultItem(DWORD & dwItemVnum, DWORD & dwMinAbs, DWORD & dwMaxAbs);

	protected:
		TAcceResult m_vAcceResult;
		TAcceMaterials m_vAcceMaterials;
};
#endif
