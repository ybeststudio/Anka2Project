#include "StdAfx.h"
#include "../eterBase/CRC32.h"
#include "../eterBase/Filename.h"
#include "PythonWindow.h"
#include "PythonSlotWindow.h"

using namespace UI;

class UI::CSlotWindow::CSlotButton : public CButton
{
	public:
		enum ESlotButtonType
		{
			SLOT_BUTTON_TYPE_PLUS,
			SLOT_BUTTON_TYPE_COVER,
		};

	public:
		CSlotButton(ESlotButtonType dwType, DWORD dwSlotNumber, CSlotWindow * pParent) : CButton(nullptr)
		{
			m_dwSlotButtonType = dwType;
			m_dwSlotNumber = dwSlotNumber;
			m_pParent = pParent;
		}
		virtual ~CSlotButton()
		{
		}

		BOOL OnMouseLeftButtonDown()
		{
			if (!IsEnable())
				return TRUE;
			if (UI::CWindowManager::Instance().IsAttaching())
				return TRUE;

			m_isPressed = TRUE;
			Down();

			return TRUE;
		}
		BOOL OnMouseLeftButtonUp()
		{
			if (!IsEnable())
				return TRUE;
			if (!IsPressed())
				return TRUE;

			if (IsIn())
				SetCurrentVisual(&m_overVisual);
			else
				SetCurrentVisual(&m_upVisual);

			m_pParent->OnPressedSlotButton(m_dwSlotButtonType, m_dwSlotNumber);

			return TRUE;
		}

		void OnMouseOverIn()
		{
			if (IsEnable())
			{
				SetCurrentVisual(&m_overVisual);
			}
			m_pParent->OnOverInItem(m_dwSlotNumber);
		}
		void OnMouseOverOut()
		{
			if (IsEnable())
			{
				SetUp();
				SetCurrentVisual(&m_upVisual);
			}
			m_pParent->OnOverOutItem();
		}

	protected:
		ESlotButtonType m_dwSlotButtonType;
		DWORD m_dwSlotNumber;
		CSlotWindow * m_pParent;
};

class UI::CSlotWindow::CCoverButton : public CSlotButton
{
	public:
		CCoverButton(ESlotButtonType dwType, DWORD dwSlotNumber, CSlotWindow * pParent) : CSlotButton(dwType, dwSlotNumber, pParent)
		{
			m_bLeftButtonEnable = TRUE;
			m_bRightButtonEnable = TRUE;
		}
		virtual ~CCoverButton()
		{
		}

		void SetLeftButtonEnable(BOOL bEnable)
		{
			m_bLeftButtonEnable = bEnable;
		}
		void SetRightButtonEnable(BOOL bEnable)
		{
			m_bRightButtonEnable = bEnable;
		}

		void OnRender()
		{
		}

		void RenderButton()
		{
			CButton::OnRender();
		}

		BOOL OnMouseLeftButtonDown()
		{
			if (!IsEnable())
				return TRUE;
			if (m_bLeftButtonEnable)
			{
				CButton::OnMouseLeftButtonDown();
			}
			else
			{
				m_pParent->OnMouseLeftButtonDown();
			}
			return TRUE;
		}
		BOOL OnMouseLeftButtonUp()
		{
			if (!IsEnable())
				return TRUE;
			if (m_bLeftButtonEnable)
			{
				CButton::OnMouseLeftButtonUp();
				m_pParent->OnMouseLeftButtonDown();
			}
			else
			{
				m_pParent->OnMouseLeftButtonUp();
			}
			return TRUE;
		}

		BOOL OnMouseRightButtonDown()
		{
			if (!IsEnable())
				return TRUE;
			if (m_bRightButtonEnable)
				CButton::OnMouseLeftButtonDown();
			return TRUE;
		}
		BOOL OnMouseRightButtonUp()
		{
			if (!IsEnable())
				return TRUE;
			m_pParent->OnMouseRightButtonDown();
			if (m_bRightButtonEnable)
				CButton::OnMouseLeftButtonUp();
			return TRUE;
		}

	protected:
		BOOL m_bLeftButtonEnable;
		BOOL m_bRightButtonEnable;
};

class UI::CSlotWindow::CCoolTimeFinishEffect : public CAniImageBox
{
	public:
		CCoolTimeFinishEffect(CSlotWindow * pParent, DWORD dwSlotIndex) : CAniImageBox(nullptr)
		{
			m_pParent = pParent;
			m_dwSlotIndex = dwSlotIndex;
		}
		virtual ~CCoolTimeFinishEffect()
		{
		}

		void OnEndFrame()
		{
			((CSlotWindow *)m_pParent)->ReserveDestroyCoolTimeFinishEffect(m_dwSlotIndex);
		}

	protected:
		DWORD m_dwSlotIndex;
};

void CSlotWindow::SetSlotType(DWORD dwType)
{
	m_dwSlotType = dwType;
}

void CSlotWindow::SetSlotStyle(DWORD dwStyle)
{
	m_dwSlotStyle = dwStyle;
}

void CSlotWindow::SetSlotScale(float fx, float fy)
{
	m_fScaleX = fx;
	m_fScaleY = fy;
}

void CSlotWindow::AppendSlot(DWORD dwIndex, int ixPosition, int iyPosition, int ixCellSize, int iyCellSize)
{
	TSlot Slot;
	Slot.pInstance = nullptr;
	Slot.pNumberLine = nullptr;
	Slot.pCoverButton = nullptr;
	Slot.pSlotButton = nullptr;
	Slot.pSignImage = nullptr;
	Slot.pFinishCoolTimeEffect = nullptr;
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
	Slot.pCoverImage = nullptr;
#endif
	Slot.pSecondCoverImage = nullptr;

	ClearSlot(&Slot);
	Slot.dwSlotNumber = dwIndex;
	Slot.dwCenterSlotNumber = dwIndex;
	Slot.dwRealSlotNumber = dwIndex;
	Slot.dwRealCenterSlotNumber = dwIndex;
	Slot.ixPosition = ixPosition;
	Slot.iyPosition = iyPosition;
	Slot.ixCellSize = ixCellSize;
	Slot.iyCellSize = iyCellSize;
	m_SlotList.push_back(Slot);
}

void CSlotWindow::SetCoverButton(DWORD dwIndex, const char *c_szUpImageName, const char *c_szOverImageName, const char *c_szDownImageName, const char *c_szDisableImageName, BOOL bLeftButtonEnable, BOOL bRightButtonEnable)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
		return;

	CCoverButton *& rpCoverButton = pSlot->pCoverButton;

	if (!rpCoverButton)
	{
		rpCoverButton = new CCoverButton(CSlotButton::SLOT_BUTTON_TYPE_COVER, pSlot->dwSlotNumber, this);
		CWindowManager::Instance().SetParent(rpCoverButton, this);
	}

	rpCoverButton->SetLeftButtonEnable(bLeftButtonEnable);
	rpCoverButton->SetRightButtonEnable(bRightButtonEnable);
	rpCoverButton->SetUpVisual(c_szUpImageName);
	rpCoverButton->SetOverVisual(c_szOverImageName);
	rpCoverButton->SetDownVisual(c_szDownImageName);
	rpCoverButton->SetDisableVisual(c_szDisableImageName);
	rpCoverButton->SetScale(m_fScaleX, m_fScaleY);
	rpCoverButton->Enable();
	rpCoverButton->Show();

	if (pSlot->pSlotButton)
	{
		SetTop(pSlot->pSlotButton);
	}
}

void CSlotWindow::EnableCoverButton(DWORD dwIndex)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
		return;

	if (!pSlot->pCoverButton)
		return;

	pSlot->pCoverButton->Enable();
}

void CSlotWindow::DisableCoverButton(DWORD dwIndex)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
		return;

	if (!pSlot->pCoverButton)
		return;

	pSlot->pCoverButton->Disable();
}

