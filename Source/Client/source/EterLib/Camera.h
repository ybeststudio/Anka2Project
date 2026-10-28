#if !defined(AFX_CAMERA_H__C5D086BE_7A03_4246_9145_336747C47D9E__INCLUDED_)
#define AFX_CAMERA_H__C5D086BE_7A03_4246_9145_336747C47D9E__INCLUDED_

#if _MSC_VER > 1000
	#pragma once
#endif

#include <map>

#include "../eterBase/Singleton.h"
#include "Ray.h"

const float CAMERA_TARGET_STANDARD = 100.0f;
const float CAMERA_TARGET_FACE = 150.0f;

typedef enum _eCameraState_
{
	CAMERA_STATE_NORMAL,
	CAMERA_STATE_CANTGODOWN,
	CAMERA_STATE_CANTGORIGHT,
	CAMERA_STATE_CANTGOLEFT,
	CAMERA_STATE_SCREEN_BY_BUILDING,
	CAMERA_STATE_SCREEN_BY_BUILDING_AND_TOOCLOSE,
} eCameraState;

class CCamera
{
	public:
		CCamera();
		virtual ~CCamera();

		static void SetCameraMaxDistance(float fMax);

		void Lock();
		void Unlock();
		bool IsLock();

		void Wheel(int nWheelLen);
		bool Drag(int nMouseX, int nMouseY, LPPOINT lpReturnPoint);

		bool EndDrag();
		void BeginDrag(int nMouseX, int nMouseY);

		bool IsDraging();

		void SetResistance(float fResistance);

	private:
		const CCamera & operator = (const CCamera &) ;
		CCamera (const CCamera & ) ;

		// Camera Update
		eCameraState	m_eCameraState;
		eCameraState	m_eCameraStatePrev;
		float m_fPitchBackup;
		float m_fRollBackup;
		float m_fDistanceBackup;
		float m_fTargetZBackUp;

		D3DXVECTOR3 m_v3EyeBackup;

		unsigned long m_ulNumScreenBuilding;

// 	protected:

		bool m_isLock;

		// Attributes for view matrix
		D3DXVECTOR3 m_v3Eye;
		D3DXVECTOR3 m_v3Target;
		D3DXVECTOR3 m_v3Up;

		// m_v3View = m_v3Target - m_v3Eye
		D3DXVECTOR3 m_v3View;
		// m_v3Cross = Cross(m_v3Up, m_v3View)
		D3DXVECTOR3 m_v3Cross;

		//ViewMatrixes
		D3DXMATRIX m_matView;
		D3DXMATRIX m_matInverseView;
		D3DXMATRIX m_matBillboard; // Special matrix for billboarding effects

		float m_fPitch;
		float m_fRoll;
		float m_fDistance;

		CRay	m_kCameraBottomToTerrainRay;
		CRay	m_kCameraFrontToTerrainRay;
		CRay	m_kCameraBackToTerrainRay;
		CRay	m_kCameraLeftToTerrainRay;
		CRay	m_kCameraRightToTerrainRay;

		CRay	m_kTargetToCameraBottomRay;

		CRay	m_ViewRay;
		CRay	m_kLeftObjectCollisionRay;
		CRay	m_kTopObjectCollisionRay;
		CRay	m_kRightObjectCollisionRay;
		CRay	m_kBottomObjectCollisionRay;

		float	m_fTerrainCollisionRadius;
		float	m_fObjectCollisionRadius;

// 	protected:
		float			m_fTarget_;

		float			m_fEyeGroundHeightRatio;
		float			m_fTargetHeightLimitRatio;
		float			m_fPitchSum;
		float			m_fRollSum;

		long			m_lMousePosX;
		long			m_lMousePosY;

		bool			m_bDrag;

//	protected:
		D3DXVECTOR3		m_v3AngularAcceleration;
		D3DXVECTOR3		m_v3AngularVelocity;

		float			m_fResistance;
	public:
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		void SetAngularAcceleration(D3DXVECTOR3 v3AngularAcceleration) { m_v3AngularAcceleration = v3AngularAcceleration; }

		//////////////////////////////////////////////////////////////////////////
		// AI
		//////////////////////////////////////////////////////////////////////////
		void SetTerrainCollisionRadius(float fTerrainCollisionRadius) { m_fTerrainCollisionRadius = fTerrainCollisionRadius; }
		void SetObjectCollisionRadius(float fObjectCollisionRadius) { m_fObjectCollisionRadius = fObjectCollisionRadius; }

		CRay & GetViewRay() { return m_ViewRay;	}
		CRay & GetLeftObjectCollisionRay() { return m_kLeftObjectCollisionRay; }
		CRay & GetRightObjectCollisionRay() { return m_kRightObjectCollisionRay; }
		CRay & GetTopObjectCollisionRay() { return m_kTopObjectCollisionRay; }
		CRay & GetBottomObjectCollisionRay() { return m_kBottomObjectCollisionRay; }

