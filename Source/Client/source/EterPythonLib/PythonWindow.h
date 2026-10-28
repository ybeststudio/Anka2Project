#pragma once



#include "StdAfx.h"

#include "../eterBase/Utils.h"

#include <cstdint>



#ifdef ENABLE_RENDER_TARGET

	#include <cstdint>

	#include "../EterLib/GrpRenderTargetTexture.h"

#endif



#ifdef INSIDE_RENDER

#include <cstdint>

#endif



namespace UI

{

	class CWindow

	{

		public:

			typedef std::list<CWindow *> TWindowContainer;



			static DWORD Type();

			BOOL IsType(DWORD dwType);



			enum EHorizontalAlign

			{

				HORIZONTAL_ALIGN_LEFT = 0,

				HORIZONTAL_ALIGN_CENTER = 1,

				HORIZONTAL_ALIGN_RIGHT = 2,

			};



			enum EVerticalAlign

			{

				VERTICAL_ALIGN_TOP = 0,

				VERTICAL_ALIGN_CENTER = 1,

				VERTICAL_ALIGN_BOTTOM = 2,

			};



			enum EFlags

			{

				FLAG_MOVABLE = (1 << 0),

				FLAG_LIMIT = (1 << 1),

				FLAG_SNAP = (1 << 2),

				FLAG_DRAGABLE = (1 << 3),

				FLAG_ATTACH = (1 << 4),

				FLAG_RESTRICT_X = (1 << 5),

				FLAG_RESTRICT_Y = (1 << 6),

				FLAG_NOT_CAPTURE = (1 << 7),

				FLAG_FLOAT = (1 << 8),

				FLAG_NOT_PICK = (1 << 9),

				FLAG_IGNORE_SIZE = (1 << 10),

				FLAG_RTL = (1 << 11),

				FLAG_ALPHA_SENSITIVE = (1 << 12),

				FLAG_REMOVE_LIMIT = (1 << 13),

				FLAG_IS_BOARD_WITHOUT_ALPHA = (1 << 14),

			};



			enum WindowTypes

			{

				WINDOW_TYPE_WINDOW,

				WINDOW_TYPE_EX_IMAGE,



				WINDOW_TYPE_MAX_NUM

			};



		public:

			CWindow(PyObject * ppyObject);

			virtual ~CWindow();



			void			AddChild(CWindow * pWin);



			void			Clear();

			void			DestroyHandle();

			void			Update();

			void			Render();



			void			SetName(const char *c_szName);

			const char *	GetName()		{ return m_strName.c_str(); }

			void			SetSize(long width, long height);

			long			GetWidth()		{ return m_lWidth; }

			long			GetHeight()		{ return m_lHeight; }



			void			SetHorizontalAlign(DWORD dwAlign);

			void			SetVerticalAlign(DWORD dwAlign);

			void			SetPosition(long x, long y);

			void			GetPosition(long * plx, long * ply);

			long			GetPositionX( void ) const		{ return m_x; }

			long			GetPositionY( void ) const		{ return m_y; }

			RECT &			GetRect()		{ return m_rect; }

			void			GetLocalPosition(long & rlx, long & rly);

			void			GetMouseLocalPosition(long & rlx, long & rly);

			long			UpdateRect();



			RECT &			GetLimitBias()	{ return m_limitBiasRect; }

			void			SetLimitBias(long l, long r, long t, long b) { m_limitBiasRect.left = l, m_limitBiasRect.right = r, m_limitBiasRect.top = t, m_limitBiasRect.bottom = b; }



			void			Show();

			void			Hide();

#ifdef INSIDE_RENDER

			virtual	bool	IsShow();

			void			OnHideWithChilds();

			void			OnHide();

#else

			bool			IsShow() { return m_bShow; }

#endif

			bool			IsRendering();

#ifdef ENABLE_UI_TAB_FOCUS_SYSTEM
			void			SetFocusable(bool bFlag)	{ m_bFocusable = bFlag; }
			bool			IsFocusable() const			{ return m_bFocusable; }
#endif



