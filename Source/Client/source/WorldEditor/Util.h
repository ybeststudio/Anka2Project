#pragma once

#include "../eterGrnLib/ModelInstance.h"
#include "./DockingBar/PageCtrl.h"

void CreateBitmapButton (CButton * pButton, int iBitmapID, CBitmap & rBitmap);
void RepositioningWindow (CPageCtrl & rPageCtrl, int ixTemporarySpace, int iyTemporarySpace);
void CreateHighColorImageList (int iBitmapID, CImageList * pImageList);

void SetDialogFloatText (HWND hWnd, int iID, float fData);
void SetDialogIntegerText (HWND hWnd, int iID, int iData);
float GetDialogFloatText (HWND hWnd, int iID);
int GetDialogIntegerText (HWND hWnd, int iID);
void SelectComboBoxItem (CComboBox & rComboBox, DWORD dwIndex);

extern char g_szProgramPath[PROGRAM_PATH_LENGTH + 1];
extern char g_szProgramWindowPath[PROGRAM_PATH_LENGTH + 1];

extern const char* GetProgramExcutingPath (const char* c_szFullFileName);

void CreateUtilData();
void DestroyUtilData();

///////////////////////////////////////////////////////////////////////////////////////////////////

void RenderSphere (const D3DXMATRIX * c_pmatWorld, float fx, float fy, float fz, float fRadius, BOOL isFilled = FALSE);
void RenderCylinder (const D3DXMATRIX * c_pmatWorld, float fx, float fy, float fz, float fRadius, float fLength, BOOL isFilled = FALSE);

///////////////////////////////////////////////////////////////////////////////////////////////////

class CTileInstance : public CGrannyModelInstance
{
	public:
		CTileInstance() {}
		~CTileInstance() {}

		void LoadTexture();
		void RenderTile();

	protected:
		CGraphicImageInstance m_ImageInstance;
};

class COpenFileDialog
{
	public:
		COpenFileDialog (CWnd * pParentWnd = NULL, DWORD dwFlag = 0, const char* c_pszTitle = NULL, const char* c_pszFilter = NULL, const char* c_pszDefaultPath = NULL,
						 const char* c_pfirstName = NULL) :
			m_pWnd (pParentWnd),
			m_dwFlag (dwFlag),
			mc_pszTitle (c_pszTitle),
			mc_pszDefaultPath (c_pszDefaultPath)
		{
			strncpy (m_szFilter, c_pszFilter, MAX_PATH);

			char* p = m_szFilter;
			while ((p = strchr (p, '|')))
			{
				* (p++) = '\0';
			}

			CWinApp * pApplication = (CWinApp*) AfxGetApp();
			assert (pApplication);

			memset (m_szFileName, 0, sizeof (m_szFileName));
			memset (m_szPathName, 0, sizeof (m_szPathName));
			if (c_pfirstName)
			{
				strcpy (m_szPathName, c_pfirstName);
			}

			memset (&m_ofn, 0, sizeof (OPENFILENAME));

			m_ofn.lStructSize		= sizeof (OPENFILENAME);
			m_ofn.hwndOwner			= m_pWnd->GetSafeHwnd();
			m_ofn.hInstance			= pApplication->m_hInstance;
			m_ofn.lpstrFilter		= m_szFilter;
			m_ofn.nFilterIndex		= 1L;

			m_ofn.lpstrFile			= m_szPathName;
			m_ofn.nMaxFile			= sizeof (m_szPathName);
			m_ofn.lpstrFileTitle	= m_szFileName;
			m_ofn.nMaxFileTitle		= sizeof (m_szFileName);

			m_ofn.Flags				= OFN_PATHMUSTEXIST | m_dwFlag;
			m_ofn.lpstrTitle		= mc_pszTitle;
			m_ofn.lpstrInitialDir	= mc_pszDefaultPath;
			m_ofn.lpstrDefExt		= NULL;
		}

		~COpenFileDialog() {};

		virtual bool DoModal()
		{
			if (!GetOpenFileName (&m_ofn))
			{
				return FALSE;
			}

			return TRUE;
		}

		const char* GetPathName()
		{
			return m_szPathName;
		}

	protected:
		char			m_szPathName[MAX_PATH + 1];		// Filename including path and extension
		char			m_szFileName[MAX_PATH + 1];		// Filename and extension without path
		char			m_szFilter[MAX_PATH + 1];			// File Filter

		CWnd* 			m_pWnd;
		DWORD			m_dwFlag;
		const char* 	mc_pszTitle;
		const char* 	mc_pszDefaultPath;
		OPENFILENAME	m_ofn;
};

class CSaveFileDialog : public COpenFileDialog
{
	public:
		CSaveFileDialog (CWnd * pParentWnd = NULL, DWORD dwFlag = 0, const char* c_pszTitle = NULL, const char* c_pszFilter = NULL, const char* c_pszDefaultPath = NULL,
						 const char* c_pfirstName = NULL) :
			COpenFileDialog (pParentWnd, dwFlag, c_pszTitle, c_pszFilter, c_pszDefaultPath,  c_pfirstName)
		{
		}

		virtual bool DoModal()
		{
			if (!GetSaveFileName (&m_ofn))
			{
				return FALSE;
			}

			return TRUE;
		}
};

extern void RenderBackGroundCharacter (float fx, float fy, float fz);
extern void RenderBackGroundTile();
extern void DestroyBackGroundData();

extern std::string RemoveStartString (const std::string & strOriginal, const std::string & strBegin);
