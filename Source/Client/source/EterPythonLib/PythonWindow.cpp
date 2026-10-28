#include "StdAfx.h"



#include "../eterBase/CRC32.h"



#include "PythonWindow.h"



#include "PythonSlotWindow.h"



#include "PythonWindowManager.h"



#include <gif_lib.h>



#include <wingdi.h>



#include <array>







#ifdef ENABLE_RENDER_TARGET



	#include "../EterLib/RenderTargetManager.h"



#endif







#ifdef ENABLE_UI_DEBUG_WINDOW



	#include <fmt/format.h>



#endif







BOOL g_bOutlineBoxEnable = FALSE;







#ifdef ENABLE_FISH_GAME



#include "../eterBase/Random.h"



#include <math.h>







#define _PI ((float) 3.141592654f)



#define _1BYPI ((float) 0.318309886f)



#define DegreeToRadian( degree ) ((degree) * (_PI / 180.0f))



#define RadianToDegree( radian ) ((radian) * (180.0f / _PI))







typedef struct SVector



{



	float x;



	float y;



	float z;



} VECTOR;







inline float DISTANCE_SQRT(long dx, long dy)



{



	return ::sqrt((float)dx * dx + (float)dy * dy);



}







void Normalize(VECTOR* pV1, VECTOR* pV2)



{



	float l = sqrtf(pV1->x * pV1->x + pV1->y * pV1->y + pV1->z * pV1->z + 1.0e-12);



	pV2->x = pV1->x / l;



	pV2->y = pV1->y / l;



	pV2->z = pV1->z / l;



}







float DotProduct(VECTOR* pV1, VECTOR* pV2)



{



	return pV1->x * pV2->x + pV1->y * pV2->y + pV1->z * pV2->z;



}







float GetDegreeFromPositionEx(float x, float y)



{



	VECTOR	vtDir;



	VECTOR	vtStan;



	float	ret;



	vtDir.x = x;



	vtDir.y = y;



	vtDir.z = 0.0f;



	Normalize(&vtDir, &vtDir);



	vtStan.x = 0.0f;



	vtStan.y = 1.0f;



	vtStan.z = 0.0f;



	ret = RadianToDegree(acosf(DotProduct(&vtDir, &vtStan)));



	if (vtDir.x < 0.0f)



		ret = 360.0f - ret;



	return (ret);



}







float GetDegreeFromPositionXY(long sx, long sy, long ex, long ey)



{



	return GetDegreeFromPositionEx(ex - sx, ey - sy);



}







float GetDegreeDeltaEx(float iDegree, float iDegree2)



{



	if (iDegree > 180.0f)



		iDegree = iDegree - 360.0f;







	if (iDegree2 > 180.0f)



		iDegree2 = iDegree2 - 360.0f;







	return fabs(iDegree - iDegree2);



}







float GetDegreeFromPositionNew(float x, float y)



{



	D3DXVECTOR3 vtDir(floor(x), floor(y), 0.0f);



	D3DXVec3Normalize(&vtDir, &vtDir);







	D3DXVECTOR3 vtStan(0, -1, 0);



	float ret = D3DXToDegree(acosf(D3DXVec3Dot(&vtDir, &vtStan)));







	if (vtDir.x < 0.0f)



		ret = 360.0f - ret;







	return ret;



}







void GetDeltaByDegree(float fDegree, float fDistance, float* x, float* y)



{



	float fRadian = DegreeToRadian(fDegree);







	*x = fDistance * sin(fRadian);



	*y = fDistance * cos(fRadian);



}



#endif







namespace UI



{



	CWindow::CWindow(PyObject * ppyObject) :



		m_x(0),



		m_y(0),



		m_lWidth(0),



		m_lHeight(0),



		m_poHandler(ppyObject),



		m_bShow(false),
#ifdef ENABLE_UI_TAB_FOCUS_SYSTEM
		m_bFocusable(false),
#endif
#ifdef ENABLE_MULTILANGUAGE_CHATTING
		m_dwLangID(0),
#endif



		m_pParent(nullptr),



		m_dwFlag(0),



		m_isUpdatingChildren(FALSE)



#ifdef INSIDE_RENDER



		, m_isInsideRender(false)



#endif



	{



#ifdef _DEBUG



		static DWORD DEBUG_dwGlobalCounter=0;



		DEBUG_dwCounter=DEBUG_dwGlobalCounter++;







		m_strName = "!!debug";



#endif







		m_HorizontalAlign = HORIZONTAL_ALIGN_LEFT;



		m_VerticalAlign = VERTICAL_ALIGN_TOP;



		m_rect.bottom = m_rect.left = m_rect.right = m_rect.top = 0;



		m_limitBiasRect.bottom = m_limitBiasRect.left = m_limitBiasRect.right = m_limitBiasRect.top = 0;



#ifdef INSIDE_RENDER



		memset(&m_renderBox, 0, sizeof(m_renderBox));



#endif



#ifdef ENABLE_CLIP_MASKING



		m_bEnableMask = false;



		m_pMaskWindow = NULL;



		memset(&m_rMaskRect, 0, sizeof(m_rMaskRect));



#endif



		m_windowType = WINDOW_TYPE_WINDOW;



	}







	CWindow::~CWindow()



	{



	}







	DWORD CWindow::Type()



	{



		static DWORD s_dwType = GetCRC32("CWindow", strlen("CWindow"));



		return (s_dwType);



	}







	BOOL CWindow::IsType(DWORD dwType)



	{



		return OnIsType(dwType);



	}







	BOOL CWindow::OnIsType(DWORD dwType)



	{



		if (CWindow::Type() == dwType)



			return TRUE;







		return FALSE;



	}







	struct FClear



	{



		void operator () (CWindow * pWin)



		{



			pWin->Clear();



		}



	};







	void CWindow::Clear()



	{



#ifdef ENABLE_CLIP_MASKING



		m_pMaskWindow = NULL;



#endif



		std::for_each(m_pChildList.begin(), m_pChildList.end(), FClear());



		m_pChildList.clear();







		m_pParent = nullptr;



		DestroyHandle();



		Hide();



	}







	void CWindow::DestroyHandle()



	{



		m_poHandler = nullptr;



	}







	void CWindow::Show()



	{



		m_bShow = true;



	}







	void CWindow::Hide()



	{



		m_bShow = false;



	}







#ifdef INSIDE_RENDER



	void CWindow::OnHideWithChilds()



	{



		OnHide();



		std::for_each(m_pChildList.begin(), m_pChildList.end(), std::mem_fn(&CWindow::OnHideWithChilds));



	}



	void CWindow::OnHide()



	{



		PyCallClassMemberFunc(m_poHandler, "OnHide", BuildEmptyTuple());



	}



#endif







	bool CWindow::IsRendering()



	{



		if (!IsShow())



			return false;







		if (!m_pParent)



			return true;







		return m_pParent->IsRendering();



	}







	void CWindow::__RemoveReserveChildren()



	{



		if (m_pReserveChildList.empty())



			return;







		TWindowContainer::iterator it;



		for(it = m_pReserveChildList.begin(); it != m_pReserveChildList.end(); ++it)



		{



			m_pChildList.remove(*it);



		}



		m_pReserveChildList.clear();



	}







	void CWindow::Update()



	{



		if (!IsShow())



			return;







		__RemoveReserveChildren();







		OnUpdate();







		m_isUpdatingChildren = TRUE;



		TWindowContainer::iterator it;



		for(it = m_pChildList.begin(); it != m_pChildList.end();)



		{



			TWindowContainer::iterator it_next = it;



			++it_next;



			(*it)->Update();



			it = it_next;



		}



		m_isUpdatingChildren = FALSE;



	}







#ifdef INSIDE_RENDER



	bool CWindow::IsShow()



	{



		if (!m_bShow)



			return false;







		if (m_isInsideRender)



			if (m_renderBox.left + m_renderBox.right >= m_lWidth || m_renderBox.top + m_renderBox.bottom >= m_lHeight)



				return false;







		return true;



	}



#endif







	void CWindow::Render()



	{



		if (!IsShow())



			return;







		OnRender();







#ifdef ENABLE_UI_DEBUG_WINDOW



        if (g_bOutlineBoxEnable)



        {



            if (CWindowManager::instance().GetPointWindow() == this)



            {



                const auto parentRect = m_pParent ? m_pParent->m_rect : RECT{ 0, 0, 0, 0 };



                CPythonGraphic::Instance().SetDiffuseColor(0.0f, 0.0f, 1.0f);



                CPythonGraphic::Instance().RenderBox2d(parentRect.left, parentRect.top, parentRect.right, parentRect.bottom);







                CPythonGraphic::Instance().SetDiffuseColor(0.0f, 1.0f, 0.0f);



                CPythonGraphic::Instance().RenderBox2d(m_rect.left, m_rect.top, m_rect.right, m_rect.bottom);







                std::ostringstream oss;



                oss << m_strName << " (" << Type() << ")";



                CPythonGraphic::Instance().RenderTextLine(m_rect.left + 5, m_rect.top - 30, oss.str().c_str(), 0xFFFFFF00);







                const auto flags = m_dwFlag;



                std::string flagInfo;



                if (flags & FLAG_MOVABLE)



                    flagInfo += "FLAG_MOVABLE ";



                if (flags & FLAG_SNAP)



                    flagInfo += "FLAG_SNAP ";



                if (flags & FLAG_DRAGABLE)



                    flagInfo += "FLAG_DRAGABLE ";



                if (flags & FLAG_ATTACH)



                    flagInfo += "FLAG_ATTACH ";



                if (flags & FLAG_RESTRICT_X)



                    flagInfo += "FLAG_RESTRICT_X ";



                if (flags & FLAG_RESTRICT_Y)



                    flagInfo += "FLAG_RESTRICT_Y ";



                if (flags & FLAG_FLOAT)



                    flagInfo += "FLAG_FLOAT ";



                if (flags & FLAG_NOT_PICK)



                    flagInfo += "FLAG_NOT_PICK ";



                if (flags & FLAG_IGNORE_SIZE)



                    flagInfo += "FLAG_IGNORE_SIZE ";



                if (flags & FLAG_RTL)



                    flagInfo += "FLAG_RTL ";







                const auto parentName = m_pParent ? m_pParent->GetName() : "None";







                std::ostringstream ossInfo;



                ossInfo << "Pos: (" << m_rect.left << ", " << m_rect.top << "), "



                        << "Size: " << (m_rect.right - m_rect.left) << "x" << (m_rect.bottom - m_rect.top) << ", "



                        << "Flags: " << flagInfo << ", "



                        << "Parent: " << parentName;







                CPythonGraphic::Instance().RenderTextLine(m_rect.left + 5, m_rect.top - 15, ossInfo.str().c_str(), 0xFFFFFFFF);



            }



        }



#else



		if (g_bOutlineBoxEnable)



		{



			CPythonGraphic::Instance().SetDiffuseColor(1.0f, 1.0f, 1.0f);



			CPythonGraphic::Instance().RenderBox2d(m_rect.left, m_rect.top, m_rect.right, m_rect.bottom);



		}



#endif







		std::for_each(m_pChildList.begin(), m_pChildList.end(), std::mem_fn(&CWindow::Render));



#ifdef INSIDE_RENDER



		OnAfterRender();



#endif



	}







	void CWindow::OnUpdate()



	{



		if (!m_poHandler)



			return;







		if (!IsShow())



			return;







		static PyObject *poFuncName_OnUpdate = PyString_InternFromString("OnUpdate");







		PyCallClassMemberFunc_ByPyString(m_poHandler, poFuncName_OnUpdate, BuildEmptyTuple());



	}







	void CWindow::OnRender()



	{



		if (!m_poHandler)



			return;







		if (!IsShow())



			return;







		PyCallClassMemberFunc(m_poHandler, "OnRender", BuildEmptyTuple());



	}







#ifdef INSIDE_RENDER



	void CWindow::OnAfterRender()



	{



		if (!m_poHandler)



			return;



		if (!IsShow())



			return;



		PyCallClassMemberFunc(m_poHandler, "OnAfterRender", BuildEmptyTuple());



	}



#endif







	void CWindow::SetName(const char *c_szName)



	{



		m_strName = c_szName;



	}







	void CWindow::SetSize(long width, long height)



	{



		m_lWidth = width;



		m_lHeight = height;







		m_rect.right = m_rect.left + m_lWidth;



		m_rect.bottom = m_rect.top + m_lHeight;







#ifdef INSIDE_RENDER



		if (m_isInsideRender)



			UpdateRenderBoxRecursive();



#endif



	}







	void CWindow::SetHorizontalAlign(DWORD dwAlign)



	{



		m_HorizontalAlign = (EHorizontalAlign)dwAlign;



		UpdateRect();



#ifdef INSIDE_RENDER



		if (m_isInsideRender)



			UpdateRenderBoxRecursive();



#endif



	}







	void CWindow::SetVerticalAlign(DWORD dwAlign)



	{



		m_VerticalAlign = (EVerticalAlign)dwAlign;



		UpdateRect();



	}







	void CWindow::SetPosition(long x, long y)



	{



		m_x = x;



		m_y = y;







		UpdateRect();



#ifdef INSIDE_RENDER



		if (m_isInsideRender)



			UpdateRenderBoxRecursive();



#endif



	}







#ifdef INSIDE_RENDER



	void CWindow::UpdateRenderBoxRecursive()



	{



		UpdateRenderBox();



		for (auto it = m_pChildList.begin(); it != m_pChildList.end(); ++it)



			(*it)->UpdateRenderBoxRecursive();



	}



#endif







	void CWindow::GetPosition(long * plx, long * ply)



	{



		*plx = m_x;



		*ply = m_y;



	}







	long CWindow::UpdateRect()



