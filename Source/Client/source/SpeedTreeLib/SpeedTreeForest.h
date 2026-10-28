#pragma once
#include <SpeedTreeRT.h>
#include "SpeedTreeWrapper.h"
#include <vector>
#include <map>

#define Forest_RenderFakeShadow (1 << -1)
#define Forest_RenderBranches (1 << 0)
#define Forest_RenderLeaves (1 << 1)
#define Forest_RenderFronds (1 << 2)
#define Forest_RenderBillboards (1 << 3)
#define Forest_RenderAll ((1 << 4) - 1)
#define Forest_RenderToShadow (1 << 5)
#define Forest_RenderToMiniMap (1 << 6)

#ifdef FIX_MEMORY_SPEEDTREE
class CSpeedTreeForest : public CSingleton<CSpeedTreeForest>
#else
class CSpeedTreeForest
#endif
{
public:
#ifdef FIX_MEMORY_SPEEDTREE
	typedef std::shared_ptr<CSpeedTreeWrapper> SpeedTreeWrapperPtr;
	typedef std::weak_ptr<CSpeedTreeWrapper> SpeedTreeWrapperWeakPtr;
	typedef std::map<DWORD, SpeedTreeWrapperPtr> TTreeMap;
#else
	typedef std::map<DWORD, CSpeedTreeWrapper*> TTreeMap;
#endif

public:
	CSpeedTreeForest();
	virtual						~CSpeedTreeForest();
	void						ClearMainTree();

#ifdef FIX_MEMORY_SPEEDTREE
	BOOL GetMainTree(DWORD dwCRC, SpeedTreeWrapperPtr& ppMainTree, const char* c_pszFileName);
	BOOL GetMainTree(DWORD dwCRC, CSpeedTreeWrapper** ppMainTree, const char* c_pszFileName)
	{
		SpeedTreeWrapperPtr pTree;
		BOOL res = GetMainTree(dwCRC, pTree, c_pszFileName);
		if (res && ppMainTree)
			*ppMainTree = pTree.get();
		return res;
	}
	SpeedTreeWrapperPtr GetMainTree(DWORD dwCRC);
	void DeleteMainTree(DWORD dwCRC);

	SpeedTreeWrapperPtr CreateInstance(float x, float y, float z, DWORD dwTreeCRC, const char* c_pszTreeName);
	void DeleteInstance(SpeedTreeWrapperPtr pTree);
#else
	BOOL						GetMainTree(DWORD dwCRC, CSpeedTreeWrapper** ppMainTree, const char* c_pszFileName);
	CSpeedTreeWrapper* GetMainTree(DWORD dwCRC);
	void						DeleteMainTree(DWORD dwCRC);

	CSpeedTreeWrapper* CreateInstance(float x, float y, float z, DWORD dwTreeCRC, const char* c_pszTreeName);
	void						DeleteInstance(CSpeedTreeWrapper* pTree);
#endif

	void CollectNearbyTreePositions(float x, float y, float radius, std::vector<D3DXVECTOR3>& positions);
	void						UpdateSystem(float fCurrentTime);
	void						Clear();
	void						SetLight(const float* afDirection, const float* afAmbient, const float* afDiffuse);
	void						SetFog(float fFogNear, float fFogFar);
	const float* GetExtents(void) const { return m_afForestExtents; }
	float						GetWindStrength(void) const { return m_fWindStrength; }
	void						SetWindStrength(float fStrength);
	void						SetupWindMatrices(float fTimeInSecs);
	virtual	void				UploadWindMatrix(unsigned int uiLocation, const float* pMatrix) const = 0;
	virtual void				Render(unsigned long ulRenderBitVector = Forest_RenderAll) = 0;
	virtual void				Render2(unsigned long ulRenderBitVector) = 0;
#ifdef JTX_WATER_REFLECTION
	virtual void				RenderReflection(unsigned long ulRenderBitVector = Forest_RenderAll) = 0;
#endif
	virtual void				Render3(unsigned long ulRenderBitVector) = 0;
#ifdef DirectX9Ex
	virtual bool				SetRenderingDevice(IDirect3DDevice9Ex* pDevice) = 0;

#else
	virtual bool				SetRenderingDevice(LPDIRECT3DDEVICE9 pDevice) = 0;
#endif
	virtual	void				UpdateCompundMatrix(const D3DXVECTOR3& c_rEyeVec, const D3DXMATRIX& c_rmatView, const D3DXMATRIX& c_rmatProj) = 0;
	virtual	void				UpdateCompundMatrix2(const D3DXVECTOR3& c_rEyeVec, const D3DXMATRIX& c_rmatView, const D3DXMATRIX& c_rmatProj) = 0;
protected:
	TTreeMap					m_pMainTreeMap;
	float						m_afLighting[12];
	float						m_afFog[4];
private:
	void						AdjustExtents(float x, float y, float z);
	float						m_afForestExtents[6];
	float						m_fWindStrength;
	float						m_fAccumTime;
};
//martysama0134's 484266e236b8a7567df2ca6d4004b570