			bool			HasParent()		{ return m_pParent ? true : false; }

			bool			HasChild()		{ return m_pChildList.empty() ? false : true; }

			int				GetChildCount()	{ return m_pChildList.size(); }



			void			IsTransparentOnPixel(long* x, long* y, bool *ret);

			void			HaveSomeChildOnOnPixel(long* x, long* y, bool *ret);



			CWindow *		GetRoot();

			CWindow *		GetParent();

#ifdef INSIDE_RENDER

			bool			IsChild(CWindow* pWin, bool bCheckRecursive = false);

#else

			bool			IsChild(CWindow * pWin);

#endif

			void			DeleteChild(CWindow * pWin);

			void			SetTop(CWindow * pWin);



			bool			IsIn(long x, long y);

			bool			IsIn();

			CWindow *		PickWindow(long x, long y);

			CWindow *		PickTopWindow(long x, long y);



			void			__RemoveReserveChildren();



			void			AddFlag(DWORD flag)		{ SET_BIT(m_dwFlag, flag);		}

			void			RemoveFlag(DWORD flag)	{ REMOVE_BIT(m_dwFlag, flag);	}

			bool			IsFlag(DWORD flag)		{ return (m_dwFlag & flag) ? true : false;	}



#ifdef INSIDE_RENDER

			void			SetInsideRender(BOOL flag);

			void			GetRenderBox(RECT* box);

			void			UpdateTextLineRenderBox();

			void			UpdateRenderBox();

			void			UpdateRenderBoxRecursive();

#endif

			virtual void	OnRender();

			virtual void	OnUpdate();

			virtual void	OnChangePosition(){}



#ifdef INSIDE_RENDER

			virtual void	OnAfterRender();

			virtual void	OnUpdateRenderBox() {}

#endif



			virtual void	OnSetFocus();

			virtual void	OnKillFocus();



			virtual void	OnMouseDrag(long lx, long ly);

			virtual void	OnMouseOverIn();

			virtual void	OnMouseOverOut();

			virtual void	OnMouseOver();

			virtual void	OnDrop();

			virtual void	OnTop();

			virtual void	OnIMEUpdate();
#ifdef ENABLE_MULTILANGUAGE_CHATTING
			DWORD			GetInputSystemLangID() const;
			void			SetInputSystemLangID(DWORD dwLangID) { m_dwLangID = dwLangID; }
#endif



			virtual void	OnMoveWindow(long x, long y);



			BOOL			RunIMETabEvent();

			BOOL			RunIMEReturnEvent();

			BOOL			RunIMEKeyDownEvent(int ikey);



			CWindow *		RunKeyDownEvent(int ikey);

			BOOL			RunKeyUpEvent(int ikey);

			BOOL			RunPressEscapeKeyEvent();

			BOOL			RunPressExitKeyEvent();



			virtual BOOL	OnIMETabEvent();

			virtual BOOL	OnIMEReturnEvent();

			virtual BOOL	OnIMEKeyDownEvent(int ikey);



			virtual BOOL	OnIMEChangeCodePage();

			virtual BOOL	OnIMEOpenCandidateListEvent();

			virtual BOOL	OnIMECloseCandidateListEvent();

			virtual BOOL	OnIMEOpenReadingWndEvent();

			virtual BOOL	OnIMECloseReadingWndEvent();



			virtual BOOL	OnMouseLeftButtonDown();

			virtual BOOL	OnMouseLeftButtonUp();

			virtual BOOL	OnMouseLeftButtonDoubleClick();

			virtual BOOL	OnMouseRightButtonDown();

			virtual BOOL	OnMouseRightButtonUp();

			virtual BOOL	OnMouseRightButtonDoubleClick();

			virtual BOOL	OnMouseMiddleButtonDown();

			virtual BOOL	OnMouseMiddleButtonUp();

			virtual BOOL	RunMouseWheelEvent(long nLen);



			virtual BOOL	OnKeyDown(int ikey);

			virtual BOOL	OnKeyUp(int ikey);

