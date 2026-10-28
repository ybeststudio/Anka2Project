#include "StdAfx.h"
#include <vector>
#include "../eterBase/Filename.h"
#include "../eterBase/MappedFile.h"
#include "../eterPack/EterPackManager.h"
#include "SpeedTreeForest.h"
#include "SpeedTreeConfig.h"
#include <cfloat>
#include <time.h>


CSpeedTreeForest::CSpeedTreeForest() : m_afLighting{}, m_afFog{}, m_fWindStrength(0.0f), m_fAccumTime(0)
{
	const float c_afInitLight[12] = {
		-0.707f, -0.300f, 0.707f, 1.0f,
		0.5f, 0.5f, 0.5f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f
	};
	memcpy(m_afLighting, c_afInitLight, sizeof(c_afInitLight));

	CSpeedTreeRT::SetNumWindMatrices(c_nNumWindMatrices);

	m_afForestExtents[0] = m_afForestExtents[1] = m_afForestExtents[2] = FLT_MAX;
	m_afForestExtents[3] = m_afForestExtents[4] = m_afForestExtents[5] = -FLT_MAX;
}

CSpeedTreeForest::~CSpeedTreeForest()
{
	Clear();
}

#ifdef FIX_MEMORY_SPEEDTREE
void CSpeedTreeForest::Clear()
{
	if (m_pMainTreeMap.begin() != m_pMainTreeMap.end())
		m_pMainTreeMap.clear();
}
#else
void CSpeedTreeForest::Clear()
{
	TTreeMap::iterator itor = m_pMainTreeMap.begin();
	UINT uiCount;

	while (itor != m_pMainTreeMap.end())
	{
		CSpeedTreeWrapper* pMainTree = (itor++)->second;
		CSpeedTreeWrapper** ppInstances = pMainTree->GetInstances(uiCount);

		for (UINT i = 0; i < uiCount; ++i)
			delete ppInstances[i];

		delete pMainTree;
	}

	m_pMainTreeMap.clear();
}
#endif

#ifdef FIX_MEMORY_SPEEDTREE
CSpeedTreeForest::SpeedTreeWrapperPtr CSpeedTreeForest::GetMainTree(DWORD dwCRC)
#else
CSpeedTreeWrapper* CSpeedTreeForest::GetMainTree(DWORD dwCRC)
#endif
{
	TTreeMap::iterator itor = m_pMainTreeMap.find(dwCRC);

	if (itor == m_pMainTreeMap.end())
		return NULL;

	return itor->second;
}

#ifdef FIX_MEMORY_SPEEDTREE
BOOL CSpeedTreeForest::GetMainTree(DWORD dwCRC, SpeedTreeWrapperPtr& ppMainTree, const char* c_pszFileName)
#else
BOOL CSpeedTreeForest::GetMainTree(DWORD dwCRC, CSpeedTreeWrapper** ppMainTree, const char* c_pszFileName)
#endif
{
	TTreeMap::iterator itor = m_pMainTreeMap.find(dwCRC);

#ifdef FIX_MEMORY_SPEEDTREE
	SpeedTreeWrapperPtr pTree;
#else
	CSpeedTreeWrapper* pTree;
#endif

	if (itor != m_pMainTreeMap.end())
		pTree = itor->second;
	else
	{
		CMappedFile file;
		LPCVOID c_pvData = nullptr;
		std::string strResolvedPath = c_pszFileName ? c_pszFileName : "";

		bool bGot = CEterPackManager::Instance().Get(file, strResolvedPath.c_str(), &c_pvData);

		// Tree-specific fallback resolution
		if (!bGot && !strResolvedPath.empty())
		{
			std::string norm = strResolvedPath;
			for (size_t i = 0; i < norm.length(); ++i)
				if (norm[i] == '\\') norm[i] = '/';

			size_t lastSlash = norm.find_last_of('/');
			std::string fileOnly = (lastSlash != std::string::npos) ? norm.substr(lastSlash + 1) : norm;

			std::string candidates[] = {
				"d:/ymir work/tree/" + fileOnly,
				"ymir work/tree/" + fileOnly,
				"d:/ymir work/zone/b/tree/1/" + fileOnly
			};

			for (const auto& cand : candidates)
			{
				if (CEterPackManager::Instance().Get(file, cand.c_str(), &c_pvData))
				{
					strResolvedPath = cand;
					bGot = true;
					break;
				}
			}
		}

		if (!bGot)
			return FALSE;

#ifdef FIX_MEMORY_SPEEDTREE
		pTree = std::make_shared<CSpeedTreeWrapper>();
#else
		pTree = new CSpeedTreeWrapper;
#endif

		if (!pTree->LoadTree(strResolvedPath.c_str(), (const BYTE*)c_pvData, file.Size()))
		{
#ifdef FIX_MEMORY_SPEEDTREE
		pTree.reset();
#else
			delete pTree;
#endif
			return FALSE;
		}

#ifdef FIX_MEMORY_SPEEDTREE
		m_pMainTreeMap.emplace(dwCRC, pTree);
#else
		m_pMainTreeMap.insert(std::map<DWORD, CSpeedTreeWrapper*>::value_type(dwCRC, pTree));
#endif;
	}

#ifdef FIX_MEMORY_SPEEDTREE
	ppMainTree = pTree;
#else
	* ppMainTree = pTree;
#endif
	return TRUE;
}

