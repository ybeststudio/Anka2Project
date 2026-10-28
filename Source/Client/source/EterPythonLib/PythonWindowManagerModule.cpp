#include "StdAfx.h"
#include "PythonWindow.h"
#include "PythonSlotWindow.h"
#include "PythonGridSlotWindow.h"

bool PyTuple_GetWindow(PyObject *poArgs, int pos, UI::CWindow ** ppRetWindow)
{
	return PyTuple_GetPointer(poArgs, pos, ppRetWindow) && *ppRetWindow != nullptr;
}

PyObject *wndMgrGetAspect(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("f", UI::CWindowManager::Instance().GetAspect());
}

PyObject *wndMgrOnceIgnoreMouseLeftButtonUpEvent(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindowManager::Instance().OnceIgnoreMouseLeftButtonUpEvent();
	return Py_BuildNone();
}

PyObject *wndMgrSetMouseHandler(PyObject *poSelf, PyObject *poArgs)
{
	PyObject *poHandler;
	if (!PyTuple_GetObject(poArgs, 0, &poHandler))
		return Py_BuildException();

	UI::CWindowManager::Instance().SetMouseHandler(poHandler);
	return Py_BuildNone();
}

/////////////////////////////////////////////////////////////////////////////////////////////////
///// Register /////
// Window
PyObject *wndMgrRegister(PyObject *poSelf, PyObject *poArgs)
{
	PyObject *po;
	if (!PyTuple_GetObject(poArgs, 0, &po))
		return Py_BuildException();
	char *szLayer;
	if (!PyTuple_GetString(poArgs, 1, &szLayer))
		return Py_BuildException();

	UI::CWindowManager& kWndMgr=UI::CWindowManager::Instance();
	UI::CWindow * pWindow = kWndMgr.RegisterWindow(po, szLayer);
	if (!pWindow)
		return Py_BuildException();
	
	return Py_BuildPointer(pWindow);
}

// SlotWindow
PyObject *wndMgrRegisterSlotWindow(PyObject *poSelf, PyObject *poArgs)
{
	PyObject *po;
	if (!PyTuple_GetObject(poArgs, 0, &po))
		return Py_BuildException();
	char *szLayer;
	if (!PyTuple_GetString(poArgs, 1, &szLayer))
		return Py_BuildException();

	UI::CWindow * pWindow = UI::CWindowManager::Instance().RegisterSlotWindow(po, szLayer);
	return Py_BuildPointer(pWindow);
}

// GridSlotWindow
PyObject *wndMgrRegisterGridSlotWindow(PyObject *poSelf, PyObject *poArgs)
{
	PyObject *po;
	if (!PyTuple_GetObject(poArgs, 0, &po))
		return Py_BuildException();
	char *szLayer;
	if (!PyTuple_GetString(poArgs, 1, &szLayer))
		return Py_BuildException();

	UI::CWindow * pWindow = UI::CWindowManager::Instance().RegisterGridSlotWindow(po, szLayer);
	return Py_BuildPointer(pWindow);
}

// TextLine
PyObject *wndMgrRegisterTextLine(PyObject *poSelf, PyObject *poArgs)
{
	PyObject *po;
	if (!PyTuple_GetObject(poArgs, 0, &po))
		return Py_BuildException();
	char *szLayer;
	if (!PyTuple_GetString(poArgs, 1, &szLayer))
		return Py_BuildException();

	UI::CWindow * pWindow = UI::CWindowManager::Instance().RegisterTextLine(po, szLayer);
	return Py_BuildPointer(pWindow);
}

// MarkBox
PyObject *wndMgrRegisterMarkBox(PyObject *poSelf, PyObject *poArgs)
{
	PyObject *po;
	if (!PyTuple_GetObject(poArgs, 0, &po))
		return Py_BuildException();
	char *szLayer;
	if (!PyTuple_GetString(poArgs, 1, &szLayer))
		return Py_BuildException();

	UI::CWindow * pWindow = UI::CWindowManager::Instance().RegisterMarkBox(po, szLayer);
	return Py_BuildPointer(pWindow);
}

// ImageBox
PyObject *wndMgrRegisterImageBox(PyObject *poSelf, PyObject *poArgs)
{
	PyObject *po;
	if (!PyTuple_GetObject(poArgs, 0, &po))
		return Py_BuildException();
	char *szLayer;
	if (!PyTuple_GetString(poArgs, 1, &szLayer))
		return Py_BuildException();

	UI::CWindow * pWindow = UI::CWindowManager::Instance().RegisterImageBox(po, szLayer);
	return Py_BuildPointer(pWindow);
}

// ExpandedImageBox
PyObject *wndMgrRegisterExpandedImageBox(PyObject *poSelf, PyObject *poArgs)
{
	PyObject *po;
	if (!PyTuple_GetObject(poArgs, 0, &po))
		return Py_BuildException();
	char *szLayer;
	if (!PyTuple_GetString(poArgs, 1, &szLayer))
		return Py_BuildException();

	UI::CWindow * pWindow = UI::CWindowManager::Instance().RegisterExpandedImageBox(po, szLayer);
	return Py_BuildPointer(pWindow);
}

// AniImageBox
PyObject *wndMgrRegisterAniImageBox(PyObject *poSelf, PyObject *poArgs)
{
	PyObject *po;
	if (!PyTuple_GetObject(poArgs, 0, &po))
		return Py_BuildException();
	char *szLayer;
	if (!PyTuple_GetString(poArgs, 1, &szLayer))
		return Py_BuildException();

	UI::CWindow * pWindow = UI::CWindowManager::Instance().RegisterAniImageBox(po, szLayer);
	return Py_BuildPointer(pWindow);
}

// RegisterButton
PyObject *wndMgrRegisterButton(PyObject *poSelf, PyObject *poArgs)
{
	PyObject *po;
	if (!PyTuple_GetObject(poArgs, 0, &po))
		return Py_BuildException();
	char *szLayer;
	if (!PyTuple_GetString(poArgs, 1, &szLayer))
		return Py_BuildException();

	UI::CWindow * pWindow = UI::CWindowManager::Instance().RegisterButton(po, szLayer);
	return Py_BuildPointer(pWindow);
}

// RadioButton
PyObject *wndMgrRegisterRadioButton(PyObject *poSelf, PyObject *poArgs)
{
	PyObject *po;
	if (!PyTuple_GetObject(poArgs, 0, &po))
		return Py_BuildException();
	char *szLayer;
	if (!PyTuple_GetString(poArgs, 1, &szLayer))
		return Py_BuildException();

	UI::CWindow * pWindow = UI::CWindowManager::Instance().RegisterRadioButton(po, szLayer);
	return Py_BuildPointer(pWindow);
}

// ToggleButton
PyObject *wndMgrRegisterToggleButton(PyObject *poSelf, PyObject *poArgs)
{
	PyObject *po;
	if (!PyTuple_GetObject(poArgs, 0, &po))
		return Py_BuildException();
	char *szLayer;
	if (!PyTuple_GetString(poArgs, 1, &szLayer))
		return Py_BuildException();

	UI::CWindow * pWindow = UI::CWindowManager::Instance().RegisterToggleButton(po, szLayer);
	return Py_BuildPointer(pWindow);
}

// DragButton
PyObject *wndMgrRegisterDragButton(PyObject *poSelf, PyObject *poArgs)
{
	PyObject *po;
	if (!PyTuple_GetObject(poArgs, 0, &po))
		return Py_BuildException();
	char *szLayer;
	if (!PyTuple_GetString(poArgs, 1, &szLayer))
		return Py_BuildException();

	UI::CWindow * pWindow = UI::CWindowManager::Instance().RegisterDragButton(po, szLayer);
	return Py_BuildPointer(pWindow);
}

#ifdef ENABLE_FLEXIBLE_WIDTH_BUTTON
PyObject * wndMgrRegisterFlexibleWidthButton(PyObject * poSelf, PyObject * poArgs)
{
	PyObject * po;
	if (!PyTuple_GetObject(poArgs, 0, &po))
		return Py_BuildException();
	char * szLayer;
	if (!PyTuple_GetString(poArgs, 1, &szLayer))
		return Py_BuildException();

	UI::CWindow * pWindow = UI::CWindowManager::Instance().RegisterFlexibleWidthButton(po, szLayer);
	return Py_BuildPointer(pWindow);
}

PyObject * wndMgrRegisterFlexibleWidthToggleButton(PyObject * poSelf, PyObject * poArgs)
{
	PyObject * po;
	if (!PyTuple_GetObject(poArgs, 0, &po))
		return Py_BuildException();
	char * szLayer;
	if (!PyTuple_GetString(poArgs, 1, &szLayer))
		return Py_BuildException();

	UI::CWindow * pWindow = UI::CWindowManager::Instance().RegisterFlexibleWidthToggleButton(po, szLayer);
	return Py_BuildPointer(pWindow);
}

PyObject * wndMgrGetFlexibleWidthButtonSideWidth(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	UI::CFlexibleWidthButton * pButton = (UI::CFlexibleWidthButton *)pWindow;
	return Py_BuildValue("i", pButton->GetSideWidth());
}

PyObject * wndMgrSetFlexibleWidthButtonSideVisual(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	int iVisualType;
	if (!PyTuple_GetInteger(poArgs, 1, &iVisualType))
		return Py_BuildException();

	int iSideType;
	if (!PyTuple_GetInteger(poArgs, 2, &iSideType))
		return Py_BuildException();

	char * szFileName;
	if (!PyTuple_GetString(poArgs, 3, &szFileName))
		return Py_BuildException();

	UI::CFlexibleWidthButton * pButton = (UI::CFlexibleWidthButton *)pWindow;
	if (!pButton->SetSideVisual(iVisualType, iSideType, szFileName))
		return Py_BuildException();

	return Py_BuildNone();
}

PyObject * wndMgrSetKeepUpVisual(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	bool bKeepUp;
	if (!PyTuple_GetBoolean(poArgs, 1, &bKeepUp))
		return Py_BuildException();

	UI::CButton * pButton = (UI::CButton *)pWindow;
	pButton->SetKeepUpVisual(bKeepUp);
	return Py_BuildNone();
}

PyObject * wndMgrIsKeepUpVisual(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	UI::CButton * pButton = (UI::CButton *)pWindow;
	return Py_BuildValue("b", pButton->IsKeepUpVisual());
}

PyObject * wndMgrGetButtonImageWidth(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	UI::CButton * pButton = (UI::CButton *)pWindow;
	return Py_BuildValue("i", pButton->GetVisualWidth());
}

PyObject * wndMgrGetButtonImageHeight(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	UI::CButton * pButton = (UI::CButton *)pWindow;
	return Py_BuildValue("i", pButton->GetVisualHeight());
}
#endif

// Box
PyObject *wndMgrRegisterBox(PyObject *poSelf, PyObject *poArgs)
{
	PyObject *po;
	if (!PyTuple_GetObject(poArgs, 0, &po))
		return Py_BuildException();
	char *szLayer;
	if (!PyTuple_GetString(poArgs, 1, &szLayer))
		return Py_BuildException();

	UI::CWindow * pWindow = UI::CWindowManager::Instance().RegisterBox(po, szLayer);
	return Py_BuildPointer(pWindow);
}

#ifdef ENABLE_RENDER_TARGET
// RenderTarget
PyObject *wndMgrRegisterRenderTarget(PyObject *poSelf, PyObject *poArgs)
{
	PyObject *po;
	if (!PyTuple_GetObject(poArgs, 0, &po))
		return Py_BuildException();
	char *szLayer;
	if (!PyTuple_GetString(poArgs, 1, &szLayer))
		return Py_BuildException();

	UI::CWindow* pWindow = UI::CWindowManager::Instance().RegisterRenderTarget(po, szLayer);
	return Py_BuildPointer(pWindow);
}

PyObject *wndRenderTargetSetRenderTarget(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow* pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();
	int index;
	if (!PyTuple_GetInteger(poArgs, 1, &index))
		return Py_BuildException();

	if (pWindow->IsType(UI::CRenderTarget::Type()))
	{
		((UI::CRenderTarget*)pWindow)->SetRenderTarget(index);
	}
	return Py_BuildNone();
}
#endif

// Bar
PyObject *wndMgrRegisterBar(PyObject *poSelf, PyObject *poArgs)
{
	PyObject *po;
	if (!PyTuple_GetObject(poArgs, 0, &po))
		return Py_BuildException();
	char *szLayer;
	if (!PyTuple_GetString(poArgs, 1, &szLayer))
		return Py_BuildException();

	UI::CWindow * pWindow = UI::CWindowManager::Instance().RegisterBar(po, szLayer);
	return Py_BuildPointer(pWindow);
}

// Line
PyObject *wndMgrRegisterLine(PyObject *poSelf, PyObject *poArgs)
{
	PyObject *po;
	if (!PyTuple_GetObject(poArgs, 0, &po))
		return Py_BuildException();
	char *szLayer;
	if (!PyTuple_GetString(poArgs, 1, &szLayer))
		return Py_BuildException();

	UI::CWindow * pWindow = UI::CWindowManager::Instance().RegisterLine(po, szLayer);
	return Py_BuildPointer(pWindow);
}

// Slot
PyObject *wndMgrRegisterBar3D(PyObject *poSelf, PyObject *poArgs)
{
	PyObject *po;
	if (!PyTuple_GetObject(poArgs, 0, &po))
		return Py_BuildException();
	char *szLayer;
	if (!PyTuple_GetString(poArgs, 1, &szLayer))
		return Py_BuildException();

	UI::CWindow * pWindow = UI::CWindowManager::Instance().RegisterBar3D(po, szLayer);
	return Py_BuildPointer(pWindow);
}

// NumberLine
PyObject *wndMgrRegisterNumberLine(PyObject *poSelf, PyObject *poArgs)
{
	PyObject *po;
	if (!PyTuple_GetObject(poArgs, 0, &po))
		return Py_BuildException();
	char *szLayer;
	if (!PyTuple_GetString(poArgs, 1, &szLayer))
		return Py_BuildException();

	UI::CWindow * pWindow = UI::CWindowManager::Instance().RegisterNumberLine(po, szLayer);
	return Py_BuildPointer(pWindow);
}
///// Register /////
/////////////////////////////////////////////////////////////////////////////////////////////////

PyObject *wndMgrDestroy(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	UI::CWindowManager::Instance().DestroyWindow(pWin);
	return Py_BuildNone();
}

PyObject *wndMgrIsFocus(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	return Py_BuildValue("i", pWindow == UI::CWindowManager::Instance().GetActivateWindow());
}

PyObject *wndMgrSetFocus(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	UI::CWindowManager::Instance().ActivateWindow(pWin);
	return Py_BuildNone();
}

PyObject *wndMgrKillFocus(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		if (pWin == UI::CWindowManager::Instance().GetActivateWindow())
		{
			UI::CWindowManager::Instance().DeactivateWindow();
		}
	}
	else
	{
		UI::CWindowManager::Instance().DeactivateWindow();
	}

	return Py_BuildNone();
}

#ifdef ENABLE_UI_TAB_FOCUS_SYSTEM
PyObject *wndMgrSetFocusable(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow *pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	bool bFlag;
	if (!PyTuple_GetBoolean(poArgs, 1, &bFlag))
		return Py_BuildException();

	pWin->SetFocusable(bFlag);
	return Py_BuildNone();
}
#endif