	{



		m_rect.top = m_y;



		if (m_pParent)



		{



			switch (m_VerticalAlign)



			{



				case VERTICAL_ALIGN_BOTTOM:



					m_rect.top = m_pParent->GetHeight() - m_rect.top;



					break;



				case VERTICAL_ALIGN_CENTER:



					m_rect.top = (m_pParent->GetHeight() - GetHeight()) / 2 + m_rect.top;



					break;



			}



			m_rect.top += m_pParent->m_rect.top;



		}



		m_rect.bottom = m_rect.top + m_lHeight;







#if defined( _USE_CPP_RTL_FLIP )



		if( m_pParent == NULL )



		{



			m_rect.left = m_x;



			m_rect.right = m_rect.left + m_lWidth;



		}



		else



		{



			if( m_pParent->IsFlag(UI::CWindow::FLAG_RTL) == true )



			{



				m_rect.left = m_pParent->GetWidth() - m_lWidth - m_x;



				switch (m_HorizontalAlign)



				{



					case HORIZONTAL_ALIGN_RIGHT:



						m_rect.left = - m_x;



						break;



					case HORIZONTAL_ALIGN_CENTER:



						m_rect.left = m_pParent->GetWidth() / 2 - GetWidth() - m_x;



						break;



				}



				m_rect.left += m_pParent->m_rect.left;



				m_rect.right = m_rect.left + m_lWidth;



			}



			else



			{



				m_rect.left = m_x;



				switch (m_HorizontalAlign)



				{



					case HORIZONTAL_ALIGN_RIGHT:



						m_rect.left = m_pParent->GetWidth() - m_rect.left;



						break;



					case HORIZONTAL_ALIGN_CENTER:



						m_rect.left = (m_pParent->GetWidth() - GetWidth()) / 2 + m_rect.left;



						break;



				}



				m_rect.left += m_pParent->m_rect.left;



				m_rect.right = m_rect.left + m_lWidth;



			}



		}



#else



		m_rect.left = m_x;



		if (m_pParent)



		{



			switch (m_HorizontalAlign)



			{



				case HORIZONTAL_ALIGN_RIGHT:



					m_rect.left = ::abs(m_pParent->GetWidth()) - m_rect.left;



					break;



				case HORIZONTAL_ALIGN_CENTER:



					m_rect.left = m_pParent->GetWidth() / 2 - GetWidth() / 2 + m_rect.left;



					break;



			}



			m_rect.left += 0L < m_pParent->GetWidth() ? m_pParent->m_rect.left : m_pParent->m_rect.right + ::abs(m_pParent->GetWidth());



		}



		m_rect.right = m_rect.left + m_lWidth;



#endif



		std::for_each(m_pChildList.begin(), m_pChildList.end(), std::mem_fn(&CWindow::UpdateRect));







		OnChangePosition();







		return 1;



	}







	void CWindow::GetLocalPosition(long & rlx, long & rly)



	{



		rlx = rlx - m_rect.left;



		rly = rly - m_rect.top;



	}







	void CWindow::GetMouseLocalPosition(long & rlx, long & rly)



	{



		CWindowManager::Instance().GetMousePosition(rlx, rly);



		rlx = rlx - m_rect.left;



		rly = rly - m_rect.top;



	}







	void CWindow::AddChild(CWindow * pWin)



	{



		m_pChildList.push_back(pWin);



		pWin->m_pParent = this;



#ifdef INSIDE_RENDER



		if (m_isInsideRender && !pWin->m_isInsideRender)



			pWin->SetInsideRender(m_isInsideRender);



#endif



	}







#ifdef INSIDE_RENDER



	void CWindow::SetInsideRender(BOOL flag)



	{



		if (!m_pParent || m_isInsideRender && m_pParent->m_isInsideRender)



			return;



		if (m_isInsideRender && flag)



			return;



		m_isInsideRender = flag;



		UpdateRenderBox();



		for (auto it = m_pChildList.begin(); it != m_pChildList.end(); ++it)



			(*it)->SetInsideRender(m_isInsideRender);



	}



	void CWindow::GetRenderBox(RECT* box)



	{



		memcpy(box, &m_renderBox, sizeof(RECT));



	}



	void CWindow::UpdateTextLineRenderBox()



	{



		int width, height;



		((CTextLine*)this)->GetTextSize(&width, &height);







		int pWidth = m_pParent->GetWidth();



		int pHeight = m_pParent->GetHeight();







		if (m_x - m_pParent->m_renderBox.left < 0)



			m_renderBox.left = -m_x + m_pParent->m_renderBox.left;



		else



			m_renderBox.left = 0;







		if (m_y - m_pParent->m_renderBox.top < 0)



			m_renderBox.top = -m_y + m_pParent->m_renderBox.top;



		else



			m_renderBox.top = 0;







		if (m_x + width > pWidth - m_pParent->m_renderBox.right)



			m_renderBox.right = m_x + width - pWidth + m_pParent->m_renderBox.right;



		else



			m_renderBox.right = 0;







		if (m_y + height > pHeight - m_pParent->m_renderBox.bottom)



			m_renderBox.bottom = m_y + height - pHeight + m_pParent->m_renderBox.bottom;



		else



			m_renderBox.bottom = 0;



	}



	void CWindow::UpdateRenderBox()



	{



#ifdef __DUNGEON_INFO__



		if (std::string(GetName()) == "not_render_tooltip")



			return;



#endif







		if (!m_isInsideRender || !m_pParent)



			memset(&m_renderBox, 0, sizeof(m_renderBox));



		else



		{



			const int width = m_lWidth;



			const int height = m_lHeight;



			const int pWidth = m_pParent->GetWidth();



			const int pHeight = m_pParent->GetHeight();



			if (m_x - m_pParent->m_renderBox.left < 0)



				m_renderBox.left = -m_x + m_pParent->m_renderBox.left;



			else



				m_renderBox.left = 0;



			if (m_y - m_pParent->m_renderBox.top < 0)



				m_renderBox.top = -m_y + m_pParent->m_renderBox.top;



			else



				m_renderBox.top = 0;



			if (m_x + width > pWidth - m_pParent->m_renderBox.right)



				m_renderBox.right = m_x + width - pWidth + m_pParent->m_renderBox.right;



			else



				m_renderBox.right = 0;



			if (m_y + height > pHeight - m_pParent->m_renderBox.bottom)



				m_renderBox.bottom = m_y + height - pHeight + m_pParent->m_renderBox.bottom;



			else



				m_renderBox.bottom = 0;



		}



		OnUpdateRenderBox();



	}



#endif







	CWindow * CWindow::GetRoot()



	{



		if (m_pParent)



			if (m_pParent->IsWindow())



				return m_pParent->GetRoot();







		return this;



	}







	CWindow * CWindow::GetParent()



	{



		return m_pParent;



	}







#ifdef INSIDE_RENDER



	bool CWindow::IsChild(CWindow* pWin, bool bCheckRecursive)



#else



	bool CWindow::IsChild(CWindow * pWin)



#endif



	{



		std::list<CWindow *>::iterator itor = m_pChildList.begin();







		while (itor != m_pChildList.end())



		{



			if (*itor == pWin)



				return true;







#ifdef INSIDE_RENDER



			if (bCheckRecursive)



				if ((*itor)->IsChild(pWin, true))



					return true;



#endif







			++itor;



		}







		return false;



	}







	void CWindow::DeleteChild(CWindow * pWin)



	{



		if (m_isUpdatingChildren)



		{



			m_pReserveChildList.push_back(pWin);



		}



		else



		{



			m_pChildList.remove(pWin);



		}



	}







	void CWindow::SetTop(CWindow * pWin)



	{



		if (!pWin->IsFlag(CWindow::FLAG_FLOAT))



			return;







		TWindowContainer::iterator itor = std::find(m_pChildList.begin(), m_pChildList.end(), pWin);



		if (m_pChildList.end() != itor)



		{



			m_pChildList.push_back(*itor);



			m_pChildList.erase(itor);







			pWin->OnTop();



		}



		else



		{



			TraceError(" CWindow::SetTop - Failed to find child window\n");



		}



	}







	void CWindow::OnMouseDrag(long lx, long ly)



	{



		PyCallClassMemberFunc(m_poHandler, "OnMouseDrag", Py_BuildValue("(ii)", lx, ly));



	}







	void CWindow::OnMoveWindow(long lx, long ly)



	{



		PyCallClassMemberFunc(m_poHandler, "OnMoveWindow", Py_BuildValue("(ii)", lx, ly));



	}







	void CWindow::OnSetFocus()



	{



		//PyCallClassMemberFunc(m_poHandler, "OnSetFocus", BuildEmptyTuple());



		PyCallClassMemberFunc(m_poHandler, "OnSetFocus", BuildEmptyTuple());



	}







	void CWindow::OnKillFocus()



	{



		PyCallClassMemberFunc(m_poHandler, "OnKillFocus", BuildEmptyTuple());



	}







	void CWindow::OnMouseOverIn()



	{



		PyCallClassMemberFunc(m_poHandler, "OnMouseOverIn", BuildEmptyTuple());



	}







	void CWindow::OnMouseOverOut()



	{



		PyCallClassMemberFunc(m_poHandler, "OnMouseOverOut", BuildEmptyTuple());



	}







	void CWindow::OnMouseOver()



	{



	}







	void CWindow::OnDrop()



	{



		PyCallClassMemberFunc(m_poHandler, "OnDrop", BuildEmptyTuple());



	}







	void CWindow::OnTop()



	{



		PyCallClassMemberFunc(m_poHandler, "OnTop", BuildEmptyTuple());



	}







	void CWindow::OnIMEUpdate()



	{



		PyCallClassMemberFunc(m_poHandler, "OnIMEUpdate", BuildEmptyTuple());



	}

#ifdef ENABLE_MULTILANGUAGE_CHATTING
	DWORD CWindow::GetInputSystemLangID() const
	{
		HKL hKeyboardLayout = GetKeyboardLayout(0);
		return PRIMARYLANGID(LOWORD(hKeyboardLayout));
	}
#endif







#ifdef ENABLE_CLIP_MASKING



	void CWindow::SetClippingMaskRect(const RECT& rMask)



	{



		m_bEnableMask = true;



		m_rMaskRect = rMask;







		TWindowContainer::const_reverse_iterator itor;



		for (itor = m_pChildList.rbegin(); itor != m_pChildList.rend(); ++itor)



		{



			CWindow* pWindow = *itor;



			if (pWindow)



			{



				pWindow->SetClippingMaskRect(rMask);



			}



		}



	}







	void CWindow::SetClippingMaskWindow(CWindow* pMaskWindow)



	{



		if (!pMaskWindow)



		{



			return;



		}







		m_bEnableMask = true;



		m_pMaskWindow = pMaskWindow;







		TWindowContainer::const_reverse_iterator itor;



		for (itor = m_pChildList.rbegin(); itor != m_pChildList.rend(); ++itor)



		{



			CWindow* pWindow = *itor;



			if (pWindow)



			{



				pWindow->SetClippingMaskWindow(pMaskWindow);



			}



		}



	}



#endif







	BOOL CWindow::RunIMETabEvent()



	{



		if (!IsRendering())



			return FALSE;







		if (OnIMETabEvent())



			return TRUE;







		TWindowContainer::reverse_iterator itor;



		for (itor = m_pChildList.rbegin(); itor != m_pChildList.rend(); ++itor)



		{



			CWindow * pWindow = *itor;







			if (pWindow->RunIMETabEvent())



				return TRUE;



		}







		return FALSE;



	}







	BOOL CWindow::RunIMEReturnEvent()



	{



		if (!IsRendering())



			return FALSE;







		if (OnIMEReturnEvent())



			return TRUE;







		TWindowContainer::reverse_iterator itor;



		for (itor = m_pChildList.rbegin(); itor != m_pChildList.rend(); ++itor)



		{



			CWindow * pWindow = *itor;







			if (pWindow->RunIMEReturnEvent())



				return TRUE;



		}







		return FALSE;



	}







	BOOL CWindow::RunIMEKeyDownEvent(int ikey)



	{



		if (!IsRendering())



			return FALSE;







		if (OnIMEKeyDownEvent(ikey))



			return TRUE;







		TWindowContainer::reverse_iterator itor;



		for (itor = m_pChildList.rbegin(); itor != m_pChildList.rend(); ++itor)



		{



			CWindow * pWindow = *itor;







			if (pWindow->RunIMEKeyDownEvent(ikey))



				return TRUE;



		}







		return FALSE;



	}







	CWindow * CWindow::RunKeyDownEvent(int ikey)



	{



		if (OnKeyDown(ikey))



			return this;







		TWindowContainer::reverse_iterator itor;



		for (itor = m_pChildList.rbegin(); itor != m_pChildList.rend(); ++itor)



		{



			CWindow * pWindow = *itor;







			if (pWindow->IsShow())



			{



				CWindow * pProcessedWindow = pWindow->RunKeyDownEvent(ikey);



				if (NULL != pProcessedWindow)



				{



					return pProcessedWindow;



				}



			}



		}







		return NULL;



	}







	BOOL CWindow::RunKeyUpEvent(int ikey)



	{



		if (OnKeyUp(ikey))



			return TRUE;







		TWindowContainer::reverse_iterator itor;



		for (itor = m_pChildList.rbegin(); itor != m_pChildList.rend(); ++itor)



		{



			CWindow * pWindow = *itor;







			if (pWindow->IsShow())



			if (pWindow->RunKeyUpEvent(ikey))



				return TRUE;



		}







		return FALSE;



	}







	BOOL CWindow::RunPressEscapeKeyEvent()



	{



		TWindowContainer::reverse_iterator itor;



		for (itor = m_pChildList.rbegin(); itor != m_pChildList.rend(); ++itor)



		{



			CWindow * pWindow = *itor;







			if (pWindow->IsShow())



			if (pWindow->RunPressEscapeKeyEvent())



				return TRUE;



		}







		if (OnPressEscapeKey())



			return TRUE;







		return FALSE;



	}







	BOOL CWindow::RunPressExitKeyEvent()



	{



		TWindowContainer::reverse_iterator itor;



		for (itor = m_pChildList.rbegin(); itor != m_pChildList.rend(); ++itor)



		{



			CWindow * pWindow = *itor;







			if (pWindow->RunPressExitKeyEvent())



				return TRUE;







			if (pWindow->IsShow())



			if (pWindow->OnPressExitKey())



				return TRUE;



		}







		return FALSE;



	}







	BOOL CWindow::OnMouseLeftButtonDown()



	{



		long lValue;



		if (PyCallClassMemberFunc(m_poHandler, "OnMouseLeftButtonDown", BuildEmptyTuple(), &lValue))



		if (0 != lValue)



			return TRUE;







		return FALSE;



	}







	BOOL CWindow::OnMouseLeftButtonUp()



	{



		PyCallClassMemberFunc(m_poHandler, "OnMouseLeftButtonUp", BuildEmptyTuple());



		return TRUE; // NOTE : ButtonUp?¨¬ ¢¯©ö¢¯?¡¤? ©ö??¢Ò¡Æ? TRUE



	}







