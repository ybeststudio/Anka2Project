#pragma once

class CMapObjectPropertyPageBase : public CDialog
{
	public:
		CMapObjectPropertyPageBase (UINT nIDTemplate, CWnd* pParentWnd = NULL);
		virtual ~CMapObjectPropertyPageBase();

		void UpdateUI (CProperty * pProperty);
		void UpdatePropertyData (const char* c_szPropertyName);
		bool Save (const char* c_szPathName, CProperty * pProperty);
		void Render (HWND hWnd);

		DWORD GetPropertyCRC32();

	protected:
		virtual void OnUpdateUI (CProperty * pProperty) = 0;
		virtual void OnUpdatePropertyData (const char* c_szPropertyName) = 0;
		virtual void OnRender (HWND hWnd) = 0;
		virtual bool OnSave (const char* c_szPathName, CProperty * pProperty) = 0;

	protected:
		DWORD m_dwCRC;
};