PyObject *wndMgrLock(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	UI::CWindowManager::Instance().LockWindow(pWin);
	return Py_BuildNone();
}

PyObject *wndMgrUnlock(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindowManager::Instance().UnlockWindow();
	return Py_BuildNone();
}

PyObject *wndMgrSetName(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();
	char *szName;
	if (!PyTuple_GetString(poArgs, 1, &szName))
		return Py_BuildException();

	pWin->SetName(szName);
	return Py_BuildNone();
}

PyObject *wndMgrSetTop(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	UI::CWindowManager::Instance().SetTop(pWin);
	return Py_BuildNone();
}

PyObject *wndMgrShow(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	pWin->Show();
	return Py_BuildNone();
}

PyObject *wndMgrHide(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	pWin->Hide();
	return Py_BuildNone();
}

PyObject *wndMgrIsShow(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	return Py_BuildValue("i", pWin->IsShow());
}

PyObject *wndMgrIsRTL(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	return Py_BuildValue("i", pWin->IsFlag(UI::CWindow::FLAG_RTL));
}

PyObject* wndMgrSetUIScale(PyObject*, PyObject* args)
{
    float scale;
    if (!PyTuple_GetFloat(args, 0, &scale)) return Py_BuildException();
    UI::CWindowManager::Instance().SetUIScale(scale);
    return Py_BuildNone();
}
PyObject* wndMgrGetUIScale(PyObject*, PyObject*)
{
    return Py_BuildValue("f", UI::CWindowManager::Instance().GetUIScale());
}

PyObject *wndMgrSetScreenSize(PyObject *poSelf, PyObject *poArgs)
{
	int width;
	if (!PyTuple_GetInteger(poArgs, 0, &width))
		return Py_BuildException();
	int height;
	if (!PyTuple_GetInteger(poArgs, 1, &height))
		return Py_BuildException();

	UI::CWindowManager::Instance().SetScreenSize(width, height);
	return Py_BuildNone();
}

PyObject *wndMgrSetParent(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();
	UI::CWindow * pParentWin;
	if (!PyTuple_GetWindow(poArgs, 1, &pParentWin))
		return Py_BuildException();

	UI::CWindowManager::Instance().SetParent(pWin, pParentWin);
	return Py_BuildNone();
}

PyObject *wndMgrSetPickAlways(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	UI::CWindowManager::Instance().SetPickAlways(pWin);
	return Py_BuildNone();
}

PyObject *wndMgrSetWndSize(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();
	int width;
	if (!PyTuple_GetInteger(poArgs, 1, &width))
		return Py_BuildException();
	int height;
	if (!PyTuple_GetInteger(poArgs, 2, &height))
		return Py_BuildException();

	pWin->SetSize(width, height);
	return Py_BuildNone();
}

PyObject *wndMgrSetWndPosition(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();
	int x;
	if (!PyTuple_GetInteger(poArgs, 1, &x))
		return Py_BuildException();
	int y;
	if (!PyTuple_GetInteger(poArgs, 2, &y))
		return Py_BuildException();

	pWin->SetPosition(x, y);
	return Py_BuildNone();
}

PyObject *wndMgrGetName(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	return Py_BuildValue("s", pWin->GetName());
}

PyObject *wndMgrGetWndWidth(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	return Py_BuildValue("i", pWin->GetWidth());
}

PyObject *wndMgrGetWndHeight(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	return Py_BuildValue("i", pWin->GetHeight());
}

PyObject *wndMgrGetWndLocalPosition(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	long lx, ly;
	pWin->GetPosition(&lx, &ly);

	return Py_BuildValue("ii", lx, ly);
}

PyObject *wndMgrGetWndGlobalPosition(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	RECT & rRect = pWindow->GetRect();
	return Py_BuildValue("ii", rRect.left, rRect.top);
}

PyObject *wndMgrGetWindowRect(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	RECT & rRect = pWin->GetRect();
	return Py_BuildValue("iiii", rRect.left, rRect.top, rRect.right - rRect.left, rRect.bottom - rRect.top);
}

PyObject *wndMgrSetWindowHorizontalAlign(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();
	int iAlign;
	if (!PyTuple_GetInteger(poArgs, 1, &iAlign))
		return Py_BuildException();

	pWin->SetHorizontalAlign(iAlign);

	return Py_BuildNone();
}

PyObject *wndMgrSetWindowVerticalAlign(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();
	int iAlign;
	if (!PyTuple_GetInteger(poArgs, 1, &iAlign))
		return Py_BuildException();

	pWin->SetVerticalAlign(iAlign);

	return Py_BuildNone();
}

PyObject * wndMgrIsIn(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();
#ifdef INSIDE_RENDER
	bool bCheckChilds;
	if (!PyTuple_GetBoolean(poArgs, 1, &bCheckChilds))
		bCheckChilds = false;

	UI::CWindow* pWinPoint = UI::CWindowManager::Instance().GetPointWindow();
	if (pWin == pWinPoint)
		return Py_BuildValue("b", true);

	if (bCheckChilds && pWinPoint)
	{
		if (pWin->IsChild(pWinPoint, true))
			return Py_BuildValue("b", true);
	}
	return Py_BuildValue("b", false);
#else
	return Py_BuildValue("i", pWin == UI::CWindowManager::Instance().GetPointWindow());
#endif
}

PyObject * wndMgrGetMouseLocalPosition(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	long lx, ly;
	pWin->GetMouseLocalPosition(lx, ly);
	return Py_BuildValue("ii", lx, ly);
}

PyObject * wndMgrGetHyperlink(PyObject * poSelf, PyObject * poArgs)
{
	char retBuf[1024];
	int retLen = CGraphicTextInstance::Hyperlink_GetText(retBuf, sizeof(retBuf)-1);
	retBuf[retLen] = '\0';

	return Py_BuildValue("s#", retBuf, retLen);
}

PyObject * wndMgrGetScreenWidth(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", UI::CWindowManager::Instance().GetScreenWidth());
}

PyObject * wndMgrGetScreenHeight(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", UI::CWindowManager::Instance().GetScreenHeight());
}

PyObject * wndMgrGetMousePosition(PyObject * poSelf, PyObject * poArgs)
{
	long lx, ly;
	UI::CWindowManager::Instance().GetMousePosition(lx, ly);
	return Py_BuildValue("ii", lx, ly);
}

PyObject * wndMgrIsDragging(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", UI::CWindowManager::Instance().IsDragging());
}

PyObject * wndMgrIsPickedWindow(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	UI::CWindow * pPickedWin = UI::CWindowManager::Instance().GetPointWindow();
	return Py_BuildValue("i", pWin == pPickedWin ? 1 : 0);
}

PyObject * wndMgrGetChildCount(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	return Py_BuildValue("i", pWin->GetChildCount());
}

PyObject * wndMgrAddFlag(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	char * pszFlag;
	if (!PyTuple_GetString(poArgs, 1, &pszFlag))
		return Py_BuildException();

	if (pszFlag && *pszFlag)
	{
		if (!stricmp(pszFlag, "movable"))
			pWin->AddFlag(UI::CWindow::FLAG_MOVABLE);
		else if (!stricmp(pszFlag, "limit"))
			pWin->AddFlag(UI::CWindow::FLAG_LIMIT);
		else if (!stricmp(pszFlag, "dragable"))
			pWin->AddFlag(UI::CWindow::FLAG_DRAGABLE);
		else if (!stricmp(pszFlag, "attach"))
			pWin->AddFlag(UI::CWindow::FLAG_ATTACH);
		else if (!stricmp(pszFlag, "restrict_x"))
			pWin->AddFlag(UI::CWindow::FLAG_RESTRICT_X);
		else if (!stricmp(pszFlag, "restrict_y"))
			pWin->AddFlag(UI::CWindow::FLAG_RESTRICT_Y);
		else if (!stricmp(pszFlag, "float"))
			pWin->AddFlag(UI::CWindow::FLAG_FLOAT);
		else if (!stricmp(pszFlag, "not_pick"))
			pWin->AddFlag(UI::CWindow::FLAG_NOT_PICK);
		else if (!stricmp(pszFlag, "ignore_size"))
			pWin->AddFlag(UI::CWindow::FLAG_IGNORE_SIZE);
		else if (!stricmp(pszFlag, "rtl"))
			pWin->AddFlag(UI::CWindow::FLAG_RTL);
		else if (!stricmp(pszFlag, "ltr"))
			pWin->RemoveFlag(UI::CWindow::FLAG_RTL);
		else if (!stricmp(pszFlag, "alpha_sensitive"))
			pWin->AddFlag(UI::CWindow::FLAG_ALPHA_SENSITIVE);
		else if (!stricmp(pszFlag, "remove_limit"))
			pWin->AddFlag(UI::CWindow::FLAG_REMOVE_LIMIT);
		else if (!stricmp(pszFlag, "window_without_alpha"))
			pWin->AddFlag(UI::CWindow::FLAG_IS_BOARD_WITHOUT_ALPHA);
		else
			TraceError("Unknown window flag %s", pszFlag);
	}

	return Py_BuildNone();
}

PyObject *wndMgrSetLimitBias(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	int l;
	if (!PyTuple_GetInteger(poArgs, 1, &l))
		return Py_BuildException();
	int r;
	if (!PyTuple_GetInteger(poArgs, 2, &r))
		return Py_BuildException();
	int t;
	if (!PyTuple_GetInteger(poArgs, 3, &t))
		return Py_BuildException();
	int b;
	if (!PyTuple_GetInteger(poArgs, 4, &b))
		return Py_BuildException();

	pWin->SetLimitBias(l, r, t, b);
	return Py_BuildNone();
}

PyObject *wndMgrUpdateRect(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	pWin->UpdateRect();
	return Py_BuildNone();
}

PyObject *wndMgrAppendSlot(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iIndex))
		return Py_BuildException();

	int ixPosition;
	if (!PyTuple_GetInteger(poArgs, 2, &ixPosition))
		return Py_BuildException();

	int iyPosition;
	if (!PyTuple_GetInteger(poArgs, 3, &iyPosition))
		return Py_BuildException();

	int ixCellSize;
	if (!PyTuple_GetInteger(poArgs, 4, &ixCellSize))
		return Py_BuildException();

	int iyCellSize;
	if (!PyTuple_GetInteger(poArgs, 5, &iyCellSize))
		return Py_BuildException();

	if (!pWin->IsType(UI::CSlotWindow::Type()))
		return Py_BuildException();

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	pSlotWin->AppendSlot(iIndex, ixPosition, iyPosition, ixCellSize, iyCellSize);

	return Py_BuildNone();
}

PyObject *wndMgrArrangeSlot(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	int iStartIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iStartIndex))
		return Py_BuildException();

	int ixCellCount;
	if (!PyTuple_GetInteger(poArgs, 2, &ixCellCount))
		return Py_BuildException();

	int iyCellCount;
	if (!PyTuple_GetInteger(poArgs, 3, &iyCellCount))
		return Py_BuildException();

	int ixCellSize;
	if (!PyTuple_GetInteger(poArgs, 4, &ixCellSize))
		return Py_BuildException();

	int iyCellSize;
	if (!PyTuple_GetInteger(poArgs, 5, &iyCellSize))
		return Py_BuildException();

	int ixBlank;
	if (!PyTuple_GetInteger(poArgs, 6, &ixBlank))
		return Py_BuildException();

	int iyBlank;
	if (!PyTuple_GetInteger(poArgs, 7, &iyBlank))
		return Py_BuildException();

	if (!pWin->IsType(UI::CGridSlotWindow::Type()))
	{
		TraceError("wndMgr.ArrangeSlot : not a grid window");
		return Py_BuildException();
	}

	UI::CGridSlotWindow * pGridSlotWin = (UI::CGridSlotWindow *)pWin;
	pGridSlotWin->ArrangeGridSlot(iStartIndex, ixCellCount, iyCellCount, ixCellSize, iyCellSize, ixBlank, iyBlank);

	return Py_BuildNone();
}

PyObject *wndMgrSetSlotBaseImage(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	char *szFileName;
	if (!PyTuple_GetString(poArgs, 1, &szFileName))
		return Py_BuildException();

	float fr;
	if (!PyTuple_GetFloat(poArgs, 2, &fr))
		return Py_BuildException();
	float fg;
	if (!PyTuple_GetFloat(poArgs, 3, &fg))
		return Py_BuildException();
	float fb;
	if (!PyTuple_GetFloat(poArgs, 4, &fb))
		return Py_BuildException();
	float fa;
	if (!PyTuple_GetFloat(poArgs, 5, &fa))
		return Py_BuildException();

	if (!pWin->IsType(UI::CSlotWindow::Type()))
	{
		TraceError("wndMgr.ArrangeSlot : not a slot window");
		return Py_BuildException();
	}

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	pSlotWin->SetSlotBaseImage(szFileName, fr, fg, fb, fa);

	return Py_BuildNone();
}

PyObject *wndMgrSetSlotCoolTimeInverse(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow* pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
		return Py_BuildException();

	float fCoolTime;
	if (!PyTuple_GetFloat(poArgs, 2, &fCoolTime))
		return Py_BuildException();

	float fRemainingTime = 0.0f;
	PyTuple_GetFloat(poArgs, 3, &fRemainingTime);

	if (!pWin->IsType(UI::CSlotWindow::Type()))
		return Py_BuildException();

	UI::CSlotWindow* pSlotWin = (UI::CSlotWindow*)pWin;
	pSlotWin->SetSlotCoolTimeInverse(iSlotIndex, fCoolTime, fRemainingTime);

	return Py_BuildNone();
}

PyObject *wndMgrSetCoverButton(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
		return Py_BuildException();

	char *szUpImageName;
	if (!PyTuple_GetString(poArgs, 2, &szUpImageName))
		return Py_BuildException();
	char *szOverImageName;
	if (!PyTuple_GetString(poArgs, 3, &szOverImageName))
		return Py_BuildException();
	char *szDownImageName;
	if (!PyTuple_GetString(poArgs, 4, &szDownImageName))
		return Py_BuildException();
	char *szDisableImageName;
	if (!PyTuple_GetString(poArgs, 5, &szDisableImageName))
		return Py_BuildException();

	int iLeftButtonEnable;
	if (!PyTuple_GetInteger(poArgs, 6, &iLeftButtonEnable))
		return Py_BuildException();
	int iRightButtonEnable;
	if (!PyTuple_GetInteger(poArgs, 7, &iRightButtonEnable))
		return Py_BuildException();

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWindow;
	pSlotWin->SetCoverButton(iSlotIndex, szUpImageName, szOverImageName, szDownImageName, szDisableImageName, iLeftButtonEnable, iRightButtonEnable);

	return Py_BuildNone();
}

PyObject *wndMgrEnableCoverButton(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
		return Py_BuildException();

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWindow;
	pSlotWin->EnableCoverButton(iSlotIndex);

	return Py_BuildNone();
}

PyObject *wndMgrDisableCoverButton(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
		return Py_BuildException();

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWindow;
	pSlotWin->DisableCoverButton(iSlotIndex);

	return Py_BuildNone();
}

PyObject *wndMgrIsDisableCoverButton(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
		return Py_BuildException();

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWindow;
	return Py_BuildValue("i", pSlotWin->IsDisableCoverButton(iSlotIndex));
}