#ifdef ENABLE_FISH_EVENT_SYSTEM
void CSlotWindow::DeleteCoverButton(DWORD dwIndex)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
		return;

	CCoverButton *& rpCoverButton = pSlot->pCoverButton;
	if (!rpCoverButton)
		return;

	rpCoverButton->Hide();
	rpCoverButton->Disable();
	rpCoverButton->DestroyHandle();
}
#endif

void CSlotWindow::SetAlwaysRenderCoverButton(DWORD dwIndex, bool bAlwaysRender)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
		return;

	if (bAlwaysRender)
		SET_BIT(pSlot->dwState, SLOT_STATE_ALWAYS_RENDER_COVER);
	else
		REMOVE_BIT(pSlot->dwState, SLOT_STATE_ALWAYS_RENDER_COVER);
}

void CSlotWindow::ShowSlotBaseImage(DWORD dwIndex)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
		return;

	pSlot->bRenderBaseSlotImage = true;
}

void CSlotWindow::HideSlotBaseImage(DWORD dwIndex)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
		return;

	pSlot->bRenderBaseSlotImage = false;
}

BOOL CSlotWindow::IsDisableCoverButton(DWORD dwIndex)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
		return FALSE;

	if (!pSlot->pCoverButton)
		return FALSE;

	return pSlot->pCoverButton->IsDisable();
}

void CSlotWindow::SetSlotBaseImage(const char *c_szFileName, float fr, float fg, float fb, float fa)
{
	__CreateBaseImage(c_szFileName, fr, fg, fb, fa);
}

void CSlotWindow::AppendSlotButton(const char *c_szUpImageName, const char *c_szOverImageName, const char *c_szDownImageName)
{
	for (TSlotListIterator itor = m_SlotList.begin(); itor != m_SlotList.end(); ++itor)
	{
		TSlot & rSlot = *itor;
		CSlotButton *& rpSlotButton = rSlot.pSlotButton;

		if (!rpSlotButton)
		{
			rpSlotButton = new CSlotButton(CSlotButton::SLOT_BUTTON_TYPE_PLUS, rSlot.dwSlotNumber, this);
			rpSlotButton->AddFlag(FLAG_FLOAT);
			CWindowManager::Instance().SetParent(rpSlotButton, this);
		}

		rpSlotButton->SetUpVisual(c_szUpImageName);
		rpSlotButton->SetOverVisual(c_szOverImageName);
		rpSlotButton->SetDownVisual(c_szDownImageName);
		rpSlotButton->SetPosition(rSlot.ixPosition + 1, rSlot.iyPosition + 19);
		rpSlotButton->Hide();
	}
}

void CSlotWindow::AppendRequirementSignImage(const char *c_szImageName)
{
	for (TSlotListIterator itor = m_SlotList.begin(); itor != m_SlotList.end(); ++itor)
	{
		TSlot & rSlot = *itor;
		CImageBox *& rpSignImage = rSlot.pSignImage;

		if (!rpSignImage)
		{
			rpSignImage = new CImageBox(nullptr);
			CWindowManager::Instance().SetParent(rpSignImage, this);
		}

		rpSignImage->LoadImage(c_szImageName);
		rpSignImage->Hide();
	}
}

BOOL CSlotWindow::HasSlot(DWORD dwIndex)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
		return FALSE;

	return TRUE;
}

void CSlotWindow::SetSlot(DWORD dwIndex, DWORD dwVirtualNumber, BYTE byWidth, BYTE byHeight, CGraphicImage * pImage, D3DXCOLOR& diffuseColor)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
		return;

	if (pSlot->isItem)
	if (pSlot->dwItemIndex == dwVirtualNumber)
	{
		pSlot->dwState = 0;
		pSlot->isItem = TRUE;
		if (pImage && pSlot->pInstance)
		{
			pSlot->pInstance->SetImagePointer(pImage);
		}
		return;
	}

	ClearSlot(pSlot);
	pSlot->dwState = 0;
	pSlot->isItem = TRUE;
	pSlot->dwItemIndex = dwVirtualNumber;

	if (pImage)
	{
		assert(NULL == pSlot->pInstance);
		pSlot->pInstance = CGraphicImageInstance::New();
		pSlot->pInstance->SetDiffuseColor(diffuseColor.r, diffuseColor.g, diffuseColor.b, diffuseColor.a);
		pSlot->pInstance->SetScale(m_fScaleX, m_fScaleY);
		pSlot->pInstance->SetImagePointer(pImage);
	}

	pSlot->byxPlacedItemSize = byWidth;
	pSlot->byyPlacedItemSize = byHeight;

	if (pSlot->pCoverButton)
	{
		pSlot->pCoverButton->Show();
	}
}

#ifdef ENABLE_MINI_GAME
void CSlotWindow::SetCardSlot(DWORD dwIndex, DWORD dwVirtualNumber, BYTE byWidth, BYTE byHeight, const char* c_szFileName, D3DXCOLOR& diffuseColor)
{
	CGraphicImage* pImage = (CGraphicImage*)CResourceManager::Instance().GetResourcePointer(c_szFileName);
	TSlot* pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
		return;

	if (pSlot->isItem)
		if (pSlot->dwItemIndex == dwVirtualNumber)
		{
			pSlot->dwState = 0;
			pSlot->isItem = TRUE;
			if (pImage && pSlot->pInstance)
			{
				pSlot->pInstance->SetImagePointer(pImage);
			}
			return;
		}

	ClearSlot(pSlot);
	pSlot->dwState = 0;
	pSlot->isItem = TRUE;
	pSlot->dwItemIndex = dwVirtualNumber;

	if (pImage)
	{
		assert(NULL == pSlot->pInstance);
		pSlot->pInstance = CGraphicImageInstance::New();
		pSlot->pInstance->SetImagePointer(pImage);
		pSlot->pInstance->SetDiffuseColor(diffuseColor.r, diffuseColor.g, diffuseColor.b, diffuseColor.a);
	}

	pSlot->byxPlacedItemSize = byWidth;
	pSlot->byyPlacedItemSize = byHeight;

	if (pSlot->pCoverButton)
	{
		pSlot->pCoverButton->Show();
	}
}
#endif

void CSlotWindow::SetSlotCount(DWORD dwIndex, DWORD dwCount)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
		return;

	if (dwCount <= 0)
	{
		if (pSlot->pNumberLine)
		{
			delete pSlot->pNumberLine;
			pSlot->pNumberLine = nullptr;
		}
	}
	else
	{
		char szCount[16+1];
		_snprintf(szCount, sizeof(szCount), "%d", dwCount);

		if (!pSlot->pNumberLine)
		{
			CNumberLine * pNumberLine = new UI::CNumberLine(this);
			pNumberLine->SetHorizontalAlign(CNumberLine::HORIZONTAL_ALIGN_RIGHT);
#ifdef ENABLE_SLOT_CHAR_ENLARGEMENT
			pNumberLine->SetCanEnlarge(true);
#endif
			pNumberLine->Show();
			pSlot->pNumberLine = pNumberLine;
		}

#ifdef ENABLE_SLOT_CHAR_ENLARGEMENT
		pSlot->pNumberLine->RefreshSlotCharState();
#endif
		pSlot->pNumberLine->SetNumber(szCount);
	}
}

