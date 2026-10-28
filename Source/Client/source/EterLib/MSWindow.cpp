#include "StdAfx.h"
#include "MsWindow.h"

#include <windowsx.h>

#define DISABLE_TASKBAR_GROUPING
#ifdef DISABLE_TASKBAR_GROUPING
	#include <Shobjidl.h>
#endif

CMSWindow::TWindowClassSet CMSWindow::ms_stWCSet;
HINSTANCE CMSWindow::ms_hInstance = nullptr;

LRESULT CALLBACK MSWindowProcedure(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	CMSWindow * pWnd = (CMSWindow *) GetWindowLongPtr(hWnd, GWLP_USERDATA);

	if (pWnd)
		return pWnd->WindowProcedure(hWnd, uiMsg, wParam, lParam);

	return DefWindowProc(hWnd, uiMsg, wParam, lParam);
}

LRESULT CMSWindow::WindowProcedure(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uiMsg)
	{
		case WM_SIZE:
			OnSize(wParam, lParam);
			break;

		case WM_ACTIVATEAPP:
			m_isActive = (wParam == WA_ACTIVE) || (wParam == WA_CLICKACTIVE);
			break;
	}

	return DefWindowProc(hWnd, uiMsg, wParam, lParam);
}

void CMSWindow::OnSize(WPARAM wParam, LPARAM /*lParam*/)
{
	if (wParam == SIZE_MINIMIZED)
	{
		InvalidateRect(m_hWnd, nullptr, true);
		m_isActive = false;
		m_isVisible = false;
	}
	else
	{
		m_isActive = true;
		m_isVisible = true;
	}
}

void CMSWindow::Destroy()
{
	if (!m_hWnd)
		return;

	if (IsWindow(m_hWnd))
		DestroyWindow(m_hWnd);

	m_hWnd = nullptr;
	m_isVisible = false;
}

bool CMSWindow::Create(const char *c_szName, int brush, DWORD cs, DWORD ws, HICON hIcon, int iCursorResource)
{
	//assert(ms_hInstance != nullptr);
	Destroy();

	const char *c_szClassName = RegisterWindowClass(cs, brush, MSWindowProcedure, hIcon, iCursorResource);

	m_hWnd = CreateWindow(
						c_szClassName,
						c_szName,
						ws,
						0, 0, 0, 0,
						nullptr,
						nullptr,
						ms_hInstance,
						nullptr);

	if (!m_hWnd)
		return false;

#ifdef DISABLE_TASKBAR_GROUPING
	OSVERSIONINFO v;
	v.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&v);
	if (v.dwMajorVersion == 6 && v.dwMinorVersion >= 1 || v.dwMajorVersion > 6)
	{
		WCHAR myAppID[128];
		swprintf(myAppID, sizeof(myAppID) / sizeof(myAppID[0]), L"MyMetin2AppID%u", GetCurrentProcessId());
		HRESULT hr = SetCurrentProcessExplicitAppUserModelID(myAppID);
		if (!SUCCEEDED(hr))
			return false;
	}
#endif

	SetWindowLongPtr(m_hWnd, GWLP_USERDATA, (LONG_PTR) this);
	//DestroyWindow(ImmGetDefaultIMEWnd(m_hWnd));

	return true;
}

void CMSWindow::SetVisibleMode(bool isVisible)
{
	m_isVisible = isVisible;

	if (m_isVisible)
	{
		ShowWindow(m_hWnd, SW_SHOW);
	}
	else
	{
		ShowWindow(m_hWnd, SW_HIDE);
	}
}

void CMSWindow::Show()
{
	m_isVisible = true;
	ShowWindow(m_hWnd, SW_SHOW);
}

void CMSWindow::Hide()
{
	m_isVisible = false;
	ShowWindow(m_hWnd, SW_HIDE);
}

bool CMSWindow::IsVisible()
{
	return m_isVisible;
}

bool CMSWindow::IsActive()
{
	return m_isActive;
}

HINSTANCE CMSWindow::GetInstance()
{
	return ms_hInstance;
}

HWND CMSWindow::GetWindowHandle()
{
	return m_hWnd;
}

int	CMSWindow::GetScreenWidth()
{
	return GetSystemMetrics(SM_CXSCREEN);
}

int	CMSWindow::GetScreenHeight()
{
	return GetSystemMetrics(SM_CYSCREEN);
}

void CMSWindow::GetWindowRect(RECT* prc)
{
	::GetWindowRect(m_hWnd, prc);
}


void CMSWindow::GetClientRect(RECT* prc)
{
	::GetClientRect(m_hWnd, prc);
}

void CMSWindow::GetMousePosition(POINT* ppt)
{
	GetCursorPos(ppt);
	ScreenToClient(m_hWnd, ppt);
}

void CMSWindow::SetPosition(int x, int y)
{
	SetWindowPos(m_hWnd, nullptr, x, y, 0, 0, SWP_NOZORDER|SWP_NOSIZE);
}

void CMSWindow::SetCenterPosition()
{
	RECT rc;

	GetClientRect(&rc);

	int windowWidth = rc.right - rc.left;
	int windowHeight = rc.bottom - rc.top;

	SetPosition((GetScreenWidth()-windowWidth)/2, (GetScreenHeight()-windowHeight)/2);
}

void CMSWindow::AdjustSize(int width, int height)
{
	SetRect(&m_rect, 0, 0, width, height);

	AdjustWindowRectEx(&m_rect,
						GetWindowStyle(m_hWnd),
						GetMenu(m_hWnd ) != nullptr,
						GetWindowExStyle(m_hWnd ) );

	MoveWindow
	(
		m_hWnd,
		0,
		0,
		m_rect.right - m_rect.left,
		m_rect.bottom - m_rect.top,
		FALSE
	);
}

void CMSWindow::SetText(const char *c_szText)
{
	SetWindowText(m_hWnd, c_szText);
}

void CMSWindow::SetSize(int width, int height)
{
	SetWindowPos(m_hWnd, nullptr, 0, 0, width, height, SWP_NOZORDER|SWP_NOMOVE);
}

const char *CMSWindow::RegisterWindowClass(DWORD style, int brush, WNDPROC pfnWndProc, HICON hIcon, int iCursorResource)
{
	char szClassName[1024];
	snprintf(szClassName, sizeof(szClassName), "eter - s%x:b%x:p:%p", style, brush, (void*) pfnWndProc);

	TWindowClassSet::iterator f = ms_stWCSet.find((char *) szClassName);

	if (f != ms_stWCSet.end())
		return *f;

	const char *c_szStaticClassName = stl_static_string(szClassName).c_str();

	ms_stWCSet.insert((char *const) c_szStaticClassName);
	
	WNDCLASS wc;

	wc.style			= 0;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.lpfnWndProc		= pfnWndProc;
	wc.hCursor			= LoadCursor(ms_hInstance, MAKEINTRESOURCE(iCursorResource));
	wc.hIcon			= hIcon ? hIcon : LoadIcon(ms_hInstance, IDI_APPLICATION);
	wc.hbrBackground	= (HBRUSH) GetStockObject(brush);
	wc.hInstance		= ms_hInstance;
	wc.lpszClassName	= c_szStaticClassName;
	wc.lpszMenuName		= "";

	if (!RegisterClass(&wc))
		return "";

	return c_szStaticClassName;
}

CMSWindow::CMSWindow()
{
	m_hWnd=NULL;
	m_isVisible=false;
}

CMSWindow::~CMSWindow()
{
}