			virtual BOOL	OnPressEscapeKey();

			virtual BOOL	OnPressExitKey();

			///////////////////////////////////////



			virtual void	SetColor(DWORD dwColor){}

			virtual BOOL	OnIsType(DWORD dwType);



			virtual BOOL	IsWindow() { return TRUE; }

			virtual BOOL	IsDisable() { return FALSE; }



#ifdef ENABLE_CLIP_MASKING

			virtual void	SetClippingMaskRect(const RECT& rMask);

			virtual void	SetClippingMaskWindow(CWindow* pMaskWindow);

#endif



#ifdef INSIDE_RENDER

		public:

			virtual void	iSetRenderingRect(int iLeft, int iTop, int iRight, int iBottom);

			virtual void	SetRenderingRect(float fLeft, float fTop, float fRight, float fBottom);

			virtual int		GetRenderingWidth();

			virtual int		GetRenderingHeight();

			void			ResetRenderingRect(bool bCallEvent = true);



		private:

			virtual void	OnSetRenderingRect();

#endif



		protected:

			std::string			m_strName;



			EHorizontalAlign	m_HorizontalAlign;

			EVerticalAlign		m_VerticalAlign;

			long				m_x, m_y;

			long				m_lWidth, m_lHeight;

			RECT				m_rect;

			RECT				m_limitBiasRect;



			bool				m_bMovable;

			bool				m_bShow;

#ifdef ENABLE_UI_TAB_FOCUS_SYSTEM
			bool				m_bFocusable;
#endif

#ifdef INSIDE_RENDER

			RECT				m_renderingRect;

			BOOL				m_isInsideRender;

			RECT				m_renderBox;

#endif



			DWORD				m_dwFlag;



			PyObject *			m_poHandler;



			CWindow	*			m_pParent;

			TWindowContainer	m_pChildList;



			BOOL				m_isUpdatingChildren;



			TWindowContainer	m_pReserveChildList;



#ifdef ENABLE_CLIP_MASKING

			bool				m_bEnableMask;

			CWindow*			m_pMaskWindow;

			RECT				m_rMaskRect;

#endif



			BYTE				m_windowType;
#ifdef ENABLE_MULTILANGUAGE_CHATTING
			DWORD				m_dwLangID;
#endif



#ifdef _DEBUG

		public:

			DWORD				DEBUG_dwCounter;

#endif

	};



	class CLayer : public CWindow

	{

		public:

			CLayer(PyObject *ppyObject) : CWindow(ppyObject) {}

			virtual ~CLayer() {}



			BOOL IsWindow() { return FALSE; }

	};



#ifdef ENABLE_RENDER_TARGET

	class CRenderTarget : public CWindow

	{

		public:

			CRenderTarget(PyObject *ppyObject);

			virtual ~CRenderTarget();



			bool SetRenderTarget(int index);



#ifdef ENABLE_WIKI_SYSTEM

			void SetRenderingRect(float fLeft, float fTop, float fRight, float fBottom);

			RECT rect_ex;

#endif



		protected:

			DWORD m_dwIndex;

			void OnRender();

	};

#endif



	class CBox : public CWindow

	{

		public:

			CBox(PyObject * ppyObject);

			virtual ~CBox();



			void SetColor(DWORD dwColor);



		protected:

			void OnRender();



		protected:

			DWORD m_dwColor;

	};



	class CBar : public CWindow

	{

		public:

			CBar(PyObject * ppyObject);

			virtual ~CBar();



			void SetColor(DWORD dwColor);



		protected:

			void OnRender();



		protected:

			DWORD m_dwColor;

	};



	class CLine : public CWindow

	{

		public:

			CLine(PyObject * ppyObject);

			virtual ~CLine();



			void SetColor(DWORD dwColor);



		protected:

			void OnRender();



		protected:

			DWORD m_dwColor;

	};



	class CBar3D : public CWindow

	{

		public:

			static DWORD Type();



		public:

			CBar3D(PyObject * ppyObject);

			virtual ~CBar3D();