void CSlotWindow::SetSlotCountNew(DWORD dwIndex, DWORD dwGrade, DWORD dwCount)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
		return;

	if (dwCount <= 0)
	{
		if (pSlot->pNumberLine)
		{
			delete pSlot->pNumberLine;
			pSlot->pNumberLine = nullptr;
		}
	}
	else
	{
		char szCount[16+1];

		switch (dwGrade)
		{
			case 0:
				_snprintf(szCount, sizeof(szCount), "%d", dwCount);
				break;
			case 1:
				_snprintf(szCount, sizeof(szCount), "m%d", dwCount);
				break;
			case 2:
				_snprintf(szCount, sizeof(szCount), "g%d", dwCount);
				break;
			case 3:
				_snprintf(szCount, sizeof(szCount), "p");
				break;
		}

		if (!pSlot->pNumberLine)
		{
			CNumberLine * pNumberLine = new UI::CNumberLine(this);
			pNumberLine->SetHorizontalAlign(CNumberLine::HORIZONTAL_ALIGN_RIGHT);
#ifdef ENABLE_SLOT_CHAR_ENLARGEMENT
			pNumberLine->SetCanEnlarge(true);
#endif
			pNumberLine->Show();
			pSlot->pNumberLine = pNumberLine;
		}

#ifdef ENABLE_SLOT_CHAR_ENLARGEMENT
		pSlot->pNumberLine->RefreshSlotCharState();
#endif
		pSlot->pNumberLine->SetNumber(szCount);
	}
}

void CSlotWindow::SetRealSlotNumber(DWORD dwIndex, DWORD dwSlotRealNumber)
{
	TSlot* pSlot;

	if (!GetSlotPointer(dwIndex, &pSlot))
		return;

	pSlot->dwRealSlotNumber = dwSlotRealNumber;
}

void CSlotWindow::SetSlotCoolTime(DWORD dwIndex, float fCoolTime, float fElapsedTime)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
		return;

	pSlot->fCoolTime = fCoolTime;
	pSlot->fStartCoolTime = CTimer::Instance().GetCurrentSecond() - fElapsedTime;
	pSlot->bInverseCooltime = false;
}

void CSlotWindow::SetSlotCoolTimeInverse(DWORD dwIndex, float fCoolTime, float fRemainingTime)
{
	TSlot* pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
		return;

	pSlot->fCoolTime = fCoolTime;
	pSlot->fStartCoolTime = fRemainingTime;
	pSlot->bInverseCooltime = true;
}

#ifdef ENABLE_CHANGE_LOOK_SYSTEM
void CSlotWindow::SetSlotCoverImage(const DWORD dwIndex, const char* FileName)
{
	TSlot* pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
		return;

	auto& CoverImage = pSlot->pCoverImage;
	if (CoverImage == nullptr)
		CoverImage = std::make_shared<CImageBox>(nullptr);

	CoverImage->LoadImage(FileName);
	CoverImage->Show();
}

void CSlotWindow::EnableSlotCoverImage(const DWORD dwIndex, const bool bEnable)
{
	TSlot* pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
		return;

	const auto& CoverImage = pSlot->pCoverImage;
	if (CoverImage == nullptr)
		return;

	if (bEnable)
		CoverImage->Show();
	else
		CoverImage->Hide();
}
#endif

void CSlotWindow::StoreSlotCoolTime(DWORD dwKey, DWORD dwSlotIndex, float fCoolTime, float fElapsedTime)
{
	std::map<DWORD, SStoreCoolDown>::iterator it = m_CoolDownStore[dwKey].find(dwSlotIndex);
	if (it != m_CoolDownStore[dwKey].end())
	{
		it->second.fCoolTime = fCoolTime;
		it->second.fElapsedTime = CTimer::Instance().GetCurrentSecond() - fElapsedTime;
		it->second.bActive = false;
	}
	else
	{
		SStoreCoolDown m_storeCoolDown;
		m_storeCoolDown.fCoolTime = fCoolTime;
		m_storeCoolDown.fElapsedTime = CTimer::Instance().GetCurrentSecond() - fElapsedTime;
		m_storeCoolDown.bActive = false;
		m_CoolDownStore[dwKey].insert(std::map<DWORD, SStoreCoolDown>::value_type(dwSlotIndex, m_storeCoolDown));
	}
}

void CSlotWindow::RestoreSlotCoolTime(DWORD dwKey)
{
	for (std::map<DWORD, SStoreCoolDown>::iterator it = m_CoolDownStore[dwKey].begin(); it != m_CoolDownStore[dwKey].end(); it++)
	{
		TSlot * pSlot;
		if (!GetSlotPointer(it->first, &pSlot))
			return;

		D3DXCOLOR color = { 1.f, 1.f, 1.f, 1.f };

		pSlot->fCoolTime = it->second.fCoolTime;
		pSlot->fStartCoolTime = it->second.fElapsedTime;
		if (it->second.bActive)
			ActivateSlot(it->first, color);
		else
			DeactivateSlot(it->first);
	}
}

void CSlotWindow::ActivateSlot(DWORD dwIndex, int iColorType)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
		return;

	pSlot->bActive = TRUE;

	const int slot_index = MAX(0, pSlot->byyPlacedItemSize - 1);

	if (!m_pSlotActiveEffect[slot_index])
	{
		__CreateSlotEnableEffect(slot_index);
	}

	std::map<DWORD, SStoreCoolDown>::iterator it = m_CoolDownStore[1].find(dwIndex);
	if (it != m_CoolDownStore[1].end())
		it->second.bActive = true;

	switch (iColorType)
	{
		case COLOR_TYPE_ORANGE:
			pSlot->d3Color = D3DXCOLOR(1.0f, 0.34509805f, 0.035294119f, 0.5f);
			break;
		case COLOR_TYPE_RED:
			pSlot->d3Color = D3DXCOLOR(1.0f, 0.0f, 0.0f, 0.5f);
			break;
		case COLOR_TYPE_GREEN:
			pSlot->d3Color = D3DXCOLOR(0.0f, 1.0f, 0.0f, 0.5f);
			break;
		case COLOR_TYPE_YELLOW:
			pSlot->d3Color = D3DXCOLOR(1.0f, 1.0f, 0.0f, 0.5f);
			break;
		case COLOR_TYPE_BLUE:
			pSlot->d3Color = D3DXCOLOR(0.0f, 0.0f, 1.0f, 0.5f);
			break;
		case COLOR_TYPE_SKY:
			pSlot->d3Color = D3DXCOLOR(0.0f, 1.0f, 1.0f, 0.5f);
			break;
		case COLOR_TYPE_PINK:
			pSlot->d3Color = D3DXCOLOR(1.0f, 0.0f, 1.0f, 0.5f);
			break;
		case COLOR_TYPE_WHITE:
		default:
			pSlot->d3Color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);
			break;
	}
}

void CSlotWindow::DeactivateSlot(DWORD dwIndex)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
		return;

	pSlot->bActive = FALSE;

	std::map<DWORD, SStoreCoolDown>::iterator it = m_CoolDownStore[1].find(dwIndex);
	if (it != m_CoolDownStore[1].end())
		it->second.bActive = false;
}

void CSlotWindow::ClearSlot(DWORD dwIndex)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
		return;

	ClearSlot(pSlot);
}

void CSlotWindow::ClearSlot(TSlot * pSlot)
{
	pSlot->bActive = FALSE;
	pSlot->byxPlacedItemSize = 1;
	pSlot->byyPlacedItemSize = 1;

	pSlot->isItem = FALSE;
	pSlot->dwState = 0;
	pSlot->fCoolTime = 0.0f;
	pSlot->fStartCoolTime = 0.0f;
	pSlot->bInverseCooltime = false;
	pSlot->dwCenterSlotNumber = 0xffffffff;

	pSlot->d3Color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);

	pSlot->dwItemIndex = 0;
	pSlot->bRenderBaseSlotImage = true;

	if (pSlot->pInstance)
	{
		CGraphicImageInstance::Delete(pSlot->pInstance);
		pSlot->pInstance = nullptr;
	}
	if (pSlot->pCoverButton)
	{
		pSlot->pCoverButton->Hide();
	}
	if (pSlot->pSlotButton)
	{
		pSlot->pSlotButton->Hide();
	}
	if (pSlot->pSignImage)
	{
 		pSlot->pSignImage->Hide();
	}
	if (pSlot->pFinishCoolTimeEffect)
	{
		pSlot->pFinishCoolTimeEffect->Hide();
	}
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
	if (pSlot->pCoverImage)
	{
		pSlot->pCoverImage->Hide();
	}