		//////////////////////////////////////////////////////////////////////////
		// Update
		//////////////////////////////////////////////////////////////////////////
		void Update();
		eCameraState GetCameraState() {return m_eCameraState;}
		void SetCameraState(eCameraState eNewCameraState);
		void IncreaseNumSrcreenBuilding();
		void ResetNumScreenBuilding();
		unsigned long & GetNumScreenBuilding() { return m_ulNumScreenBuilding; }

		const float & GetPitchBackUp() { return m_fPitchBackup; }
		const float & GetRollBackUp() { return m_fRollBackup; }
		const float & GetDistanceBackUp() { return m_fDistanceBackup; }

		//////////////////////////////////////////////////////////////////////////
		// properties
		//////////////////////////////////////////////////////////////////////////

		const D3DXVECTOR3 & GetEye() const		{ return m_v3Eye; }
		const D3DXVECTOR3 & GetTarget() const	{ return m_v3Target; }
		const D3DXVECTOR3 & GetUp() const		{ return m_v3Up; }
		const D3DXVECTOR3 & GetView() const		{ return m_v3View; }
		const D3DXVECTOR3 & GetCross() const	{ return m_v3Cross; }

		const D3DXMATRIX & GetViewMatrix() const		{ return m_matView; }
		const D3DXMATRIX & GetInverseViewMatrix() const	{ return m_matInverseView; }
		const D3DXMATRIX & GetBillboardMatrix()const	{ return m_matBillboard; }

		void SetViewParams(const D3DXVECTOR3 & v3Eye, const D3DXVECTOR3& v3Target, const D3DXVECTOR3& v3Up );

		void SetEye(const D3DXVECTOR3 & v3Eye);
		void SetTarget(const D3DXVECTOR3 & v3Target);
		void SetUp(const D3DXVECTOR3 & v3Up);

		float GetPitch() const { return m_fPitch; }
		float GetRoll() const { return m_fRoll; }
		float GetDistance() const { return m_fDistance; }

		void Pitch(const float fPitchDelta);
		void Roll(const float fRollDelta);
		void SetDistance(const float fdistance);

		//////////////////////////////////////////////////////////////////////////
		// camera movement
		//////////////////////////////////////////////////////////////////////////

		void Move(const D3DXVECTOR3 & v3Displacement);
		void Zoom(float fRatio);

		void MoveAlongView(float fDistance);
		void MoveAlongCross(float fDistance);
		void MoveAlongUp(float fDistance);

		void MoveLateral(float fDistance);
		void MoveFront(float fDistance);
		void MoveVertical(float fDistance);

	//	void RotateUpper(float fDegree);

		void RotateEyeAroundTarget(float fPitchDegree, float fRollDegree);

		void RotateEyeAroundPoint(const D3DXVECTOR3 & v3Point, float fPitchDegree, float fRollDegree);

	protected:
		void SetViewMatrix();
		void CalculateRoll();

	public:
		float GetTargetHeight();
		void SetTargetHeight(float fTarget);
		bool isTerrainCollisionEnable() { return m_bProcessTerrainCollision; }
		void SetTerrainCollision(bool bEnable) { m_bProcessTerrainCollision = bEnable; }

	private:
		void ProcessTerrainCollision();
		void ProcessBuildingCollision();

	private:
		bool m_bProcessTerrainCollision;

		static float CAMERA_MIN_DISTANCE;
		static float CAMERA_MAX_DISTANCE;
};

typedef std::map<BYTE, CCamera *> TCameraMap;

class CCameraManager : public CSingleton<CCameraManager>
{
	public:
		enum ECameraNum
		{
			NO_CURRENT_CAMERA,
			DEFAULT_PERSPECTIVE_CAMERA,
			DEFAULT_ORTHO_CAMERA,
#ifdef ENABLE_RENDER_TARGET
			SHOPDECO_CAMERA,
#endif
			CAMERA_MAX = 255
		};

		CCameraManager();
		virtual ~CCameraManager();

		bool AddCamera(unsigned char ucCameraNum);
		bool RemoveCamera(unsigned char ucCameraNum);

		CCamera * GetCurrentCamera();
		void SetCurrentCamera(unsigned char ucCameraNum);
		void ResetToPreviousCamera();

		bool isCurrentCamera(unsigned char ucCameraNum);

		unsigned char GetCurrentCameraNum();

		bool isTerrainCollisionEnable();
		void SetTerrainCollision(bool bEnable);

	private:
		TCameraMap		m_CameraMap;
		CCamera *		m_pCurrentCamera;
		CCamera *		m_pPreviousCamera;
};

#endif // !defined(AFX_CAMERA_H__C5D086BE_7A03_4246_9145_336747C47D9E__INCLUDED_)

