/*
This file was created by Paul Barvinko (pbarvinko@yahoo.com).
This file is distributed "as is", e.g. there are no warranties 
and obligations and you could use it in your applications on your
own risk. 
Your comments and questions are welcome.
If using in your applications, please mention author in credits for your app.
*/
#if !defined(AFX_COMPAGES_H__31B58BF3_84BE_11D1_BF9E_000000000000__INCLUDED_)
#define AFX_COMPAGES_H__31B58BF3_84BE_11D1_BF9E_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// compages.h : header file
//

#include "grres.h"
#include "afxtempl.h"
#include "grview.h"

/////////////////////////////////////////////////////////////////////////////
// CTitlePrompt dialog

class CTitlePrompt : public CDialog
{
// Construction
public:
	CTitlePrompt(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTitlePrompt)
	enum { IDD = IDD_CHANGE_TITLE };
	CString	m_Title;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTitlePrompt)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTitlePrompt)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CPointsCombo window

class CPointsCombo : public CComboBox
{
// Construction
public:
	CPointsCombo();
	virtual void DrawComboItem(COLORREF color, char* title, LPDRAWITEMSTRUCT lpDrawItemStruct);

// Attributes
public:
	
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPointsCombo)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPointsCombo();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPointsCombo)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

class CGraphPage;

class CGrPointsCombo : public CPointsCombo
{
	public:
		CGrPointsCombo() : CPointsCombo() {parentPtr = NULL;};

		void CreatePointsCombo(CGraphPage* _parent);
		virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//data
		CGraphPage* parentPtr;
};

/////////////////////////////////////////////////////////////////////////////
// CGraphPage dialog

class CGraphPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CGraphPage)

// Construction
public:
	CGraphPage();
	CGraphPage(CGraphView* _parent);
	~CGraphPage();

	void SetActiveSel(int selnum);
	void SetHideShowAttr(int selnum);

// Dialog Data

	CGraphView* parentPtr;
	CArray<CGraphProp*, CGraphProp*> grprops;

	//{{AFX_DATA(CGraphPage)
	enum { IDD = IDD_GRAPHS_PROPPAGE };
	CString	m_GraphTitle;
	CGrPointsCombo	m_GraphCombo;
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CGraphPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CGraphPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeGraphChoice();
	afx_msg void OnGraphChColor();
	afx_msg void OnGraphChTitle();
	afx_msg void OnGraphHideshow();
	afx_msg void OnGraphsShowall();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
// CAxisPage dialog

class CAxisPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CAxisPage)

// Construction
public:
	CAxisPage();
	CAxisPage(CGraphView* parent);
	~CAxisPage();

// Dialog Data

	CGraphView* parentPtr;

	//{{AFX_DATA(CAxisPage)
	enum { IDD = IDD_GRAPH_AXISPROPS };
	CString	m_XTitle;
	CString	m_XUOM;
	int		m_XAxisPrec;
	int		m_YAxisPrec;
	CString	m_YTitle;
	CString	m_YUOM;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAxisPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CAxisPage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMPAGES_H__31B58BF3_84BE_11D1_BF9E_000000000000__INCLUDED_)