#endif
	pSlot->d3CoolTimeColor = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.5f);
	pSlot->bHasCoolTimeColor = false;
	pSlot->iYinAndYangConnection = 0;
	if (pSlot->pSecondCoverImage)
	{
		pSlot->pSecondCoverImage->Hide();
	}
}

void CSlotWindow::ClearAllSlot()
{
	Destroy();
}

void CSlotWindow::RefreshSlot()
{
	OnRefreshSlot();

	if (IsRendering())
	{
		TSlot * pSlot;
		if (GetPickedSlotPointer(&pSlot))
		{
			OnOverOutItem();
			OnOverInItem(pSlot->dwSlotNumber);
		}
	}
}

void CSlotWindow::OnRefreshSlot()
{
}

DWORD CSlotWindow::GetSlotCount()
{
	return m_SlotList.size();
}

void CSlotWindow::GetSlotNumber(std::vector<DWORD>* _v)
{
	for (const auto& itor : m_SlotList)
		_v->emplace_back(itor.dwSlotNumber);
}

void CSlotWindow::LockSlot(DWORD dwIndex)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
		return;

	pSlot->dwState |= SLOT_STATE_LOCK;
}

void CSlotWindow::UnlockSlot(DWORD dwIndex)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
		return;

	pSlot->dwState ^= SLOT_STATE_LOCK;
}

BOOL CSlotWindow::IsLockSlot(DWORD dwIndex)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
		return FALSE;

	return (pSlot->dwState & SLOT_STATE_LOCK) ? TRUE : FALSE;
}

void CSlotWindow::SetCantUseSlot(DWORD dwIndex)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
		return;

	pSlot->dwState |= SLOT_STATE_CANT_USE;
}

void CSlotWindow::SetUseSlot(DWORD dwIndex)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
		return;

	pSlot->dwState ^= SLOT_STATE_CANT_USE;
}

void CSlotWindow::EnableSlot(DWORD dwIndex)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
		return;

	REMOVE_BIT(pSlot->dwState, SLOT_STATE_DISABLE);
}

void CSlotWindow::DisableSlot(DWORD dwIndex)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
		return;
	SET_BIT(pSlot->dwState, SLOT_STATE_DISABLE);
}

#ifdef ENABLE_SLOT_MARKING_SYSTEM
void CSlotWindow::SetCanMouseEventSlot(DWORD dwIndex)
{
	TSlot* pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
		return;

	REMOVE_BIT(pSlot->dwState, SLOT_STATE_CANT_MOUSE_EVENT);
}

void CSlotWindow::SetCantMouseEventSlot(DWORD dwIndex)
{
	TSlot* pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
		return;

	SET_BIT(pSlot->dwState, SLOT_STATE_CANT_MOUSE_EVENT);
}

void CSlotWindow::SetUsableSlotOnTopWnd(DWORD dwIndex)
{
	TSlot* pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
		return;

	REMOVE_BIT(pSlot->dwState, SLOT_STATE_UNUSABLE);
}

void CSlotWindow::SetUnusableSlotOnTopWnd(DWORD dwIndex)
{
	TSlot* pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
		return;

	SET_BIT(pSlot->dwState, SLOT_STATE_UNUSABLE);
}
#endif

void CSlotWindow::SelectSlot(DWORD dwSelectingIndex)
{
	std::list<DWORD>::iterator itor = m_dwSelectedSlotIndexList.begin();
	for (; itor != m_dwSelectedSlotIndexList.end();)
	{
		if (dwSelectingIndex == *itor)
		{
			itor = m_dwSelectedSlotIndexList.erase(itor);
			return;
		}
		else
		{
			++itor;
		}
	}

	TSlot * pSlot;
	if (GetSlotPointer(dwSelectingIndex, &pSlot))
	{
		if (!pSlot->isItem)
			return;

		m_dwSelectedSlotIndexList.push_back(dwSelectingIndex);
	}
}

BOOL CSlotWindow::isSelectedSlot(DWORD dwIndex)
{
	std::list<DWORD>::iterator itor = m_dwSelectedSlotIndexList.begin();
	for (; itor != m_dwSelectedSlotIndexList.end(); ++itor)
	{
		if (dwIndex == *itor)
			return TRUE;
	}

	return FALSE;
}

void CSlotWindow::ClearSelected()
{
	m_dwSelectedSlotIndexList.clear();
}

DWORD CSlotWindow::GetSelectedSlotCount()
{
	return m_dwSelectedSlotIndexList.size();
}

DWORD CSlotWindow::GetSelectedSlotNumber(DWORD dwIndex)
{
	if (dwIndex >= m_dwSelectedSlotIndexList.size())
		return DWORD(-1);

	DWORD dwCount = 0;
	std::list<DWORD>::iterator itor = m_dwSelectedSlotIndexList.begin();
	for (; itor != m_dwSelectedSlotIndexList.end(); ++itor)
	{
		if (dwIndex == dwCount)
			break;

		++dwCount;
	}

	return *itor;
}

void CSlotWindow::ShowSlotButton(DWORD dwSlotNumber)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwSlotNumber, &pSlot))
		return;

	if (pSlot->pSlotButton)
	{
		pSlot->pSlotButton->Show();
	}
}

void CSlotWindow::HideAllSlotButton()
{
	for (TSlotListIterator itor = m_SlotList.begin(); itor != m_SlotList.end(); ++itor)
	{
		TSlot & rSlot = *itor;

		if (rSlot.pSlotButton)
		{
			rSlot.pSlotButton->Hide();
		}
	}
}

void CSlotWindow::ShowRequirementSign(DWORD dwSlotNumber)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwSlotNumber, &pSlot))
		return;

	if (!pSlot->pSignImage)
		return;

	pSlot->pSignImage->Show();
}

void CSlotWindow::HideRequirementSign(DWORD dwSlotNumber)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwSlotNumber, &pSlot))
		return;

	if (!pSlot->pSignImage)
		return;

	pSlot->pSignImage->Hide();
}

BOOL CSlotWindow::OnMouseLeftButtonDown()
{
	TSlot * pSlot;
	if (!GetPickedSlotPointer(&pSlot))
	{
		UI::CWindowManager::Instance().DeattachIcon();
		return TRUE;
	}

#ifdef ENABLE_SLOT_MARKING_SYSTEM
	if (!(pSlot->dwState & SLOT_STATE_CANT_MOUSE_EVENT))
		if (pSlot->isItem && !(pSlot->dwState & SLOT_STATE_LOCK))
			OnSelectItemSlot(pSlot->dwSlotNumber);
		else
			OnSelectEmptySlot(pSlot->dwSlotNumber);
#else
	if (pSlot->isItem && !(pSlot->dwState & SLOT_STATE_LOCK))
		OnSelectItemSlot(pSlot->dwSlotNumber);
	else
		OnSelectEmptySlot(pSlot->dwSlotNumber);
#endif

	return TRUE;
}

BOOL CSlotWindow::OnMouseLeftButtonUp()
{
	if (UI::CWindowManager::Instance().IsAttaching())
		if (UI::CWindowManager::Instance().IsDragging())
			if (IsIn())
			{
				UI::CWindow * pWin = UI::CWindowManager::Instance().GetPointWindow();

				if (pWin)
				if (pWin->IsType(UI::CSlotWindow::Type()))
				{
					UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;

					TSlot * pSlot;
					if (!pSlotWin->GetPickedSlotPointer(&pSlot))
					{
						UI::CWindowManager::Instance().DeattachIcon();
						return TRUE;
					}

#ifdef ENABLE_SLOT_MARKING_SYSTEM
					if (!(pSlot->dwState & SLOT_STATE_CANT_MOUSE_EVENT))
						if (pSlot->isItem)
							pSlotWin->OnSelectItemSlot(pSlot->dwSlotNumber);
						else
							pSlotWin->OnSelectEmptySlot(pSlot->dwSlotNumber);
#else
					if (pSlot->isItem)
						pSlotWin->OnSelectItemSlot(pSlot->dwSlotNumber);
					else
						pSlotWin->OnSelectEmptySlot(pSlot->dwSlotNumber);
#endif

					return TRUE;
				}

				return FALSE;
			}

	return FALSE;
}