#ifdef FIX_MEMORY_SPEEDTREE
CSpeedTreeForest::SpeedTreeWrapperPtr CSpeedTreeForest::CreateInstance(float x, float y, float z, DWORD dwTreeCRC, const char* c_szTreeName)
#else
CSpeedTreeWrapper* CSpeedTreeForest::CreateInstance(float x, float y, float z, DWORD dwTreeCRC, const char* c_szTreeName)
#endif
{
#ifdef FIX_MEMORY_SPEEDTREE
	SpeedTreeWrapperPtr pMainTree;
	if (!GetMainTree(dwTreeCRC, pMainTree, c_szTreeName))
#else
	CSpeedTreeWrapper* pMainTree;
	if (!GetMainTree(dwTreeCRC, &pMainTree, c_szTreeName))
#endif
		return NULL;

#ifdef FIX_MEMORY_SPEEDTREE
	SpeedTreeWrapperPtr pTreeInst = pMainTree->MakeInstance();
#else
	CSpeedTreeWrapper* pTreeInst = pMainTree->MakeInstance();
#endif
	pTreeInst->SetPosition(x, y, z);
	pTreeInst->RegisterBoundingSphere();
	return pTreeInst;
}

#ifdef FIX_MEMORY_SPEEDTREE
void CSpeedTreeForest::DeleteInstance(SpeedTreeWrapperPtr pInstance)
#else
void CSpeedTreeForest::DeleteInstance(CSpeedTreeWrapper* pInstance)
#endif
{
	if (!pInstance)
		return;

#ifdef FIX_MEMORY_SPEEDTREE
	SpeedTreeWrapperPtr pParentTree = pInstance->InstanceOf();
#else
	CSpeedTreeWrapper* pParentTree = pInstance->InstanceOf();
#endif

	if (!pParentTree)
		return;

	pParentTree->DeleteInstance(pInstance);
}

void CSpeedTreeForest::UpdateSystem(float fCurrentTime)
{
	// Called several times per frame (update, main render, reflection, shadow pass). Advance the
	// wind clock only when real time moved, clamp hitches, and always re-upload the matrices
	// since other shaders reuse those vertex constant registers between passes.
	static float fLastTime = fCurrentTime;
	float fElapsedTime = fCurrentTime - fLastTime;
	if (fElapsedTime > 0.0f)
	{
		fLastTime = fCurrentTime;
		m_fAccumTime += __min(fElapsedTime, 0.1f);
	}
	else if (fElapsedTime < 0.0f)
	{
		fLastTime = fCurrentTime;
	}

	// SpeedTreeRT expects absolute time (leaf rocking), not a per-call delta.
	CSpeedTreeRT::SetTime(m_fAccumTime);
	SetupWindMatrices(m_fAccumTime);
}

void CSpeedTreeForest::AdjustExtents(float x, float y, float z)
{
	m_afForestExtents[0] = __min(m_afForestExtents[0], x);
	m_afForestExtents[1] = __min(m_afForestExtents[1], y);
	m_afForestExtents[2] = __min(m_afForestExtents[2], z);
	m_afForestExtents[3] = __max(m_afForestExtents[3], x);
	m_afForestExtents[4] = __max(m_afForestExtents[4], y);
	m_afForestExtents[5] = __max(m_afForestExtents[5], z);
}

void CSpeedTreeForest::SetWindStrength(float fStrength)
{
	if (m_fWindStrength == fStrength)
		return;

	m_fWindStrength = fStrength;

	TTreeMap::iterator itor = m_pMainTreeMap.begin();
	UINT uiCount;

	while (itor != m_pMainTreeMap.end())
	{
		SpeedTreeWrapperPtr pMainTree = (itor++)->second;
		std::list<CSpeedTreeForest::SpeedTreeWrapperWeakPtr> ppInstances = pMainTree->GetInstances(uiCount);

		for (auto it : ppInstances)
		{
			if (it.expired())
				continue;

			auto pInstance = it.lock();

			pInstance->GetSpeedTree()->SetWindStrength(m_fWindStrength);
		}
	}
}

