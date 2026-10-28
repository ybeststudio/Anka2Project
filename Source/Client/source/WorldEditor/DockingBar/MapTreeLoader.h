#pragma once

#include "../../GameLib/PropertyLoader.h"

class CTreeLoader : public CPropertyLoader
{
	public:
		CTreeLoader();
		virtual ~CTreeLoader();

		void SetTreeControler (CPropertyTreeControler * pPropertyTree);
		void SetParentItem (HTREEITEM hItem);

		void DownFolder (const char* c_szFilter, const char* c_szPathName);
		bool OnFolder (const char* c_szFilter, const char* c_szPathName, const char* c_szFileName);
		bool OnFile (const char* c_szPathName, const char* c_szFileName);

	protected:
		CPropertyTreeControler* m_pTreeControler;
		HTREEITEM	m_hParentItem;
		HTREEITEM	m_hNextItem;
};