	BOOL CWindow::OnMouseLeftButtonDoubleClick()



	{



		long lValue;



		if (PyCallClassMemberFunc(m_poHandler, "OnMouseLeftButtonDoubleClick", BuildEmptyTuple(), &lValue))



		if (0 != lValue)



			return TRUE;







		return FALSE;



	}







	BOOL CWindow::OnMouseRightButtonDown()



	{



		long lValue;



		if (PyCallClassMemberFunc(m_poHandler, "OnMouseRightButtonDown", BuildEmptyTuple(), &lValue))



		if (0 != lValue)



			return TRUE;







		return FALSE;



	}







	BOOL CWindow::OnMouseRightButtonUp()



	{



		long lValue;



		if (PyCallClassMemberFunc(m_poHandler, "OnMouseRightButtonUp", BuildEmptyTuple(), &lValue))



		if (0 != lValue)



			return TRUE;







		return FALSE;



	}







	BOOL CWindow::OnMouseRightButtonDoubleClick()



	{



		long lValue;



		if (PyCallClassMemberFunc(m_poHandler, "OnMouseRightButtonDoubleClick", BuildEmptyTuple(), &lValue))



		if (0 != lValue)



			return TRUE;







		return FALSE;



	}







	BOOL CWindow::OnMouseMiddleButtonDown()



	{



		long lValue;



		if (PyCallClassMemberFunc(m_poHandler, "OnMouseMiddleButtonDown", BuildEmptyTuple(), &lValue))



		if (0 != lValue)



			return TRUE;







		return FALSE;



	}







	BOOL CWindow::OnMouseMiddleButtonUp()



	{



		long lValue;



		if (PyCallClassMemberFunc(m_poHandler, "OnMouseMiddleButtonUp", BuildEmptyTuple(), &lValue))



		if (0 != lValue)



			return TRUE;







		return FALSE;



	}







	BOOL CWindow::RunMouseWheelEvent(long nLen)



	{



		bool bValue = false;



		if (PyCallClassMemberFunc(m_poHandler, "OnRunMouseWheel", Py_BuildValue("(l)", nLen), &bValue))



			return bValue;



		return bValue;



	}







	BOOL CWindow::OnIMETabEvent()



	{



		long lValue;



		if (PyCallClassMemberFunc(m_poHandler, "OnIMETab", BuildEmptyTuple(), &lValue))



		if (0 != lValue)



			return TRUE;







		return FALSE;



	}







	BOOL CWindow::OnIMEReturnEvent()



	{



		long lValue;



		if (PyCallClassMemberFunc(m_poHandler, "OnIMEReturn", BuildEmptyTuple(), &lValue))



		if (0 != lValue)



			return TRUE;







		return FALSE;



	}







	BOOL CWindow::OnIMEKeyDownEvent(int ikey)



	{



		long lValue;



		if (PyCallClassMemberFunc(m_poHandler, "OnIMEKeyDown", Py_BuildValue("(i)", ikey), &lValue))



		if (0 != lValue)



			return TRUE;







		return FALSE;



	}







	BOOL CWindow::OnIMEChangeCodePage()



	{



		long lValue;



		if (PyCallClassMemberFunc(m_poHandler, "OnIMEChangeCodePage", BuildEmptyTuple(), &lValue))



		if (0 != lValue)



			return TRUE;







		return FALSE;



	}







	BOOL CWindow::OnIMEOpenCandidateListEvent()



	{



		long lValue;



		if (PyCallClassMemberFunc(m_poHandler, "OnIMEOpenCandidateList", BuildEmptyTuple(), &lValue))



		if (0 != lValue)



			return TRUE;







		return FALSE;



	}







	BOOL CWindow::OnIMECloseCandidateListEvent()



	{



		long lValue;



		if (PyCallClassMemberFunc(m_poHandler, "OnIMECloseCandidateList", BuildEmptyTuple(), &lValue))



		if (0 != lValue)



			return TRUE;







		return FALSE;



	}







	BOOL CWindow::OnIMEOpenReadingWndEvent()



	{



		long lValue;



		if (PyCallClassMemberFunc(m_poHandler, "OnIMEOpenReadingWnd", BuildEmptyTuple(), &lValue))



		if (0 != lValue)



			return TRUE;







		return FALSE;



	}







	BOOL CWindow::OnIMECloseReadingWndEvent()



	{



		long lValue;



		if (PyCallClassMemberFunc(m_poHandler, "OnIMECloseReadingWnd", BuildEmptyTuple(), &lValue))



		if (0 != lValue)



			return TRUE;







		return FALSE;



	}







	BOOL CWindow::OnKeyDown(int ikey)



	{



		long lValue;



		if (PyCallClassMemberFunc(m_poHandler, "OnKeyDown", Py_BuildValue("(i)", ikey), &lValue))



		if (0 != lValue)



			return TRUE;







		return FALSE;



	}







	BOOL CWindow::OnKeyUp(int ikey)



	{



		long lValue;



		if (PyCallClassMemberFunc(m_poHandler, "OnKeyUp", Py_BuildValue("(i)", ikey), &lValue))



		if (0 != lValue)



			return TRUE;







		return FALSE;



	}







	BOOL CWindow::OnPressEscapeKey()



	{



		long lValue;



		if (PyCallClassMemberFunc(m_poHandler, "OnPressEscapeKey", BuildEmptyTuple(), &lValue))



		if (0 != lValue)



			return TRUE;







		return FALSE;



	}







	BOOL CWindow::OnPressExitKey()



	{



		long lValue;



		if (PyCallClassMemberFunc(m_poHandler, "OnPressExitKey", BuildEmptyTuple(), &lValue))



		if (0 != lValue)



			return TRUE;







		return FALSE;



	}







	/////







	bool CWindow::IsIn(long x, long y)



	{



		if (x >= m_rect.left && x <= m_rect.right)



			if (y >= m_rect.top && y <= m_rect.bottom)



				return true;







		return false;



	}







	bool CWindow::IsIn()



	{



		long lx, ly;



		UI::CWindowManager::Instance().GetMousePosition(lx, ly);







		return IsIn(lx, ly);



	}







	CWindow * CWindow::PickWindow(long x, long y)



	{



		std::list<CWindow *>::reverse_iterator ritor = m_pChildList.rbegin();



		for (; ritor != m_pChildList.rend(); ++ritor)



		{



			CWindow * pWin = *ritor;



			if (pWin->IsShow())



			{



				if (!pWin->IsFlag(CWindow::FLAG_IGNORE_SIZE))



				{



					if (!pWin->IsIn(x, y)) {



						if (0L <= pWin->GetWidth()) {



							continue;



						}



					}



				}







				CWindow * pResult = pWin->PickWindow(x, y);



				if (pResult)



					return pResult;



			}



		}







		if (IsFlag(CWindow::FLAG_NOT_PICK))



			return NULL;







		if (IsFlag(CWindow::FLAG_IS_BOARD_WITHOUT_ALPHA))



		{



			return NULL;



		}







		if (IsFlag(CWindow::FLAG_ALPHA_SENSITIVE))



		{



			bool isFullTransparent = true;



			IsTransparentOnPixel(&x, &y, &isFullTransparent);



			if (isFullTransparent)



				return NULL;



		}







		return (this);



	}







	void CWindow::IsTransparentOnPixel(long* x, long* y, bool *ret)



	{



		if (IsShow() && IsIn(*x, *y))



		{



			if (m_windowType == WINDOW_TYPE_EX_IMAGE)



			{



				D3DXCOLOR pixel = ((CExpandedImageBox*)this)->GetPixelColor(*x - m_rect.left, *y - m_rect.top);







				if ((BYTE)pixel.a != 0)



				{



					*ret = false;



					return;



				}



			}



			else if (m_pChildList.empty())



			{



				*ret = false;



				return;



			}



		}



		if (!m_pChildList.empty())



		{



			std::list<CWindow*>::reverse_iterator ritor = m_pChildList.rbegin();



			for (; ritor != m_pChildList.rend(); ritor++)



			{



				(*ritor)->IsTransparentOnPixel(x, y, ret);



				if (!*ret)



					return;



			}



		}



	}







	void CWindow::HaveSomeChildOnOnPixel(long* x, long* y, bool *ret)



	{



		if (IsShow() && IsIn(*x, *y))



		{



			if (m_windowType == WINDOW_TYPE_EX_IMAGE)



			{



				D3DXCOLOR pixel = ((CExpandedImageBox*)this)->GetPixelColor(*x - m_rect.left, *y - m_rect.top);







				if ((BYTE)pixel.a != 0)



				{



					*ret = false;



					return;



				}



			}



			else if (m_pChildList.empty())



			{



				*ret = false;



				return;



			}



		}



		if (!m_pChildList.empty())



		{



			std::list<CWindow*>::reverse_iterator ritor = m_pChildList.rbegin();



			for (; ritor != m_pChildList.rend(); ritor++)



			{



				(*ritor)->IsTransparentOnPixel(x, y, ret);



				if (!*ret)



					return;



			}



		}



	}



	CWindow * CWindow::PickTopWindow(long x, long y)



	{



		std::list<CWindow *>::reverse_iterator ritor = m_pChildList.rbegin();



		for (; ritor != m_pChildList.rend(); ++ritor)



		{



			CWindow * pWin = *ritor;



			if (pWin->IsShow())



				if (pWin->IsIn(x, y))



					if (!pWin->IsFlag(CWindow::FLAG_NOT_PICK))



						return pWin;



		}







		return NULL;



	}







#ifdef ENABLE_RENDER_TARGET



	CRenderTarget::CRenderTarget(PyObject *ppyObject) : CWindow(ppyObject)



	{



		m_dwIndex = -1;



#ifdef ENABLE_WIKI_SYSTEM



		memset(&rect_ex, 0, sizeof(rect_ex));



#endif



	}







	CRenderTarget::~CRenderTarget() = default;







	bool CRenderTarget::SetRenderTarget(int index)



	{



		if (!CRenderTargetManager::Instance().GetRenderTarget(index))



		{



			if (!CRenderTargetManager::Instance().CreateRenderTarget(index, GetWidth(), GetHeight()))



			{



				TraceError("CRenderTargetManager could not create the texture. w %d h %d", GetWidth(), GetHeight());



				return false;



			}



		}



		m_dwIndex = index;







		UpdateRect();



		return true;



	}







#ifdef ENABLE_WIKI_SYSTEM



	void CRenderTarget::SetRenderingRect(float fLeft, float fTop, float fRight, float fBottom)



	{



		memset(&rect_ex, 0, sizeof(rect_ex));



		float fWidth = float(GetWidth());



		float fHeight = float(GetHeight());







		rect_ex.left = fWidth * fLeft;



		rect_ex.top = fHeight * fTop;



		rect_ex.right = fWidth * fRight;



		rect_ex.bottom = fHeight * fBottom;



	}



#endif







	void CRenderTarget::OnRender()



	{



		auto target = CRenderTargetManager::Instance().GetRenderTarget(m_dwIndex);



		if (!target)



		{



			TraceError("SetRenderingRect -> target empty!");



			return;



		}







#ifdef ENABLE_WIKI_SYSTEM



		RECT rect_real;



		rect_real.left = m_rect.left - rect_ex.left;



		rect_real.top = m_rect.top - rect_ex.top;



		rect_real.right = m_rect.right + rect_ex.right;



		rect_real.bottom = m_rect.bottom + rect_ex.bottom;



		target->SetRenderingRect(&rect_real);



#else



		target->SetRenderingRect(&m_rect);



#endif



		target->RenderTexture();



	}







#ifdef INSIDE_RENDER



	void CWindow::iSetRenderingRect(int iLeft, int iTop, int iRight, int iBottom)



	{



		m_renderingRect.left = iLeft;



		m_renderingRect.top = iTop;



		m_renderingRect.right = iRight;



		m_renderingRect.bottom = iBottom;



		OnSetRenderingRect();



	}



	void CWindow::SetRenderingRect(float fLeft, float fTop, float fRight, float fBottom)



	{



		float fWidth = float(GetWidth());



		float fHeight = float(GetHeight());



		if (IsType(CTextLine::Type()))



		{



			int iWidth, iHeight;



			((CTextLine*)this)->GetTextSize(&iWidth, &iHeight);



			fWidth = float(iWidth);



			fHeight = float(iHeight);



		}



		iSetRenderingRect(fWidth * fLeft, fHeight * fTop, fWidth * fRight, fHeight * fBottom);



	}



	int CWindow::GetRenderingWidth()



	{



		return max(0, GetWidth() + m_renderingRect.right + m_renderingRect.left);



	}



	int CWindow::GetRenderingHeight()



	{



		return max(0, GetHeight() + m_renderingRect.bottom + m_renderingRect.top);



	}



	void CWindow::ResetRenderingRect(bool bCallEvent)



	{



		m_renderingRect.bottom = m_renderingRect.left = m_renderingRect.right = m_renderingRect.top = 0;



		if (bCallEvent)



			OnSetRenderingRect();



	}



	void CWindow::OnSetRenderingRect() {}



#endif







#endif







	CBox::CBox(PyObject *ppyObject) : CWindow(ppyObject), m_dwColor(0xff000000)



	{



	}



	CBox::~CBox()



	{



	}







	void CBox::SetColor(DWORD dwColor)



	{



		m_dwColor = dwColor;



	}







	void CBox::OnRender()



	{



		CPythonGraphic::Instance().SetDiffuseColor(m_dwColor);



#ifdef INSIDE_RENDER



		CPythonGraphic::Instance().RenderBar2d(m_rect.left + m_renderBox.left, m_rect.top + m_renderBox.top, m_rect.right - m_renderBox.right, m_rect.bottom - m_renderBox.bottom);



#else



		CPythonGraphic::Instance().RenderBox2d(m_rect.left, m_rect.top, m_rect.right, m_rect.bottom);



#endif



	}







	///////////////////////////////////////////////////////////////////////////////////////////////



	///////////////////////////////////////////////////////////////////////////////////////////////



	///////////////////////////////////////////////////////////////////////////////////////////////







	CBar::CBar(PyObject * ppyObject) : CWindow(ppyObject), m_dwColor(0xff000000)



	{



	}



	CBar::~CBar()



	{



	}







	void CBar::SetColor(DWORD dwColor)