PyObject *wndMgrSetAlwaysRenderCoverButton(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	int iSlotIndex;
	bool bAlwaysRender = false;

	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
		return Py_BuildException();

	if (!PyTuple_GetBoolean(poArgs, 2, &bAlwaysRender))
		return Py_BuildException();

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWindow;
	pSlotWin->SetAlwaysRenderCoverButton(iSlotIndex, bAlwaysRender);

	return Py_BuildNone();
}

PyObject *wndMgrAppendSlotButton(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	char *szUpImageName;
	if (!PyTuple_GetString(poArgs, 1, &szUpImageName))
		return Py_BuildException();
	char *szOverImageName;
	if (!PyTuple_GetString(poArgs, 2, &szOverImageName))
		return Py_BuildException();
	char *szDownImageName;
	if (!PyTuple_GetString(poArgs, 3, &szDownImageName))
		return Py_BuildException();

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWindow;
	pSlotWin->AppendSlotButton(szUpImageName, szOverImageName, szDownImageName);

	return Py_BuildNone();
}

PyObject *wndMgrAppendRequirementSignImage(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	char *szImageName;
	if (!PyTuple_GetString(poArgs, 1, &szImageName))
		return Py_BuildException();

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWindow;
	pSlotWin->AppendRequirementSignImage(szImageName);

	return Py_BuildNone();
}

PyObject *wndMgrShowSlotButton(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	int iSlotNumber;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotNumber))
		return Py_BuildException();

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWindow;
	pSlotWin->ShowSlotButton(iSlotNumber);

	return Py_BuildNone();
}

PyObject *wndMgrHideAllSlotButton(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWindow;
	pSlotWin->HideAllSlotButton();

	return Py_BuildNone();
}

PyObject *wndMgrShowRequirementSign(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	int iSlotNumber;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotNumber))
		return Py_BuildException();

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWindow;
	pSlotWin->ShowRequirementSign(iSlotNumber);

	return Py_BuildNone();
}

PyObject *wndMgrHideRequirementSign(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	int iSlotNumber;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotNumber))
		return Py_BuildException();

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWindow;
	pSlotWin->HideRequirementSign(iSlotNumber);

	return Py_BuildNone();
}

PyObject *wndMgrRefreshSlot(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	if (!pWin->IsType(UI::CSlotWindow::Type()))
		return Py_BuildException();

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	pSlotWin->RefreshSlot();

	return Py_BuildNone();
}

PyObject *wndMgrSetUseMode(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 1, &iFlag))
		return Py_BuildException();

	if (!pWin->IsType(UI::CSlotWindow::Type()))
		return Py_BuildException();

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	pSlotWin->SetUseMode(iFlag);

	return Py_BuildNone();
}

PyObject *wndMgrSetUsableItem(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 1, &iFlag))
		return Py_BuildException();

	if (!pWin->IsType(UI::CSlotWindow::Type()))
		return Py_BuildException();

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	pSlotWin->SetUsableItem(iFlag);

	return Py_BuildNone();
}

PyObject *wndMgrClearSlot(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
		return Py_BuildException();

	if (!pWin->IsType(UI::CSlotWindow::Type()))
		return Py_BuildException();

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	pSlotWin->ClearSlot(iSlotIndex);

	return Py_BuildNone();
}

PyObject *wndMgrClearAllSlot(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	if (!pWin->IsType(UI::CSlotWindow::Type()))
		return Py_BuildException();

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	pSlotWin->ClearAllSlot();

	return Py_BuildNone();
}

PyObject *wndMgrHasSlot(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
		return Py_BuildException();

	if (!pWin->IsType(UI::CSlotWindow::Type()))
		return Py_BuildException();

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;

	return Py_BuildValue("i", pSlotWin->HasSlot(iSlotIndex));
}

PyObject *wndMgrSetSlot(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
		return Py_BuildException();

	int iItemIndex;
	if (!PyTuple_GetInteger(poArgs, 2, &iItemIndex))
		return Py_BuildException();

	int iWidth;
	if (!PyTuple_GetInteger(poArgs, 3, &iWidth))
		return Py_BuildException();

	int iHeight;
	if (!PyTuple_GetInteger(poArgs, 4, &iHeight))
		return Py_BuildException();

	CGraphicImage* pGraphicImage = nullptr;
	if (!PyTuple_GetPointer(poArgs, 5, &pGraphicImage))
	{
		int iDummy = 0;
		if (!PyTuple_GetInteger(poArgs, 5, &iDummy))
			return Py_BuildException();
	}

	D3DXCOLOR diffuseColor;
	PyObject *pTuple;
	if (!PyTuple_GetObject(poArgs, 6, &pTuple))
	{
		diffuseColor = D3DXCOLOR(1.0, 1.0, 1.0, 1.0);
		//return Py_BuildException();
	}
	else
	// get diffuse color from pTuple
	{
		if (PyTuple_Size(pTuple) != 4)
			return Py_BuildException();
		if (!PyTuple_GetFloat(pTuple, 0, &diffuseColor.r))
			return Py_BuildException();
		if (!PyTuple_GetFloat(pTuple, 1, &diffuseColor.g))
			return Py_BuildException();
		if (!PyTuple_GetFloat(pTuple, 2, &diffuseColor.b))
			return Py_BuildException();
		if (!PyTuple_GetFloat(pTuple, 3, &diffuseColor.a))
			return Py_BuildException();
	}

	if (!pWin->IsType(UI::CSlotWindow::Type()))
		return Py_BuildException();

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	pSlotWin->SetSlot(iSlotIndex, iItemIndex, iWidth, iHeight, pGraphicImage, diffuseColor);

	return Py_BuildNone();
}

PyObject *wndMgrSetSlotCount(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
		return Py_BuildException();

	int iCount;
	if (!PyTuple_GetInteger(poArgs, 2, &iCount))
		return Py_BuildException();

	if (!pWin->IsType(UI::CSlotWindow::Type()))
		return Py_BuildException();

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	pSlotWin->SetSlotCount(iSlotIndex, iCount);

	return Py_BuildNone();
}

PyObject *wndMgrSetSlotCountNew(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
		return Py_BuildException();

	int iGrade;
	if (!PyTuple_GetInteger(poArgs, 2, &iGrade))
		return Py_BuildException();

	int iCount;
	if (!PyTuple_GetInteger(poArgs, 3, &iCount))
		return Py_BuildException();

	if (!pWin->IsType(UI::CSlotWindow::Type()))
		return Py_BuildException();

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	pSlotWin->SetSlotCountNew(iSlotIndex, iGrade, iCount);

	return Py_BuildNone();
}

PyObject* wndMgrSetRealSlotNumber(PyObject* poSelf, PyObject* poArgs)
{
	UI::CWindow* pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
		return Py_BuildException();

	int iRealSlotNumber;
	if (!PyTuple_GetInteger(poArgs, 2, &iRealSlotNumber))
		return Py_BuildException();

	if (!pWin->IsType(UI::CSlotWindow::Type()))
		return Py_BuildException();

	UI::CSlotWindow* pSlotWin = (UI::CSlotWindow*)pWin;
	pSlotWin->SetRealSlotNumber(iSlotIndex, iRealSlotNumber);

	return Py_BuildNone();
}

PyObject *wndMgrSetSlotCoolTime(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
		return Py_BuildException();

	float fCoolTime;
	if (!PyTuple_GetFloat(poArgs, 2, &fCoolTime))
		return Py_BuildException();

	float fElapsedTime = 0.0f;
	PyTuple_GetFloat(poArgs, 3, &fElapsedTime);

	if (!pWin->IsType(UI::CSlotWindow::Type()))
		return Py_BuildException();

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	pSlotWin->SetSlotCoolTime(iSlotIndex, fCoolTime, fElapsedTime);

	return Py_BuildNone();
}

PyObject *wndMgrStoreSlotCoolTime(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	int iKey;
	if (!PyTuple_GetInteger(poArgs, 1, &iKey))
		return Py_BuildException();

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 2, &iSlotIndex))
		return Py_BuildException();

	float fCoolTime;
	if (!PyTuple_GetFloat(poArgs, 3, &fCoolTime))
		return Py_BuildException();

	float fElapsedTime = 0.0f;
	PyTuple_GetFloat(poArgs, 4, &fElapsedTime);

	if (!pWin->IsType(UI::CSlotWindow::Type()))
		return Py_BuildException();

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	pSlotWin->StoreSlotCoolTime(iKey, iSlotIndex, fCoolTime, fElapsedTime);

	return Py_BuildNone();
}

PyObject *wndMgrRestoreSlotCoolTime(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	int iKey;
	if (!PyTuple_GetInteger(poArgs, 1, &iKey))
		return Py_BuildException();

	if (!pWin->IsType(UI::CSlotWindow::Type()))
		return Py_BuildException();

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	pSlotWin->RestoreSlotCoolTime(iKey);

	return Py_BuildNone();
}

PyObject *wndMgrSetToggleSlot(PyObject *poSelf, PyObject *poArgs)
{
	assert(!"wndMgrSetToggleSlot - »ç¿ëÇÏÁö ¾Ê´Â ÇÔ¼ö");
	return Py_BuildNone();
}

PyObject *wndMgrActivateSlot(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
		return Py_BuildException();

	int iColorType;
	if (!PyTuple_GetInteger(poArgs, 2, &iColorType))
		return Py_BuildException();

	if (!pWin->IsType(UI::CSlotWindow::Type()))
		return Py_BuildException();

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	pSlotWin->ActivateSlot(iSlotIndex, iColorType);
	return Py_BuildNone();
}

PyObject *wndMgrDeactivateSlot(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
		return Py_BuildException();

	if (!pWin->IsType(UI::CSlotWindow::Type()))
		return Py_BuildException();

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	pSlotWin->DeactivateSlot(iSlotIndex);
	return Py_BuildNone();
}

PyObject *wndMgrEnableSlot(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
		return Py_BuildException();

	if (!pWin->IsType(UI::CSlotWindow::Type()))
		return Py_BuildException();

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	pSlotWin->EnableSlot(iSlotIndex);
	return Py_BuildNone();
}

PyObject *wndMgrDisableSlot(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
		return Py_BuildException();

	if (!pWin->IsType(UI::CSlotWindow::Type()))
		return Py_BuildException();

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	pSlotWin->DisableSlot(iSlotIndex);
	return Py_BuildNone();
}

PyObject *wndMgrShowSlotBaseImage(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
		return Py_BuildException();

	if (!pWin->IsType(UI::CSlotWindow::Type()))
		return Py_BuildException();

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	pSlotWin->ShowSlotBaseImage(iSlotIndex);
	return Py_BuildNone();
}

PyObject *wndMgrHideSlotBaseImage(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
		return Py_BuildException();

	if (!pWin->IsType(UI::CSlotWindow::Type()))
		return Py_BuildException();

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	pSlotWin->HideSlotBaseImage(iSlotIndex);
	return Py_BuildNone();
}

PyObject *wndMgrSetSlotType(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	int iType;
	if (!PyTuple_GetInteger(poArgs, 1, &iType))
		return Py_BuildException();

	if (!pWin->IsType(UI::CSlotWindow::Type()))
		return Py_BuildException();

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	pSlotWin->SetSlotType(iType);

	return Py_BuildNone();
}

PyObject *wndMgrSetSlotStyle(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	int iStyle;
	if (!PyTuple_GetInteger(poArgs, 1, &iStyle))
		return Py_BuildException();

	if (!pWin->IsType(UI::CSlotWindow::Type()))
		return Py_BuildException();

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	pSlotWin->SetSlotStyle(iStyle);

	return Py_BuildNone();
}

PyObject *wndMgrSetSlotScale(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	float fx;
	if (!PyTuple_GetFloat(poArgs, 1, &fx))
		return Py_BuildException();

	float fy;
	if (!PyTuple_GetFloat(poArgs, 2, &fy))
		return Py_BuildException();

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	pSlotWin->SetSlotScale(fx, fy);

	return Py_BuildNone();
}

PyObject *wndMgrSelectSlot(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iIndex))
		return Py_BuildException();

	if (!pWin->IsType(UI::CSlotWindow::Type()))
		return Py_BuildException();

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	pSlotWin->SelectSlot(iIndex);

	return Py_BuildNone();
}

PyObject *wndMgrClearSelected(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	if (!pWin->IsType(UI::CSlotWindow::Type()))
		return Py_BuildException();

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	pSlotWin->ClearSelected();

	return Py_BuildNone();
}

PyObject *wndMgrGetSelectedSlotCount(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	if (!pWin->IsType(UI::CSlotWindow::Type()))
		return Py_BuildException();

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	return Py_BuildValue("i", pSlotWin->GetSelectedSlotCount());
}

PyObject *wndMgrGetSelectedSlotNumber(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();
	int iSlotNumber;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotNumber))
		return Py_BuildException();

	if (!pWin->IsType(UI::CSlotWindow::Type()))
		return Py_BuildException();

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	return Py_BuildValue("i", pSlotWin->GetSelectedSlotNumber(iSlotNumber));
}

PyObject *wndMgrIsSelectedSlot(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();
	int iSlotNumber;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotNumber))
		return Py_BuildException();

	if (!pWin->IsType(UI::CSlotWindow::Type()))
		return Py_BuildException();

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	return Py_BuildValue("i", pSlotWin->isSelectedSlot(iSlotNumber));
}

PyObject *wndMgrGetSlotCount(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	if (!pWin->IsType(UI::CSlotWindow::Type()))
		return Py_BuildException();

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	return Py_BuildValue("i", pSlotWin->GetSlotCount());
}

PyObject *wndMgrGetSlotNumber(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow* pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	if (!pWin->IsType(UI::CSlotWindow::Type()))
		return Py_BuildException();

	UI::CSlotWindow* pSlotWin = (UI::CSlotWindow*)pWin;

	std::vector<DWORD> v_SlotsNumber;
	pSlotWin->GetSlotNumber(&v_SlotsNumber);

	PyObject *poList = PyList_New(0);
	for (const auto& slot_number : v_SlotsNumber)
		PyList_Append(poList, PyInt_FromLong(slot_number));

	return poList;
}

PyObject *wndMgrLockSlot(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
		return Py_BuildException();

	if (!pWin->IsType(UI::CSlotWindow::Type()))
		return Py_BuildException();

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	pSlotWin->LockSlot(iSlotIndex);
	return Py_BuildNone();
}

PyObject *wndMgrUnlockSlot(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
		return Py_BuildException();

	if (!pWin->IsType(UI::CSlotWindow::Type()))
		return Py_BuildException();

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	pSlotWin->UnlockSlot(iSlotIndex);
	return Py_BuildNone();
}

PyObject *wndMgrIsLockSlot(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
		return Py_BuildException();

	if (!pWin->IsType(UI::CSlotWindow::Type()))
		return Py_BuildException();

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	return Py_BuildValue("i", pSlotWin->IsLockSlot(iSlotIndex));
}

PyObject *wndBarSetColor(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();
	int iColor;
	if (!PyTuple_GetInteger(poArgs, 1, &iColor))
		return Py_BuildException();

	if (pWindow->IsType(UI::CBar3D::Type()))
	{
		int iLeftColor = iColor;

		int iRightColor;
		if (!PyTuple_GetInteger(poArgs, 2, &iRightColor))
			return Py_BuildException();
		int iCenterColor;
		if (!PyTuple_GetInteger(poArgs, 3, &iCenterColor))
			return Py_BuildException();

		((UI::CBar3D *)pWindow)->SetColor(iLeftColor, iRightColor, iCenterColor);
	}
	else
	{
		((UI::CWindow *)pWindow)->SetColor(iColor);
	}
	return Py_BuildNone();
}