			void SetColor(DWORD dwLeft, DWORD dwRight, DWORD dwCenter);



		protected:

			void OnRender();



		protected:

			DWORD m_dwLeftColor;

			DWORD m_dwRightColor;

			DWORD m_dwCenterColor;

	};



	// Text

	class CTextLine : public CWindow

	{

		public:

			CTextLine(PyObject * ppyObject);

			virtual ~CTextLine();



#ifdef INSIDE_RENDER

			static DWORD Type();

#endif



			void SetMax(int iMax);

			void SetHorizontalAlign(int iType);

			void SetVerticalAlign(int iType);

			void SetSecret(BOOL bFlag);

			void SetOutline(BOOL bFlag);

			void SetFeather(BOOL bFlag);

			void SetMultiLine(BOOL bFlag);

			void SetFontName(const char * c_szFontName);

			void SetFontColor(DWORD dwColor);

			void SetLimitWidth(float fWidth);



#ifdef INSIDE_RENDER

			void SetFixedRenderPos(WORD startPos, WORD endPos) { m_TextInstance.SetFixedRenderPos(startPos, endPos); }

			void GetRenderPositions(WORD& startPos, WORD& endPos) { m_TextInstance.GetRenderPositions(startPos, endPos); }

			bool IsShowCursor();

			bool IsShow();

			int GetRenderingWidth();

			int GetRenderingHeight();

			void OnSetRenderingRect();

#endif



			void ShowCursor();

			void HideCursor();



			int GetCursorPosition();



			void SetText(const char * c_szText);
#ifdef ENABLE_MULTILANGUAGE_CHATTING
			void SetFontColorRange(DWORD dwColor, int iStart, int iEnd);
			void SetTextWithCodePage(const char * c_szText, DWORD dwCodePage);
			void SetText(const char * c_szText, const DWORD * c_pdwLangID);
#endif

			const char * GetText();



			void GetTextSize(int* pnWidth, int* pnHeight);



#ifdef ENABLE_MULTI_TEXTLINE

			int GetTextLineCount();

			void DisableEnterToken();

			void SetLineHeight(int iHeight);

			int GetLineHeight();

#endif

#ifdef ENABLE_SUNG_MAHI_TOWER

			void GetCharSize(short* sWidth);

#endif

		protected:

			void OnUpdate();

			void OnRender();

			void OnChangePosition();



			virtual void OnSetText(const char * c_szText);



#ifdef INSIDE_RENDER

			void OnUpdateRenderBox() {

				UpdateTextLineRenderBox();

				m_TextInstance.SetRenderBox(m_renderBox);

			}

#endif



		protected:

			CGraphicTextInstance m_TextInstance;

	};



	class CNumberLine : public CWindow

	{

		public:

			CNumberLine(PyObject * ppyObject);

			CNumberLine(CWindow * pParent);

			virtual ~CNumberLine();



			void SetPath(const char * c_szPath);

			void SetHorizontalAlign(int iType);

			void SetNumber(const char *c_szNumber);

			const std::string & GetNumber() const { return m_strNumber; }
#ifdef ENABLE_SLOT_CHAR_ENLARGEMENT
			void SetCanEnlarge(bool bFlag) { m_bCanEnlarge = bFlag; }
			bool CanEnlarge() const { return m_bCanEnlarge; }
			void RefreshSlotCharState();
#endif



		protected:

			void ClearNumber();

			void OnRender();

			void OnChangePosition();



		protected:

			std::string m_strPath;

			std::string m_strNumber;

#ifdef ENABLE_SLOT_CHAR_ENLARGEMENT
			std::vector<CGraphicImageInstance *> m_ImageInstanceVector[2];
			DWORD m_dwWidthSummary[2];
			bool m_bCanEnlarge;
			bool m_bIsEnlarged;
#else
			std::vector<CGraphicImageInstance *> m_ImageInstanceVector;
			DWORD m_dwWidthSummary;
#endif

			int m_iHorizontalAlign;

	};



	// Image

	class CImageBox : public CWindow