BOOL CSlotWindow::OnMouseRightButtonDown()
{
	TSlot * pSlot;
	if (!GetPickedSlotPointer(&pSlot))
		return TRUE;

#ifdef ENABLE_SLOT_MARKING_SYSTEM
	if (!(pSlot->dwState & SLOT_STATE_CANT_MOUSE_EVENT))
		if (pSlot->isItem)
			OnUnselectItemSlot(pSlot->dwSlotNumber);
		else
			OnUnselectEmptySlot(pSlot->dwSlotNumber);
#else
	if (pSlot->isItem)
		OnUnselectItemSlot(pSlot->dwSlotNumber);
	else
		OnUnselectEmptySlot(pSlot->dwSlotNumber);
#endif

	return TRUE;
}

BOOL CSlotWindow::OnMouseLeftButtonDoubleClick()
{
#ifdef ENABLE_SLOT_MARKING_SYSTEM
	TSlot * pSlot;
	if (GetPickedSlotPointer(&pSlot))
		if (!(pSlot->dwState & SLOT_STATE_CANT_MOUSE_EVENT))
			OnUseSlot();
#else
	OnUseSlot();
#endif

	return TRUE;
}

void CSlotWindow::OnMouseOverOut()
{
	OnOverOutItem();
}

void CSlotWindow::OnMouseOver()
{
	CWindow * pPointWindow = UI::CWindowManager::Instance().GetPointWindow();
	if (this == pPointWindow)
	{
		TSlot * pSlot;
		if (GetPickedSlotPointer(&pSlot))
		{
			if (OnOverInItem(pSlot->dwSlotNumber))
				return;
		}
	}

	OnOverOutItem();
}

void CSlotWindow::OnSelectEmptySlot(int iSlotNumber)
{
	PyCallClassMemberFunc(m_poHandler, "OnSelectEmptySlot", Py_BuildValue("(i)", iSlotNumber));
}
void CSlotWindow::OnSelectItemSlot(int iSlotNumber)
{
	PyCallClassMemberFunc(m_poHandler, "OnSelectItemSlot", Py_BuildValue("(i)", iSlotNumber));

	if (UI::CWindowManager::Instance().IsAttaching())
		OnOverOutItem();
}

void CSlotWindow::OnUnselectEmptySlot(int iSlotNumber)
{
	PyCallClassMemberFunc(m_poHandler, "OnUnselectEmptySlot", Py_BuildValue("(i)", iSlotNumber));
}

void CSlotWindow::OnUnselectItemSlot(int iSlotNumber)
{
	PyCallClassMemberFunc(m_poHandler, "OnUnselectItemSlot", Py_BuildValue("(i)", iSlotNumber));
}

void CSlotWindow::OnUseSlot()
{
	TSlot * pSlot;
	if (GetPickedSlotPointer(&pSlot))
	if (pSlot->isItem)
	{
		PyCallClassMemberFunc(m_poHandler, "OnUseSlot", Py_BuildValue("(i)", pSlot->dwSlotNumber));
	}
}

BOOL CSlotWindow::OnOverInItem(DWORD dwSlotNumber)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwSlotNumber, &pSlot))
		return FALSE;

	if (!pSlot->isItem)
	{
		// empty slot: let python show what goes there (inventory equipment slots); left via OnOverOutItem
		if (pSlot->dwSlotNumber != m_dwToolTipSlotNumber)
		{
			OnOverOutItem();	// hide the tooltip of the item slot we came from
			m_dwToolTipSlotNumber = dwSlotNumber;
			PyCallClassMemberFunc(m_poHandler, "OnOverInEmptySlot", Py_BuildValue("(i)", dwSlotNumber));
		}
		return TRUE;
	}

	if (pSlot->dwSlotNumber == m_dwToolTipSlotNumber)
		return TRUE;

	m_dwToolTipSlotNumber = dwSlotNumber;
	PyCallClassMemberFunc(m_poHandler, "OnOverInItem", Py_BuildValue("(i)", dwSlotNumber));

	return TRUE;
}

void CSlotWindow::OnOverOutItem()
{
	if (SLOT_NUMBER_NONE == m_dwToolTipSlotNumber)
		return;

	m_dwToolTipSlotNumber = SLOT_NUMBER_NONE;
	PyCallClassMemberFunc(m_poHandler, "OnOverOutItem", Py_BuildValue("()"));
}

void CSlotWindow::OnPressedSlotButton(DWORD dwType, DWORD dwSlotNumber, BOOL isLeft)
{
	if (CSlotButton::SLOT_BUTTON_TYPE_PLUS == dwType)
	{
		PyCallClassMemberFunc(m_poHandler, "OnPressedSlotButton", Py_BuildValue("(i)", dwSlotNumber));
	}
	else if (CSlotButton::SLOT_BUTTON_TYPE_COVER == dwType)
	{
		if (isLeft)
		{
			OnMouseLeftButtonDown();
		}
	}
}

void CSlotWindow::OnUpdate()
{
	for (std::deque<DWORD>::iterator itor = m_ReserveDestroyEffectDeque.begin(); itor != m_ReserveDestroyEffectDeque.end(); ++itor)
	{
		DWORD dwSlotIndex = *itor;

		TSlot * pSlot;
		if (!GetSlotPointer(dwSlotIndex, &pSlot))
			continue;

		__DestroyFinishCoolTimeEffect(pSlot);
	}
	m_ReserveDestroyEffectDeque.clear();

	for (int i = 0; i < SLOT_ACTIVE_EFFECT_COUNT; i++)
	{
		if (m_pSlotActiveEffect[i])
			m_pSlotActiveEffect[i]->Update();
	}
}