	{



		m_dwColor = dwColor;



	}







	void CBar::OnRender()



	{



		CPythonGraphic::Instance().SetDiffuseColor(m_dwColor);



		CPythonGraphic::Instance().RenderBar2d(m_rect.left, m_rect.top, m_rect.right, m_rect.bottom);



	}







	///////////////////////////////////////////////////////////////////////////////////////////////



	///////////////////////////////////////////////////////////////////////////////////////////////



	///////////////////////////////////////////////////////////////////////////////////////////////







	CLine::CLine(PyObject * ppyObject) : CWindow(ppyObject), m_dwColor(0xff000000)



	{



	}



	CLine::~CLine()



	{



	}







	void CLine::SetColor(DWORD dwColor)



	{



		m_dwColor = dwColor;



	}







	void CLine::OnRender()



	{



		CPythonGraphic & rkpyGraphic = CPythonGraphic::Instance();



		rkpyGraphic.SetDiffuseColor(m_dwColor);



		rkpyGraphic.RenderLine2d(m_rect.left, m_rect.top, m_rect.right, m_rect.bottom);



	}







	///////////////////////////////////////////////////////////////////////////////////////////////



	///////////////////////////////////////////////////////////////////////////////////////////////



	///////////////////////////////////////////////////////////////////////////////////////////////







	DWORD CBar3D::Type()



	{



		static DWORD s_dwType = GetCRC32("CBar3D", strlen("CBar3D"));



		return (s_dwType);



	}







	CBar3D::CBar3D(PyObject * ppyObject) : CWindow(ppyObject)



	{



		m_dwLeftColor = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);



		m_dwRightColor = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.0f);



		m_dwCenterColor = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);



	}



	CBar3D::~CBar3D()



	{



	}







	void CBar3D::SetColor(DWORD dwLeft, DWORD dwRight, DWORD dwCenter)



	{



		m_dwLeftColor = dwLeft;



		m_dwRightColor = dwRight;



		m_dwCenterColor = dwCenter;



	}







	void CBar3D::OnRender()



	{



		CPythonGraphic & rkpyGraphic = CPythonGraphic::Instance();







		rkpyGraphic.SetDiffuseColor(m_dwCenterColor);



		rkpyGraphic.RenderBar2d(m_rect.left, m_rect.top, m_rect.right, m_rect.bottom);







		rkpyGraphic.SetDiffuseColor(m_dwLeftColor);



		rkpyGraphic.RenderLine2d(m_rect.left, m_rect.top, m_rect.right, m_rect.top);



		rkpyGraphic.RenderLine2d(m_rect.left, m_rect.top, m_rect.left, m_rect.bottom);







		rkpyGraphic.SetDiffuseColor(m_dwRightColor);



		rkpyGraphic.RenderLine2d(m_rect.left, m_rect.bottom, m_rect.right, m_rect.bottom);



		rkpyGraphic.RenderLine2d(m_rect.right, m_rect.top, m_rect.right, m_rect.bottom);



	}







	///////////////////////////////////////////////////////////////////////////////////////////////



	///////////////////////////////////////////////////////////////////////////////////////////////



	///////////////////////////////////////////////////////////////////////////////////////////////







	CTextLine::CTextLine(PyObject * ppyObject) : CWindow(ppyObject)



	{



		m_TextInstance.SetColor(0.78f, 0.78f, 0.78f);



		m_TextInstance.SetHorizonalAlign(CGraphicTextInstance::HORIZONTAL_ALIGN_LEFT);



		m_TextInstance.SetVerticalAlign(CGraphicTextInstance::VERTICAL_ALIGN_TOP);



	}



	CTextLine::~CTextLine()



	{



		m_TextInstance.Destroy();



	}







#ifdef INSIDE_RENDER



	DWORD CTextLine::Type()



	{



		static DWORD s_dwType = GetCRC32("CTextLine", strlen("CTextLine"));



		return (s_dwType);



	}



#endif







	void CTextLine::SetMax(int iMax)



	{



		m_TextInstance.SetMax(iMax);



	}



	void CTextLine::SetHorizontalAlign(int iType)



	{



		m_TextInstance.SetHorizonalAlign(iType);



	}



	void CTextLine::SetVerticalAlign(int iType)



	{



		m_TextInstance.SetVerticalAlign(iType);



	}



	void CTextLine::SetSecret(BOOL bFlag)



	{



		m_TextInstance.SetSecret(bFlag ? true : false);



	}



	void CTextLine::SetOutline(BOOL bFlag)



	{



		m_TextInstance.SetOutline(bFlag ? true : false);



	}



	void CTextLine::SetFeather(BOOL bFlag)



	{



		m_TextInstance.SetFeather(bFlag ? true : false);



	}



	void CTextLine::SetMultiLine(BOOL bFlag)



	{



		m_TextInstance.SetMultiLine(bFlag ? true : false);



	}



	void CTextLine::SetFontName(const char * c_szFontName)



	{



		std::string stFontName = c_szFontName;



		stFontName += ".fnt";







		CResourceManager& rkResMgr=CResourceManager::Instance();



		CResource* pkRes = rkResMgr.GetTypeResourcePointer(stFontName.c_str());



		CGraphicText* pkResFont=static_cast<CGraphicText*>(pkRes);



		m_TextInstance.SetTextPointer(pkResFont);



	}



	void CTextLine::SetFontColor(DWORD dwColor)



	{



		m_TextInstance.SetColor(dwColor);



	}



	void CTextLine::SetLimitWidth(float fWidth)



	{



		m_TextInstance.SetLimitWidth(fWidth);



	}



	void CTextLine::SetText(const char * c_szText)



	{



		OnSetText(c_szText);



	}

#ifdef ENABLE_MULTILANGUAGE_CHATTING
	void CTextLine::SetFontColorRange(DWORD dwColor, int iStart, int iEnd)
	{
		m_TextInstance.SetColorRange(dwColor, iStart, iEnd);
	}

	void CTextLine::SetTextWithCodePage(const char * c_szText, DWORD dwCodePage)
	{
		m_TextInstance.SetValue(c_szText, &dwCodePage);
		m_TextInstance.Update();
	}

	void CTextLine::SetText(const char * c_szText, const DWORD * c_pdwLangID)
	{
		if (c_pdwLangID)
			m_TextInstance.SetLangID(*c_pdwLangID);
		else
			m_TextInstance.SetLangID(0);
		SetText(c_szText);
	}
#endif



	void CTextLine::GetTextSize(int* pnWidth, int* pnHeight)



	{



		m_TextInstance.GetTextSize(pnWidth, pnHeight);



	}



#ifdef ENABLE_MULTI_TEXTLINE



	int CTextLine::GetTextLineCount()



	{



		return m_TextInstance.GetTextLineCount();



	}



	void CTextLine::DisableEnterToken()



	{



		m_TextInstance.DisableEnterToken();



	}



	void CTextLine::SetLineHeight(int iHeight)



	{



		m_TextInstance.SetLineHeight(iHeight);



	}



	int CTextLine::GetLineHeight()



	{



		return m_TextInstance.GetLineHeight();



	}



#endif



#ifdef ENABLE_SUNG_MAHI_TOWER



	void CTextLine::GetCharSize(short* sWidth)



	{



		m_TextInstance.GetCharacterWidth(sWidth);



	}



#endif



	const char * CTextLine::GetText()



	{



		return m_TextInstance.GetValueStringReference().c_str();



	}



	void CTextLine::ShowCursor()



	{



		m_TextInstance.ShowCursor();



	}



	void CTextLine::HideCursor()



	{



		m_TextInstance.HideCursor();



	}



#ifdef INSIDE_RENDER



	bool CTextLine::IsShowCursor()



	{



		return m_TextInstance.IsShowCursor();



	}



#endif



	int CTextLine::GetCursorPosition()



	{



		long lx, ly;



		CWindow::GetMouseLocalPosition(lx, ly);



		return m_TextInstance.PixelPositionToCharacterPosition(lx);



	}







	void CTextLine::OnSetText(const char * c_szText)



	{



		m_TextInstance.SetValue(c_szText);



		m_TextInstance.Update();



#ifdef INSIDE_RENDER



		if (m_isInsideRender)



			UpdateRenderBoxRecursive();



#endif



	}







    void CTextLine::OnUpdate()



    {



        if (IsShow())



        {



#ifdef ENABLE_CLIP_MASKING



            if (m_bEnableMask && m_pMaskWindow)



            {



                m_rMaskRect = m_pMaskWindow->GetRect();



            }



#endif







            m_TextInstance.Update();



        }



    }







    void CTextLine::OnRender()



    {



        if (IsShow())



        {



#ifdef ENABLE_CLIP_MASKING



            m_TextInstance.Render(m_bEnableMask ? &m_rMaskRect : nullptr);



#else



            m_TextInstance.Render();



#endif



        }



    }







#ifdef INSIDE_RENDER



	bool CTextLine::IsShow()



	{



		if (!m_bShow)



			return false;



		if (m_isInsideRender)



		{



			int cW, cH;



			GetTextSize(&cW, &cH);



			if (m_renderBox.left + m_renderBox.right >= cW || m_renderBox.top + m_renderBox.bottom >= cH)



				return false;



		}



		return true;



	}







	int CTextLine::GetRenderingWidth()



	{



		int iTextWidth, iTextHeight;



		GetTextSize(&iTextWidth, &iTextHeight);



		return iTextWidth + m_renderingRect.right + m_renderingRect.left;



	}



	int CTextLine::GetRenderingHeight()



	{



		int iTextWidth, iTextHeight;



		GetTextSize(&iTextWidth, &iTextHeight);



		return iTextHeight + m_renderingRect.bottom + m_renderingRect.top;



	}



	void CTextLine::OnSetRenderingRect()



	{



		int iTextWidth, iTextHeight;



		GetTextSize(&iTextWidth, &iTextHeight);



		m_TextInstance.iSetRenderingRect(m_renderingRect.left, -m_renderingRect.top, m_renderingRect.right, m_renderingRect.bottom);



	}



