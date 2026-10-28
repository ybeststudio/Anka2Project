#pragma once

class CModelInstanceAccessor : public CGrannyModelInstance
{
	public:
		CModelInstanceAccessor();
		virtual ~CModelInstanceAccessor();

		void ClearModel();
		void ClearMotion();
		void ReleaseArtificialMotion();

		void UpdateLocalTime (float fLocalTime);
		float GetLocalTime();
		float GetDuration();

		BOOL SetAccessorModel (CGraphicThing * pThing);
		BOOL SetAccessorMotion (CGraphicThing * pThing);
		void SetArtificialMotion (const CGrannyMotion * pMotion);

		// Interceptor Functions
		void Deform (const D3DXMATRIX* c_pWorldMatrix);
		void UpdateWorldPose (const D3DXMATRIX* c_pWorldMatrix);

		// Access Data
		BOOL IsModelThing();
		BOOL IsMotionThing();
		DWORD GetBoneCount();
		BOOL GetBoneName (DWORD dwIndex, std::string * pstrBoneName);

	protected:
		float m_fLocalTime;

		CGraphicThing* m_pModelThing;
		CGraphicThing* m_pcurMotionThing;

		// NOTE : Motion을 Release 시켜버리면 Granny 내부 애니메이션 처리시 충돌 발생
		//        아마도 Motion이 끝난 이후에도 MotionFileData 자체를 참고 할때가 있는 듯..
		//        모아 두웠다 프로그램 종료시 클리어 한다. - [levites]
		std::list<CGraphicThing*> m_pMotionBackupList;

		int m_boneCount;
		granny_local_pose* 	m_pgrnLocalPose;
};