void CSlotWindow::OnRender()
{
	RenderSlotBaseImage();

	switch (m_dwSlotStyle)
	{
		case SLOT_STYLE_PICK_UP:
			OnRenderPickingSlot();
			break;
		case SLOT_STYLE_SELECT:
			OnRenderSelectedSlot();
			break;
	}

	std::for_each(m_pChildList.begin(), m_pChildList.end(), std::mem_fn(&CWindow::OnRender));
	TSlotListIterator itor;

	for (itor = m_SlotList.begin(); itor != m_SlotList.end(); ++itor)
	{
		TSlot & rSlot = *itor;

		if (rSlot.pSlotButton)
		{
			rSlot.pSlotButton->SetPosition(rSlot.ixPosition + 1, rSlot.iyPosition + 19);
		}
		if (rSlot.pSignImage)
		{
			rSlot.pSignImage->SetPosition(rSlot.ixPosition - 7, rSlot.iyPosition + 10);
		}

		if (rSlot.pInstance)
		{
			rSlot.pInstance->SetPosition(m_rect.left + rSlot.ixPosition, m_rect.top + rSlot.iyPosition);
			rSlot.pInstance->Render();
		}

		if (!rSlot.isItem)
		{
			if (IS_SET(rSlot.dwState, SLOT_STATE_ALWAYS_RENDER_COVER))
			{
				rSlot.pCoverButton->Show();
				rSlot.pCoverButton->SetPosition(rSlot.ixPosition, rSlot.iyPosition);
				rSlot.pCoverButton->RenderButton();
			}

			continue;
		}

		if (IS_SET(rSlot.dwState, SLOT_STATE_DISABLE))
		{
			CPythonGraphic::Instance().SetDiffuseColor(1.0f, 0.0f, 0.0f, 0.3f);
			CPythonGraphic::Instance().RenderBar2d(m_rect.left + rSlot.ixPosition,
				m_rect.top + rSlot.iyPosition,
				m_rect.left + rSlot.ixPosition + rSlot.ixCellSize,
				m_rect.top + rSlot.iyPosition + rSlot.iyCellSize);
		}

#ifdef ENABLE_SLOT_MARKING_SYSTEM
		if (IS_SET(rSlot.dwState, SLOT_STATE_CANT_MOUSE_EVENT))
		{
			CPythonGraphic::Instance().SetDiffuseColor(1.0f, 0.0f, 0.0f, 0.3f);
			CPythonGraphic::Instance().RenderBar2d(m_rect.left + rSlot.ixPosition,
				m_rect.top + rSlot.iyPosition,
				m_rect.left + rSlot.ixPosition + rSlot.byxPlacedItemSize * ITEM_WIDTH,
				m_rect.top + rSlot.iyPosition + rSlot.byyPlacedItemSize * ITEM_HEIGHT);
		}

		if (IS_SET(rSlot.dwState, SLOT_STATE_UNUSABLE))
		{
			CPythonGraphic::Instance().SetDiffuseColor(1.0f, 1.0f, 1.0f, 0.3f);
			CPythonGraphic::Instance().RenderBar2d(m_rect.left + rSlot.ixPosition,
				m_rect.top + rSlot.iyPosition,
				m_rect.left + rSlot.ixPosition + rSlot.byxPlacedItemSize * ITEM_WIDTH,
				m_rect.top + rSlot.iyPosition + rSlot.byyPlacedItemSize * ITEM_HEIGHT);
		}
#endif

		if (rSlot.fCoolTime != 0.0f)
		{
			float fScaleX = 1.0f, fScaleY = 1.0f;
			if (rSlot.pInstance)
			{
				fScaleX = rSlot.pInstance->GetScale().x;
				fScaleY = rSlot.pInstance->GetScale().y;
			}

			float fXCenter = (m_rect.left + rSlot.ixPosition) + 16.0f * fScaleX;
			float fYCenter = (m_rect.top + rSlot.iyPosition) + 16.0f * fScaleY;
			float fRadius = 16.0f * fScaleX;

			if (!rSlot.bInverseCooltime)
			{
				float fcurTime = CTimer::Instance().GetCurrentSecond();
				float fPercentage = (fcurTime - rSlot.fStartCoolTime) / rSlot.fCoolTime;


				if (rSlot.bHasCoolTimeColor)
					CPythonGraphic::Instance().RenderCoolTimeBox(fXCenter, fYCenter, fRadius, fPercentage, rSlot.d3CoolTimeColor);
				else
					CPythonGraphic::Instance().RenderCoolTimeBox(fXCenter, fYCenter, fRadius, fPercentage);

				if (fcurTime - rSlot.fStartCoolTime >= rSlot.fCoolTime)
				{
					if ((fcurTime - rSlot.fStartCoolTime) - rSlot.fCoolTime < 1.0f)
						__CreateFinishCoolTimeEffect(&rSlot);

					rSlot.fCoolTime = 0.0f;
					rSlot.fStartCoolTime = 0.0f;
				}
			}
			else
			{
				float fPercentage = rSlot.fStartCoolTime / rSlot.fCoolTime;
				CPythonGraphic::Instance().RenderCoolTimeBoxInverse(fXCenter, fYCenter, fRadius, fPercentage);
			}
		}

		if (rSlot.pCoverButton)
		{
			rSlot.pCoverButton->SetPosition(rSlot.ixPosition, rSlot.iyPosition);
			rSlot.pCoverButton->RenderButton();
		}

		if (rSlot.pNumberLine)
		{
#ifdef ENABLE_SLOT_CHAR_ENLARGEMENT
			rSlot.pNumberLine->RefreshSlotCharState();
#endif
			int ix = rSlot.byxPlacedItemSize*ITEM_WIDTH + rSlot.ixPosition - 4;
			int iy = rSlot.iyPosition + rSlot.byyPlacedItemSize*ITEM_HEIGHT - 12 + 2;
			rSlot.pNumberLine->SetPosition(ix, iy);
			rSlot.pNumberLine->Update();
			rSlot.pNumberLine->Render();
		}

		if (rSlot.pFinishCoolTimeEffect)
		{
			rSlot.pFinishCoolTimeEffect->SetPosition(rSlot.ixPosition, rSlot.iyPosition);
			rSlot.pFinishCoolTimeEffect->Update();
			rSlot.pFinishCoolTimeEffect->Render();
		}

#ifdef ENABLE_CHANGE_LOOK_SYSTEM
		if (rSlot.pCoverImage)
		{
			rSlot.pCoverImage->SetPosition(m_rect.left + rSlot.ixPosition, m_rect.top + rSlot.iyPosition);
			rSlot.pCoverImage->Render();
		}
#endif

		if (rSlot.pSecondCoverImage && rSlot.pSecondCoverImage->IsShow())
		{
			rSlot.pSecondCoverImage->SetPosition(m_rect.left + rSlot.ixPosition, m_rect.top + rSlot.iyPosition);
			rSlot.pSecondCoverImage->Render();
		}

		if (rSlot.bActive)
		{
			const int slot_index = MAX(0, rSlot.byyPlacedItemSize - 1);
			if (m_pSlotActiveEffect[slot_index])
			{
				const int ix = m_rect.left + rSlot.ixPosition;
				const int iy = m_rect.top + rSlot.iyPosition;
				m_pSlotActiveEffect[slot_index]->SetPosition(ix, iy);
				m_pSlotActiveEffect[slot_index]->SetDiffuseColor(rSlot.d3Color.r, rSlot.d3Color.g, rSlot.d3Color.b, rSlot.d3Color.a);
				m_pSlotActiveEffect[slot_index]->Render();
			}
		}
	}

	RenderLockedSlot();
}

void CSlotWindow::RenderSlotBaseImage()
{
	if (!m_pBaseImageInstance)
		return;

	for (TSlotListIterator itor = m_SlotList.begin(); itor != m_SlotList.end(); ++itor)
	{
		TSlot & rSlot = *itor;

		if (!rSlot.bRenderBaseSlotImage)
			continue;

		m_pBaseImageInstance->SetPosition(m_rect.left + rSlot.ixPosition, m_rect.top + rSlot.iyPosition);
		m_pBaseImageInstance->Render();
	}
}

void CSlotWindow::OnRenderPickingSlot()
{
	if (!UI::CWindowManager::Instance().IsAttaching())
		return;

	TSlot * pSlot;
	if (!GetSelectedSlotPointer(&pSlot))
		return;

	CPythonGraphic::Instance().SetDiffuseColor(1.0f, 1.0f, 1.0f, 0.5f);
	CPythonGraphic::Instance().RenderBar2d(m_rect.left + pSlot->ixPosition,
											m_rect.top + pSlot->iyPosition,
											m_rect.left + pSlot->ixPosition + pSlot->ixCellSize,
											m_rect.top + pSlot->iyPosition + pSlot->iyCellSize);
}

void CSlotWindow::OnRenderSelectedSlot()
{
	std::list<DWORD>::iterator itor = m_dwSelectedSlotIndexList.begin();
	for (; itor != m_dwSelectedSlotIndexList.end(); ++itor)
	{
		TSlot * pSlot;
		if (!GetSlotPointer(*itor, &pSlot))
			continue;

		CPythonGraphic::Instance().SetDiffuseColor(1.0f, 1.0f, 1.0f, 0.5f);
		CPythonGraphic::Instance().RenderBar2d(m_rect.left + pSlot->ixPosition,
												m_rect.top + pSlot->iyPosition,
												m_rect.left + pSlot->ixPosition + pSlot->ixCellSize,
												m_rect.top + pSlot->iyPosition + pSlot->iyCellSize);
	}
}

