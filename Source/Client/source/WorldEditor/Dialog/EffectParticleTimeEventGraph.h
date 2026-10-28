#if !defined(AFX_EFFECTTIMEEVENTGRAPH_H__FEB08EC0_B265_4D09_9AC0_0D2C438B35EB__INCLUDED_)
#define AFX_EFFECTTIMEEVENTGRAPH_H__FEB08EC0_B265_4D09_9AC0_0D2C438B35EB__INCLUDED_

#pragma once

// EffectTimeBarGraph.h : header file
//

#include "GraphCtrl.h"

class CFloatEditDialog;

/////////////////////////////////////////////////////////////////////////////
// CEffectTimeEventGraph window

class CEffectTimeEventGraph : public CStatic
{
	public:
		class IValueAccessor
		{
			public:
				IValueAccessor() {}
				virtual ~IValueAccessor() {}

				virtual DWORD GetValueCount() = 0;

				virtual void Insert (float fTime, float fValue) = 0;
				virtual void Delete (DWORD dwIndex) = 0;

				virtual bool GetTime (DWORD dwIndex, float* pfTime) = 0;
				virtual bool GetValue (DWORD dwIndex, float* pfValue) = 0;

				virtual void SetTime (DWORD dwIndex, float fTime) = 0;
				virtual void SetValue (DWORD dwIndex, float fValue) = 0;
		};

		typedef struct SPoint
		{
			SPoint (int ix_ = 0, int iy_ = 0) : ix (ix_), iy (iy_) {}

			int ix;
			int iy;
		} TPoint;
		typedef std::vector<TPoint> TPointVector;

		enum
		{
			POINT_NONE = 0xffffffff,

			MAX_GRAPH_COUNT = 3,
		};

		// Construction
	public:
		CEffectTimeEventGraph();
		void Initialize (int iTimerID);

		// Attributes
	public:

		// Operations
	public:
		void Resizing (int iWidth, int iHeight);
		void SetAccessorPointer (IValueAccessor * pAccessor);

		void SetMaxTime (float fMaxTime);
		void SetMaxValue (float fMaxValue);
		void SetStartValue (float fStartValue);

	protected:
		void RenderGrid();
		void RenderGraph();
		void ConnectPoint (TPoint & rLeft, TPoint & rRight);

		bool isCreatingMode();

		void TimeToScreen (float fTime, int* px);
		void ScreenToTime (int ix, float* pfTime);

		void ValueToScreen (float fValue, int* piy);
		void ScreenToValue (int iy, float* pfValue);

		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CEffectTimeEventGraph)
		//}}AFX_VIRTUAL

		// Implementation
	public:
		virtual ~CEffectTimeEventGraph();

		// Generated message map functions
	protected:
		//{{AFX_MSG(CEffectTimeEventGraph)
		afx_msg void OnLButtonDown (UINT nFlags, CPoint point);
		afx_msg void OnLButtonUp (UINT nFlags, CPoint point);
		afx_msg void OnMouseMove (UINT nFlags, CPoint point);
		afx_msg void OnRButtonDown (UINT nFlags, CPoint point);
		afx_msg void OnRButtonUp (UINT nFlags, CPoint point);
		afx_msg void OnTimer (UINT_PTR nIDEvent);
		afx_msg void OnMButtonDown (UINT nFlags, CPoint point);
		afx_msg void OnMButtonUp (UINT nFlags, CPoint point);
		afx_msg void OnLButtonDblClk (UINT nFlags, CPoint point);
		//}}AFX_MSG

		DECLARE_MESSAGE_MAP()

	protected:
		// Mouse
		bool m_isLButtonDown;
		bool m_isMButtonDown;
		CPoint m_LastPoint;

		float m_fMaxTime;
		float m_fMaxValue;
		float m_fStartValue;

		DWORD m_dwSelectedTableIndex;
		DWORD m_dwSelectedIndex;
		DWORD m_dwGrippedIndex;

		int m_iWidth;
		int m_iHeight;
		int m_ixTemporary;
		int m_iyTemporary;
		int m_ixGridCount;
		int m_iyGridCount;
		float m_fxGridStep;
		float m_fyGridStep;

		IValueAccessor* m_pAccessor;
		TPointVector m_PointVector;

		CScreen m_Screen;

		static CFloatEditDialog* ms_pFloatEditDialog;
};

/////////////////////////////////////////////////////////////////////////////
// template class for IValueAccessor


template <typename T>
class CTimeEventTableAccessor : public CEffectTimeEventGraph::IValueAccessor
{
	public:
		CTimeEventTableAccessor() {}
		~CTimeEventTableAccessor() {}

		typedef std::vector<CTimeEvent<T>> TTimeEventTableType;

		TTimeEventTableType* m_pTimeEventTable;

		void SetTablePointer (TTimeEventTableType * pTimeEventTable)
		{
			m_pTimeEventTable = pTimeEventTable;
		}

		DWORD GetValueCount()
		{
			return m_pTimeEventTable->size();
		}

		void GetTimeValue (float fTime, T * pfValue)
		{
			//m_pTimeEventTable->
			GetTimeEventBlendValue (fTime, *m_pTimeEventTable, pfValue);
		}

		bool GetTime (DWORD dwIndex, float* pfTime)
		{
			if (dwIndex >= m_pTimeEventTable->size())
			{
				return false;
			}

			*pfTime = m_pTimeEventTable->at (dwIndex).m_fTime;

			return true;
		}

		bool GetValue (DWORD dwIndex, T * pfValue)
		{
			if (dwIndex >= m_pTimeEventTable->size())
			{
				return false;
			}

			*pfValue = m_pTimeEventTable->at (dwIndex).m_Value;

			return true;
		}

		void SetTime (DWORD dwIndex, float fTime)
		{
			if (dwIndex >= m_pTimeEventTable->size())
			{
				return;
			}

			m_pTimeEventTable->at (dwIndex).m_fTime = fTime;
		}
		void SetValue (DWORD dwIndex, T fValue)
		{
			if (dwIndex >= m_pTimeEventTable->size())
			{
				return;
			}

			m_pTimeEventTable->at (dwIndex).m_Value = fValue;
		}

		void Insert (float fTime, T fValue)
		{
			InsertItemTimeEvent (m_pTimeEventTable, fTime, fValue);
		}

		void InsertBlend (float fTime)
		{
			T fValue;
			GetTimeValue (fTime, &fValue);
			Insert (fTime, fValue);
		}

		void Delete (DWORD dwIndex)
		{
			DeleteVectorItem<CTimeEvent<T>> (m_pTimeEventTable, dwIndex);
		}
};

typedef CTimeEventTableAccessor<float> CTimeEventFloatAccessor;


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EFFECTTIMEEVENTGRAPH_H__FEB08EC0_B265_4D09_9AC0_0D2C438B35EB__INCLUDED_)
