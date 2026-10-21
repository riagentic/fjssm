/*
This file was created by Paul Barvinko (pbarvinko@yahoo.com).
This file is distributed "as is", e.g. there are no warranties 
and obligations and you could use it in your applications on your
own risk. 
Your comments and questions are welcome.
If using in your applications, please mention author in credits for your app.
*/
#if !defined(AFX_PVIEW_H__A2D654B2_7892_11D1_BD0E_00C04FB6D88F__INCLUDED_)
#define AFX_PVIEW_H__A2D654B2_7892_11D1_BD0E_00C04FB6D88F__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PView.h : header file
//
#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "grres.h"
#include "listctrl.h"
#include "compages.h"

class CGraphView;
class CPointsView;

/////////////////////////////////////////////////////////////////////////////
// CPointEdit dialog

class CPointEdit : public CDialog
{
// Construction
public:
	CPointEdit(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPointEdit)
	enum { IDD = IDD_POINT_EDIT };
	double	m_XValue;
	double	m_YValue;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPointEdit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPointEdit)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CPointList window

class CPointList : public CListCtrlEx
{
// Construction
public:
	CPointList();
	void CreatePointList(CPointsView* parent){parentPtr = parent;};


// Attributes
public:
	CPointsView* parentPtr;

// Operations
public:
	int GetSelectedItem();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPointList)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPointList();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPointList)
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


class CPViewPointCombo : public CPointsCombo
{
	public:
		CPViewPointCombo() : CPointsCombo() {parentPtr = NULL;};
		void CreatePointsCombo(CPointsView* parent){parentPtr = parent;};
		virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

		CPointsView* parentPtr;
};

/////////////////////////////////////////////////////////////////////////////
// CPointsView form view

class CPointsView : public CFormView
{
protected:
	CPointsView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CPointsView)

// Form Data
public:
	//{{AFX_DATA(CPointsView)
	enum { IDD = IDD_GRAPHS };
	CPointList	m_PointList;
	CPViewPointCombo	m_GraphCombo;
	//}}AFX_DATA

// Attributes
public:
	int m_CurrentGraph;
	int m_maxColSize;
	CGraphView* parentView;

// Operations
public:
	int GetMaxColSize();
	CGraphView* GetMainGraphView();
	void SetCurrentGraph(int grnum, BOOL bRedraw);
	int InitGlobalVariables();
	void AddPoint(SGraphChange* sgc);
	void ChangePoint(SGraphChange* sgc);
	void DeletePoint(SGraphChange* sgc);
	void InitListView();
	void UpdateGraph(SGraphChange* sgc);
	void RemoveGraph(SGraphChange* sgc);
	void OnPointAdd(BOOL bInsert);
// Overrides

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPointsView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CPointsView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CPointsView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelchangeGraphCombo();
	afx_msg void OnPointDelete();
	afx_msg void OnPointEdit();
	afx_msg void OnPointInsert();
	afx_msg void OnPointAppend();
	afx_msg void OnPviewHide();
	afx_msg void OnPviewProperties();
	afx_msg void OnPviewRefresh();
	afx_msg void OnCopyAstext();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PVIEW_H__A2D654B2_7892_11D1_BD0E_00C04FB6D88F__INCLUDED_)