#endif







	void CTextLine::OnChangePosition()



	{



		// FOR_ARABIC_ALIGN



		//if (m_TextInstance.GetHorizontalAlign() == CGraphicTextInstance::HORIZONTAL_ALIGN_ARABIC)



		if( GetDefaultCodePage() == CP_ARABIC )



		{



			m_TextInstance.SetPosition(m_rect.right, m_rect.top);



		}



		else



		{



			m_TextInstance.SetPosition(m_rect.left, m_rect.top);



		}



	}







	CNumberLine::CNumberLine(PyObject * ppyObject) : CWindow(ppyObject)
	{
		m_strPath = "d:/ymir work/ui/game/taskbar/";
		m_iHorizontalAlign = HORIZONTAL_ALIGN_LEFT;
#ifdef ENABLE_SLOT_CHAR_ENLARGEMENT
		m_dwWidthSummary[0] = 0;
		m_dwWidthSummary[1] = 0;
		m_bCanEnlarge = false;
		m_bIsEnlarged = false;
#else
		m_dwWidthSummary = 0;
#endif
	}

	CNumberLine::CNumberLine(CWindow * pParent) : CWindow(nullptr)
	{
		m_strPath = "d:/ymir work/ui/game/taskbar/";
		m_iHorizontalAlign = HORIZONTAL_ALIGN_LEFT;
#ifdef ENABLE_SLOT_CHAR_ENLARGEMENT
		m_dwWidthSummary[0] = 0;
		m_dwWidthSummary[1] = 0;
		m_bCanEnlarge = false;
		m_bIsEnlarged = false;
#else
		m_dwWidthSummary = 0;
#endif
		m_pParent = pParent;
	}

	CNumberLine::~CNumberLine()
	{
		ClearNumber();
	}

	void CNumberLine::SetPath(const char * c_szPath)



	{



		m_strPath = c_szPath;



	}



	void CNumberLine::SetHorizontalAlign(int iType)



	{



		m_iHorizontalAlign = iType;



	}



	void CNumberLine::SetNumber(const char * c_szNumber)
	{
		if (0 == m_strNumber.compare(c_szNumber))
			return;

		ClearNumber();

		m_strNumber = c_szNumber;

#ifdef ENABLE_SLOT_CHAR_ENLARGEMENT
		for (int mode = 0; mode < 2; ++mode)
		{
			for (DWORD i = 0; i < m_strNumber.size(); ++i)
			{
				char cChar = m_strNumber[i];
				std::string strImageFileName;

				if (':' == cChar)
					strImageFileName = m_strPath + (mode == 1 ? "big_colon.sub" : "colon.sub");
				else if ('?' == cChar)
					strImageFileName = m_strPath + (mode == 1 ? "big_questionmark.sub" : "questionmark.sub");
				else if ('/' == cChar)
					strImageFileName = m_strPath + (mode == 1 ? "big_slash.sub" : "slash.sub");
				else if ('%' == cChar)
					strImageFileName = m_strPath + (mode == 1 ? "big_percent.sub" : "percent.sub");
				else if ('+' == cChar)
					strImageFileName = m_strPath + (mode == 1 ? "big_plus.sub" : "plus.sub");
				else if ('m' == cChar)
					strImageFileName = m_strPath + (mode == 1 ? "big_m.sub" : "m.sub");
				else if ('g' == cChar)
					strImageFileName = m_strPath + (mode == 1 ? "big_g.sub" : "g.sub");
				else if ('p' == cChar)
					strImageFileName = m_strPath + (mode == 1 ? "big_p.sub" : "p.sub");
				else if (cChar >= '0' && cChar <= '9')
				{
					strImageFileName = m_strPath;
					if (mode == 1)
						strImageFileName += "big_";
					strImageFileName += cChar;
					strImageFileName += ".sub";
				}
				else
					continue;

				if (!CResourceManager::Instance().IsFileExist(strImageFileName.c_str()))
					continue;

				CGraphicImage * pImage = (CGraphicImage *)CResourceManager::Instance().GetResourcePointer(strImageFileName.c_str());
				if (pImage)
				{
					CGraphicImageInstance * pInstance = CGraphicImageInstance::New();
					pInstance->SetImagePointer(pImage);
					m_ImageInstanceVector[mode].push_back(pInstance);
					m_dwWidthSummary[mode] += pInstance->GetWidth();
				}
			}
		}
#else
		for (DWORD i = 0; i < m_strNumber.size(); ++i)
		{
			char cChar = m_strNumber[i];
			std::string strImageFileName;

			if (':' == cChar)
				strImageFileName = m_strPath + "colon.sub";
			else if ('?' == cChar)
				strImageFileName = m_strPath + "questionmark.sub";
			else if ('/' == cChar)
				strImageFileName = m_strPath + "slash.sub";
			else if ('%' == cChar)
				strImageFileName = m_strPath + "percent.sub";
			else if ('+' == cChar)
				strImageFileName = m_strPath + "plus.sub";
			else if ('m' == cChar)
				strImageFileName = m_strPath + "m.sub";
			else if ('g' == cChar)
				strImageFileName = m_strPath + "g.sub";
			else if ('p' == cChar)
				strImageFileName = m_strPath + "p.sub";
			else if (cChar >= '0' && cChar <= '9')
				{
				strImageFileName = m_strPath;
				strImageFileName += cChar;
				strImageFileName += ".sub";
			}
			else
				continue;

			if (!CResourceManager::Instance().IsFileExist(strImageFileName.c_str()))
				continue;

			CGraphicImage * pImage = (CGraphicImage *)CResourceManager::Instance().GetResourcePointer(strImageFileName.c_str());
			if (pImage)
			{
				CGraphicImageInstance * pInstance = CGraphicImageInstance::New();
				pInstance->SetImagePointer(pImage);
				m_ImageInstanceVector.push_back(pInstance);
				m_dwWidthSummary += pInstance->GetWidth();
			}
		}
#endif
		OnChangePosition();
	}

	void CNumberLine::ClearNumber()
	{
#ifdef ENABLE_SLOT_CHAR_ENLARGEMENT
		for (int mode = 0; mode < 2; ++mode)
		{
			for (DWORD i = 0; i < m_ImageInstanceVector[mode].size(); ++i)
			{
				CGraphicImageInstance::Delete(m_ImageInstanceVector[mode][i]);
			}
			m_ImageInstanceVector[mode].clear();
			m_dwWidthSummary[mode] = 0;
		}
#else
		for (DWORD i = 0; i < m_ImageInstanceVector.size(); ++i)
		{
			CGraphicImageInstance::Delete(m_ImageInstanceVector[i]);
		}
		m_ImageInstanceVector.clear();
		m_dwWidthSummary = 0;
#endif
		m_strNumber = "";
	}

	void CNumberLine::OnRender()
	{
#ifdef ENABLE_SLOT_CHAR_ENLARGEMENT
		int mode = m_bIsEnlarged ? 1 : 0;
		for (DWORD i = 0; i < m_ImageInstanceVector[mode].size(); ++i)
		{
			m_ImageInstanceVector[mode][i]->Render();
		}
#else
		for (DWORD i = 0; i < m_ImageInstanceVector.size(); ++i)
		{
			m_ImageInstanceVector[i]->Render();
		}
#endif
	}

	void CNumberLine::OnChangePosition()
	{
		int ix = m_x;
		int iy = m_y;

		if (m_pParent)
		{
			ix = m_rect.left;
			iy = m_rect.top;
		}

#ifdef ENABLE_SLOT_CHAR_ENLARGEMENT
		int mode = m_bIsEnlarged ? 1 : 0;
		if (mode == 1)
		{
			ix += 2;
			iy -= 5;
		}

		switch (m_iHorizontalAlign)
		{
			case HORIZONTAL_ALIGN_LEFT:
				break;
			case HORIZONTAL_ALIGN_CENTER:
				ix -= int(m_dwWidthSummary[mode]) / 2;
				break;
			case HORIZONTAL_ALIGN_RIGHT:
				ix -= int(m_dwWidthSummary[mode]);
				break;
		}

		for (DWORD i = 0; i < m_ImageInstanceVector[mode].size(); ++i)
		{
			m_ImageInstanceVector[mode][i]->SetPosition(ix, iy);
			ix += m_ImageInstanceVector[mode][i]->GetWidth();
		}
#else
		switch (m_iHorizontalAlign)
		{
			case HORIZONTAL_ALIGN_LEFT:
				break;
			case HORIZONTAL_ALIGN_CENTER:
				ix -= int(m_dwWidthSummary) / 2;
				break;
			case HORIZONTAL_ALIGN_RIGHT:
				ix -= int(m_dwWidthSummary);
				break;
		}

		for (DWORD i = 0; i < m_ImageInstanceVector.size(); ++i)
		{
			m_ImageInstanceVector[i]->SetPosition(ix, iy);
			ix += m_ImageInstanceVector[i]->GetWidth();
		}
#endif
	}

#ifdef ENABLE_SLOT_CHAR_ENLARGEMENT
	void CNumberLine::RefreshSlotCharState()
	{
		if (!m_bCanEnlarge)
		{
			m_bIsEnlarged = false;
			return;
		}

		if (UI::CWindowManager::Instance().NeedEnlargeSlotChar())
			m_bIsEnlarged = true;
		else			m_bIsEnlarged = false;
	}
#endif

	CImageBox::CImageBox(PyObject * ppyObject) : CWindow(ppyObject)



#ifdef ENABLE_NEW_DUNGEON_LIB



		,m_fCoolTime(0.0f),



		m_fCoolTimeStart(0.0f)



#endif



	{



		m_pImageInstance = nullptr;



	}



	CImageBox::~CImageBox()



	{



		OnDestroyInstance();



	}







#ifdef ENABLE_NEW_DUNGEON_LIB



	void CImageBox::SetCoolTime(float fCoolTime)



	{



		m_fCoolTime = fCoolTime;



	}







	void CImageBox::SetCoolTimeStart(float fCoolTimeStart)



	{



		m_fCoolTimeStart = fCoolTimeStart;



	}



#endif







	void CImageBox::OnCreateInstance()



	{



		OnDestroyInstance();







		m_pImageInstance = CGraphicImageInstance::New();



	}







	void CImageBox::OnDestroyInstance()



	{



		if (m_pImageInstance)



		{



			CGraphicImageInstance::Delete(m_pImageInstance);



			m_pImageInstance=NULL;



		}



	}







	BOOL CImageBox::LoadImage(const char *c_szFileName)



	{



		if (!c_szFileName[0])



			return FALSE;







		OnCreateInstance();







		CResource * pResource = CResourceManager::Instance().GetResourcePointer(c_szFileName);



		if (!pResource)



			return FALSE;



		if (!pResource->IsType(CGraphicImage::Type()))



			return FALSE;







		m_pImageInstance->SetImagePointer(static_cast<CGraphicImage*>(pResource));



		if (m_pImageInstance->IsEmpty())



			return FALSE;







		SetSize(m_pImageInstance->GetWidth(), m_pImageInstance->GetHeight());



		UpdateRect();







		return TRUE;



	}







	void CImageBox::SetDiffuseColor(float fr, float fg, float fb, float fa)



	{



		if (!m_pImageInstance)



			return;







		m_pImageInstance->SetDiffuseColor(fr, fg, fb, fa);



	}







	int CImageBox::GetWidth()



	{



		if (!m_pImageInstance)



			return 0;







		return m_pImageInstance->GetWidth();



	}







	int CImageBox::GetHeight()



	{



		if (!m_pImageInstance)



			return 0;







		return m_pImageInstance->GetHeight();



	}







    void CImageBox::OnUpdate()



    {



#ifdef ENABLE_CLIP_MASKING



        if (!m_pImageInstance)



        {



            return;



        }







        if (!IsShow())



        {



            return;



        }







        if (!m_bEnableMask)



        {



            return;



        }







        if (!m_pMaskWindow)



        {



            return;



        }







        m_rMaskRect = m_pMaskWindow->GetRect();



#endif



    }







    void CImageBox::OnRender()



    {



        if (!m_pImageInstance)



        {



            return;



        }







#ifdef ENABLE_NEW_TARGET_UI



		if (IsShow() && m_bIsShowImage)



		{



			if (m_fDisplayProcent != -1.0f)



				m_pImageInstance->DisplayImageProcent(m_fDisplayProcent);



#ifdef ENABLE_NEW_DUNGEON_LIB



            else if (m_fCoolTime == 0.0f)



            {



#endif



#ifdef ENABLE_CLIP_MASKING



            m_pImageInstance->Render(m_bEnableMask ? &m_rMaskRect : nullptr);



#else



            m_pImageInstance->Render();



#endif



#ifdef ENABLE_NEW_DUNGEON_LIB



            }



            else



            {



#ifdef ENABLE_CLIP_MASKING



                m_pImageInstance->RenderCoolTime((CTimer::Instance().GetCurrentSecond() - m_fCoolTimeStart) / m_fCoolTime, m_bEnableMask ? &m_rMaskRect : nullptr);



#else



                m_pImageInstance->RenderCoolTime((CTimer::Instance().GetCurrentSecond() - m_fCoolTimeStart) / m_fCoolTime);



#endif



            }



#endif



		}



#else



		if (IsShow())



		{



#ifdef ENABLE_NEW_DUNGEON_LIB



            if (m_fCoolTime == 0.0f)



            {



#endif



#ifdef ENABLE_CLIP_MASKING



            m_pImageInstance->Render(m_bEnableMask ? &m_rMaskRect : nullptr);



#else



            m_pImageInstance->Render();



#endif



#ifdef ENABLE_NEW_DUNGEON_LIB



            }



            else



            {



#ifdef ENABLE_CLIP_MASKING



                m_pImageInstance->RenderCoolTime((CTimer::Instance().GetCurrentSecond() - m_fCoolTimeStart) / m_fCoolTime, m_bEnableMask ? &m_rMaskRect : nullptr);



#else



                m_pImageInstance->RenderCoolTime((CTimer::Instance().GetCurrentSecond() - m_fCoolTimeStart) / m_fCoolTime);



#endif



            }



#endif



		}



#endif



    }







	void CImageBox::OnChangePosition()



	{



		if (!m_pImageInstance)



			return;







		m_pImageInstance->SetPosition(m_rect.left, m_rect.top);



	}







#ifdef ENABLE_OFFICAL_FEATURES



	void CImageBox::LeftRightReverse()



	{



		if (!m_pImageInstance)



			return;







		m_pImageInstance->LeftRightReverse();



	}



#endif







	CMarkBox::CMarkBox(PyObject *ppyObject) : CWindow(ppyObject)



	{



		m_pMarkInstance = nullptr;



	}







	CMarkBox::~CMarkBox()



	{



		OnDestroyInstance();



	}







	void CMarkBox::OnCreateInstance()



	{



		OnDestroyInstance();



		m_pMarkInstance = CGraphicMarkInstance::New();



	}







	void CMarkBox::OnDestroyInstance()



	{



		if (m_pMarkInstance)



		{



			CGraphicMarkInstance::Delete(m_pMarkInstance);



			m_pMarkInstance=NULL;



		}



	}







	void CMarkBox::LoadImage(const char * c_szFilename)



	{



		OnCreateInstance();







		m_pMarkInstance->SetImageFileName(c_szFilename);



		m_pMarkInstance->Load();



		SetSize(m_pMarkInstance->GetWidth(), m_pMarkInstance->GetHeight());







		UpdateRect();



	}







	void CMarkBox::SetScale(FLOAT fScale)



	{



		if (!m_pMarkInstance)



			return;







		m_pMarkInstance->SetScale(fScale);



	}







	void CMarkBox::SetIndex(UINT uIndex)



	{



		if (!m_pMarkInstance)



			return;







		m_pMarkInstance->SetIndex(uIndex);



	}







	void CMarkBox::SetDiffuseColor(float fr, float fg, float fb, float fa)



	{



		if (!m_pMarkInstance)



			return;







		m_pMarkInstance->SetDiffuseColor(fr, fg, fb, fa);



	}







	void CMarkBox::OnUpdate()



	{



	}







	void CMarkBox::OnRender()



	{



		if (!m_pMarkInstance)



			return;







		if (IsShow())



			m_pMarkInstance->Render();



	}







	void CMarkBox::OnChangePosition()



	{



		if (!m_pMarkInstance)



			return;







		m_pMarkInstance->SetPosition(m_rect.left, m_rect.top);



	}







	DWORD CExpandedImageBox::Type()



	{



		static DWORD s_dwType = GetCRC32("CExpandedImageBox", strlen("CExpandedImageBox"));



		return (s_dwType);



	}







	BOOL CExpandedImageBox::OnIsType(DWORD dwType)



	{



		if (CExpandedImageBox::Type() == dwType)



			return TRUE;







		return FALSE;



	}







	CExpandedImageBox::CExpandedImageBox(PyObject * ppyObject) : CImageBox(ppyObject)



	{



		m_windowType = WINDOW_TYPE_EX_IMAGE;



	}



	CExpandedImageBox::~CExpandedImageBox()



	{



		OnDestroyInstance();



	}







	void CExpandedImageBox::OnCreateInstance()



	{



		OnDestroyInstance();







		m_pImageInstance = CGraphicExpandedImageInstance::New();



	}







	void CExpandedImageBox::OnDestroyInstance()



	{



		if (m_pImageInstance)



		{



			CGraphicExpandedImageInstance::Delete((CGraphicExpandedImageInstance*)m_pImageInstance);



			m_pImageInstance=NULL;



		}



	}







	void CExpandedImageBox::SetScale(float fx, float fy)



	{



		if (!m_pImageInstance)



			return;







		((CGraphicExpandedImageInstance*)m_pImageInstance)->SetScale(fx, fy);



		CWindow::SetSize(long(float(GetWidth())*fx), long(float(GetHeight())*fy));



	}







	void CExpandedImageBox::SetOrigin(float fx, float fy)



	{



		if (!m_pImageInstance)



			return;







		((CGraphicExpandedImageInstance*)m_pImageInstance)->SetOrigin(fx, fy);



	}







	void CExpandedImageBox::SetRotation(float fRotation)



	{



		if (!m_pImageInstance)



			return;







		((CGraphicExpandedImageInstance*)m_pImageInstance)->SetRotation(fRotation);



	}







	void CExpandedImageBox::SetRenderingRect(float fLeft, float fTop, float fRight, float fBottom)



	{



		if (!m_pImageInstance)



			return;







		((CGraphicExpandedImageInstance*)m_pImageInstance)->SetRenderingRect(fLeft, fTop, fRight, fBottom);



	}







	void CExpandedImageBox::SetRenderingRectWithScale(float fLeft, float fTop, float fRight, float fBottom)



	{



		if (!m_pImageInstance)



			return;







		((CGraphicExpandedImageInstance*)m_pImageInstance)->SetRenderingRectWithScale(fLeft, fTop, fRight, fBottom);



	}







