#if !defined(AFX_TEST_H__EB65D28D_8CB5_43B5_9827_35EC3B77E29D__INCLUDED_)
#define AFX_TEST_H__EB65D28D_8CB5_43B5_9827_35EC3B77E29D__INCLUDED_

#pragma once

// Test.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBlendTypeComboBox window

class CBlendTypeComboBox : public CComboBox
{
		// Construction
	public:
		CBlendTypeComboBox();
		BOOL Create();

		void SelectBlendType (DWORD dwIndex);
		int GetBlendType() const;

		// Attributes
	public:

		// Operations
	public:

		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CTest)
		//}}AFX_VIRTUAL

		// Implementation
	public:
		virtual ~CBlendTypeComboBox();

		// Generated message map functions
	protected:
		//{{AFX_MSG(CTest)
		// NOTE - the ClassWizard will add and remove member functions here.
		//}}AFX_MSG

		DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// COperationTypeComboBox window

class COperationTypeComboBox : public CComboBox
{
		// Construction
	public:
		COperationTypeComboBox();
		BOOL Create();

		void SelectOperationType (DWORD dwIndex);
		int GetOperationType() const;

		// Attributes
	public:

		// Operations
	public:

		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CTest)
		//}}AFX_VIRTUAL

		// Implementation
	public:
		virtual ~COperationTypeComboBox();

		// Generated message map functions
	protected:
		//{{AFX_MSG(CTest)
		// NOTE - the ClassWizard will add and remove member functions here.
		//}}AFX_MSG

		DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEST_H__EB65D28D_8CB5_43B5_9827_35EC3B77E29D__INCLUDED_)