PyObject *wndMgrAttachIcon(PyObject *poSelf, PyObject *poArgs)
{
	int iType;
	if (!PyTuple_GetInteger(poArgs, 0, &iType))
		return Py_BuildException();
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iIndex))
		return Py_BuildException();
	int iSlotNumber;
	if (!PyTuple_GetInteger(poArgs, 2, &iSlotNumber))
		return Py_BuildException();
	int iWidth;
	if (!PyTuple_GetInteger(poArgs, 3, &iWidth))
		return Py_BuildException();
	int iHeight;
	if (!PyTuple_GetInteger(poArgs, 4, &iHeight))
		return Py_BuildException();

	UI::CWindowManager::Instance().AttachIcon(iType, iIndex, iSlotNumber, iWidth, iHeight);
	return Py_BuildNone();
}

PyObject *wndMgrDeattachIcon(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindowManager::Instance().DeattachIcon();
	return Py_BuildNone();
}

PyObject *wndMgrSetAttachingFlag(PyObject *poSelf, PyObject *poArgs)
{
	BOOL bFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &bFlag))
		return Py_BuildException();

	UI::CWindowManager::Instance().SetAttachingFlag(bFlag);
	return Py_BuildNone();
}

// Text
PyObject *wndTextSetMax(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();
	int iMax;
	if (!PyTuple_GetInteger(poArgs, 1, &iMax))
		return Py_BuildException();

	((UI::CTextLine*)pWindow)->SetMax(iMax);
	return Py_BuildNone();
}
PyObject *wndTextSetHorizontalAlign(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();
	int iType;
	if (!PyTuple_GetInteger(poArgs, 1, &iType))
		return Py_BuildException();

	((UI::CTextLine*)pWindow)->SetHorizontalAlign(iType);
	return Py_BuildNone();
}
PyObject *wndTextSetVerticalAlign(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();
	int iType;
	if (!PyTuple_GetInteger(poArgs, 1, &iType))
		return Py_BuildException();

	((UI::CTextLine*)pWindow)->SetVerticalAlign(iType);
	return Py_BuildNone();
}
PyObject *wndTextSetSecret(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 1, &iFlag))
		return Py_BuildException();

	((UI::CTextLine*)pWindow)->SetSecret(iFlag);
	return Py_BuildNone();
}
PyObject *wndTextSetOutline(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 1, &iFlag))
		return Py_BuildException();

	((UI::CTextLine*)pWindow)->SetOutline(iFlag);
	return Py_BuildNone();
}
PyObject *wndTextSetFeather(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 1, &iFlag))
		return Py_BuildException();

	((UI::CTextLine*)pWindow)->SetFeather(iFlag);
	return Py_BuildNone();
}
PyObject *wndTextSetMultiLine(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 1, &iFlag))
		return Py_BuildException();

	((UI::CTextLine*)pWindow)->SetMultiLine(iFlag);
	return Py_BuildNone();
}
PyObject *wndTextSetFontName(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();
	char *szFontName;
	if (!PyTuple_GetString(poArgs, 1, &szFontName))
		return Py_BuildException();

	((UI::CTextLine*)pWindow)->SetFontName(szFontName);
	return Py_BuildNone();
}
PyObject *wndTextSetFontColor(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	if (2 == PyTuple_Size(poArgs))
	{
		int iColor;
		if (!PyTuple_GetInteger(poArgs, 1, &iColor))
			return Py_BuildException();
		((UI::CTextLine*)pWindow)->SetFontColor(iColor);
	}
	else if (4 == PyTuple_Size(poArgs))
	{
		float fr;
		if (!PyTuple_GetFloat(poArgs, 1, &fr))
			return Py_BuildException();
		float fg;
		if (!PyTuple_GetFloat(poArgs, 2, &fg))
			return Py_BuildException();
		float fb;
		if (!PyTuple_GetFloat(poArgs, 3, &fb))
			return Py_BuildException();
		float fa = 1.0f;

		BYTE argb[4] =
		{
			(BYTE) (255.0f * fb),
			(BYTE) (255.0f * fg),
			(BYTE) (255.0f * fr),
			(BYTE) (255.0f * fa)
		};
		((UI::CTextLine*)pWindow)->SetFontColor(*((DWORD *) argb));
	}
	else
	{
		return Py_BuildException();
	}

	return Py_BuildNone();
}
PyObject *wndTextSetLimitWidth(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();
	float fWidth;
	if (!PyTuple_GetFloat(poArgs, 1, &fWidth))
		return Py_BuildException();

	((UI::CTextLine*)pWindow)->SetLimitWidth(fWidth);
	return Py_BuildNone();
}
PyObject *wndTextSetText(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();
	char *szText;
	if (!PyTuple_GetString(poArgs, 1, &szText))
		return Py_BuildException();

#ifdef ENABLE_MULTILANGUAGE_CHATTING
	bool bUsingCurrentSystemLangID = false;
	if (PyTuple_Size(poArgs) > 2)
	{
		if (!PyTuple_GetBoolean(poArgs, 2, &bUsingCurrentSystemLangID))
			return Py_BuildException();
	}

	if (bUsingCurrentSystemLangID)
	{
		DWORD dwLangID = pWindow->GetInputSystemLangID();
		((UI::CTextLine*)pWindow)->SetText(szText, &dwLangID);
	}
	else
	{
		((UI::CTextLine*)pWindow)->SetText(szText, nullptr);
	}
#else
	((UI::CTextLine*)pWindow)->SetText(szText);
#endif
	return Py_BuildNone();
}

#ifdef ENABLE_MULTILANGUAGE_CHATTING
PyObject *wndTextSetFontColorRange(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	int iColor;
	if (!PyTuple_GetInteger(poArgs, 1, &iColor))
		return Py_BuildException();

	int iStart;
	if (!PyTuple_GetInteger(poArgs, 2, &iStart))
		return Py_BuildException();

	int iEnd;
	if (!PyTuple_GetInteger(poArgs, 3, &iEnd))
		return Py_BuildException();

	((UI::CTextLine*)pWindow)->SetFontColorRange((DWORD)iColor, iStart, iEnd);
	return Py_BuildNone();
}

PyObject *wndTextSetTextWithCodePage(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	char *szText;
	if (!PyTuple_GetString(poArgs, 1, &szText))
		return Py_BuildException();

	int iCodePage;
	if (!PyTuple_GetInteger(poArgs, 2, &iCodePage))
		return Py_BuildException();

	((UI::CTextLine*)pWindow)->SetTextWithCodePage(szText, (DWORD)iCodePage);
	return Py_BuildNone();
}

PyObject *wndGetInputSystemLangID(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	return Py_BuildValue("i", pWindow->GetInputSystemLangID());
}
#endif
PyObject *wndTextGetText(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	return Py_BuildValue("s", ((UI::CTextLine*)pWindow)->GetText());
}

PyObject *wndTextGetTextSize(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	int nWidth;
	int nHeight;

	UI::CTextLine* pkTextLine=(UI::CTextLine*)pWindow;
	pkTextLine->GetTextSize(&nWidth, &nHeight);

	return Py_BuildValue("(ii)", nWidth, nHeight);
}

#ifdef ENABLE_SUNG_MAHI_TOWER
PyObject * wndTextGetCharSize(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	short sWidth;

	UI::CTextLine* pkTextLine=(UI::CTextLine*)pWindow;
	pkTextLine->GetCharSize(&sWidth);

	return Py_BuildValue("i", sWidth);
}
#endif

PyObject * wndTextShowCursor(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	((UI::CTextLine*)pWindow)->ShowCursor();
	return Py_BuildNone();
}
PyObject * wndTextHideCursor(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	((UI::CTextLine*)pWindow)->HideCursor();
	return Py_BuildNone();
}

#ifdef INSIDE_RENDER
PyObject* wndTextIsShowCursor(PyObject*/* poSelf*/, PyObject* poArgs)
{
	UI::CWindow* pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	return Py_BuildValue("b", ((UI::CTextLine*)pWindow)->IsShowCursor());
}

PyObject* wndMgrSetInsideRender(PyObject*/* poSelf*/, PyObject* poArgs)
{
	UI::CWindow* pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	bool val;
	if (!PyTuple_GetBoolean(poArgs, 1, &val))
		return Py_BuildException();

	pWin->SetInsideRender(val);
	return Py_BuildNone();
}

PyObject* wndSetRenderingRect(PyObject*/* poSelf*/, PyObject* poArgs)
{
	UI::CWindow* pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();
	float fLeft;
	if (!PyTuple_GetFloat(poArgs, 1, &fLeft))
		return Py_BuildException();
	float fTop;
	if (!PyTuple_GetFloat(poArgs, 2, &fTop))
		return Py_BuildException();
	float fRight;
	if (!PyTuple_GetFloat(poArgs, 3, &fRight))
		return Py_BuildException();
	float fBottom;
	if (!PyTuple_GetFloat(poArgs, 4, &fBottom))
		return Py_BuildException();

	pWindow->SetRenderingRect(fLeft, fTop, fRight, fBottom);
	return Py_BuildNone();
}

PyObject* wndMgrGetRenderBox(PyObject*/* poSelf*/, PyObject* poArgs)
{
	UI::CWindow* pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	RECT val;
	pWin->GetRenderBox(&val);
	return Py_BuildValue("iiii", val.left, val.top, val.right, val.bottom);
}

PyObject* wndTextSetFixedRenderPos(PyObject*/* poSelf*/, PyObject* poArgs)
{
	UI::CWindow* pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();
	int startPos, endPos;
	if (!PyTuple_GetInteger(poArgs, 1, &startPos))
		return Py_BuildException();
	if (!PyTuple_GetInteger(poArgs, 2, &endPos))
		return Py_BuildException();

	((UI::CTextLine*)pWindow)->SetFixedRenderPos(startPos, endPos);
	return Py_BuildNone();
}

PyObject* wndTextGetRenderPos(PyObject*/* poSelf*/, PyObject* poArgs)
{
	UI::CWindow* pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();
	WORD startPos, endPos;

	((UI::CTextLine*)pWindow)->GetRenderPositions(startPos, endPos);
	return Py_BuildValue("ii", startPos, endPos);
}

PyObject* wndImageUnloadImage(PyObject*/* poSelf*/, PyObject* poArgs)
{
    UI::CWindow* pWindow;
    if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
    {
        return Py_BuildException();
    }

    dynamic_cast<UI::CImageBox*>(pWindow)->UnloadImage();
    return Py_BuildNone();
}
#endif

PyObject * wndTextGetCursorPosition(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	return Py_BuildValue("i", ((UI::CTextLine*)pWindow)->GetCursorPosition());
}

PyObject * wndNumberSetNumber(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();
	char * szNumber;
	if (!PyTuple_GetString(poArgs, 1, &szNumber))
		return Py_BuildException();

	((UI::CNumberLine*)pWindow)->SetNumber(szNumber);

	return Py_BuildNone();
}

PyObject * wndNumberSetNumberHorizontalAlignCenter(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	((UI::CNumberLine*)pWindow)->SetHorizontalAlign(UI::CWindow::HORIZONTAL_ALIGN_CENTER);

	return Py_BuildNone();
}

PyObject *wndNumberSetNumberHorizontalAlignRight(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	((UI::CNumberLine*)pWindow)->SetHorizontalAlign(UI::CWindow::HORIZONTAL_ALIGN_RIGHT);

	return Py_BuildNone();
}

PyObject *wndNumberSetPath(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();
	char *szPath;
	if (!PyTuple_GetString(poArgs, 1, &szPath))
		return Py_BuildException();

	((UI::CNumberLine*)pWindow)->SetPath(szPath);

	return Py_BuildNone();
}

#ifdef ENABLE_SLOT_CHAR_ENLARGEMENT
PyObject * wndNumberSetCanEnlarge(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	bool bFlag;
	if (!PyTuple_GetBoolean(poArgs, 1, &bFlag))
		return Py_BuildException();

	((UI::CNumberLine*)pWindow)->SetCanEnlarge(bFlag);
	return Py_BuildNone();
}

PyObject * wndNumberRefreshSlotCharState(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	((UI::CNumberLine*)pWindow)->RefreshSlotCharState();
	return Py_BuildNone();
}
#endif

PyObject *wndMarkBox_SetImageFilename(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();
	char *szFileName;
	if (!PyTuple_GetString(poArgs, 1, &szFileName))
		return Py_BuildException();

	((UI::CMarkBox*)pWindow)->LoadImage(szFileName);
	return Py_BuildNone();
}

PyObject *wndMarkBox_SetImage(PyObject *poSelf, PyObject *poArgs)
{
	// ¾Æ¹«°Íµµ ÇÏÁö ¾ÊÀ½
	return Py_BuildNone();
}

PyObject *wndMarkBox_Load(PyObject *poSelf, PyObject *poArgs)
{
	// ¾Æ¹«°Íµµ ÇÏÁö ¾ÊÀ½
	return Py_BuildNone();
}

PyObject *wndMarkBox_SetIndex(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	int nIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &nIndex))
		return Py_BuildException();

	((UI::CMarkBox*)pWindow)->SetIndex(nIndex);
	return Py_BuildNone();
}

PyObject *wndMarkBox_SetScale(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	float fScale;
	if (!PyTuple_GetFloat(poArgs, 1, &fScale))
		return Py_BuildException();

	((UI::CMarkBox*)pWindow)->SetScale(fScale);
	return Py_BuildNone();
}


PyObject *wndMarkBox_SetDiffuseColor(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();
	float fr;
	if (!PyTuple_GetFloat(poArgs, 1, &fr))
		return Py_BuildException();
	float fg;
	if (!PyTuple_GetFloat(poArgs, 2, &fg))
		return Py_BuildException();
	float fb;
	if (!PyTuple_GetFloat(poArgs, 3, &fb))
		return Py_BuildException();
	float fa;
	if (!PyTuple_GetFloat(poArgs, 4, &fa))
		return Py_BuildException();

	((UI::CMarkBox*)pWindow)->SetDiffuseColor(fr, fg, fb, fa);

	return Py_BuildNone();
}


PyObject *wndImageLoadImage(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();
	char *szFileName;
	if (!PyTuple_GetString(poArgs, 1, &szFileName))
		return Py_BuildException();

	if (!((UI::CImageBox*)pWindow)->LoadImage(szFileName))
		return Py_BuildException("Failed to load image (filename: %s)", szFileName);

	return Py_BuildNone();
}

PyObject *wndImageSetDiffuseColor(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();
	float fr;
	if (!PyTuple_GetFloat(poArgs, 1, &fr))
		return Py_BuildException();
	float fg;
	if (!PyTuple_GetFloat(poArgs, 2, &fg))
		return Py_BuildException();
	float fb;
	if (!PyTuple_GetFloat(poArgs, 3, &fb))
		return Py_BuildException();
	float fa;
	if (!PyTuple_GetFloat(poArgs, 4, &fa))
		return Py_BuildException();

	((UI::CImageBox*)pWindow)->SetDiffuseColor(fr, fg, fb, fa);

	return Py_BuildNone();
}

PyObject *wndImageGetWidth(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	return Py_BuildValue("i", ((UI::CImageBox*)pWindow)->GetWidth());
}

PyObject *wndImageGetHeight(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	return Py_BuildValue("i", ((UI::CImageBox*)pWindow)->GetHeight());
}

