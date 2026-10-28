// WorldEditorDoc.h : interface of the CWorldEditorDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_WORLDEDITORDOC_H__9635693F_AA8E_4775_9F20_82F7B6933B3B__INCLUDED_)
#define AFX_WORLDEDITORDOC_H__9635693F_AA8E_4775_9F20_82F7B6933B3B__INCLUDED_

#pragma once

// Test Code
#include "./DataCtrl/UndoBuffer.h"

class CWorldEditorDoc : public CDocument
{
	public:
		typedef std::map<int, CSceneBase*> TSceneMap;
		typedef std::list<CSceneBase*> TSceneList;
		typedef TSceneList::iterator TSceneIterator;

	protected: // create from serialization only
		CWorldEditorDoc();
		DECLARE_DYNCREATE (CWorldEditorDoc)

		// Attributes
	public:
		CMapManagerAccessor m_MapManagerAccessor;
		CObjectData m_ObjectData;
		CEffectAccessor m_EffectAccessor;

		CSceneBase* m_pActiveScene;
		CSceneMap m_SceneMap;
		CSceneObject m_SceneObject;
		CSceneEffect m_SceneEffect;
		CSceneFly m_SceneFly;

		CUndoBuffer m_UndoBuffer;

		int m_iActiveMode;
		TSceneMap m_ScenePointerMap;
		TSceneList m_ScenePointerList;

		bool	m_bIsEditingMapAttribute;
		bool	m_bIsEditingDirectionalLight;

		// Operations
	public:
		void SetActiveMode (int iMode);
		int GetActiveMode();
		CSceneBase* GetActiveScene();

		CMapManagerAccessor* GetMapManagerAccessor();
		CObjectData* GetObjectData();
		CEffectAccessor* GetEffectAccessor();

		CSceneMap* GetSceneMap();
		CSceneObject* GetSceneObject();
		CSceneEffect* GetSceneEffect();
		CSceneFly* 	GetSceneFly();

		CUndoBuffer* GetUndoBuffer();

		BOOL CreateSceneIterator (TSceneIterator & itor);
		BOOL NextSceneIterator (TSceneIterator & itor);

		bool	IsEditingMapAttribute() const
		{
			return m_bIsEditingMapAttribute;
		}
		bool	IsEditingDirectionalLight() const
		{
			return m_bIsEditingDirectionalLight;
		}

		void	SetEditingMapAttribute (bool enable)
		{
			m_bIsEditingMapAttribute = enable;
		}
		void	SetEditingDirectionalLight (bool enable)
		{
			m_bIsEditingDirectionalLight = enable;
		}


		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CWorldEditorDoc)
	public:
		virtual BOOL OnNewDocument();
		virtual void Serialize (CArchive& ar);
		//}}AFX_VIRTUAL

		// Implementation
	public:
		virtual ~CWorldEditorDoc();
		#ifdef _DEBUG
		virtual void AssertValid() const;
		virtual void Dump (CDumpContext& dc) const;
		#endif

		// Generated message map functions
	protected:
		//{{AFX_MSG(CWorldEditorDoc)
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WORLDEDITORDOC_H__9635693F_AA8E_4775_9F20_82F7B6933B3B__INCLUDED_)