// #if defined(ENABLE_IMAGE_CLIP_RECT) || defined(ENABLE_BATTLE_PASS_SYSTEM)



	void CExpandedImageBox::SetImageClipRect(float fLeft, float fTop, float fRight, float fBottom, bool bIsVertical)



	{



		if (!m_pImageInstance)



			return;







		const RECT& c_rRect = GetRect();







		float fDifLeft = (c_rRect.left < fLeft) ? -(float(fLeft - c_rRect.left) / float(GetWidth())) : 0.0f;



		float fDifTop = (c_rRect.top < fTop) ? -(float(fTop - c_rRect.top) / float(GetHeight())) : 0.0f;



		float fDifRight = (c_rRect.right > fRight) ? -(float(c_rRect.right - fRight) / float(GetWidth())) : 0.0f;



		float fDifBottom = (c_rRect.bottom > fBottom) ? -(float(c_rRect.bottom - fBottom) / float(GetHeight())) : 0.0f;







		if (bIsVertical)



			((CGraphicExpandedImageInstance*)m_pImageInstance)->SetRenderingRect(fLeft, fDifTop, fRight, fDifBottom);



		else



			((CGraphicExpandedImageInstance*)m_pImageInstance)->SetRenderingRect(fDifLeft, fDifTop, fDifRight, fDifBottom);



	}



// #endif







	void CExpandedImageBox::SetRenderingMode(int iMode)



	{



		((CGraphicExpandedImageInstance*)m_pImageInstance)->SetRenderingMode(iMode);



	}







#ifdef INSIDE_RENDER



	int CExpandedImageBox::GetRenderingWidth()



	{



		return CWindow::GetWidth() + m_renderingRect.right + m_renderingRect.left;



	}



	int CExpandedImageBox::GetRenderingHeight()



	{



		return CWindow::GetHeight() + m_renderingRect.bottom + m_renderingRect.top;



	}



	void CExpandedImageBox::OnSetRenderingRect()



	{



		if (!m_pImageInstance)



			return;







		((CGraphicExpandedImageInstance*)m_pImageInstance)->iSetRenderingRect(m_renderingRect.left, m_renderingRect.top, m_renderingRect.right, m_renderingRect.bottom);



	}



	void CExpandedImageBox::SetExpandedRenderingRect(float fLeftTop, float fLeftBottom, float fTopLeft, float fTopRight, float fRightTop, float fRightBottom, float fBottomLeft, float fBottomRight)



	{



		if (!m_pImageInstance)



			return;







		((CGraphicExpandedImageInstance*)m_pImageInstance)->SetExpandedRenderingRect(fLeftTop, fLeftBottom, fTopLeft, fTopRight, fRightTop, fRightBottom, fBottomLeft, fBottomRight);



	}



	void CExpandedImageBox::SetTextureRenderingRect(float fLeft, float fTop, float fRight, float fBottom)



	{



		if (!m_pImageInstance)



			return;







		((CGraphicExpandedImageInstance*)m_pImageInstance)->SetTextureRenderingRect(fLeft, fTop, fRight, fBottom);



	}



	DWORD CExpandedImageBox::GetPixelColor(DWORD x, DWORD y)



	{



		return ((CGraphicExpandedImageInstance*)m_pImageInstance)->GetPixelColor(x, y);



	}



	void CExpandedImageBox::OnUpdateRenderBox()



	{



		if (!m_pImageInstance)



			return;







		((CGraphicExpandedImageInstance*)m_pImageInstance)->SetRenderBox(m_renderBox);



	}



#endif







	void CExpandedImageBox::OnUpdate()



	{



#ifdef __DUNGEON_INFO__



		static PyObject* poFuncName_OnUpdate = PyString_InternFromString("OnUpdate");



		PyCallClassMemberFunc_ByPyString(m_poHandler, poFuncName_OnUpdate, BuildEmptyTuple());



#endif



	}







	void CExpandedImageBox::OnRender()



	{



		if (!m_pImageInstance)



			return;







		if (IsShow())



#ifdef ENABLE_NEW_DUNGEON_LIB



		{



			if (m_fCoolTime == 0.0f)



				m_pImageInstance->Render();



			else



				((CGraphicExpandedImageInstance*)m_pImageInstance)->RenderCoolTime((CTimer::Instance().GetCurrentSecond() - m_fCoolTimeStart) / m_fCoolTime);



		}



#else



			m_pImageInstance->Render();



#endif



	}







	DWORD CAniImageBox::Type()



	{



		static DWORD s_dwType = GetCRC32("CAniImageBox", strlen("CAniImageBox"));



		return (s_dwType);



	}







	BOOL CAniImageBox::OnIsType(DWORD dwType)



	{



		if (CAniImageBox::Type() == dwType)



			return TRUE;







		return FALSE;



	}







	CAniImageBox::CAniImageBox(PyObject *ppyObject)



		:	CWindow(ppyObject),



			m_bycurDelay(0),



			m_fCurDelay(0.0f),



			m_byDelay(4),



			m_bycurIndex(0),



			m_fScaleX(1),



			m_fScaleY(1)



#ifdef ENABLE_FISH_GAME



			, nextX(0.0), nextY(0.0), moveStart(false), fSpeed(0.0)



#endif



	{



		m_ImageVector.clear();



	}







	CAniImageBox::~CAniImageBox()



	{



		for_each(m_ImageVector.begin(), m_ImageVector.end(), CGraphicExpandedImageInstance::DeleteExpandedImageInstance);



	}







	void CAniImageBox::SetDelay(int iDelay)



	{



		m_byDelay = iDelay;



		m_bycurDelay = 0;



		m_fCurDelay = 0.0f;



	}







	void CAniImageBox::SetDiffuseColor(float r, float g, float b, float a)



	{



		m_SetDiffuseColor = std::bind(&CGraphicExpandedImageInstance::SetDiffuseColor, std::placeholders::_1, r, g, b, a);



		std::for_each(m_ImageVector.begin(), m_ImageVector.end(), m_SetDiffuseColor);



	}







	void CAniImageBox::AppendImage(const char* c_szFileName)



	{



		m_ImageFileNames.push(c_szFileName);



	}







	struct FSetRenderingRect



	{



		float fLeft, fTop, fRight, fBottom;



		void operator () (CGraphicExpandedImageInstance * pInstance)



		{



			pInstance->SetRenderingRect(fLeft, fTop, fRight, fBottom);



		}



	};







	void CAniImageBox::SetRenderingRect(float fLeft, float fTop, float fRight, float fBottom)



	{



		m_SetRenderingRect = std::bind(&CGraphicExpandedImageInstance::SetRenderingRect, std::placeholders::_1, fLeft, fTop, fRight, fBottom);



		std::for_each(m_ImageVector.begin(), m_ImageVector.end(), m_SetRenderingRect);



	}







	struct FSetRenderingRectWithScale



	{



		float fLeft, fTop, fRight, fBottom;



		void operator () (CGraphicExpandedImageInstance* pInstance)



		{



			pInstance->SetRenderingRectWithScale(fLeft, fTop, fRight, fBottom);



		}



	};







	void CAniImageBox::SetRenderingRectWithScale(float fLeft, float fTop, float fRight, float fBottom)



	{



		FSetRenderingRectWithScale setRenderingRectWithScale;



		setRenderingRectWithScale.fLeft = fLeft;



		setRenderingRectWithScale.fTop = fTop;



		setRenderingRectWithScale.fRight = fRight;



		setRenderingRectWithScale.fBottom = fBottom;



		for_each(m_ImageVector.begin(), m_ImageVector.end(), setRenderingRectWithScale);



	}







	struct FSetRenderingMode



	{



		int iMode;



		void operator () (CGraphicExpandedImageInstance * pInstance)



		{



			pInstance->SetRenderingMode(iMode);



		}



	};







	void CAniImageBox::SetRenderingMode(int iMode)



	{



		m_SetRenderingMode = std::bind(&CGraphicExpandedImageInstance::SetRenderingMode, std::placeholders::_1, iMode);



		std::for_each(m_ImageVector.begin(), m_ImageVector.end(), m_SetRenderingMode);



	}







	struct FSetAniImgScale



	{



		float fScaleX, fScaleY;



		void operator () (CGraphicExpandedImageInstance* pInstance)



		{



			pInstance->SetScale(fScaleX, fScaleY);



		}



	};







	void CAniImageBox::SetAniImgScale(float fScaleX, float fScaleY)



	{



		m_fScaleX = fScaleX;



		m_fScaleY = fScaleY;







		FSetAniImgScale setAniImgScale;



		setAniImgScale.fScaleX = m_fScaleX;



		setAniImgScale.fScaleY = m_fScaleY;







		for_each(m_ImageVector.begin(), m_ImageVector.end(), setAniImgScale);



	}







	void CAniImageBox::ResetFrame()



	{



		m_bycurIndex = 0;



		m_bycurDelay = 0;



		m_fCurDelay = 0.0f;



	}







	void CAniImageBox::OnUpdate()



	{



		if (!m_ImageFileNames.empty())



		{



			const std::string& stFileName = m_ImageFileNames.front();







			CResource* pResource = CResourceManager::Instance().GetResourcePointer(stFileName.c_str());



			if (pResource->IsType(CGraphicImage::Type()))



			{



				CGraphicExpandedImageInstance* pImageInstance = CGraphicExpandedImageInstance::New();



				pImageInstance->SetImagePointer(static_cast<CGraphicImage*>(pResource));



				pImageInstance->SetScale(m_fScaleX, m_fScaleY);







				if (pImageInstance->IsEmpty())



				{



					CGraphicExpandedImageInstance::Delete(pImageInstance);



				}



				else



				{



					pImageInstance->SetPosition(m_rect.left, m_rect.top);







					if (m_SetRenderingRect)



						m_SetRenderingRect(pImageInstance);







					if (m_SetRenderingMode)



						m_SetRenderingMode(pImageInstance);







					if (m_SetDiffuseColor)



						m_SetDiffuseColor(pImageInstance);







					m_ImageVector.push_back(pImageInstance);



				}



			}







			m_ImageFileNames.pop();



		}







				if (m_ImageVector.empty())
			return;

		float fElapsed = CTimer::Instance().GetElapsedSecond();
		if (fElapsed <= 0.0f)
			fElapsed = 1.0f / 60.0f;

		m_fCurDelay += fElapsed;

		const float fTargetDelay = static_cast<float>(m_byDelay > 0 ? m_byDelay : 1) / 60.0f;

		if (m_fCurDelay > fTargetDelay * 5.0f)
			m_fCurDelay = fTargetDelay;

		if (m_fCurDelay < fTargetDelay)
			return;

		while (m_fCurDelay >= fTargetDelay)
		{
			m_fCurDelay -= fTargetDelay;

			++m_bycurIndex;
			if (m_bycurIndex >= m_ImageVector.size())
			{
				m_bycurIndex = 0;

				OnEndFrame();
				break;
			}
		}

		#ifdef ENABLE_FISH_GAME



		CheckMove();



#endif



	}







#ifdef ENABLE_FISH_GAME



	void CAniImageBox::SetRotation(float rotation)



	{



		for (BYTE a = 0; a < m_ImageVector.size(); a++)



		{



			CGraphicExpandedImageInstance* pImage = m_ImageVector[a];



			if (pImage)



				pImage->SetRotation(rotation);



		}



	}







	void CAniImageBox::SetMovePos(float directionX, float directionY)



	{



		nextX = directionX;



		nextY = directionY;



	}







	void CAniImageBox::SetMoveSpeed(float speed)



	{



		fSpeed = speed;



	}







	float CAniImageBox::GetRotation()



	{



		if (!m_ImageVector.size())



			return 0.0f;



		return m_ImageVector[0]->GetRotation();



	}







	void CAniImageBox::SetMoveStatus(bool bFlag)



	{



		moveStart = bFlag;



	}







	void CAniImageBox::OnMoveDone()



	{



		PyCallClassMemberFunc(m_poHandler, "OnMoveDone", BuildEmptyTuple());



	}







	void CAniImageBox::CheckMove()



	{



		if (!moveStart)



			return;







		int x = GetPositionX(), y = GetPositionY();



		float fMinDistance = fSpeed * 0.5;



		float fDist = DISTANCE_SQRT(nextX - x, nextY - y);







		if (fDist <= fMinDistance)



		{



			moveStart = false;



			OnMoveDone();



			return;



		}







		float degree = GetDegreeFromPositionXY(x, y, nextX, nextY);



		float fx, fy;







		GetDeltaByDegree(degree, fSpeed, &fx, &fy);







		x += (int)fx;



		y += (int)fy;







		fDist = DISTANCE_SQRT(nextX - x, nextY - y);







		if (fDist <= fMinDistance)



		{



			moveStart = false;



			OnMoveDone();



			return;



		}







		SetRotation((360-degree)+180);



		SetPosition(x, y);



	}



