#ifndef __ViewportManager_Header__
#define __ViewportManager_Header__

class CViewportManager
{
	public:
		CViewportManager();
		~CViewportManager();

		void Initialize();

		void MouseMove (CPoint Mouse);

		void ModeButtonDown();
		void ModeButtonUp();

		void ControlKeyDown();
		void ControlKeyUp();

		void LeftButtonDown (CPoint Mouse);
		void LeftButtonUp (CPoint Mouse);

		void TranslateStart (float x, float y);
		void MiddleButtonDown (CPoint Mouse);
		void MiddleButtonUp (CPoint Mouse);

		void RightButtonDown (CPoint Mouse);
		void RightButtonUp (CPoint Mouse);

		void MouseWheel (float Wheel);

		// Data Accessor
		void SetCursorPosition (int ix, int iy);
		void SetScreenStatus (int iWidth, int iHeight);
		float GetFrustumFarDistance();
		float GetFrustumNearDistance()
		{
			return m_fFrustumNear;
		}
		float GetDistance();
		float GetPitch();
		float GetRotation();

		bool isTranslationMode();
		bool CheckControlKey();

		bool isLeftButton();
		bool isMiddleButton();
		bool isRightButton();

		void MoveMousePosition (float x, float y, float& xMove, float& yMove);

	protected:
		enum
		{
			TranslateMode,
			//			RotationMode,
		};

	protected:
		int					m_Mode;

		bool				m_ControlKey;

		bool				m_ClickedLeftButton;
		bool				m_ClickedMiddleButton;
		bool				m_ClickedRightButton;
		CPoint				m_ClickedMiddleButtonPoint;
		CPoint				m_ClickedLeftButtonPoint;
		CPoint				m_ClickedRightButtonPoint;

		float				m_fFrustumNear;
		float				m_fFrustumFar;

		float				m_xOldPosition;
		float				m_yOldPosition;

		int					m_iWidth;
		int					m_iHeight;

		int					m_ixCursor;
		int					m_iyCursor;
};

#endif