PyObject *wndImageSetScale(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();
	float fx;
	if (!PyTuple_GetFloat(poArgs, 1, &fx))
		return Py_BuildException();
	float fy;
	if (!PyTuple_GetFloat(poArgs, 2, &fy))
		return Py_BuildException();

	((UI::CExpandedImageBox*)pWindow)->SetScale(fx, fy);

	return Py_BuildNone();
}

PyObject *wndImageSetOrigin(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();
	float fx;
	if (!PyTuple_GetFloat(poArgs, 1, &fx))
		return Py_BuildException();
	float fy;
	if (!PyTuple_GetFloat(poArgs, 2, &fy))
		return Py_BuildException();

	((UI::CExpandedImageBox*)pWindow)->SetOrigin(fx, fy);

	return Py_BuildNone();
}

PyObject *wndImageSetRotation(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();
	float fRotation;
	if (!PyTuple_GetFloat(poArgs, 1, &fRotation))
		return Py_BuildException();

	((UI::CExpandedImageBox*)pWindow)->SetRotation(fRotation);

	return Py_BuildNone();
}

PyObject *wndImageSetRenderingRect(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();
	float fLeft;
	if (!PyTuple_GetFloat(poArgs, 1, &fLeft))
		return Py_BuildException();
	float fTop;
	if (!PyTuple_GetFloat(poArgs, 2, &fTop))
		return Py_BuildException();
	float fRight;
	if (!PyTuple_GetFloat(poArgs, 3, &fRight))
		return Py_BuildException();
	float fBottom;
	if (!PyTuple_GetFloat(poArgs, 4, &fBottom))
		return Py_BuildException();

	if (pWindow->IsType(UI::CExpandedImageBox::Type()))
		((UI::CExpandedImageBox*)pWindow)->SetRenderingRect(fLeft, fTop, fRight, fBottom);
	else if (pWindow->IsType(UI::CAniImageBox::Type()))
		((UI::CAniImageBox*)pWindow)->SetRenderingRect(fLeft, fTop, fRight, fBottom);
#ifdef ENABLE_WIKI_SYSTEM
	else if (pWindow->IsType(UI::CRenderTarget::Type()))
		((UI::CRenderTarget*)pWindow)->SetRenderingRect(fLeft, fTop, fRight, fBottom);
#endif
#if defined(ENABLE_TRACK_WINDOW) && defined(ENABLE_ADVANCED_GAME_OPTIONS)
	else if (pWindow->IsType(UI::CButton::Type()))
		((UI::CButton*)pWindow)->SetRenderingRect(fLeft, fTop, fRight, fBottom);
	else if (pWindow->IsType(UI::CRadioButton::Type()))
		((UI::CRadioButton*)pWindow)->SetRenderingRect(fLeft, fTop, fRight, fBottom);
	else if (pWindow->IsType(UI::CToggleButton::Type()))
		((UI::CToggleButton*)pWindow)->SetRenderingRect(fLeft, fTop, fRight, fBottom);
#endif

	return Py_BuildNone();
}

PyObject *wndImageSetRenderingMode(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();
	int iMode;
	if (!PyTuple_GetInteger(poArgs, 1, &iMode))
		return Py_BuildException();

	if (pWindow->IsType(UI::CExpandedImageBox::Type()))
		((UI::CExpandedImageBox*)pWindow)->SetRenderingMode(iMode);

	return Py_BuildNone();
}

PyObject *wndImageSetDelay(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();
	float fDelay;
	if (!PyTuple_GetFloat(poArgs, 1, &fDelay))
		return Py_BuildException();

	((UI::CAniImageBox*)pWindow)->SetDelay(fDelay);

	return Py_BuildNone();
}

PyObject *wndImageAppendImage(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();
	char *szFileName;
	if (!PyTuple_GetString(poArgs, 1, &szFileName))
		return Py_BuildException();

	((UI::CAniImageBox*)pWindow)->AppendImage(szFileName);

	return Py_BuildNone();
}

#ifdef ENABLE_MINI_GAME
PyObject* wndMgrSetCardSlot(PyObject* poSelf, PyObject* poArgs)
{
	UI::CWindow* pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
		return Py_BuildException();

	int iItemIndex;
	if (!PyTuple_GetInteger(poArgs, 2, &iItemIndex))
		return Py_BuildException();

	int iWidth;
	if (!PyTuple_GetInteger(poArgs, 3, &iWidth))
		return Py_BuildException();

	int iHeight;
	if (!PyTuple_GetInteger(poArgs, 4, &iHeight))
		return Py_BuildException();

	char* c_szFileName;
	if (!PyTuple_GetString(poArgs, 5, &c_szFileName))
		return Py_BuildException();

	D3DXCOLOR diffuseColor;
	PyObject* pTuple;
	if (!PyTuple_GetObject(poArgs, 6, &pTuple))
	{
		diffuseColor = D3DXCOLOR(1.0, 1.0, 1.0, 1.0);
		//return Py_BuildException();
	}
	else
		// get diffuse color from pTuple
	{
		if (PyTuple_Size(pTuple) != 4)
			return Py_BuildException();
		if (!PyTuple_GetFloat(pTuple, 0, &diffuseColor.r))
			return Py_BuildException();
		if (!PyTuple_GetFloat(pTuple, 1, &diffuseColor.g))
			return Py_BuildException();
		if (!PyTuple_GetFloat(pTuple, 2, &diffuseColor.b))
			return Py_BuildException();
		if (!PyTuple_GetFloat(pTuple, 3, &diffuseColor.a))
			return Py_BuildException();
	}

	if (!pWin->IsType(UI::CSlotWindow::Type()))
		return Py_BuildException();

	UI::CSlotWindow* pSlotWin = (UI::CSlotWindow*)pWin;
	pSlotWin->SetCardSlot(iSlotIndex, iItemIndex, iWidth, iHeight, c_szFileName, diffuseColor);

	return Py_BuildNone();
}

PyObject* wndImageResetFrame(PyObject* poSelf, PyObject* poArgs)
{
	UI::CWindow* pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	((UI::CAniImageBox*)pWindow)->ResetFrame();

	return Py_BuildNone();
}
#endif

PyObject *wndButtonSetUpVisual(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();
	char *szFileName;
	if (!PyTuple_GetString(poArgs, 1, &szFileName))
		return Py_BuildException();

	((UI::CButton*)pWindow)->SetUpVisual(szFileName);

	return Py_BuildNone();
}
PyObject *wndButtonSetOverVisual(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();
	char *szFileName;
	if (!PyTuple_GetString(poArgs, 1, &szFileName))
		return Py_BuildException();

	((UI::CButton*)pWindow)->SetOverVisual(szFileName);

	return Py_BuildNone();
}
PyObject *wndButtonSetDownVisual(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();
	char *szFileName;
	if (!PyTuple_GetString(poArgs, 1, &szFileName))
		return Py_BuildException();

	((UI::CButton*)pWindow)->SetDownVisual(szFileName);

	return Py_BuildNone();
}
PyObject *wndButtonSetDisableVisual(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();
	char *szFileName;
	if (!PyTuple_GetString(poArgs, 1, &szFileName))
		return Py_BuildException();

	((UI::CButton*)pWindow)->SetDisableVisual(szFileName);

	return Py_BuildNone();
}
PyObject *wndButtonGetUpVisualFileName(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	return Py_BuildValue("s", ((UI::CButton*)pWindow)->GetUpVisualFileName());
}
PyObject *wndButtonGetOverVisualFileName(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	return Py_BuildValue("s", ((UI::CButton*)pWindow)->GetOverVisualFileName());
}
PyObject *wndButtonGetDownVisualFileName(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	return Py_BuildValue("s", ((UI::CButton*)pWindow)->GetDownVisualFileName());
}
PyObject *wndButtonFlash(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	((UI::CButton*)pWindow)->Flash();

	return Py_BuildNone();
}
PyObject *wndButtonEnableFlash(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow* pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	((UI::CButton*)pWindow)->EnableFlash();

	return Py_BuildNone();
}

PyObject *wndButtonDisableFlash(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow* pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	((UI::CButton*)pWindow)->DisableFlash();

	return Py_BuildNone();
}
PyObject *wndButtonEnable(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	((UI::CButton*)pWindow)->Enable();

	return Py_BuildNone();
}
PyObject *wndButtonDisable(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	((UI::CButton*)pWindow)->Disable();

	return Py_BuildNone();
}
PyObject *wndButtonDown(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	((UI::CButton*)pWindow)->Down();

	return Py_BuildNone();
}
PyObject *wndButtonSetUp(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	((UI::CButton*)pWindow)->SetUp();

	return Py_BuildNone();
}
PyObject *wndButtonSetRestrictMovementArea(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();
	int ix;
	if (!PyTuple_GetInteger(poArgs, 1, &ix))
		return Py_BuildException();
	int iy;
	if (!PyTuple_GetInteger(poArgs, 2, &iy))
		return Py_BuildException();
	int iwidth;
	if (!PyTuple_GetInteger(poArgs, 3, &iwidth))
		return Py_BuildException();
	int iheight;
	if (!PyTuple_GetInteger(poArgs, 4, &iheight))
		return Py_BuildException();

	((UI::CDragButton*)pWindow)->SetRestrictMovementArea(ix, iy, iwidth, iheight);

	return Py_BuildNone();
}

PyObject *wndButtonIsDown(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	return Py_BuildValue("i", ((UI::CButton*)pWindow)->IsPressed());
}

#ifdef ENABLE_FISH_EVENT_SYSTEM
PyObject * wndMgrSetPickedAreaRender(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 1, &iFlag))
		return Py_BuildException();

	if (!pWin->IsType(UI::CGridSlotWindow::Type()))
	{
		TraceError("wndMgr.SetPickedAreaRender : not a grid window");
		return Py_BuildException();
	}

	UI::CGridSlotWindow * pGridSlotWin = (UI::CGridSlotWindow *)pWin;
	pGridSlotWin->SetPickedAreaRender(iFlag ? true : false);

	return Py_BuildNone();
}
PyObject * wndMgrDeleteCoverButton(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
		return Py_BuildException();

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWindow;
	pSlotWin->DeleteCoverButton(iSlotIndex);

	return Py_BuildNone();
}
PyObject * wndMgrSetDisableDeattach(PyObject * poSelf, PyObject * poArgs)
{
	BOOL bFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &bFlag))
		return Py_BuildException();

	UI::CWindowManager::Instance().SetDisableDeattach(bFlag ? true : false);
	return Py_BuildNone();
}
#endif

extern BOOL g_bOutlineBoxEnable;
extern BOOL g_bShowOverInWindowName;

PyObject *wndMgrSetOutlineFlag(PyObject *poSelf, PyObject *poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BuildException();

	g_bOutlineBoxEnable = iFlag;

	return Py_BuildNone();
}

PyObject *wndMgrShowOverInWindowName(PyObject *poSelf, PyObject *poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BuildException();

	g_bShowOverInWindowName = iFlag;

	return Py_BuildNone();
}

PyObject *wndSetAniImgScale(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow* pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();
	float fScaleX;
	if (!PyTuple_GetFloat(poArgs, 1, &fScaleX))
		return Py_BuildException();
	float fScaleY;
	if (!PyTuple_GetFloat(poArgs, 2, &fScaleY))
		return Py_BuildException();

	((UI::CAniImageBox*)pWindow)->SetAniImgScale(fScaleX, fScaleY);

	return Py_BuildNone();
}

PyObject *wndSetClipRect(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow* pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();
	float fLeft;
	if (!PyTuple_GetFloat(poArgs, 1, &fLeft))
		return Py_BuildException();
	float fTop;
	if (!PyTuple_GetFloat(poArgs, 2, &fTop))
		return Py_BuildException();
	float fRight;
	if (!PyTuple_GetFloat(poArgs, 3, &fRight))
		return Py_BuildException();
	float fBottom;
	if (!PyTuple_GetFloat(poArgs, 4, &fBottom))
		return Py_BuildException();
	int isVertical;
	if (!PyTuple_GetInteger(poArgs, 5, &isVertical))
		return Py_BuildException();

	if (pWindow->IsType(UI::CExpandedImageBox::Type()))
		((UI::CExpandedImageBox*)pWindow)->SetImageClipRect(fLeft, fTop, fRight, fBottom, isVertical ? true : false);

	return Py_BuildNone();
}

PyObject *wndMgrSetRenderingRectWithScale(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow* pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	float fLeft;
	if (!PyTuple_GetFloat(poArgs, 1, &fLeft))
		return Py_BuildException();

	float fTop;
	if (!PyTuple_GetFloat(poArgs, 2, &fTop))
		return Py_BuildException();

	float fRight;
	if (!PyTuple_GetFloat(poArgs, 3, &fRight))
		return Py_BuildException();

	float fBottom;
	if (!PyTuple_GetFloat(poArgs, 4, &fBottom))
		return Py_BuildException();

	if (pWindow->IsType(UI::CExpandedImageBox::Type()))
		((UI::CExpandedImageBox*)pWindow)->SetRenderingRectWithScale(fLeft, fTop, fRight, fBottom);
	else if (pWindow->IsType(UI::CAniImageBox::Type()))
		((UI::CAniImageBox*)pWindow)->SetRenderingRectWithScale(fLeft, fTop, fRight, fBottom);

	return Py_BuildNone();
}

#ifdef ENABLE_SKILL_COLOR_SYSTEM
PyObject *wndMgrGetRealMousePosition(PyObject *poSelf, PyObject *poArgs)
{
	POINT kCursorPos;
	GetCursorPos(&kCursorPos);
	return Py_BuildValue("ii", kCursorPos.x, kCursorPos.y);
}

PyObject *wndMgrGetColorAtPosition(PyObject *poSelf, PyObject *poArgs)
{
	int iX;
	if (!PyTuple_GetInteger(poArgs, 0, &iX))
		return Py_BadArgument();

	int iY;
	if (!PyTuple_GetInteger(poArgs, 1, &iY))
		return Py_BadArgument();

	POINT p;
	COLORREF color;
	HDC hDC;
	BOOL b;

	hDC = GetDC(nullptr);
	if (hDC == nullptr)
		return Py_BuildException();;

	b = GetCursorPos(&p);
	if (!b)
		return Py_BuildException();

	color = GetPixel(hDC, p.x, p.y);
	if (color == CLR_INVALID)
		return Py_BuildException();

	ReleaseDC(GetDesktopWindow(), hDC);
	return Py_BuildValue("iii", GetRValue(color), GetGValue(color), GetBValue(color));
}
#endif

#ifdef ENABLE_MULTI_TEXTLINE
PyObject *wndTextGetTextLineCount(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	return Py_BuildValue("i", ((UI::CTextLine*)pWindow)->GetTextLineCount());
}

PyObject *wndTextDisableEnterToken(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	((UI::CTextLine*)pWindow)->DisableEnterToken();
	return Py_BuildNone();
}

PyObject *wndTextSetLineHeight(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	int iLineHeight;
	if (!PyTuple_GetInteger(poArgs, 1, &iLineHeight))
		return Py_BuildException();

	((UI::CTextLine*)pWindow)->SetLineHeight(iLineHeight);
	return Py_BuildNone();
}

PyObject *wndTextGetLineHeight(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	return Py_BuildValue("(i)", ((UI::CTextLine*)pWindow)->GetLineHeight());
}
#endif