#endif







	void CAniImageBox::OnRender()



	{



		if (m_bycurIndex < m_ImageVector.size())



		{



			CGraphicExpandedImageInstance * pImage = m_ImageVector[m_bycurIndex];



			pImage->Render();



		}



	}







	struct FChangePosition



	{



		float fx, fy;



		void operator () (CGraphicExpandedImageInstance * pInstance)



		{



			pInstance->SetPosition(fx, fy);



		}



	};







	void CAniImageBox::OnChangePosition()



	{



		std::for_each(m_ImageVector.begin(), m_ImageVector.end(),



			std::bind(&CGraphicExpandedImageInstance::SetPosition, std::placeholders::_1, m_rect.left, m_rect.top));



	}







	void CAniImageBox::OnEndFrame()



	{



		PyCallClassMemberFunc(m_poHandler, "OnEndFrame", BuildEmptyTuple());



	}



    uint32_t CGifImageBox::Type()



    {



        static uint32_t s_dwType = GetCRC32("CGifImageBox", strlen("CGifImageBox"));



        return (s_dwType);



    }







    BOOL CGifImageBox::OnIsType(uint32_t dwType)



    {



        if (CGifImageBox::Type() == dwType)



            return TRUE;







        return FALSE;



    }







    CGifImageBox::CGifImageBox(PyObject* ppyObject)



        : CAniImageBox(ppyObject)



    {



    }



    CGifImageBox::~CGifImageBox()



    {



    }







   bool RawFrameToTGAImage(SavedImage* currentFrame, GifFileType* gifFile, std::vector <uint8_t>& outData)



    {



        if (!currentFrame || !gifFile)



        {



            TraceError("RawFrameToTGAImage: Invalid parameters.");



            return false;



        }







        auto& imageDesc = currentFrame->ImageDesc;



        auto* colorMap = (imageDesc.ColorMap) ? imageDesc.ColorMap : gifFile->SColorMap;



        if (!colorMap)



        {



            TraceError("RawFrameToTGAImage: ColorMap is null.");



            return false;



        }







        const int width = imageDesc.Width;



        const int height = imageDesc.Height;







        // TGA Header



        std::array<uint8_t, 18> header = {0};



        header[2] = 2; // Uncompressed RGB



        header[12] = width & 0xFF;



        header[13] = (width >> 8) & 0xFF;



        header[14] = height & 0xFF;



        header[15] = (height >> 8) & 0xFF;



        header[16] = 32; // 32 bits per pixel







        outData.insert(outData.end(), header.begin(), header.end());







        // Get transparency information from the GIF



        GraphicsControlBlock gcb;



        DGifSavedExtensionToGCB(gifFile, currentFrame - gifFile->SavedImages, &gcb);







        for (int y = height - 1; y >= 0; --y)



        {



            for (int x = 0; x < width; ++x)



            {



                const int index = y * width + x;



                const GifByteType colorIndex = currentFrame->RasterBits[index];



                if (colorIndex < colorMap->ColorCount)



                {



                    const GifColorType& gifColor = colorMap->Colors[colorIndex];



                    outData.push_back(gifColor.Blue);



                    outData.push_back(gifColor.Green);



                    outData.push_back(gifColor.Red);



                    // If the pixel is transparent in the GIF, make it transparent in the TGA



                    outData.push_back((gcb.TransparentColor && colorIndex == gcb.TransparentColor) ? 0x00 : 0xFF);



                }



                else



                {



                    TraceError("RawFrameToTGAImage: Color index out of range.");



                    return false;



                }



            }



        }







        return true;



    }







    bool CGifImageBox::LoadGif(const char* c_szFileName)



    {



        if (!c_szFileName || !*c_szFileName)



            return false;







        // Open GIF file



        int error;



        GifFileType* gif = DGifOpenFileName(c_szFileName, &error);



        if (!gif)



        {



            const char* c_szError = GifErrorString(error);



            TraceError("Failed to open GIF file: %s with error: %s", c_szFileName, c_szError);



            return false;



        }







        // Read the GIF into memory



        if (DGifSlurp(gif) == GIF_ERROR)



        {



            TraceError("Failed to read GIF file: %s", c_szFileName);



            DGifCloseFile(gif, &error);



            return false;



        }







        // Iterate through the frames



        for (int i = 0; i < gif->ImageCount; ++i)



        {



            // Get the current frame



            SavedImage* currentImage = &gif->SavedImages[i];







            // Convert the frame to TGA image



            std::vector <uint8_t> tgaBuffer;



            if (!RawFrameToTGAImage(currentImage, gif, tgaBuffer))



            {



                TraceError("Failed to convert GIF(%s) frame #%d to TGA image.", c_szFileName, i);



                continue;



            }







            // Create name for the frame



            char frameFileName[256]{ '\0' };



            snprintf(frameFileName, sizeof(frameFileName), "gif_frame%d_%s", i, c_szFileName);







            // Create the image instance



            CGraphicImage* pImage = new CGraphicImage(frameFileName, tgaBuffer.data(), tgaBuffer.size(), D3DX_FILTER_LINEAR);



            if (!pImage)



            {



                TraceError("Failed to create image instance for GIF(%s) frame #%d.", c_szFileName, i);



                continue;



            }







            // Create the expanded image instance



            CGraphicExpandedImageInstance* pImageInstance = CGraphicExpandedImageInstance::New();



            pImageInstance->SetImagePointer(pImage);







            // Add the image instance to the vector if it's not empty



            if (pImageInstance->IsEmpty())



            {



                CGraphicExpandedImageInstance::Delete(pImageInstance);



            }



            else



            {



                pImageInstance->SetPosition(m_rect.left, m_rect.top);



                m_ImageVector.push_back(pImageInstance);



            }



        }







        // Clean up



        DGifCloseFile(gif, &error);







        return true;



    }







    bool CGifImageBox::UnloadGif()



    {



        for (auto pImage : m_ImageVector)



            CGraphicExpandedImageInstance::Delete(pImage);







        m_ImageVector.clear();



        return true;



    }







	///////////////////////////////////////////////////////////////////////////////////////////////



	///////////////////////////////////////////////////////////////////////////////////////////////



	///////////////////////////////////////////////////////////////////////////////////////////////







	CButton::CButton(PyObject *ppyObject)



		:	CWindow(ppyObject),



			m_pcurVisual(nullptr),



			m_bEnable(TRUE),



			m_isPressed(FALSE),



			m_isFlash(FALSE)



#ifdef ENABLE_FLEXIBLE_WIDTH_BUTTON



			, m_bKeepUpVisual(FALSE)



#endif



	{



		CWindow::AddFlag(CWindow::FLAG_NOT_CAPTURE);



	}



	CButton::~CButton()



	{



	}







	BOOL CButton::SetUpVisual(const char *c_szFileName)



	{



		CResource * pResource = CResourceManager::Instance().GetResourcePointer(c_szFileName);



		if (!pResource->IsType(CGraphicImage::Type()))



			return FALSE;







		m_upVisual.SetImagePointer(static_cast<CGraphicImage*>(pResource));



		if (m_upVisual.IsEmpty())



			return FALSE;







		SetSize(m_upVisual.GetWidth(), m_upVisual.GetHeight());



		//



		SetCurrentVisual(&m_upVisual);



		//







		return TRUE;



	}



	BOOL CButton::SetOverVisual(const char *c_szFileName)



	{



		CResource * pResource = CResourceManager::Instance().GetResourcePointer(c_szFileName);



		if (!pResource->IsType(CGraphicImage::Type()))



			return FALSE;







		m_overVisual.SetImagePointer(static_cast<CGraphicImage*>(pResource));



		if (m_overVisual.IsEmpty())



			return FALSE;







		SetSize(m_overVisual.GetWidth(), m_overVisual.GetHeight());







		return TRUE;



	}



	BOOL CButton::SetDownVisual(const char *c_szFileName)



	{



		CResource * pResource = CResourceManager::Instance().GetResourcePointer(c_szFileName);



		if (!pResource->IsType(CGraphicImage::Type()))



			return FALSE;







		m_downVisual.SetImagePointer(static_cast<CGraphicImage*>(pResource));



		if (m_downVisual.IsEmpty())



			return FALSE;







		SetSize(m_downVisual.GetWidth(), m_downVisual.GetHeight());







		return TRUE;



	}



	BOOL CButton::SetDisableVisual(const char *c_szFileName)



	{



		CResource * pResource = CResourceManager::Instance().GetResourcePointer(c_szFileName);



		if (!pResource->IsType(CGraphicImage::Type()))



			return FALSE;







		m_disableVisual.SetImagePointer(static_cast<CGraphicImage*>(pResource));



		if (m_downVisual.IsEmpty())



			return FALSE;







		SetSize(m_disableVisual.GetWidth(), m_disableVisual.GetHeight());







		return TRUE;



	}







	const char *CButton::GetUpVisualFileName()



	{



		return m_upVisual.GetGraphicImagePointer()->GetFileName();



	}



	const char *CButton::GetOverVisualFileName()



	{



		return m_overVisual.GetGraphicImagePointer()->GetFileName();



	}



	const char *CButton::GetDownVisualFileName()



	{



		return m_downVisual.GetGraphicImagePointer()->GetFileName();



	}







	void CButton::Flash()



	{



		m_isFlash = TRUE;



	}







	void CButton::EnableFlash()



	{



		m_isFlash = TRUE;



	}







	void CButton::DisableFlash()



	{



		m_isFlash = FALSE;



	}







	void CButton::Enable()



	{



		if (!IsDisable())



		{



			SetUp();



			m_bEnable = TRUE;



		}



	}







	void CButton::Disable()



	{



		if (IsDisable())



		{



			m_bEnable = FALSE;



			if (!m_disableVisual.IsEmpty())



				SetCurrentVisual(&m_disableVisual);



		}



	}







	BOOL CButton::IsDisable()



	{



		return m_bEnable;



	}







	void CButton::SetUp()



	{



		SetCurrentVisual(&m_upVisual);



		m_isPressed = FALSE;



	}







	void CButton::Up()



	{



		if (IsIn())



			SetCurrentVisual(&m_overVisual);



		else



			SetCurrentVisual(&m_upVisual);







		PyCallClassMemberFunc(m_poHandler, "CallEvent", BuildEmptyTuple());



	}







	void CButton::Over()



	{



		SetCurrentVisual(&m_overVisual);



	}







	void CButton::Down()



	{



		m_isPressed = TRUE;



		SetCurrentVisual(&m_downVisual);



		PyCallClassMemberFunc(m_poHandler, "DownEvent", BuildEmptyTuple());



	}







#if defined(ENABLE_EMOTICONS_SYSTEM) || defined(ENABLE_FLEXIBLE_WIDTH_BUTTON)



	void CButton::SetScale (float fx, float fy)



	{



		if (m_upVisual.IsEmpty())



		{



			return;



		}







		if (m_overVisual.IsEmpty())



		{



			return;



		}







		if (m_downVisual.IsEmpty())



		{



			return;



		}







		m_upVisual.SetScale (fx, fy);



		m_overVisual.SetScale (fx, fy);



		m_downVisual.SetScale (fx, fy);







		CWindow::SetSize (long (float (GetWidth())*fx), long (float (GetHeight())*fy));



	}



#endif







#ifdef ENABLE_FLEXIBLE_WIDTH_BUTTON



	long CButton::GetVisualWidth()



	{



		if (!m_upVisual.IsEmpty())



			return m_upVisual.GetWidth();



		return 0;



	}







	long CButton::GetVisualHeight()



	{



		if (!m_upVisual.IsEmpty())



			return m_upVisual.GetHeight();



		return 0;



	}



#endif







#ifdef ENABLE_CLIP_MASKING



	void CButton::OnUpdate()



	{



		if (!m_bEnableMask)



			return;







		if (!m_pMaskWindow)



			return;







		m_rMaskRect = m_pMaskWindow->GetRect();



	}







	void CButton::OnRender()



	{



		if (!IsShow())



			return;







		if (m_pcurVisual)



		{



			if (m_isFlash)



			if (!IsIn())



			if (int(timeGetTime() / 500) % 2)



			{



				return;



			}



			m_pcurVisual->Render(m_bEnableMask ? &m_rMaskRect : NULL);



		}







		PyCallClassMemberFunc(m_poHandler, "OnRender", BuildEmptyTuple());



	}



#else



	void CButton::OnUpdate()



	{



	}







	void CButton::OnRender()



	{



		if (!IsShow())



			return;







		if (m_pcurVisual)



		{



			if (m_isFlash)



			if (!IsIn())



			if (int(timeGetTime() / 500)%2)



			{



				return;



			}







			m_pcurVisual->Render();



		}







		PyCallClassMemberFunc(m_poHandler, "OnRender", BuildEmptyTuple());



	}



#endif







	void CButton::OnChangePosition()



	{



		if (m_pcurVisual)



			m_pcurVisual->SetPosition(m_rect.left, m_rect.top);



	}







	BOOL CButton::OnMouseLeftButtonDown()



	{



		if (!IsEnable())



			return TRUE;







		m_isPressed = TRUE;



		Down();







		return TRUE;



	}







	BOOL CButton::OnMouseLeftButtonDoubleClick()



	{



		if (!IsEnable())



			return TRUE;







		OnMouseLeftButtonDown();







		return TRUE;



	}







	BOOL CButton::OnMouseLeftButtonUp()



	{



		if (!IsEnable())



			return TRUE;



		if (!IsPressed())



			return TRUE;







		m_isPressed = FALSE;



		Up();







		return TRUE;



	}







	void CButton::OnMouseOverIn()



	{



		if (!IsEnable())



			return;







		Over();



		PyCallClassMemberFunc(m_poHandler, "ShowToolTip", BuildEmptyTuple());



#if defined(__BL__DETAILS_UI__) && defined(ENABLE_SKILL_COLOR_SYSTEM)



		PyCallClassMemberFunc(m_poHandler, "OnMouseOverIn", BuildEmptyTuple());



#endif



	}







	void CButton::OnMouseOverOut()



	{



		if (!IsEnable())



			return;







		SetUp();



		PyCallClassMemberFunc(m_poHandler, "HideToolTip", BuildEmptyTuple());



#if defined(__BL__DETAILS_UI__) && defined(ENABLE_SKILL_COLOR_SYSTEM)



		PyCallClassMemberFunc(m_poHandler, "OnMouseOverOut", BuildEmptyTuple());



#endif



	}







#if defined(ENABLE_TRACK_WINDOW) && defined(ENABLE_ADVANCED_GAME_OPTIONS)



	DWORD CButton::Type()



	{



		static DWORD s_dwType = GetCRC32("CButton", strlen("CButton"));



		return (s_dwType);



	}







	BOOL CButton::OnIsType(DWORD dwType)



	{



		if (CButton::Type() == dwType)



			return TRUE;



		return FALSE;



	}







	void CButton::SetRenderingRect(float fLeft, float fTop, float fRight, float fBottom)



	{



		m_upVisual.SetRenderingRect(fLeft, fTop, fRight, fBottom);



		m_overVisual.SetRenderingRect(fLeft, fTop, fRight, fBottom);



		m_downVisual.SetRenderingRect(fLeft, fTop, fRight, fBottom);



	}







	void CButton::SetCurrentVisual(CGraphicExpandedImageInstance* pVisual)



#else



	void CButton::SetCurrentVisual(CGraphicImageInstance * pVisual)



#endif



	{



		m_pcurVisual = pVisual;



		m_pcurVisual->SetPosition(m_rect.left, m_rect.top);



	}







	BOOL CButton::IsEnable()



	{



		return m_bEnable;



	}







	BOOL CButton::IsPressed()



	{



		return m_isPressed;



	}