	{

		public:

			CImageBox(PyObject * ppyObject);

			virtual ~CImageBox();



#ifdef INSIDE_RENDER

			void UnloadImage()

			{

				OnDestroyInstance();

				SetSize(GetWidth(), GetHeight());

				UpdateRect();

			}

#endif



#ifdef ENABLE_OFFICAL_FEATURES

			void LeftRightReverse();

#endif

			BOOL LoadImage(const char *c_szFileName);

			void SetDiffuseColor(float fr, float fg, float fb, float fa);



			int GetWidth();

			int GetHeight();



#ifdef ENABLE_NEW_DUNGEON_LIB

			void SetCoolTime(float fCoolTime);

			void SetCoolTimeStart(float fCoolTimeStart);

			float m_fCoolTime;

			float m_fCoolTimeStart;

#endif

		protected:

			virtual void OnCreateInstance();

			virtual void OnDestroyInstance();



			virtual void OnUpdate();

			virtual void OnRender();

			void OnChangePosition();



		protected:

			CGraphicImageInstance * m_pImageInstance;

	};

	class CMarkBox : public CWindow

	{

		public:

			CMarkBox(PyObject * ppyObject);

			virtual ~CMarkBox();



			void LoadImage(const char * c_szFilename);

			void SetDiffuseColor(float fr, float fg, float fb, float fa);

			void SetIndex(UINT uIndex);

			void SetScale(FLOAT fScale);



		protected:

			virtual void OnCreateInstance();

			virtual void OnDestroyInstance();



			virtual void OnUpdate();

			virtual void OnRender();

			void OnChangePosition();

		protected:

			CGraphicMarkInstance * m_pMarkInstance;

	};



	class CExpandedImageBox : public CImageBox

	{

		public:

			static DWORD Type();



		public:

			CExpandedImageBox(PyObject * ppyObject);

			virtual ~CExpandedImageBox();



			void SetScale(float fx, float fy);

			void SetOrigin(float fx, float fy);

			void SetRotation(float fRotation);

			void SetRenderingRect(float fLeft, float fTop, float fRight, float fBottom);

			void SetRenderingRectWithScale(float fLeft, float fTop, float fRight, float fBottom);

			void SetRenderingMode(int iMode);

// #if defined(ENABLE_IMAGE_CLIP_RECT) || defined(ENABLE_BATTLE_PASS_SYSTEM)

			void SetImageClipRect(float fLeft, float fTop, float fRight, float fBottom, bool bIsVertical = false);

// #endif

#ifdef INSIDE_RENDER

			int GetRenderingWidth();

			int GetRenderingHeight();

			void OnSetRenderingRect();

			void SetExpandedRenderingRect(float fLeftTop, float fLeftBottom, float fTopLeft, float fTopRight, float fRightTop, float fRightBottom, float fBottomLeft, float fBottomRight);

			void SetTextureRenderingRect(float fLeft, float fTop, float fRight, float fBottom);

			DWORD GetPixelColor(DWORD x, DWORD y);

			void OnUpdateRenderBox();

#endif



		protected:

			void OnCreateInstance();

			void OnDestroyInstance();



			virtual void OnUpdate();

			virtual void OnRender();



			BOOL OnIsType(DWORD dwType);

	};



	class CAniImageBox : public CWindow

	{

		public:

			static DWORD Type();



		public:

			CAniImageBox(PyObject *ppyObject);

			virtual ~CAniImageBox();



			void SetDelay(int iDelay);

			void SetDiffuseColor(float r, float g, float b, float a);

			void AppendImage(const char *c_szFileName);

			void SetRenderingRect(float fLeft, float fTop, float fRight, float fBottom);

			void SetRenderingRectWithScale(float fLeft, float fTop, float fRight, float fBottom);

			void SetRenderingMode(int iMode);

			void SetAniImgScale(float x, float y);



			void ResetFrame();



#ifdef ENABLE_FISH_GAME

			void SetRotation(float fRotation);

			void SetMoveStatus(bool bFlag);

