// XBrowseForFolder.cpp  Version 1.0
//
// Author:  Hans Dietrich
//          hdietrich2@hotmail.com
//
// Description:
//     XBrowseForFolder.cpp implements XBrowseForFolder(), a function that
//     wraps SHBrowseForFolder().
//
// History
//     Version 1.0 - 2003 September 25
//     - Initial public release
//
// This software is released into the public domain.  You are free to use it
// in any way you like.
//
// This software is provided "as is" with no expressed or implied warranty.
// I accept no liability for any damage or loss of business that this software
// may cause.
//
///////////////////////////////////////////////////////////////////////////////

// if you don't want to use MFC, comment out the following line:
#include "StdAfx.h"

#ifndef __AFX_H__
	#include "windows.h"
	#include "crtdbg.h"
	#include "tchar.h"
#endif

#include "Shlobj.h"
#include "io.h"
#include "XBrowseForFolder.h"

#pragma warning(disable: 4127)	// conditional expression is constant (_ASSERTE)


#ifndef __AFX_H__

#define TRACE (void(0))

///////////////////////////////////////////////////////////////////////////////
// CRect - a minimal CRect class

class CRect : public tagRECT
{
	public:
		CRect() { }
		CRect (int l, int t, int r, int b)
		{
			left = l;
			top = t;
			right = r;
			bottom = b;
		}
		int Width() const
		{
			return right - left;
		}
		int Height() const
		{
			return bottom - top;
		}
		void SwapLeftRight()
		{
			SwapLeftRight (LPRECT (this));
		}
		static void SwapLeftRight (LPRECT lpRect)
		{
			LONG temp = lpRect->left;
			lpRect->left = lpRect->right;
			lpRect->right = temp;
		}
		operator LPRECT()
		{
			return this;
		}
};
#endif


///////////////////////////////////////////////////////////////////////////////
// ScreenToClientX - helper function in case non-MFC
static void ScreenToClientX (HWND hWnd, LPRECT lpRect)
{
	_ASSERTE (::IsWindow (hWnd));
	::ScreenToClient (hWnd, (LPPOINT)lpRect);
	::ScreenToClient (hWnd, ((LPPOINT)lpRect) +1);
}

///////////////////////////////////////////////////////////////////////////////
// MoveWindowX - helper function in case non-MFC
static void MoveWindowX (HWND hWnd, CRect& rect, BOOL bRepaint)
{
	_ASSERTE (::IsWindow (hWnd));
	::MoveWindow (hWnd, rect.left, rect.top,
				  rect.Width(), rect.Height(), bRepaint);
}

///////////////////////////////////////////////////////////////////////////////
// BrowseCallbackProc - SHBrowseForFolder callback function
static int CALLBACK BrowseCallbackProc (HWND hwnd,		// Window handle to the browse dialog box
										UINT uMsg,		// Value identifying the event
										LPARAM lParam,	// Value dependent upon the message
										LPARAM lpData)	// Application-defined value that was