void CSpeedTreeForest::SetupWindMatrices(float fTimeInSecs)
{
	// matrix computational data
	static float afMatrixTimes[c_nNumWindMatrices] = { 0.0f };
	static float afFrequencies[c_nNumWindMatrices][2] =
	{
		{ 0.12f, 0.52f },
		{ 0.06f, 0.20f },
		{ 0.10f, 0.15f },
		{ 0.27f, 0.40f }
	};

	// compute time since last call (real seconds; was a fixed +1.1 per call, so the sway speed
	// depended on frame rate and on how many passes called this)
	static float fTimeOfLastCall = fTimeInSecs;
	float fTimeSinceLastCall = __max(0.0f, fTimeInSecs - fTimeOfLastCall);
	fTimeOfLastCall = fTimeInSecs;

	// sway frequency grows with strength; integrate it so strength changes never make the phase jump
	float const fBaseFreq = 1.2f + m_fWindStrength * 2.5f;
	for (int i = 0; i < c_nNumWindMatrices; ++i)
		afMatrixTimes[i] += fTimeSinceLastCall * fBaseFreq;

	// slow travelling gusts (0.4 .. 1.0) on top of the steady wind
	float const fGust = 0.70f + 0.30f * sinf(fTimeInSecs * 0.31f) * sinf(fTimeInSecs * 0.137f + 1.3f);

	// compute maximum branch throw (degrees)
	float const fBaseAngle = m_fWindStrength * 18.0f * fGust;

	// build rotation matrices
	for (int j = 0; j < c_nNumWindMatrices; ++j)
	{
		// compute percentages for each axis (+ a little high-frequency flutter)
		float const fFlutter = 0.15f * sinf(fTimeInSecs * (5.0f + j));
		float const fXPercent = sinf(afFrequencies[j][0] * 4.0f * afMatrixTimes[j] + j * 1.7f) + fFlutter;
		float const fYPercent = cosf(afFrequencies[j][1] * 4.0f * afMatrixTimes[j] + j * 0.9f) + fFlutter * 0.5f;


		// build compound rotation matrix (rotate on 'x' then on 'y')
		const float c_fDeg2Rad = 57.2957795f;
		float fSinX = sinf(fBaseAngle * fXPercent / c_fDeg2Rad);
		float fSinY = sinf(fBaseAngle * fYPercent / c_fDeg2Rad);
		float fCosX = cosf(fBaseAngle * fXPercent / c_fDeg2Rad);
		float fCosY = cosf(fBaseAngle * fYPercent / c_fDeg2Rad);

		float afMatrix[16] = { 0.0f };
		afMatrix[0] = fCosY;
		afMatrix[2] = -fSinY;
		afMatrix[4] = fSinX * fSinY;
		afMatrix[5] = fCosX;
		afMatrix[6] = fSinX * fCosY;
		afMatrix[8] = fSinY * fCosX;
		afMatrix[9] = -fSinX;
		afMatrix[10] = fCosX * fCosY;
		afMatrix[15] = 1.0f;

#ifdef WRAPPER_USE_CPU_WIND
		CSpeedTreeRT::SetWindMatrix(j, afMatrix);
#endif

#ifdef JTX_GPU_TREE_WIND
		// graphics API specific
		UploadWindMatrix(c_nVertexShader_WindMatrices + j * 4, afMatrix);

#endif
	}
}

//DOAR UN SINGUR LIGHT SE POATE ACTIVA!
//#define LIGHT_TEST_1
#define LIGHT_TEST_2
//#define LIGHT_TEST_3

void CSpeedTreeForest::SetLight(const float* afDirection, const float* afAmbient, const float* afDiffuse)
{
	m_afLighting[0] = afDirection[0];
	m_afLighting[1] = afDirection[1];
	m_afLighting[2] = afDirection[2];
	m_afLighting[3] = 1.0f;

	// Prevent pitch black on shadowed side of trees
	float fAmbR = (afAmbient[0] < 0.45f) ? 0.45f : afAmbient[0];
	float fAmbG = (afAmbient[1] < 0.45f) ? 0.45f : afAmbient[1];
	float fAmbB = (afAmbient[2] < 0.45f) ? 0.45f : afAmbient[2];

	m_afLighting[4] = fAmbR;
	m_afLighting[5] = fAmbG;
	m_afLighting[6] = fAmbB;
	m_afLighting[7] = 1.0f;

	m_afLighting[8] = afDiffuse[0];
	m_afLighting[9] = afDiffuse[1];
	m_afLighting[10] = afDiffuse[2];
	m_afLighting[11] = 1.0f;
}

void CSpeedTreeForest::SetFog(float fFogNear, float fFogFar)
{
	const float c_fFogLinearScale = (1.0f / (fFogFar - fFogNear));

	m_afFog[0] = fFogNear;
	m_afFog[1] = fFogFar;
	m_afFog[2] = c_fFogLinearScale;
	m_afFog[3] = 0.0f;
}

//martysama0134's 484266e236b8a7567df2ca6d4004b570

void CSpeedTreeForest::CollectNearbyTreePositions(float x, float y, float radius, std::vector<D3DXVECTOR3>& positions)
{
    positions.clear();
    const float radiusSq = radius * radius;
    auto collect = [&](CSpeedTreeWrapper* tree) {
        if (!tree) return;
        const float* p = tree->GetPosition();
        const float dx = p[0] - x, dy = p[1] - y;
        if (dx * dx + dy * dy <= radiusSq)
            positions.emplace_back(p[0], p[1], p[2]);
    };
    for (auto& entry : m_pMainTreeMap)
    {
        unsigned int count = 0;
#ifdef FIX_MEMORY_SPEEDTREE
        for (auto& weak : entry.second->GetInstances(count))
            if (auto tree = weak.lock()) collect(tree.get());
#else
        CSpeedTreeWrapper** instances = entry.second->GetInstances(count);
        for (unsigned int i = 0; i < count; ++i) collect(instances[i]);
#endif
    }
}