void CSlotWindow::RenderLockedSlot()
{
	CPythonGraphic::Instance().SetDiffuseColor(0.0f, 0.0f, 0.0f, 0.5f);
	for (TSlotListIterator itor = m_SlotList.begin(); itor != m_SlotList.end(); ++itor)
	{
		TSlot & rSlot = *itor;

		if (!rSlot.isItem)
			continue;

		if (rSlot.dwState & SLOT_STATE_LOCK)
		{
			CPythonGraphic::Instance().RenderBar2d(m_rect.left + rSlot.ixPosition,
												   m_rect.top  + rSlot.iyPosition,
												   m_rect.left + rSlot.ixPosition + rSlot.ixCellSize,
												   m_rect.top  + rSlot.iyPosition + rSlot.iyCellSize);
		}
	}
}

BOOL CSlotWindow::GetSlotPointer(DWORD dwIndex, TSlot ** ppSlot)
{
	for (TSlotListIterator itor = m_SlotList.begin(); itor != m_SlotList.end(); ++itor)
	{
		TSlot & rSlot = *itor;

		if (dwIndex == rSlot.dwSlotNumber)
		{
			*ppSlot = &rSlot;
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CSlotWindow::GetSelectedSlotPointer(TSlot ** ppSlot)
{
	long lx, ly;
	GetMouseLocalPosition(lx, ly);

	for (TSlotListIterator itor = m_SlotList.begin(); itor != m_SlotList.end(); ++itor)
	{
		TSlot & rSlot = *itor;

		if (lx >= rSlot.ixPosition)
		if (ly >= rSlot.iyPosition)
		if (lx <= rSlot.ixPosition + rSlot.ixCellSize)
		if (ly <= rSlot.iyPosition + rSlot.iyCellSize)
		{
			*ppSlot = &rSlot;
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CSlotWindow::GetPickedSlotPointer(TSlot ** ppSlot)
{
	long lx, ly;
	CWindowManager::Instance().GetMousePosition(lx, ly);

	int ixLocal = lx - m_rect.left;
	int iyLocal = ly - m_rect.top;

	// NOTE : ¿?ÂÊ ¸ÇÀ§ »ó´Ü ÇÑ°÷ÀÌ ±âÁØ ÀÌ¶ó´Â Á¡À» ÀÌ¿ëÇØ ¿?ÂÊ À§¿¡¼­ºÎÅÍ ¿À¸¥ÂÊ ¾Æ·¡·Î
	//        Â÷·Ê·Î °Ë»öÇØ °¨À¸·Î½á µ¤Çô ÀÖ´Â SlotÀº ÀÚµ¿ ¹«½Ã µÈ´Ù´Â Æ¯¼ºÀ» ÀÌ¿ëÇÑ´Ù. - [levites]
	for (TSlotListIterator itor = m_SlotList.begin(); itor != m_SlotList.end(); ++itor)
	{
		TSlot & rSlot = *itor;

		int ixCellSize = rSlot.ixCellSize;
		int iyCellSize = rSlot.iyCellSize;

		// NOTE : ItemÀÌ Hide µÇ¾îÀÖÀ» °æ¿ì¸¦ À§ÇÑ..
		if (rSlot.isItem)
		{
			ixCellSize = max(rSlot.ixCellSize, int(rSlot.byxPlacedItemSize * ITEM_WIDTH));
			iyCellSize = max(rSlot.iyCellSize, int(rSlot.byyPlacedItemSize * ITEM_HEIGHT));
		}

		if (ixLocal >= rSlot.ixPosition)
		if (iyLocal >= rSlot.iyPosition)
		if (ixLocal <= rSlot.ixPosition + ixCellSize)
		if (iyLocal <= rSlot.iyPosition + iyCellSize)
		{
			*ppSlot = &rSlot;
			return TRUE;
		}
	}

	return FALSE;
}

void CSlotWindow::SetUseMode(BOOL bFlag)
{
	m_isUseMode = bFlag;
}

void CSlotWindow::SetUsableItem(BOOL bFlag)
{
	m_isUsableItem = bFlag;
}

void CSlotWindow::ReserveDestroyCoolTimeFinishEffect(DWORD dwSlotIndex)
{
	m_ReserveDestroyEffectDeque.push_back(dwSlotIndex);
}

DWORD CSlotWindow::Type()
{
	static int s_Type = GetCRC32("CSlotWindow", strlen("CSlotWindow"));
	return s_Type;
}

BOOL CSlotWindow::OnIsType(DWORD dwType)
{
	if (CSlotWindow::Type() == dwType)
		return TRUE;

	return CWindow::OnIsType(dwType);
}

void CSlotWindow::__CreateToggleSlotImage()
{
	__DestroyToggleSlotImage();

	m_pToggleSlotImage = new CImageBox(nullptr);
	m_pToggleSlotImage->LoadImage("d:/ymir work/ui/public/slot_toggle.sub");
	m_pToggleSlotImage->Show();
}

void CSlotWindow::__CreateSlotEnableEffect(int index)
{
	__DestroySlotEnableEffect(index);
	m_pSlotActiveEffect[index] = new CAniImageBox(nullptr);

	char path[128];
	for (int i = 0; i <= 12; i++)
	{
		if (index == 0)
			snprintf(path, sizeof(path), "d:/ymir work/ui/public/slotactiveeffect/%02d.sub", i);
		else
			snprintf(path, sizeof(path), "d:/ymir work/ui/public/slotactiveeffect/slot%d/%02d.sub", (index + 1), i);

		m_pSlotActiveEffect[index]->AppendImage(path);
	}
	m_pSlotActiveEffect[index]->SetRenderingMode(CGraphicExpandedImageInstance::RENDERING_MODE_SCREEN);
	m_pSlotActiveEffect[index]->ResetFrame();
	m_pSlotActiveEffect[index]->SetDelay(4);
	m_pSlotActiveEffect[index]->Show();
}

void CSlotWindow::__CreateFinishCoolTimeEffect(TSlot * pSlot)
{
	__DestroyFinishCoolTimeEffect(pSlot);

	CAniImageBox * pFinishCoolTimeEffect = new CCoolTimeFinishEffect(this, pSlot->dwSlotNumber);

	char path[128];
	for (int i = 0; i <= 11; i++)
	{
		snprintf(path, sizeof(path), "d:/ymir work/ui/public/slotfinishcooltimeeffect/%02d.sub", i);
		pFinishCoolTimeEffect->AppendImage(path);
	}

	pFinishCoolTimeEffect->SetRenderingMode(CGraphicExpandedImageInstance::RENDERING_MODE_SCREEN);
	pFinishCoolTimeEffect->ResetFrame();
	pFinishCoolTimeEffect->SetDelay(2);
	pFinishCoolTimeEffect->Show();

	pSlot->pFinishCoolTimeEffect = pFinishCoolTimeEffect;
}

void CSlotWindow::__CreateBaseImage(const char *c_szFileName, float fr, float fg, float fb, float fa)
{
	__DestroyBaseImage();

	CGraphicImage * pImage = (CGraphicImage *)CResourceManager::Instance().GetResourcePointer(c_szFileName);
	m_pBaseImageInstance = CGraphicImageInstance::New();
	m_pBaseImageInstance->SetImagePointer(pImage);
	m_pBaseImageInstance->SetDiffuseColor(fr, fg, fb, fa);
	m_pBaseImageInstance->SetScale(m_fScaleX, m_fScaleY);
}

void CSlotWindow::__DestroyToggleSlotImage()
{
	if (m_pToggleSlotImage)
	{
		delete m_pToggleSlotImage;
		m_pToggleSlotImage = nullptr;
	}
}

void CSlotWindow::__DestroySlotEnableEffect(int index)
{
	if (m_pSlotActiveEffect[index])
	{
		delete m_pSlotActiveEffect[index];
		m_pSlotActiveEffect[index] = nullptr;
	}
}

void CSlotWindow::__DestroyFinishCoolTimeEffect(TSlot * pSlot)
{
	if (pSlot->pFinishCoolTimeEffect)
	{
		delete pSlot->pFinishCoolTimeEffect;
		pSlot->pFinishCoolTimeEffect = nullptr;
	}
}

void CSlotWindow::__DestroyBaseImage()
{
	if (m_pBaseImageInstance)
	{
		CGraphicImageInstance::Delete(m_pBaseImageInstance);
		m_pBaseImageInstance = nullptr;
	}
}

void CSlotWindow::__Initialize()
{
	m_dwSlotType = 0;
	m_dwSlotStyle = SLOT_STYLE_PICK_UP;
	m_dwToolTipSlotNumber = SLOT_NUMBER_NONE;
	m_CoolDownStore.clear();

	m_isUseMode = FALSE;
	m_isUsableItem = FALSE;
	m_bAlwaysUsableSlotMode = false;

	m_pToggleSlotImage = nullptr;

	for (int i = 0; i < SLOT_ACTIVE_EFFECT_COUNT; i++)
		m_pSlotActiveEffect[i] = nullptr;

	m_pBaseImageInstance = nullptr;

	m_fScaleX = 1.0f;
	m_fScaleY = 1.0f;
}

void CSlotWindow::Destroy()
{
	for (TSlotListIterator itor = m_SlotList.begin(); itor != m_SlotList.end(); ++itor)
	{
		TSlot & rSlot = *itor;

		ClearSlot(&rSlot);

		if (rSlot.pNumberLine)
		{
			delete rSlot.pNumberLine;
			rSlot.pNumberLine = nullptr;
		}
		if (rSlot.pCoverButton)
		{
			CWindowManager::Instance().DestroyWindow(rSlot.pCoverButton);
		}
		if (rSlot.pSlotButton)
		{
			CWindowManager::Instance().DestroyWindow(rSlot.pSlotButton);
		}
		if (rSlot.pSignImage)
		{
			CWindowManager::Instance().DestroyWindow(rSlot.pSignImage);
		}
		if (rSlot.pFinishCoolTimeEffect)
		{
			CWindowManager::Instance().DestroyWindow(rSlot.pFinishCoolTimeEffect);
		}
	}

	m_SlotList.clear();

	__DestroyToggleSlotImage();

	for (int i = 0; i < SLOT_ACTIVE_EFFECT_COUNT; i++)
		__DestroySlotEnableEffect(i);

	__DestroyBaseImage();

	__Initialize();
}

CSlotWindow::CSlotWindow(PyObject *ppyObject) : CWindow(ppyObject)
{
	__Initialize();
}

CSlotWindow::~CSlotWindow()
{
	Destroy();
}

BOOL CSlotWindow::IsActiveSlot(DWORD dwIndex)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
		return FALSE;

	return pSlot->bActive;
}

DWORD CSlotWindow::GetSlotItemVnum(DWORD dwIndex)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
		return 0;

	return pSlot->dwItemIndex;
}

DWORD CSlotWindow::GetSlotItemCount(DWORD dwIndex)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
		return 0;

	if (!pSlot->pNumberLine)
		return pSlot->isItem ? 1 : 0;

	const std::string & strNumber = pSlot->pNumberLine->GetNumber();
	if (strNumber.empty())
		return pSlot->isItem ? 1 : 0;

	return strtoul(strNumber.c_str(), nullptr, 10);
}

BOOL CSlotWindow::GetSlotLocalPosition(DWORD dwIndex, int * px, int * py)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
		return FALSE;

	if (px)
		*px = pSlot->ixPosition;
	if (py)
		*py = pSlot->iyPosition;

	return TRUE;
}

BOOL CSlotWindow::GetSlotGlobalPosition(DWORD dwIndex, int * px, int * py)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
		return FALSE;

	if (px)
		*px = m_rect.left + pSlot->ixPosition;
	if (py)
		*py = m_rect.top + pSlot->iyPosition;

	return TRUE;
}

void CSlotWindow::ResetSlotCoolTime(DWORD dwIndex)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
		return;

	pSlot->fCoolTime = 0.0f;
	pSlot->fStartCoolTime = 0.0f;
	pSlot->bInverseCooltime = false;
}