			void SetMoveSpeed(float speed);

			void SetMovePos(float directionX, float directionY);

			void CheckMove();

			void OnMoveDone();

			float GetRotation();

#endif



		protected:

			void OnUpdate();

			void OnRender();

			void OnChangePosition();

			virtual void OnEndFrame();



#ifdef ENABLE_FISH_GAME

			float nextX, nextY;

			bool moveStart;

			float fSpeed;

#endif



			BOOL OnIsType(DWORD dwType);



		protected:

			BYTE m_bycurDelay;

			float m_fCurDelay;

			BYTE m_byDelay;

			BYTE m_bycurIndex;

			std::vector<CGraphicExpandedImageInstance*> m_ImageVector;



			std::queue<std::string> m_ImageFileNames;

			std::function<void(CGraphicExpandedImageInstance*)> m_SetRenderingRect, m_SetRenderingMode, m_SetDiffuseColor;



			float m_fScaleX;

			float m_fScaleY;

	};



    class CGifImageBox : public CAniImageBox

    {

    public:

        static uint32_t Type();



        public:

            CGifImageBox(PyObject* ppyObject);

            virtual ~CGifImageBox();



            bool LoadGif(const char* c_szFileName);

            bool UnloadGif();



        protected:

            BOOL OnIsType(uint32_t dwType);

    };

	class CButton : public CWindow

	{

		public:

			CButton(PyObject *ppyObject);

			virtual ~CButton();



			BOOL SetUpVisual(const char *c_szFileName);

			BOOL SetOverVisual(const char *c_szFileName);

			BOOL SetDownVisual(const char *c_szFileName);

			BOOL SetDisableVisual(const char *c_szFileName);



#if defined(ENABLE_TRACK_WINDOW) && defined(ENABLE_ADVANCED_GAME_OPTIONS)

			static DWORD Type();

			void SetRenderingRect(float fLeft, float fTop, float fRight, float fBottom);

#endif



			const char *GetUpVisualFileName();

			const char *GetOverVisualFileName();

			const char *GetDownVisualFileName();



			void Flash();

			void EnableFlash();

			void DisableFlash();

			void Enable();

			void Disable();



			void SetUp();

			void Up();

			void Over();

			void Down();



#if defined(ENABLE_EMOTICONS_SYSTEM) || defined(ENABLE_FLEXIBLE_WIDTH_BUTTON)

			void SetScale(float fx, float fy);

#endif

#ifdef ENABLE_FLEXIBLE_WIDTH_BUTTON

			void SetKeepUpVisual(bool bKeepUp) { m_bKeepUpVisual = bKeepUp ? TRUE : FALSE; }

			BOOL IsKeepUpVisual() const { return m_bKeepUpVisual; }

			long GetVisualWidth();

			long GetVisualHeight();

#endif



			BOOL IsDisable();

			BOOL IsPressed();

#ifdef INSIDE_RENDER

			void OnSetRenderingRect();

			void OnUpdateRenderBox();

#endif



#ifdef ENABLE_OFFICAL_FEATURES

			void LeftRightReverse();

#endif



		protected:

			void OnUpdate();

			void OnRender();

			void OnChangePosition();



			BOOL OnMouseLeftButtonDown();

			BOOL OnMouseLeftButtonDoubleClick();

			BOOL OnMouseLeftButtonUp();

			void OnMouseOverIn();

			void OnMouseOverOut();



			BOOL IsEnable();



#if defined(ENABLE_TRACK_WINDOW) && defined(ENABLE_ADVANCED_GAME_OPTIONS)

			void SetCurrentVisual(CGraphicExpandedImageInstance* pVisual);

			BOOL OnIsType(DWORD dwType);

#else

			void SetCurrentVisual(CGraphicImageInstance * pVisual);

#endif



		protected:

			BOOL m_bEnable;

			BOOL m_isPressed;

			BOOL m_isFlash;

#ifdef ENABLE_FLEXIBLE_WIDTH_BUTTON