#ifdef ENABLE_NEW_DUNGEON_LIB
PyObject *wndImageSetCoolTime(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow* pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();
	float fCoolTime;
	if (!PyTuple_GetFloat(poArgs, 1, &fCoolTime))
		return Py_BuildException();

	((UI::CImageBox*)pWindow)->SetCoolTime(fCoolTime);
	return Py_BuildNone();
}

PyObject *wndImageSetStartCoolTime(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow* pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();
	float fCoolTimeStart;
	if (!PyTuple_GetFloat(poArgs, 1, &fCoolTimeStart))
		return Py_BuildException();

	((UI::CImageBox*)pWindow)->SetCoolTimeStart(fCoolTimeStart);
	return Py_BuildNone();
}
#endif

#ifdef ENABLE_SLOT_MARKING_SYSTEM
PyObject *wndMgrSetCanMouseEventSlot(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow* pWnd;
	if (!PyTuple_GetWindow(poArgs, 0, &pWnd))
		return Py_BuildException();

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
		return Py_BuildException();

	if (!pWnd->IsType(UI::CSlotWindow::Type()))
		return Py_BuildException();

	UI::CSlotWindow* pSlotWnd = (UI::CSlotWindow*)pWnd;
	pSlotWnd->SetCanMouseEventSlot(iSlotIndex);
	return Py_BuildNone();
}

PyObject *wndMgrSetCantMouseEventSlot(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow* pWnd;
	if (!PyTuple_GetWindow(poArgs, 0, &pWnd))
		return Py_BuildException();

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
		return Py_BuildException();

	if (!pWnd->IsType(UI::CSlotWindow::Type()))
		return Py_BuildException();

	UI::CSlotWindow* pSlotWnd = (UI::CSlotWindow*)pWnd;
	pSlotWnd->SetCantMouseEventSlot(iSlotIndex);
	return Py_BuildNone();
}

PyObject *wndMgrSetUsableSlotOnTopWnd(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow* pWnd;
	if (!PyTuple_GetWindow(poArgs, 0, &pWnd))
		return Py_BuildException();

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
		return Py_BuildException();

	if (!pWnd->IsType(UI::CSlotWindow::Type()))
		return Py_BuildException();

	UI::CSlotWindow* pSlotWnd = (UI::CSlotWindow*)pWnd;
	pSlotWnd->SetUsableSlotOnTopWnd(iSlotIndex);
	return Py_BuildNone();
}

PyObject *wndMgrSetUnusableSlotOnTopWnd(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow* pWnd;
	if (!PyTuple_GetWindow(poArgs, 0, &pWnd))
		return Py_BuildException();

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
		return Py_BuildException();

	if (!pWnd->IsType(UI::CSlotWindow::Type()))
		return Py_BuildException();

	UI::CSlotWindow* pSlotWnd = (UI::CSlotWindow*)pWnd;
	pSlotWnd->SetUnusableSlotOnTopWnd(iSlotIndex);
	return Py_BuildNone();
}
#endif

#if defined(ENABLE_EMOTICONS_SYSTEM) || defined(ENABLE_FLEXIBLE_WIDTH_BUTTON)
PyObject *wndButtonSetScale(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow *pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}

	float fx;
	if (!PyTuple_GetFloat(poArgs, 1, &fx))
	{
		return Py_BuildException();
	}

	float fy;
	if (!PyTuple_GetFloat(poArgs, 2, &fy))
	{
		return Py_BuildException();
	}

	((UI::CButton*)pWindow)->SetScale(fx, fy);

	return Py_BuildNone();
}
#endif

#ifdef ENABLE_OFFICAL_FEATURES
PyObject* wndImageLeftRightReverse(PyObject* poSelf, PyObject* poArgs)
{
	UI::CWindow* pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	((UI::CImageBox*)pWindow)->LeftRightReverse();
	return Py_BuildNone();
}

PyObject* wndButtonLeftRightReverse(PyObject* poSelf, PyObject* poArgs)
{
	UI::CWindow* pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	((UI::CButton*)pWindow)->LeftRightReverse();
	return Py_BuildNone();
}
#endif

#ifdef ENABLE_FISH_GAME
PyObject *wndImageGetRotation(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow *pWindow;

	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	if (pWindow->IsType(UI::CAniImageBox::Type()))
		return Py_BuildValue("f", ((UI::CAniImageBox*)pWindow)->GetRotation());

	return Py_BuildValue("f", 0.0);
}

PyObject *wndImageMoveStart(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow *pWindow;

	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	if (pWindow->IsType(UI::CAniImageBox::Type()))
		((UI::CAniImageBox*)pWindow)->SetMoveStatus(true);

	return Py_BuildNone();
}

PyObject *wndImageMoveStop(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow *pWindow;

	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	if (pWindow->IsType(UI::CAniImageBox::Type()))
		((UI::CAniImageBox*)pWindow)->SetMoveStatus(false);

	return Py_BuildNone();
}

PyObject *wndImageSetMovePos(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow *pWindow;

	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	float directionX;
	if (!PyTuple_GetFloat(poArgs, 1, &directionX))
		return Py_BuildException();

	float directionY;
	if (!PyTuple_GetFloat(poArgs, 2, &directionY))
		return Py_BuildException();

	if (pWindow->IsType(UI::CAniImageBox::Type()))
		((UI::CAniImageBox*)pWindow)->SetMovePos(directionX, directionY);

	return Py_BuildNone();
}

PyObject *wndImageSetMoveSpeed(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow *pWindow;

	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	float speed;
	if (!PyTuple_GetFloat(poArgs, 1, &speed))
		return Py_BuildException();

	if (pWindow->IsType(UI::CAniImageBox::Type()))
		((UI::CAniImageBox*)pWindow)->SetMoveSpeed(speed);

	return Py_BuildNone();
}
#endif

#ifdef ENABLE_CHANGE_LOOK_SYSTEM
PyObject *wndMgrSetSlotCoverImage(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow *pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	DWORD dwSlotIndex;
	if (!PyTuple_GetUnsignedLong(poArgs, 1, &dwSlotIndex))
		return Py_BuildException();

	char *szFileName;
	if (!PyTuple_GetString(poArgs, 2, &szFileName))
		return Py_BuildException();

	if (!pWin->IsType(UI::CSlotWindow::Type()))
		return Py_BuildException();

	dynamic_cast<UI::CSlotWindow*>(pWin)->SetSlotCoverImage(dwSlotIndex, szFileName);

	return Py_BuildNone();
}

PyObject *wndMgrEnableSlotCoverImage(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow *pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
		return Py_BuildException();

	DWORD dwSlotIndex;
	if (!PyTuple_GetUnsignedLong(poArgs, 1, &dwSlotIndex))
		return Py_BuildException();

	bool bOnOff;
	if (!PyTuple_GetBoolean(poArgs, 2, &bOnOff))
		return Py_BuildException();

	if (!pWin->IsType(UI::CSlotWindow::Type()))
		return Py_BuildException();

	dynamic_cast<UI::CSlotWindow*>(pWin)->EnableSlotCoverImage(dwSlotIndex, bOnOff);

	return Py_BuildNone();
}
#endif

#ifdef ENABLE_CLIP_MASKING
PyObject *wndMgrSetClippingMaskRect(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow* pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	RECT r;
	if (!PyTuple_GetLong(poArgs, 1, &r.left))
		return Py_BuildException();

	if (!PyTuple_GetLong(poArgs, 2, &r.top))
		return Py_BuildException();

	if (!PyTuple_GetLong(poArgs, 3, &r.right))
		return Py_BuildException();

	if (!PyTuple_GetLong(poArgs, 4, &r.bottom))
		return Py_BuildException();

	pWindow->SetClippingMaskRect(r);
	return Py_BuildNone();
}

PyObject *wndMgrSetClippingMaskWindow(PyObject *poSelf, PyObject *poArgs)
{
	UI::CWindow *pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	UI::CWindow *pMaskWindow;
	if (!PyTuple_GetWindow(poArgs, 1, &pMaskWindow))
		return Py_BuildException();

	pWindow->SetClippingMaskWindow(pMaskWindow);
	return Py_BuildNone();
}
#endif

PyObject* wndMgrRegisterGifImageBox(PyObject* poSelf, PyObject* poArgs)
{
    PyObject* po;
    if (!PyTuple_GetObject(poArgs, 0, &po))
        return Py_BuildException();
    char* szLayer;
    if (!PyTuple_GetString(poArgs, 1, &szLayer))
        return Py_BuildException();

    UI::CWindow* pWindow = UI::CWindowManager::Instance().RegisterGifImageBox(po, szLayer);
    return Py_BuildPointer(pWindow);
}

PyObject* wndGifImageLoadGif(PyObject* poSelf, PyObject* poArgs)
{
    UI::CWindow* pWindow;
    if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
        return Py_BuildException();
    char* szFileName;
    if (!PyTuple_GetString(poArgs, 1, &szFileName))
        return Py_BuildException();

    ((UI::CGifImageBox*)pWindow)->LoadGif(szFileName);

    return Py_BuildNone();
}

PyObject* wndGifImageUnloadGif(PyObject* poSelf, PyObject* poArgs)
{
    UI::CWindow* pWindow;
    if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
        return Py_BuildException();

    ((UI::CGifImageBox*)pWindow)->UnloadGif();

    return Py_BuildNone();
}

PyObject * wndMgrGetSlotItemVnum(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
		return Py_BuildException();

	return Py_BuildValue("i", ((UI::CSlotWindow *)pWindow)->GetSlotItemVnum(iSlotIndex));
}

PyObject * wndMgrGetSlotItemCount(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
		return Py_BuildException();

	return Py_BuildValue("i", ((UI::CSlotWindow *)pWindow)->GetSlotItemCount(iSlotIndex));
}

PyObject * wndMgrGetSlotLocalPosition(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
		return Py_BuildException();

	int x = 0, y = 0;
	((UI::CSlotWindow *)pWindow)->GetSlotLocalPosition(iSlotIndex, &x, &y);
	return Py_BuildValue("ii", x, y);
}

PyObject * wndMgrGetSlotGlobalPosition(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
		return Py_BuildException();

	int x = 0, y = 0;
	((UI::CSlotWindow *)pWindow)->GetSlotGlobalPosition(iSlotIndex, &x, &y);
	return Py_BuildValue("ii", x, y);
}

PyObject * wndMgrIsActiveSlot(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
		return Py_BuildException();

	return Py_BuildValue("b", ((UI::CSlotWindow *)pWindow)->IsActiveSlot(iSlotIndex));
}

PyObject * wndMgrResetSlotCoolTime(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
		return Py_BuildException();

	((UI::CSlotWindow *)pWindow)->ResetSlotCoolTime(iSlotIndex);
	return Py_BuildNone();
}

PyObject * wndMgrSetSlotCoolTimeColor(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
		return Py_BuildException();

	float r, g, b, a;
	if (!PyTuple_GetFloat(poArgs, 2, &r))
		return Py_BuildException();
	if (!PyTuple_GetFloat(poArgs, 3, &g))
		return Py_BuildException();
	if (!PyTuple_GetFloat(poArgs, 4, &b))
		return Py_BuildException();
	if (!PyTuple_GetFloat(poArgs, 5, &a))
		return Py_BuildException();

	((UI::CSlotWindow *)pWindow)->SetSlotCoolTimeColor(iSlotIndex, r, g, b, a);
	return Py_BuildNone();
}

PyObject * wndMgrSetSlotDiffuseColor(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
		return Py_BuildException();

	int iColorType;
	if (!PyTuple_GetInteger(poArgs, 2, &iColorType))
		return Py_BuildException();

	((UI::CSlotWindow *)pWindow)->SetSlotDiffuseColor(iSlotIndex, iColorType);
	return Py_BuildNone();
}

PyObject * wndMgrSetSlotImage(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
		return Py_BuildException();

	char * szFileName;
	if (!PyTuple_GetString(poArgs, 2, &szFileName))
		return Py_BuildException();

	((UI::CSlotWindow *)pWindow)->SetSlotImage(iSlotIndex, szFileName);
	return Py_BuildNone();
}

PyObject * wndMgrSetSecondSlotCoverImage(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
		return Py_BuildException();

	char * szFileName;
	if (!PyTuple_GetString(poArgs, 2, &szFileName))
		return Py_BuildException();

	((UI::CSlotWindow *)pWindow)->SetSecondSlotCoverImage(iSlotIndex, szFileName);
	return Py_BuildNone();
}

PyObject * wndMgrEnableSecondSlotCoverImage(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
		return Py_BuildException();

	bool bEnable;
	if (!PyTuple_GetBoolean(poArgs, 2, &bEnable))
		return Py_BuildException();

	((UI::CSlotWindow *)pWindow)->EnableSecondSlotCoverImage(iSlotIndex, bEnable);
	return Py_BuildNone();
}

PyObject * wndMgrSetSlotConnectedYinAndYang(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
		return Py_BuildException();

	int iType;
	if (!PyTuple_GetInteger(poArgs, 2, &iType))
		return Py_BuildException();

	((UI::CSlotWindow *)pWindow)->SetSlotConnectedYinAndYang(iSlotIndex, iType);
	return Py_BuildNone();
}

PyObject * wndMgrClearSlotYinAndYangConnection(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
		return Py_BuildException();

	((UI::CSlotWindow *)pWindow)->ClearSlotYinAndYangConnection(iSlotIndex);
	return Py_BuildNone();
}

PyObject * wndMgrHideSlotButton(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
		return Py_BuildException();

	((UI::CSlotWindow *)pWindow)->HideSlotButton(iSlotIndex);
	return Py_BuildNone();
}

PyObject * wndMgrIsSlotOver(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
		return Py_BuildException();

	return Py_BuildValue("b", ((UI::CSlotWindow *)pWindow)->IsSlotOver(iSlotIndex));
}

PyObject * wndMgrSetAlwaysUsableSlotMode(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	bool bFlag;
	if (!PyTuple_GetBoolean(poArgs, 1, &bFlag))
		return Py_BuildException();

	((UI::CSlotWindow *)pWindow)->SetAlwaysUsableSlotMode(bFlag);
	return Py_BuildNone();
}

PyObject * wndMgrSetCorverButtonScale(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
		return Py_BuildException();

	float fx, fy;
	if (!PyTuple_GetFloat(poArgs, 2, &fx))
		return Py_BuildException();
	if (!PyTuple_GetFloat(poArgs, 3, &fy))
		return Py_BuildException();

	((UI::CSlotWindow *)pWindow)->SetCoverButtonScale(iSlotIndex, fx, fy);
	return Py_BuildNone();
}

PyObject * wndMgrSetBaseImageScale(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	float fx, fy;
	if (!PyTuple_GetFloat(poArgs, 1, &fx))
		return Py_BuildException();
	if (!PyTuple_GetFloat(poArgs, 2, &fy))
		return Py_BuildException();

	((UI::CSlotWindow *)pWindow)->SetBaseImageScale(fx, fy);
	return Py_BuildNone();
}

PyObject * wndMgrIsDisable(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	return Py_BuildValue("i", pWindow->IsDisable());
}

PyObject * wndMgrIsMouseInWindow(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	return Py_BuildValue("b", pWindow->IsIn());
}

PyObject * wndMgrIsRendering(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
		return Py_BuildException();

	return Py_BuildValue("i", pWindow->IsRendering() ? 1 : 0);
}