// specified in the lParam member of the
// BROWSEINFO structure
{
	switch (uMsg)
	{
		case BFFM_INITIALIZED:		// sent when the browse dialog box has finished initializing.
		{
			// remove context help button from dialog caption
			LONG lStyle = ::GetWindowLong (hwnd, GWL_STYLE);
			lStyle &= ~DS_CONTEXTHELP;
			::SetWindowLong (hwnd, GWL_STYLE, lStyle);
			lStyle = ::GetWindowLong (hwnd, GWL_EXSTYLE);
			lStyle &= ~WS_EX_CONTEXTHELP;
			::SetWindowLong (hwnd, GWL_EXSTYLE, lStyle);

			// set initial directory
			::SendMessage (hwnd, BFFM_SETSELECTION, TRUE, lpData);

			// find the folder tree and make dialog larger
			HWND hwndTree = FindWindowEx (hwnd, NULL, _T ("SysTreeView32"), NULL);
			if (hwndTree)
			{
				// make the dialog larger
				CRect rectDlg;
				::GetWindowRect (hwnd, &rectDlg);
				rectDlg.right += 40;
				rectDlg.bottom += 30;
				MoveWindowX (hwnd, rectDlg, TRUE);
				::GetClientRect (hwnd, &rectDlg);

				// move the Cancel button
				CRect rectCancel (0, 0, 0, 0);
				HWND hwndCancel = ::GetDlgItem (hwnd, IDCANCEL);
				if (hwndCancel)
				{
					::GetWindowRect (hwndCancel, &rectCancel);
				}
				ScreenToClientX (hwnd, &rectCancel);
				int h = rectCancel.Height();
				int w = rectCancel.Width();
				rectCancel.bottom = rectDlg.bottom - 5;
				rectCancel.top = rectCancel.bottom - h;
				rectCancel.right = rectDlg.right - 5;
				rectCancel.left = rectCancel.right - w;
				if (hwndCancel)
				{
					MoveWindowX (hwndCancel, rectCancel, FALSE);
				}

				// move the OK button
				CRect rectOK (0, 0, 0, 0);
				HWND hwndOK = ::GetDlgItem (hwnd, IDOK);
				if (hwndOK)
				{
					::GetWindowRect (hwndOK, &rectOK);
				}
				ScreenToClientX (hwnd, &rectOK);
				rectOK.bottom = rectDlg.bottom - 5;
				rectOK.top = rectOK.bottom - h;
				rectOK.right = rectCancel.left - 10;
				rectOK.left = rectOK.right - w;
				if (hwndOK)
				{
					MoveWindowX (hwndOK, rectOK, FALSE);
				}

				// expand the folder tree to fill the dialog
				CRect rectTree;
				::GetWindowRect (hwndTree, &rectTree);
				ScreenToClientX (hwnd, &rectTree);
				rectTree.top = 5;
				rectTree.left = 5;
				rectTree.bottom = rectOK.top - 5;
				rectTree.right = rectDlg.right - 5;
				MoveWindowX (hwndTree, rectTree, FALSE);
			}
			else
			{
				TRACE (_T ("ERROR - tree control not found.\n"));
				_ASSERTE (hwndTree);
			}
		}
		break;

		case BFFM_SELCHANGED:		// sent when the selection has changed
		{
			TCHAR szDir[MAX_PATH * 2] = { 0 };

			// fail if non-filesystem
			BOOL bRet = SHGetPathFromIDList ((LPITEMIDLIST) lParam, szDir);
			if (bRet)
			{
				// fail if folder not accessible
				if (_taccess (szDir, 00) != 0)
				{
					bRet = FALSE;
				}
				else
				{
					SHFILEINFO sfi;
					::SHGetFileInfo ((LPCTSTR)lParam, 0, &sfi, sizeof (sfi),
									 SHGFI_PIDL | SHGFI_ATTRIBUTES);
					TRACE (_T ("dwAttributes=0x%08X\n"), sfi.dwAttributes);

					// fail if pidl is a link
					if (sfi.dwAttributes & SFGAO_LINK)
					{
						TRACE (_T ("SFGAO_LINK\n"));
						bRet = FALSE;
					}
				}
			}

			// if invalid selection, disable the OK button
			if (!bRet)
			{
				::EnableWindow (GetDlgItem (hwnd, IDOK), FALSE);
			}

			TRACE (_T ("szDir=%s\n"), szDir);
		}
		break;
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
//
// XBrowseForFolder()
//
// Purpose:     Invoke the SHBrowseForFolder API.  If lpszInitialFolder is
//              supplied, it will be the folder initially selected in the tree
//              folder list.  Otherwise, the initial folder will be set to the
//              current directory.  The selected folder will be returned in
//              lpszBuf.
//
// Parameters:  hWnd              - handle to the owner window for the dialog
//              lpszInitialFolder - initial folder in tree;  if NULL, the initial
//                                  folder will be the current directory.
//              lpszBuf           - buffer for the returned folder path
//              dwBufSize         - size of lpszBuf in TCHARs
//
// Returns:     BOOL - TRUE = success;  FALSE = user hit Cancel
//
BOOL XBrowseForFolder (HWND hWnd,
					   LPCTSTR lpszInitialFolder,
					   LPTSTR lpszBuf,
					   DWORD dwBufSize)
{
	_ASSERTE (lpszBuf);
	_ASSERTE (dwBufSize >= MAX_PATH);

	if (lpszBuf == NULL || dwBufSize < MAX_PATH)
	{
		return FALSE;
	}

	lpszBuf[0] = _T ('\0');

	TCHAR szInitialPath[MAX_PATH * 2];
	ZeroMemory (szInitialPath, sizeof (szInitialPath));

	if (lpszInitialFolder && lpszInitialFolder[0] != _T ('\0'))
	{
		_tcsncpy (szInitialPath, lpszInitialFolder,
				  sizeof (szInitialPath) / sizeof (TCHAR) -2);
	}
	else
	{
		// no initial folder, set to current directory
		::GetCurrentDirectory (sizeof (szInitialPath) / sizeof (TCHAR) -2,
							   szInitialPath);
	}

	BROWSEINFO bi;
	ZeroMemory (&bi, sizeof (BROWSEINFO));

	bi.hwndOwner = hWnd;
	bi.ulFlags   = BIF_RETURNONLYFSDIRS;	// do NOT use BIF_NEWDIALOGSTYLE,
	// BIF_EDITBOX, or BIF_STATUSTEXT
	bi.lpfn      = BrowseCallbackProc;
	bi.lParam    = (LPARAM) szInitialPath;

	LPITEMIDLIST pidl = SHBrowseForFolder (&bi);

	BOOL bRet = FALSE;

	if (pidl)
	{
		TCHAR szBuffer[MAX_PATH * 2];
		szBuffer[0] = _T ('\0');

		if (SHGetPathFromIDList (pidl, szBuffer))
		{
			ZeroMemory (lpszBuf, dwBufSize);
			_tcsncpy (lpszBuf, szBuffer, dwBufSize - 1);
			bRet = TRUE;
		}
		else
		{
			TRACE (_T ("SHGetPathFromIDList failed\n"));
		}

		IMalloc *pMalloc = NULL;
		if (SUCCEEDED (SHGetMalloc (&pMalloc)) && pMalloc)
		{
			pMalloc->Free (pidl);
			pMalloc->Release();
		}
	}

	return bRet;
}
