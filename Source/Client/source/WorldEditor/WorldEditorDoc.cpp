// WorldEditorDoc.cpp : implementation of the CWorldEditorDoc class
//

#include "StdAfx.h"
#include "WorldEditor.h"

#include "WorldEditorDoc.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWorldEditorDoc

IMPLEMENT_DYNCREATE (CWorldEditorDoc, CDocument)

BEGIN_MESSAGE_MAP (CWorldEditorDoc, CDocument)
	//{{AFX_MSG_MAP(CWorldEditorDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorldEditorDoc construction/destruction

CWorldEditorDoc::CWorldEditorDoc()
{
	m_pActiveScene = &m_SceneMap;

	m_bIsEditingMapAttribute = false;
	m_bIsEditingDirectionalLight = false;
}

CWorldEditorDoc::~CWorldEditorDoc()
{
}

BOOL CWorldEditorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
	{
		return FALSE;
	}

	m_SceneObject.SetObjectData (&m_ObjectData);
	m_SceneEffect.SetEffectAccessor (&m_EffectAccessor);

	m_ScenePointerMap.clear();
	m_ScenePointerMap.insert (TSceneMap::value_type (ID_VIEW_MAP, &m_SceneMap));
	m_ScenePointerMap.insert (TSceneMap::value_type (ID_VIEW_OBJECT, &m_SceneObject));
	m_ScenePointerMap.insert (TSceneMap::value_type (ID_VIEW_EFFECT, &m_SceneEffect));
	m_ScenePointerMap.insert (TSceneMap::value_type (ID_VIEW_FLY, &m_SceneFly));

	m_ScenePointerList.clear();
	m_ScenePointerList.push_back (&m_SceneMap);
	m_ScenePointerList.push_back (&m_SceneObject);
	m_ScenePointerList.push_back (&m_SceneEffect);
	m_ScenePointerList.push_back (&m_SceneFly);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CWorldEditorDoc normal functions

void CWorldEditorDoc::SetActiveMode (int iMode)
{
	std::map<int, CSceneBase*>::iterator itor = m_ScenePointerMap.find (iMode);
	if (m_ScenePointerMap.end() == itor)
	{
		return;
	}

	m_iActiveMode = iMode;
	m_pActiveScene = itor->second;
}

int CWorldEditorDoc::GetActiveMode()
{
	return m_iActiveMode;
}


/////////////////////////////////////////////////////////////////////////////
// CWorldEditorDoc serialization

void CWorldEditorDoc::Serialize (CArchive& ar)
{
	if (ar.IsStoring())
	{
		// NOTE : add storing code here
	}
	else
	{
		// NOTE : add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CWorldEditorDoc diagnostics

#ifdef _DEBUG
void CWorldEditorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CWorldEditorDoc::Dump (CDumpContext& dc) const
{
	CDocument::Dump (dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWorldEditorDoc commands
CMapManagerAccessor* CWorldEditorDoc::GetMapManagerAccessor()
{
	return & m_MapManagerAccessor;
}

CObjectData* CWorldEditorDoc::GetObjectData()
{
	return & m_ObjectData;
}

CEffectAccessor* CWorldEditorDoc::GetEffectAccessor()
{
	return & m_EffectAccessor;
}

CSceneBase* CWorldEditorDoc::GetActiveScene()
{
	return m_pActiveScene;
}

CSceneMap* CWorldEditorDoc::GetSceneMap()
{
	return & m_SceneMap;
}

CSceneObject* CWorldEditorDoc::GetSceneObject()
{
	return & m_SceneObject;
}

CSceneEffect* CWorldEditorDoc::GetSceneEffect()
{
	return & m_SceneEffect;
}

CSceneFly* CWorldEditorDoc::GetSceneFly()
{
	return & m_SceneFly;
}

CUndoBuffer* CWorldEditorDoc::GetUndoBuffer()
{
	return & m_UndoBuffer;
}

BOOL CWorldEditorDoc::CreateSceneIterator (TSceneIterator & itor)
{
	if (m_ScenePointerList.empty())
	{
		return FALSE;
	}

	itor = m_ScenePointerList.begin();

	return TRUE;
}

BOOL CWorldEditorDoc::NextSceneIterator (TSceneIterator & itor)
{
	++itor;

	if (m_ScenePointerList.end() == itor)
	{
		return FALSE;
	}

	return TRUE;
}

