#pragma once

class CGraphCtrl : public CScreen
{
	public:
		class IValueAccessor
		{
			public:
				IValueAccessor() {}
				virtual ~IValueAccessor() {}

				virtual DWORD GetValueCount() = 0;

				virtual void Insert (float fTime, DWORD dwValue) = 0;
				virtual void Delete (DWORD dwIndex) = 0;

				virtual bool GetTime (DWORD dwIndex, float* pTime) = 0;
				virtual bool GetValue (DWORD dwIndex, DWORD * pValue) = 0;

				virtual void SetTime (DWORD dwIndex, float fTime) = 0;
				virtual void SetValue (DWORD dwIndex, DWORD dwValue) = 0;
		};

		typedef struct SPoint
		{
			SPoint (int ix_ = 0, int iy_ = 0) : ix (ix_), iy (iy_) {}

			int ix;
			int iy;
		} TPoint;

	public:
		enum
		{
			POINT_NONE = 0xffffffff,

			HORIZON_ZOOMING_MAX = 10,
			HORIZON_ZOOMING_MIN = 1,
		};
		enum EGraphType
		{
			GRAPH_TYPE_BOOLEAN,
			GRAPH_TYPE_BRIDGE,
			GRAPH_TYPE_LINEAR,
			GRAPH_TYPE_BLOCK,
		};
		enum EValueType
		{
			VALUE_TYPE_CENTER,
			VALUE_TYPE_ONLY_UP,
			VALUE_TYPE_ONLY_DOWN,
			VALUE_TYPE_UP_AND_DOWN,
		};

		typedef std::vector<CGraphicTextInstance*> TTextInstanceVector;
		typedef std::vector<TPoint> TPointVector;

	public:
		CGraphCtrl();
		virtual ~CGraphCtrl();

		void Initialize();
		void SetGraphType (int iType);
		void SetValueType (int iType);
		void SetAccessorPointer (IValueAccessor * pAccessor);
		void SetSize (int iWidth, int iHeight);

		void Move (float fx, float fy);

		void Update();
		void GraphBegin();
		void GraphEnd (RECT * pRect, HWND hWnd);
		void Render();
		void RenderTimeLine (float fTime);
		void RenderEndLine (float fTime);

		void ZoomInHorizon();
		void ZoomOutHorizon();

		void OnMouseMove (int ix, int iy);
		void OnLButtonDown (int ix, int iy);
		void OnLButtonUp (int ix, int iy);
		void OnRButtonDown (int ix, int iy);
		void OnRButtonUp (int ix, int iy);
		void OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags);

		DWORD GetSelectedIndex();
		void SetSelectedIndex (DWORD dwSelectedIndex);

	protected:
		void BuildHorizontalLine (int iCount);
		void UpdateHorizontalLine (int iStartPosition, int iZoom);

		void RenderGrid();
		void RenderGraph();
		void ConnectPoint (TPoint & rLeft, TPoint & rRight);

		void TimeToScreen (float fTime, int* px);
		void ScreenToTime (DWORD ix, float* pTime);

		bool isCreatingMode();

	protected:
		int m_iGraphType;
		int m_iValueType;

		IValueAccessor* m_pAccessor;

		// Mouse Control
		DWORD m_dwSelectedIndex;
		DWORD m_dwGrippedIndex;

		TPointVector m_PointVector;

		// For Rendering
		int m_iWidth;
		int m_iHeight;

		float m_fxPosition;
		float m_fyPosition;
		int m_iHorizontalZoom;

		int m_ixTemporary;
		int m_iyTemporary;
		int m_ixGridStep;
		int m_iyGridStep;
		int m_ixGridCount;
		int m_iyGridCount;

		// Text
		TTextInstanceVector m_HorizontalTextLine;
		TTextInstanceVector m_VerticalTextLine;

		CGraphicTextInstance::TPool m_textInstancePool;
};