void initwndMgr()
{
	static PyMethodDef s_methods[] =
	{
		// WindowManager
        { "SetUIScale", wndMgrSetUIScale, METH_VARARGS },
        { "GetUIScale", wndMgrGetUIScale, METH_VARARGS },
		{ "SetMouseHandler",			wndMgrSetMouseHandler,				METH_VARARGS },
		{ "SetScreenSize",				wndMgrSetScreenSize,				METH_VARARGS },
		{ "GetScreenWidth",				wndMgrGetScreenWidth,				METH_VARARGS },
		{ "GetScreenHeight",			wndMgrGetScreenHeight,				METH_VARARGS },
		{ "AttachIcon",					wndMgrAttachIcon,					METH_VARARGS },
		{ "DeattachIcon",				wndMgrDeattachIcon,					METH_VARARGS },
		{ "SetAttachingFlag",			wndMgrSetAttachingFlag,				METH_VARARGS },
		{ "GetAspect",					wndMgrGetAspect,					METH_VARARGS },
		{ "GetHyperlink",				wndMgrGetHyperlink,					METH_VARARGS },
		{ "OnceIgnoreMouseLeftButtonUpEvent",	wndMgrOnceIgnoreMouseLeftButtonUpEvent,		METH_VARARGS },

		// Window
		{ "Register",					wndMgrRegister,						METH_VARARGS },
		{ "RegisterSlotWindow",			wndMgrRegisterSlotWindow,			METH_VARARGS },
		{ "RegisterGridSlotWindow",		wndMgrRegisterGridSlotWindow,		METH_VARARGS },
		{ "RegisterTextLine",			wndMgrRegisterTextLine,				METH_VARARGS },
		{ "RegisterMarkBox",			wndMgrRegisterMarkBox,				METH_VARARGS },
		{ "RegisterImageBox",			wndMgrRegisterImageBox,				METH_VARARGS },
		{ "RegisterExpandedImageBox",	wndMgrRegisterExpandedImageBox,		METH_VARARGS },
		{ "RegisterAniImageBox",		wndMgrRegisterAniImageBox,			METH_VARARGS },
        { "RegisterGifImageBox",        wndMgrRegisterGifImageBox,     		METH_VARARGS },
        { "LoadGif",                    wndGifImageLoadGif,         		METH_VARARGS },
        { "UnloadGif",    				wndGifImageUnloadGif,       		METH_VARARGS },
		{ "RegisterButton",				wndMgrRegisterButton,				METH_VARARGS },
		{ "RegisterRadioButton",		wndMgrRegisterRadioButton,			METH_VARARGS },
		{ "RegisterToggleButton",		wndMgrRegisterToggleButton,			METH_VARARGS },
		{ "RegisterDragButton",			wndMgrRegisterDragButton,			METH_VARARGS },
#ifdef ENABLE_FLEXIBLE_WIDTH_BUTTON
		{ "RegisterFlexibleWidthButton",			wndMgrRegisterFlexibleWidthButton,			METH_VARARGS },
		{ "RegisterFlexibleWidthToggleButton",		wndMgrRegisterFlexibleWidthToggleButton,	METH_VARARGS },
		{ "GetFlexibleWidthButtonSideWidth",		wndMgrGetFlexibleWidthButtonSideWidth,		METH_VARARGS },
		{ "SetFlexibleWidthButtonSideVisual",		wndMgrSetFlexibleWidthButtonSideVisual,		METH_VARARGS },
		{ "SetKeepUpVisual",						wndMgrSetKeepUpVisual,						METH_VARARGS },
		{ "IsKeepUpVisual",							wndMgrIsKeepUpVisual,						METH_VARARGS },
		{ "GetButtonImageWidth",					wndMgrGetButtonImageWidth,					METH_VARARGS },
		{ "GetButtonImageHeight",					wndMgrGetButtonImageHeight,					METH_VARARGS },
#endif
#ifdef ENABLE_RENDER_TARGET
		{ "RegisterRenderTarget",		wndMgrRegisterRenderTarget,			METH_VARARGS },
		{ "SetRenderTarget",			wndRenderTargetSetRenderTarget,		METH_VARARGS },
#endif
		{ "RegisterBox",				wndMgrRegisterBox,					METH_VARARGS },
		{ "RegisterBar",				wndMgrRegisterBar,					METH_VARARGS },
		{ "RegisterLine",				wndMgrRegisterLine,					METH_VARARGS },
		{ "RegisterBar3D",				wndMgrRegisterBar3D,				METH_VARARGS },
		{ "RegisterNumberLine",			wndMgrRegisterNumberLine,			METH_VARARGS },
		{ "Destroy",					wndMgrDestroy,						METH_VARARGS },
		{ "AddFlag",					wndMgrAddFlag,						METH_VARARGS },
		{ "IsRTL",						wndMgrIsRTL,						METH_VARARGS },

		// Base Window
		{ "SetName",					wndMgrSetName,						METH_VARARGS },
		{ "GetName",					wndMgrGetName,						METH_VARARGS },

		{ "SetTop",						wndMgrSetTop,						METH_VARARGS },
		{ "Show",						wndMgrShow,							METH_VARARGS },
		{ "Hide",						wndMgrHide,							METH_VARARGS },
		{ "IsShow",						wndMgrIsShow,						METH_VARARGS },
		{ "SetParent",					wndMgrSetParent,					METH_VARARGS },
		{ "SetPickAlways",				wndMgrSetPickAlways,				METH_VARARGS },

		{ "IsFocus",					wndMgrIsFocus,						METH_VARARGS },
		{ "SetFocus",					wndMgrSetFocus,						METH_VARARGS },
		{ "KillFocus",					wndMgrKillFocus,					METH_VARARGS },
#ifdef ENABLE_UI_TAB_FOCUS_SYSTEM
		{ "SetFocusable",				wndMgrSetFocusable,					METH_VARARGS },
#endif
		{ "Lock",						wndMgrLock,							METH_VARARGS },
		{ "Unlock",						wndMgrUnlock,						METH_VARARGS },

		{ "SetWindowSize",				wndMgrSetWndSize,					METH_VARARGS },
		{ "SetWindowPosition",			wndMgrSetWndPosition,				METH_VARARGS },
		{ "GetWindowWidth",				wndMgrGetWndWidth,					METH_VARARGS },
		{ "GetWindowHeight",			wndMgrGetWndHeight,					METH_VARARGS },
		{ "GetWindowLocalPosition",		wndMgrGetWndLocalPosition,			METH_VARARGS },
		{ "GetWindowGlobalPosition",	wndMgrGetWndGlobalPosition,			METH_VARARGS },
		{ "GetWindowRect",				wndMgrGetWindowRect,				METH_VARARGS },
		{ "SetWindowHorizontalAlign",	wndMgrSetWindowHorizontalAlign,		METH_VARARGS },
		{ "SetWindowVerticalAlign",		wndMgrSetWindowVerticalAlign,		METH_VARARGS },

		{ "GetChildCount",				wndMgrGetChildCount,				METH_VARARGS },

		{ "IsPickedWindow",				wndMgrIsPickedWindow,				METH_VARARGS },
		{ "IsIn",						wndMgrIsIn,							METH_VARARGS },
		{ "GetMouseLocalPosition",		wndMgrGetMouseLocalPosition,		METH_VARARGS },
		{ "GetMousePosition",			wndMgrGetMousePosition,				METH_VARARGS },
		{ "IsDragging",					wndMgrIsDragging,					METH_VARARGS },

		{ "SetLimitBias",				wndMgrSetLimitBias,					METH_VARARGS },

		{ "UpdateRect",					wndMgrUpdateRect,					METH_VARARGS },

		// Slot Window
		{ "AppendSlot",					wndMgrAppendSlot,					METH_VARARGS },
		{ "ArrangeSlot",				wndMgrArrangeSlot,					METH_VARARGS },
		{ "ClearSlot",					wndMgrClearSlot,					METH_VARARGS },
		{ "ClearAllSlot",				wndMgrClearAllSlot,					METH_VARARGS },
		{ "HasSlot",					wndMgrHasSlot,						METH_VARARGS },
		{ "SetSlot",					wndMgrSetSlot,						METH_VARARGS },
		{ "SetSlotCount",				wndMgrSetSlotCount,					METH_VARARGS },
		{ "SetSlotCountNew",			wndMgrSetSlotCountNew,				METH_VARARGS },
		{ "SetSlotRealNumber",			wndMgrSetRealSlotNumber,			METH_VARARGS },
		{ "SetSlotCoolTime",			wndMgrSetSlotCoolTime,				METH_VARARGS },
		{ "StoreSlotCoolTime",			wndMgrStoreSlotCoolTime,			METH_VARARGS },
		{ "RestoreSlotCoolTime",		wndMgrRestoreSlotCoolTime,			METH_VARARGS },
		{ "SetToggleSlot",				wndMgrSetToggleSlot,				METH_VARARGS },
		{ "ActivateSlot",				wndMgrActivateSlot,					METH_VARARGS },
		{ "DeactivateSlot",				wndMgrDeactivateSlot,				METH_VARARGS },
		{ "EnableSlot",					wndMgrEnableSlot,					METH_VARARGS },
		{ "DisableSlot",				wndMgrDisableSlot,					METH_VARARGS },
		{ "ShowSlotBaseImage",			wndMgrShowSlotBaseImage,			METH_VARARGS },
		{ "HideSlotBaseImage",			wndMgrHideSlotBaseImage,			METH_VARARGS },
		{ "SetSlotType",				wndMgrSetSlotType,					METH_VARARGS },
		{ "SetSlotStyle",				wndMgrSetSlotStyle,					METH_VARARGS },
		{ "SetSlotScale",				wndMgrSetSlotScale,					METH_VARARGS },
		{ "SetSlotBaseImage",			wndMgrSetSlotBaseImage,				METH_VARARGS },
		{ "SetSlotCoolTimeInverse",		wndMgrSetSlotCoolTimeInverse,		METH_VARARGS },

		{ "SetCoverButton",				wndMgrSetCoverButton,				METH_VARARGS },
		{ "EnableCoverButton",			wndMgrEnableCoverButton,			METH_VARARGS },
		{ "DisableCoverButton",			wndMgrDisableCoverButton,			METH_VARARGS },
		{ "IsDisableCoverButton",		wndMgrIsDisableCoverButton,			METH_VARARGS },
		{ "SetAlwaysRenderCoverButton",	wndMgrSetAlwaysRenderCoverButton,	METH_VARARGS },
		
		{ "AppendSlotButton",			wndMgrAppendSlotButton,				METH_VARARGS },
		{ "AppendRequirementSignImage",	wndMgrAppendRequirementSignImage,	METH_VARARGS },
		{ "ShowSlotButton",				wndMgrShowSlotButton,				METH_VARARGS },
		{ "HideAllSlotButton",			wndMgrHideAllSlotButton,			METH_VARARGS },
		{ "ShowRequirementSign",		wndMgrShowRequirementSign,			METH_VARARGS },
		{ "HideRequirementSign",		wndMgrHideRequirementSign,			METH_VARARGS },
		{ "RefreshSlot",				wndMgrRefreshSlot,					METH_VARARGS },
		{ "SetUseMode",					wndMgrSetUseMode,					METH_VARARGS },
		{ "SetUsableItem",				wndMgrSetUsableItem,				METH_VARARGS },

		{ "SelectSlot",					wndMgrSelectSlot,					METH_VARARGS },
		{ "ClearSelected",				wndMgrClearSelected,				METH_VARARGS },
		{ "GetSelectedSlotCount",		wndMgrGetSelectedSlotCount,			METH_VARARGS },
		{ "GetSelectedSlotNumber",		wndMgrGetSelectedSlotNumber,		METH_VARARGS },
		{ "IsSelectedSlot",				wndMgrIsSelectedSlot,				METH_VARARGS },
		{ "GetSlotCount",				wndMgrGetSlotCount,					METH_VARARGS },
		{ "GetSlotNumber",				wndMgrGetSlotNumber,				METH_VARARGS },
		{ "LockSlot",					wndMgrLockSlot,						METH_VARARGS },
		{ "UnlockSlot",					wndMgrUnlockSlot,					METH_VARARGS },
		{ "IsLockSlot",					wndMgrIsLockSlot,					METH_VARARGS },

		// Bar
		{ "SetColor",					wndBarSetColor,						METH_VARARGS },

		// TextLine
		{ "SetMax",						wndTextSetMax,						METH_VARARGS },
		{ "SetHorizontalAlign",			wndTextSetHorizontalAlign,			METH_VARARGS },
		{ "SetVerticalAlign",			wndTextSetVerticalAlign,			METH_VARARGS },
		{ "SetSecret",					wndTextSetSecret,					METH_VARARGS },
		{ "SetOutline",					wndTextSetOutline,					METH_VARARGS },
		{ "SetFeather",					wndTextSetFeather,					METH_VARARGS },
		{ "SetMultiLine",				wndTextSetMultiLine,				METH_VARARGS },
		{ "SetText",					wndTextSetText,						METH_VARARGS },
#ifdef ENABLE_MULTILANGUAGE_CHATTING
		{ "SetFontColorRange",				wndTextSetFontColorRange,			METH_VARARGS },
		{ "SetTextWithCodePage",			wndTextSetTextWithCodePage,			METH_VARARGS },
		{ "GetInputSystemLangID",			wndGetInputSystemLangID,			METH_VARARGS },
#endif
		{ "SetFontName",				wndTextSetFontName,					METH_VARARGS },
		{ "SetFontColor",				wndTextSetFontColor,				METH_VARARGS },
		{ "SetLimitWidth",				wndTextSetLimitWidth,				METH_VARARGS },
		{ "GetText",					wndTextGetText,						METH_VARARGS },
		{ "GetTextSize",				wndTextGetTextSize,					METH_VARARGS },
#ifdef ENABLE_SUNG_MAHI_TOWER
		{ "GetCharSize",				wndTextGetCharSize,					METH_VARARGS },
#endif
		{ "ShowCursor",					wndTextShowCursor,					METH_VARARGS },
		{ "HideCursor",					wndTextHideCursor,					METH_VARARGS },
		{ "GetCursorPosition",			wndTextGetCursorPosition,			METH_VARARGS },

#ifdef INSIDE_RENDER
		{ "SetInsideRender",			wndMgrSetInsideRender,				METH_VARARGS },
		{ "SetRenderingRect",			wndSetRenderingRect,				METH_VARARGS },
		{ "GetRenderBox",				wndMgrGetRenderBox,					METH_VARARGS },
		{ "SetFixedRenderPos",			wndTextSetFixedRenderPos,			METH_VARARGS },
		{ "GetRenderPos",				wndTextGetRenderPos,				METH_VARARGS },
		{ "UnloadImage",				wndImageUnloadImage,				METH_VARARGS },
		{ "IsShowCursor",				wndTextIsShowCursor,				METH_VARARGS },
#endif

		// NumberLine
		{ "SetNumber",						wndNumberSetNumber,							METH_VARARGS },
		{ "SetNumberHorizontalAlignCenter",	wndNumberSetNumberHorizontalAlignCenter,	METH_VARARGS },
		{ "SetNumberHorizontalAlignRight",	wndNumberSetNumberHorizontalAlignRight,		METH_VARARGS },
		{ "SetPath",						wndNumberSetPath,							METH_VARARGS },
#ifdef ENABLE_SLOT_CHAR_ENLARGEMENT
		{ "SetCanEnlarge",						wndNumberSetCanEnlarge,						METH_VARARGS },
		{ "RefreshSlotCharState",				wndNumberRefreshSlotCharState,				METH_VARARGS },
#endif

		// ImageBox
		{ "MarkBox_SetImage",			wndMarkBox_SetImage,				METH_VARARGS },
		{ "MarkBox_SetImageFilename",	wndMarkBox_SetImageFilename,		METH_VARARGS },
		{ "MarkBox_Load",				wndMarkBox_Load,					METH_VARARGS },
		{ "MarkBox_SetIndex",			wndMarkBox_SetIndex,				METH_VARARGS },
		{ "MarkBox_SetScale",			wndMarkBox_SetScale,				METH_VARARGS },
		{ "MarkBox_SetDiffuseColor",	wndMarkBox_SetDiffuseColor,			METH_VARARGS },

		// ImageBox
		{ "LoadImage",					wndImageLoadImage,					METH_VARARGS },
		{ "SetDiffuseColor",			wndImageSetDiffuseColor,			METH_VARARGS },
		{ "GetWidth",					wndImageGetWidth,					METH_VARARGS },
		{ "GetHeight",					wndImageGetHeight,					METH_VARARGS },
		// ExpandedImageBox
		{ "SetScale",					wndImageSetScale,					METH_VARARGS },
		{ "SetOrigin",					wndImageSetOrigin,					METH_VARARGS },
		{ "SetRotation",				wndImageSetRotation,				METH_VARARGS },
		{ "SetRenderingRect",			wndImageSetRenderingRect,			METH_VARARGS },
		{ "SetRenderingMode",			wndImageSetRenderingMode,			METH_VARARGS },
		// AniImageBox
		{ "SetDelay",					wndImageSetDelay,					METH_VARARGS },
		{ "AppendImage",				wndImageAppendImage,				METH_VARARGS },

#ifdef ENABLE_MINI_GAME
		{ "SetCardSlot",				wndMgrSetCardSlot,					METH_VARARGS },
		{ "ResetFrame",					wndImageResetFrame,					METH_VARARGS },
#endif
#ifdef ENABLE_OFFICAL_FEATURES
		{ "LeftRightReverseImageBox", 	wndImageLeftRightReverse, 			METH_VARARGS },
#endif

		// Button
		{ "SetUpVisual",				wndButtonSetUpVisual,				METH_VARARGS },
		{ "SetOverVisual",				wndButtonSetOverVisual,				METH_VARARGS },
		{ "SetDownVisual",				wndButtonSetDownVisual,				METH_VARARGS },
		{ "SetDisableVisual",			wndButtonSetDisableVisual,			METH_VARARGS },
		{ "GetUpVisualFileName",		wndButtonGetUpVisualFileName,		METH_VARARGS },
		{ "GetOverVisualFileName",		wndButtonGetOverVisualFileName,		METH_VARARGS },
		{ "GetDownVisualFileName",		wndButtonGetDownVisualFileName,		METH_VARARGS },
		{ "Flash",						wndButtonFlash,						METH_VARARGS },
		{ "EnableFlash",				wndButtonEnableFlash,				METH_VARARGS },
		{ "DisableFlash",				wndButtonDisableFlash,				METH_VARARGS },
		{ "Enable",						wndButtonEnable,					METH_VARARGS },
		{ "Disable",					wndButtonDisable,					METH_VARARGS },
		{ "Down",						wndButtonDown,						METH_VARARGS },
		{ "SetUp",						wndButtonSetUp,						METH_VARARGS },
		{ "IsDown",						wndButtonIsDown,					METH_VARARGS },
#ifdef ENABLE_FISH_EVENT_SYSTEM
		{ "SetPickedAreaRender",		wndMgrSetPickedAreaRender,			METH_VARARGS },
		{ "DeleteCoverButton",			wndMgrDeleteCoverButton,			METH_VARARGS },
		{ "SetDisableDeattach",			wndMgrSetDisableDeattach,			METH_VARARGS },
#endif
#ifdef ENABLE_OFFICAL_FEATURES
		{ "LeftRightReverse", 			wndButtonLeftRightReverse, 			METH_VARARGS },
#endif

		// DragButton
		{ "SetRestrictMovementArea",	wndButtonSetRestrictMovementArea,	METH_VARARGS },

		// For Debug
		{ "SetOutlineFlag",				wndMgrSetOutlineFlag,				METH_VARARGS },
		{ "ShowOverInWindowName",		wndMgrShowOverInWindowName,			METH_VARARGS },

		{ "SetAniImgScale",				wndSetAniImgScale,					METH_VARARGS },

		{ "SetClipRect",				wndSetClipRect,						METH_VARARGS },
		{ "SetRenderingRectWithScale",	wndMgrSetRenderingRectWithScale,	METH_VARARGS },

#ifdef ENABLE_SKILL_COLOR_SYSTEM
		{ "GetRealMousePosition",		wndMgrGetRealMousePosition,			METH_VARARGS },
		{ "GetColorAtPosition",			wndMgrGetColorAtPosition,			METH_VARARGS },
#endif

#ifdef ENABLE_MULTI_TEXTLINE
		{ "GetTextLineCount",			wndTextGetTextLineCount,			METH_VARARGS },
		{ "DisableEnterToken",			wndTextDisableEnterToken,			METH_VARARGS },
		{ "SetLineHeight",				wndTextSetLineHeight,				METH_VARARGS },
		{ "GetLineHeight",				wndTextGetLineHeight,				METH_VARARGS },
#endif

#ifdef ENABLE_NEW_DUNGEON_LIB
		{ "SetCoolTimeImageBox",		wndImageSetCoolTime,				METH_VARARGS },
		{ "SetStartCoolTimeImageBox",	wndImageSetStartCoolTime,			METH_VARARGS },
#endif

#ifdef ENABLE_SLOT_MARKING_SYSTEM
		{ "SetCanMouseEventSlot",		wndMgrSetCanMouseEventSlot,			METH_VARARGS },
		{ "SetCantMouseEventSlot",		wndMgrSetCantMouseEventSlot,		METH_VARARGS },
		{ "SetUsableSlotOnTopWnd",		wndMgrSetUsableSlotOnTopWnd,		METH_VARARGS },
		{ "SetUnusableSlotOnTopWnd",	wndMgrSetUnusableSlotOnTopWnd,		METH_VARARGS },
#endif

#if defined(ENABLE_EMOTICONS_SYSTEM) || defined(ENABLE_FLEXIBLE_WIDTH_BUTTON)
		{ "SetButtonScale",				wndButtonSetScale,					METH_VARARGS },
#endif

#ifdef ENABLE_FISH_GAME
		{ "GetRotation",				wndImageGetRotation,				METH_VARARGS },
		{ "MoveStart",					wndImageMoveStart,					METH_VARARGS },
		{ "MoveStop",					wndImageMoveStop,					METH_VARARGS },
		{ "SetMovePos",					wndImageSetMovePos,					METH_VARARGS },
		{ "SetMoveSpeed",				wndImageSetMoveSpeed,				METH_VARARGS },
#endif

#ifdef ENABLE_CHANGE_LOOK_SYSTEM
		{ "SetSlotCoverImage",			wndMgrSetSlotCoverImage,			METH_VARARGS },
		{ "EnableSlotCoverImage",		wndMgrEnableSlotCoverImage,			METH_VARARGS },
#endif

#ifdef ENABLE_CLIP_MASKING
		{ "SetClippingMaskRect",		wndMgrSetClippingMaskRect,			METH_VARARGS },
		{ "SetClippingMaskWindow",		wndMgrSetClippingMaskWindow,		METH_VARARGS },
#endif

		{ "GetSlotItemVnum",			wndMgrGetSlotItemVnum,				METH_VARARGS },
		{ "GetSlotItemCount",			wndMgrGetSlotItemCount,				METH_VARARGS },
		{ "GetSlotLocalPosition",		wndMgrGetSlotLocalPosition,			METH_VARARGS },
		{ "GetSlotGlobalPosition",		wndMgrGetSlotGlobalPosition,		METH_VARARGS },
		{ "IsActiveSlot",				wndMgrIsActiveSlot,					METH_VARARGS },
		{ "ResetSlotCoolTime",			wndMgrResetSlotCoolTime,			METH_VARARGS },
		{ "SetSlotCoolTimeColor",		wndMgrSetSlotCoolTimeColor,			METH_VARARGS },
		{ "SetSlotDiffuseColor",		wndMgrSetSlotDiffuseColor,			METH_VARARGS },
		{ "SetSlotImage",				wndMgrSetSlotImage,					METH_VARARGS },
		{ "SetSecondSlotCoverImage",	wndMgrSetSecondSlotCoverImage,		METH_VARARGS },
		{ "EnableSecondSlotCoverImage",	wndMgrEnableSecondSlotCoverImage,	METH_VARARGS },
		{ "SetSlotConnectedYinAndYang",	wndMgrSetSlotConnectedYinAndYang,	METH_VARARGS },
		{ "ClearSlotYinAndYangConnection", wndMgrClearSlotYinAndYangConnection, METH_VARARGS },
		{ "HideSlotButton",				wndMgrHideSlotButton,				METH_VARARGS },
		{ "IsSlotOver",					wndMgrIsSlotOver,					METH_VARARGS },
		{ "SetAlwaysUsableSlotMode",	wndMgrSetAlwaysUsableSlotMode,		METH_VARARGS },
		{ "SetCorverButtonScale",		wndMgrSetCorverButtonScale,			METH_VARARGS },
		{ "SetBaseImageScale",			wndMgrSetBaseImageScale,			METH_VARARGS },
		{ "IsDisable",					wndMgrIsDisable,					METH_VARARGS },
		{ "IsMouseInWindow",			wndMgrIsMouseInWindow,				METH_VARARGS },
		{ "IsRendering",				wndMgrIsRendering,					METH_VARARGS },

		{ nullptr, nullptr },
	};

	PyObject *poModule = Py_InitModule("wndMgr", s_methods);


	PyModule_AddIntConstant(poModule, "SLOT_STYLE_NONE",				UI::SLOT_STYLE_NONE);
	PyModule_AddIntConstant(poModule, "SLOT_STYLE_PICK_UP",				UI::SLOT_STYLE_PICK_UP);
	PyModule_AddIntConstant(poModule, "SLOT_STYLE_SELECT",				UI::SLOT_STYLE_SELECT);

	PyModule_AddIntConstant(poModule, "TEXT_HORIZONTAL_ALIGN_LEFT",		CGraphicTextInstance::HORIZONTAL_ALIGN_LEFT);
	PyModule_AddIntConstant(poModule, "TEXT_HORIZONTAL_ALIGN_RIGHT",	CGraphicTextInstance::HORIZONTAL_ALIGN_RIGHT);
	PyModule_AddIntConstant(poModule, "TEXT_HORIZONTAL_ALIGN_CENTER",	CGraphicTextInstance::HORIZONTAL_ALIGN_CENTER);
	PyModule_AddIntConstant(poModule, "TEXT_VERTICAL_ALIGN_TOP",		CGraphicTextInstance::VERTICAL_ALIGN_TOP);
	PyModule_AddIntConstant(poModule, "TEXT_VERTICAL_ALIGN_BOTTOM",		CGraphicTextInstance::VERTICAL_ALIGN_BOTTOM);
	PyModule_AddIntConstant(poModule, "TEXT_VERTICAL_ALIGN_CENTER",		CGraphicTextInstance::VERTICAL_ALIGN_CENTER);

	PyModule_AddIntConstant(poModule, "HORIZONTAL_ALIGN_LEFT",			UI::CWindow::HORIZONTAL_ALIGN_LEFT);
	PyModule_AddIntConstant(poModule, "HORIZONTAL_ALIGN_CENTER",		UI::CWindow::HORIZONTAL_ALIGN_CENTER);
	PyModule_AddIntConstant(poModule, "HORIZONTAL_ALIGN_RIGHT",			UI::CWindow::HORIZONTAL_ALIGN_RIGHT);
	PyModule_AddIntConstant(poModule, "VERTICAL_ALIGN_TOP",				UI::CWindow::VERTICAL_ALIGN_TOP);
	PyModule_AddIntConstant(poModule, "VERTICAL_ALIGN_CENTER",			UI::CWindow::VERTICAL_ALIGN_CENTER);
	PyModule_AddIntConstant(poModule, "VERTICAL_ALIGN_BOTTOM",			UI::CWindow::VERTICAL_ALIGN_BOTTOM);

	PyModule_AddIntConstant(poModule, "RENDERING_MODE_MODULATE",		CGraphicExpandedImageInstance::RENDERING_MODE_MODULATE);

	PyModule_AddIntConstant(poModule, "COLOR_TYPE_ORANGE",				UI::COLOR_TYPE_ORANGE);
	PyModule_AddIntConstant(poModule, "COLOR_TYPE_WHITE",				UI::COLOR_TYPE_WHITE);
	PyModule_AddIntConstant(poModule, "COLOR_TYPE_RED",					UI::COLOR_TYPE_RED);
	PyModule_AddIntConstant(poModule, "COLOR_TYPE_GREEN",				UI::COLOR_TYPE_GREEN);
	PyModule_AddIntConstant(poModule, "COLOR_TYPE_YELLOW",				UI::COLOR_TYPE_YELLOW);
	PyModule_AddIntConstant(poModule, "COLOR_TYPE_BLUE",				UI::COLOR_TYPE_BLUE);
	PyModule_AddIntConstant(poModule, "COLOR_TYPE_SKY",					UI::COLOR_TYPE_SKY);
	PyModule_AddIntConstant(poModule, "COLOR_TYPE_PINK",				UI::COLOR_TYPE_PINK);

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	PyModule_AddIntConstant(poModule, "HILIGHTSLOT_ACCE",				UI::HILIGHTSLOT_ACCE);
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
	PyModule_AddIntConstant(poModule, "HILIGHTSLOT_AURA",				UI::HILIGHTSLOT_AURA);
#endif
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
	PyModule_AddIntConstant(poModule, "HILIGHTSLOT_CHANGE_LOOK",		UI::HILIGHTSLOT_CHANGE_LOOK);
#endif
	PyModule_AddIntConstant(poModule, "HILIGHTSLOT_CUBE",				UI::HILIGHTSLOT_CUBE);
	PyModule_AddIntConstant(poModule, "HILIGHTSLOT_MAX",				UI::HILIGHTSLOT_MAX);

	PyModule_AddIntConstant(poModule, "COLOR_RED", 0);
	PyModule_AddIntConstant(poModule, "COLOR_GREEN", 1);
	PyModule_AddIntConstant(poModule, "COLOR_BLUE", 2);
	PyModule_AddIntConstant(poModule, "COLOR_TYPE_YIN_WEAPON", 7);
	PyModule_AddIntConstant(poModule, "COLOR_TYPE_YIN_GLOVE", 8);
	PyModule_AddIntConstant(poModule, "SIDE_LEFT", 0);
	PyModule_AddIntConstant(poModule, "SIDE_RIGHT", 1);
	PyModule_AddIntConstant(poModule, "VISUAL_UP", 0);
	PyModule_AddIntConstant(poModule, "VISUAL_OVER", 1);
	PyModule_AddIntConstant(poModule, "VISUAL_DOWN", 2);
	PyModule_AddIntConstant(poModule, "VISUAL_DISABLE", 3);
}