void CSlotWindow::SetSlotCoolTimeColor(DWORD dwIndex, float fr, float fg, float fb, float fa)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
		return;

	pSlot->d3CoolTimeColor = D3DXCOLOR(fr, fg, fb, fa);
	pSlot->bHasCoolTimeColor = true;
}

void CSlotWindow::SetSlotDiffuseColor(DWORD dwIndex, int iColorType)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
		return;

	switch (iColorType)
	{
		case 0:
			pSlot->d3Color = D3DXCOLOR(1.0f, 0.34509805f, 0.035294119f, 0.5f);
			break;
		case 1:
			pSlot->d3Color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);
			break;
		case 2:
			pSlot->d3Color = D3DXCOLOR(1.0f, 0.0f, 0.0f, 0.5f);
			break;
		case 3:
			pSlot->d3Color = D3DXCOLOR(0.0f, 1.0f, 0.0f, 0.5f);
			break;
		case 4:
			pSlot->d3Color = D3DXCOLOR(1.0f, 1.0f, 0.0f, 0.5f);
			break;
		case 5:
			pSlot->d3Color = D3DXCOLOR(0.0f, 1.0f, 1.0f, 0.5f);
			break;
		case 6:
			pSlot->d3Color = D3DXCOLOR(1.0f, 0.0f, 1.0f, 0.5f);
			break;
		case 7:
			pSlot->d3Color = D3DXCOLOR(1.0f, 0.53f, 0.12f, 0.5f);
			break;
		case 8:
			pSlot->d3Color = D3DXCOLOR(0.27f, 0.78f, 1.0f, 0.5f);
			break;
		default:
			pSlot->d3Color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);
			break;
	}
}

void CSlotWindow::SetSlotImage(DWORD dwIndex, const char * c_szFileName)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
		return;

	CGraphicImage * pImage = (CGraphicImage *)CResourceManager::Instance().GetResourcePointer(c_szFileName);
	if (!pImage)
		return;

	if (!pSlot->pInstance)
	{
		pSlot->pInstance = CGraphicImageInstance::New();
		pSlot->pInstance->SetScale(m_fScaleX, m_fScaleY);
	}

	pSlot->pInstance->SetImagePointer(pImage);
}

void CSlotWindow::SetSecondSlotCoverImage(const DWORD dwIndex, const char * c_szFileName)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
		return;

	if (!pSlot->pSecondCoverImage)
	{
		pSlot->pSecondCoverImage = std::make_shared<CImageBox>(nullptr);
		CWindowManager::Instance().SetParent(pSlot->pSecondCoverImage.get(), this);
	}

	pSlot->pSecondCoverImage->LoadImage(c_szFileName);
}

void CSlotWindow::EnableSecondSlotCoverImage(const DWORD dwIndex, const bool bEnable)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
		return;

	if (pSlot->pSecondCoverImage)
	{
		if (bEnable)
			pSlot->pSecondCoverImage->Show();
		else
			pSlot->pSecondCoverImage->Hide();
	}
}

void CSlotWindow::SetSlotConnectedYinAndYang(DWORD dwIndex, int iType)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
		return;

	pSlot->iYinAndYangConnection = iType;
}

void CSlotWindow::ClearSlotYinAndYangConnection(DWORD dwIndex)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
		return;

	pSlot->iYinAndYangConnection = 0;
}

void CSlotWindow::HideSlotButton(DWORD dwSlotNumber)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwSlotNumber, &pSlot))
		return;

	if (pSlot->pSlotButton)
		pSlot->pSlotButton->Hide();
}

void CSlotWindow::SetCoverButtonScale(DWORD dwIndex, float fx, float fy)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
		return;

	if (pSlot->pCoverButton)
		pSlot->pCoverButton->SetScale(fx, fy);
}

void CSlotWindow::SetBaseImageScale(float fx, float fy)
{
	if (m_pBaseImageInstance)
		m_pBaseImageInstance->SetScale(fx, fy);
}

