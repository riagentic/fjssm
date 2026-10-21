/*
This file was created by Paul Barvinko (pbarvinko@yahoo.com).
This file is distributed "as is", e.g. there are no warranties 
and obligations and you could use it in your applications on your
own risk. 
Your comments and questions are welcome.
If using in your applications, please mention author in credits for your app.
*/
#if !defined(AFX_GRPPAGE_H__31B58BF4_84BE_11D1_BF9E_000000000000__INCLUDED_)
#define AFX_GRPPAGE_H__31B58BF4_84BE_11D1_BF9E_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// grppage.h : header file
//

#include "grres.h"

class CGraphix;

/////////////////////////////////////////////////////////////////////////////
// CGraphGenPage dialog

class CGraphGenPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CGraphGenPage)

// Construction
public:
	CGraphGenPage();
	CGraphGenPage(CGraphix* _parent);
	~CGraphGenPage();

// Dialog Data

	CGraphix* parentPtr;

	//{{AFX_DATA(CGraphGenPage)
	enum { IDD = IDD_GRAPH_GENPAGE };
	double	m_x1;
	double	m_x2;
	double	m_y1;
	double	m_y2;
	BOOL	m_Autoscale;
	BOOL	m_MouseCoords;
	BOOL	m_PointMarks;
	BOOL	m_ShowAxis;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CGraphGenPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CGraphGenPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRPPAGE_H__31B58BF4_84BE_11D1_BF9E_000000000000__INCLUDED_)
