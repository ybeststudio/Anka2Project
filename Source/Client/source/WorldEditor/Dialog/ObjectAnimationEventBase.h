#pragma once

#include "../DataCtrl/ObjectAnimationAccessor.h"

class CObjectAnimationEventBase : public CDialog
{
	public:
		CObjectAnimationEventBase (UINT nIDTemplate, CWnd* pParentWnd = NULL) : CDialog (nIDTemplate, pParentWnd) {}
		virtual ~CObjectAnimationEventBase() {}

		virtual BOOL Create (CWnd * pParent, const CRect & c_rRect) = 0;
		virtual void Close() = 0;

		virtual void GetData (CRaceMotionDataAccessor::TMotionEventData * pData) = 0;
		virtual void SetData (const CRaceMotionDataAccessor::TMotionEventData * c_pData) = 0;
};