			BOOL m_bKeepUpVisual;

#endif



#if defined(ENABLE_TRACK_WINDOW) && defined(ENABLE_ADVANCED_GAME_OPTIONS)

			CGraphicExpandedImageInstance* m_pcurVisual;

			CGraphicExpandedImageInstance m_upVisual;

			CGraphicExpandedImageInstance m_overVisual;

			CGraphicExpandedImageInstance m_downVisual;

			CGraphicExpandedImageInstance m_disableVisual;

#else

			CGraphicImageInstance* m_pcurVisual;

			CGraphicImageInstance m_upVisual;

			CGraphicImageInstance m_overVisual;

			CGraphicImageInstance m_downVisual;

			CGraphicImageInstance m_disableVisual;

#endif

	};



	class CRadioButton : public CButton

	{

		public:

#if defined(ENABLE_TRACK_WINDOW) && defined(ENABLE_ADVANCED_GAME_OPTIONS)

			static DWORD Type();

#endif

			CRadioButton(PyObject *ppyObject);

			virtual ~CRadioButton();



		protected:

			BOOL OnMouseLeftButtonDown();

			BOOL OnMouseLeftButtonUp();

			void OnMouseOverIn();

			void OnMouseOverOut();

#if defined(ENABLE_TRACK_WINDOW) && defined(ENABLE_ADVANCED_GAME_OPTIONS)

			BOOL OnIsType(DWORD dwType);

#endif

	};



	class CToggleButton : public CButton

	{

		public:

#if defined(ENABLE_TRACK_WINDOW) && defined(ENABLE_ADVANCED_GAME_OPTIONS)

			static DWORD Type();

#endif

			CToggleButton(PyObject *ppyObject);

			virtual ~CToggleButton();



		protected:

			BOOL OnMouseLeftButtonDown();

			BOOL OnMouseLeftButtonUp();

			void OnMouseOverIn();

			void OnMouseOverOut();

#if defined(ENABLE_TRACK_WINDOW) && defined(ENABLE_ADVANCED_GAME_OPTIONS)

			BOOL OnIsType(DWORD dwType);

#endif

	};



	class CDragButton : public CButton

	{

		public:

			CDragButton(PyObject *ppyObject);

			virtual ~CDragButton();



			void SetRestrictMovementArea(int ix, int iy, int iwidth, int iheight);



		protected:

			void OnChangePosition();

			void OnMouseOverIn();

			void OnMouseOverOut();



		protected:

			RECT m_restrictArea;

	};



#ifdef ENABLE_FLEXIBLE_WIDTH_BUTTON

	class CFlexibleWidthButton : public CButton

	{

		public:

#if defined(ENABLE_TRACK_WINDOW) && defined(ENABLE_ADVANCED_GAME_OPTIONS)

			static DWORD Type();

			BOOL OnIsType(DWORD dwType);

#endif

			CFlexibleWidthButton(PyObject * ppyObject);

			virtual ~CFlexibleWidthButton();



			virtual void OnRender();

			virtual void OnChangePosition();



			BOOL SetSideVisual(int iVisualType, int iSideType, const char * c_szFileName);

			int GetSideWidth() const;

			virtual BOOL SetUpVisual(const char * c_szFileName);



		protected:

			CGraphicExpandedImageInstance m_sideVisuals[4][2];

			int m_iSideWidth;

			float m_fCenterWidth;

	};



	class CFlexibleWidthToggleButton : public CFlexibleWidthButton

	{

		public:

#if defined(ENABLE_TRACK_WINDOW) && defined(ENABLE_ADVANCED_GAME_OPTIONS)

			static DWORD Type();

			BOOL OnIsType(DWORD dwType);

#endif

			CFlexibleWidthToggleButton(PyObject * ppyObject);

			virtual ~CFlexibleWidthToggleButton();



		protected:

			BOOL OnMouseLeftButtonDown();

			BOOL OnMouseLeftButtonUp();

			void OnMouseOverIn();

			void OnMouseOverOut();

	};

#endif

};



extern BOOL g_bOutlineBoxEnable;