#ifdef INSIDE_RENDER



	void CButton::OnUpdateRenderBox()



	{



		m_upVisual.SetRenderBox(m_renderBox);



		m_overVisual.SetRenderBox(m_renderBox);



		m_downVisual.SetRenderBox(m_renderBox);



		m_disableVisual.SetRenderBox(m_renderBox);



	}



	void CButton::OnSetRenderingRect()



	{



		m_upVisual.iSetRenderingRect(m_renderingRect.left, m_renderingRect.top, m_renderingRect.right, m_renderingRect.bottom);



		m_overVisual.iSetRenderingRect(m_renderingRect.left, m_renderingRect.top, m_renderingRect.right, m_renderingRect.bottom);



		m_downVisual.iSetRenderingRect(m_renderingRect.left, m_renderingRect.top, m_renderingRect.right, m_renderingRect.bottom);



		m_disableVisual.iSetRenderingRect(m_renderingRect.left, m_renderingRect.top, m_renderingRect.right, m_renderingRect.bottom);



	}



#endif







#ifdef ENABLE_OFFICAL_FEATURES



	void CButton::LeftRightReverse()



	{



		m_upVisual.LeftRightReverse();



		m_overVisual.LeftRightReverse();



		m_downVisual.LeftRightReverse();



		m_disableVisual.LeftRightReverse();



	}



#endif







	///////////////////////////////////////////////////////////////////////////////////////////////



	///////////////////////////////////////////////////////////////////////////////////////////////



	///////////////////////////////////////////////////////////////////////////////////////////////







	CRadioButton::CRadioButton(PyObject * ppyObject) : CButton(ppyObject)



	{



	}



	CRadioButton::~CRadioButton()



	{



	}







#if defined(ENABLE_TRACK_WINDOW) && defined(ENABLE_ADVANCED_GAME_OPTIONS)



	DWORD CRadioButton::Type()



	{



		static DWORD s_dwType = GetCRC32("CRadioButton", strlen("CRadioButton"));



		return (s_dwType);



	}







	BOOL CRadioButton::OnIsType(DWORD dwType)



	{



		if (CRadioButton::Type() == dwType)



			return TRUE;



		return FALSE;



	}



#endif







	BOOL CRadioButton::OnMouseLeftButtonDown()



	{



		if (!IsEnable())



			return TRUE;







		if (!m_isPressed)



		{



			Down();



			PyCallClassMemberFunc(m_poHandler, "CallEvent", BuildEmptyTuple());



		}







		return TRUE;



	}



	BOOL CRadioButton::OnMouseLeftButtonUp()



	{



		return TRUE;



	}



	void CRadioButton::OnMouseOverIn()



	{



		if (!IsEnable())



			return;







		if (!m_isPressed)



		{



			SetCurrentVisual(&m_overVisual);



		}







		PyCallClassMemberFunc(m_poHandler, "ShowToolTip", BuildEmptyTuple());



	}



	void CRadioButton::OnMouseOverOut()



	{



		if (!IsEnable())



			return;







		if (!m_isPressed)



		{



			SetCurrentVisual(&m_upVisual);



		}







		PyCallClassMemberFunc(m_poHandler, "HideToolTip", BuildEmptyTuple());



	}







	///////////////////////////////////////////////////////////////////////////////////////////////



	///////////////////////////////////////////////////////////////////////////////////////////////



	///////////////////////////////////////////////////////////////////////////////////////////////







	CToggleButton::CToggleButton(PyObject *ppyObject) : CButton(ppyObject)



	{



	}



	CToggleButton::~CToggleButton()



	{



	}







#if defined(ENABLE_TRACK_WINDOW) && defined(ENABLE_ADVANCED_GAME_OPTIONS)



	DWORD CToggleButton::Type()



	{



		static DWORD s_dwType = GetCRC32("CToggleButton", strlen("CToggleButton"));



		return (s_dwType);



	}







	BOOL CToggleButton::OnIsType(DWORD dwType)



	{



		if (CToggleButton::Type() == dwType)



			return TRUE;



		return FALSE;



	}



#endif







	BOOL CToggleButton::OnMouseLeftButtonDown()



	{



		if (!IsEnable())



			return TRUE;







		if (m_isPressed)



		{



			SetUp();



			if (IsIn())



				SetCurrentVisual(&m_overVisual);



			else



				SetCurrentVisual(&m_upVisual);



			PyCallClassMemberFunc(m_poHandler, "OnToggleUp", BuildEmptyTuple());



		}



		else



		{



			Down();



			PyCallClassMemberFunc(m_poHandler, "OnToggleDown", BuildEmptyTuple());



		}







		return TRUE;



	}



	BOOL CToggleButton::OnMouseLeftButtonUp()



	{



		return TRUE;



	}







	void CToggleButton::OnMouseOverIn()



	{



		if (!IsEnable())



			return;







		if (!m_isPressed)



		{



			SetCurrentVisual(&m_overVisual);



		}







		PyCallClassMemberFunc(m_poHandler, "ShowToolTip", BuildEmptyTuple());



	}



	void CToggleButton::OnMouseOverOut()



	{



		if (!IsEnable())



			return;







		if (!m_isPressed)



		{



			SetCurrentVisual(&m_upVisual);



		}







		PyCallClassMemberFunc(m_poHandler, "HideToolTip", BuildEmptyTuple());



	}







	///////////////////////////////////////////////////////////////////////////////////////////////



	///////////////////////////////////////////////////////////////////////////////////////////////



	///////////////////////////////////////////////////////////////////////////////////////////////







	CDragButton::CDragButton(PyObject *ppyObject) : CButton(ppyObject)



	{



		CWindow::RemoveFlag(CWindow::FLAG_NOT_CAPTURE);



		m_restrictArea.left = 0;



		m_restrictArea.top = 0;



		m_restrictArea.right = CWindowManager::Instance().GetScreenWidth();



		m_restrictArea.bottom = CWindowManager::Instance().GetScreenHeight();



	}



	CDragButton::~CDragButton()



	{



	}







	void CDragButton::SetRestrictMovementArea(int ix, int iy, int iwidth, int iheight)



	{



		m_restrictArea.left = ix;



		m_restrictArea.top = iy;



		m_restrictArea.right = ix + iwidth;



		m_restrictArea.bottom = iy + iheight;



	}







	void CDragButton::OnChangePosition()



	{



		m_x = max(m_x, m_restrictArea.left);



		m_y = max(m_y, m_restrictArea.top);



		m_x = min(m_x, max(0, m_restrictArea.right - m_lWidth));



		m_y = min(m_y, max(0, m_restrictArea.bottom - m_lHeight));







		m_rect.left = m_x;



		m_rect.top = m_y;







		if (m_pParent)



		{



			const RECT & c_rRect = m_pParent->GetRect();



			m_rect.left += c_rRect.left;



			m_rect.top += c_rRect.top;



		}







		m_rect.right = m_rect.left + m_lWidth;



		m_rect.bottom = m_rect.top + m_lHeight;







		std::for_each(m_pChildList.begin(), m_pChildList.end(), std::mem_fn(&CWindow::UpdateRect));







		if (m_pcurVisual)



			m_pcurVisual->SetPosition(m_rect.left, m_rect.top);







		if (IsPressed())



			PyCallClassMemberFunc(m_poHandler, "OnMove", BuildEmptyTuple());



	}







	void CDragButton::OnMouseOverIn()



	{



		if (!IsEnable())







			return;







		CButton::OnMouseOverIn();



		PyCallClassMemberFunc(m_poHandler, "OnMouseOverIn", BuildEmptyTuple());



	}







	void CDragButton::OnMouseOverOut()



	{



		if (!IsEnable())



			return;







		CButton::OnMouseOverIn();



		PyCallClassMemberFunc(m_poHandler, "OnMouseOverOut", BuildEmptyTuple());
	}

#ifdef ENABLE_FLEXIBLE_WIDTH_BUTTON



#if defined(ENABLE_TRACK_WINDOW) && defined(ENABLE_ADVANCED_GAME_OPTIONS)



	DWORD CFlexibleWidthButton::Type()



	{



		static DWORD s_dwType = GetCRC32("CFlexibleWidthButton", strlen("CFlexibleWidthButton"));



		return (s_dwType);



	}







	BOOL CFlexibleWidthButton::OnIsType(DWORD dwType)



	{



		if (CFlexibleWidthButton::Type() == dwType)



			return TRUE;



		return CButton::OnIsType(dwType);



	}



#endif







	CFlexibleWidthButton::CFlexibleWidthButton(PyObject * ppyObject) : CButton(ppyObject)



	{



		m_iSideWidth = 0;



		m_fCenterWidth = 0.0f;



	}







	CFlexibleWidthButton::~CFlexibleWidthButton()



	{



	}







	BOOL CFlexibleWidthButton::SetSideVisual(int iVisualType, int iSideType, const char * c_szFileName)



	{



		if (!c_szFileName || iVisualType < 0 || iVisualType >= 4 || iSideType < 0 || iSideType >= 2)



			return FALSE;







		CResource * pResource = CResourceManager::Instance().GetResourcePointer(c_szFileName);



		if (!pResource)



			return FALSE;







		m_sideVisuals[iVisualType][iSideType].SetImagePointer(static_cast<CGraphicImage *>(pResource));



		if (m_sideVisuals[iVisualType][iSideType].IsEmpty())



			return FALSE;







		m_iSideWidth = m_sideVisuals[iVisualType][iSideType].GetWidth();



		return TRUE;



	}







	int CFlexibleWidthButton::GetSideWidth() const



	{



		return m_iSideWidth;



	}







	BOOL CFlexibleWidthButton::SetUpVisual(const char * c_szFileName)



	{



		if (!CButton::SetUpVisual(c_szFileName))



			return FALSE;







		m_fCenterWidth = (float)m_upVisual.GetWidth();



		return TRUE;



	}







	void CFlexibleWidthButton::OnChangePosition()



	{



		CButton::OnChangePosition();







		if (m_pcurVisual)



		{



			m_pcurVisual->SetPosition(m_rect.left + m_iSideWidth, m_rect.top);



		}







		for (int i = 0; i < 4; ++i)



		{



			for (int j = 0; j < 2; ++j)



			{



				if (m_sideVisuals[i][j].IsEmpty())



					continue;







				if (j == 0) // SIDE_LEFT



				{



					m_sideVisuals[i][j].SetPosition((float)m_rect.left, (float)m_rect.top);



				}



				else // SIDE_RIGHT



				{



					float fRightX = (float)(m_rect.left + GetWidth() - m_iSideWidth);



					m_sideVisuals[i][j].SetPosition(fRightX, (float)m_rect.top);



				}



			}



		}



	}







	void CFlexibleWidthButton::OnRender()



	{



		if (!IsShow())



			return;







		int iVisualType = 0; // VISUAL_UP



		if (IsDisable())



			iVisualType = 3; // VISUAL_DISABLE



		else if (m_isPressed)



			iVisualType = 2; // VISUAL_DOWN



		else if (IsIn())



			iVisualType = 1; // VISUAL_OVER







		if (m_pcurVisual)



		{



			if (m_isFlash && !IsIn() && (int(timeGetTime() / 500) % 2))



			{



				return;



			}







#ifdef ENABLE_CLIP_MASKING



			m_pcurVisual->Render(m_bEnableMask ? &m_rMaskRect : NULL);



#else



			m_pcurVisual->Render();



#endif



		}







		for (int iSide = 0; iSide < 2; ++iSide)



		{



			if (!m_sideVisuals[iVisualType][iSide].IsEmpty())



			{



#ifdef ENABLE_CLIP_MASKING



				m_sideVisuals[iVisualType][iSide].Render(m_bEnableMask ? &m_rMaskRect : NULL);



#else



				m_sideVisuals[iVisualType][iSide].Render();



#endif



			}



		}







		PyCallClassMemberFunc(m_poHandler, "OnRender", BuildEmptyTuple());



	}







	///////////////////////////////////////////////////////////////////////////////////////////////



	// CFlexibleWidthToggleButton



	///////////////////////////////////////////////////////////////////////////////////////////////



#if defined(ENABLE_TRACK_WINDOW) && defined(ENABLE_ADVANCED_GAME_OPTIONS)



	DWORD CFlexibleWidthToggleButton::Type()



	{



		static DWORD s_dwType = GetCRC32("CFlexibleWidthToggleButton", strlen("CFlexibleWidthToggleButton"));



		return (s_dwType);



	}







	BOOL CFlexibleWidthToggleButton::OnIsType(DWORD dwType)



	{



		if (CFlexibleWidthToggleButton::Type() == dwType)



			return TRUE;



		return CFlexibleWidthButton::OnIsType(dwType);



	}



#endif







	CFlexibleWidthToggleButton::CFlexibleWidthToggleButton(PyObject * ppyObject) : CFlexibleWidthButton(ppyObject)



	{



	}







	CFlexibleWidthToggleButton::~CFlexibleWidthToggleButton()



	{



	}







	BOOL CFlexibleWidthToggleButton::OnMouseLeftButtonDown()



	{



		if (!IsEnable())



			return TRUE;







		if (m_isPressed)



		{



			SetUp();



			if (IsIn())



				SetCurrentVisual(&m_overVisual);



			else



				SetCurrentVisual(&m_upVisual);



			PyCallClassMemberFunc(m_poHandler, "OnToggleUp", BuildEmptyTuple());



		}



		else



		{



			Down();



			if (m_bKeepUpVisual)



				SetCurrentVisual(&m_upVisual);



			PyCallClassMemberFunc(m_poHandler, "OnToggleDown", BuildEmptyTuple());



		}







		return TRUE;



	}







	BOOL CFlexibleWidthToggleButton::OnMouseLeftButtonUp()



	{



		return TRUE;



	}







	void CFlexibleWidthToggleButton::OnMouseOverIn()



	{



		if (!IsEnable())



			return;







		if (!m_isPressed)



		{



			SetCurrentVisual(&m_overVisual);



		}







		PyCallClassMemberFunc(m_poHandler, "ShowToolTip", BuildEmptyTuple());



	}







	void CFlexibleWidthToggleButton::OnMouseOverOut()



	{



		if (!IsEnable())



			return;







		if (!m_isPressed || m_bKeepUpVisual)



		{



			SetCurrentVisual(&m_upVisual);



		}







		PyCallClassMemberFunc(m_poHandler, "HideToolTip", BuildEmptyTuple());



	}



#endif







}



