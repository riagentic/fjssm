/*
This file was created by Paul Barvinko (pbarvinko@yahoo.com).
This file is distributed "as is", e.g. there are no warranties 
and obligations and you could use it in your applications on your
own risk. 
Your comments and questions are welcome.
If using in your applications, please mention author in credits for your app.
*/
#if !defined(AFX_GRWND_H__6B968182_76FF_11D1_BD0B_00C04FB6D88F__INCLUDED_)
#define AFX_GRWND_H__6B968182_76FF_11D1_BD0B_00C04FB6D88F__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Grwnd.h : header file
//

#include "coords.h"
#include "gruler.h"

typedef struct SGraphChange
{
	int graphnum;
	int index;
	int index_ex;
	BOOL bRedraw;
}SGraphChange;

//CGrafix flags
#define GRAPH_AUTOSCALE 0x00000001
#define GRAPH_SQUAREPOINTS 0x00000002
#define GRAPH_SHOW_TOOLTIP 0x00000004
#define GRAPH_DRAW_AXIS 0x00000008

/////////////////////////////////////////////////////////////////////////////

class CGraphContainer;
class CGraphView;
class FOffScreen;

// CGraphix view

typedef struct SFRect
{
	double x1, x2, y1, y2;
}SFRect;

class CGraphix : public CWnd
{
protected:
	CGraphix();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CGraphix)

	virtual ~CGraphix();

// Attributes
public:
	//offscreem stuff
	FOffScreen* offscreen;
	void UpdateGraphWindow(CRect* rect, BOOL Redraw);
	void DoRedraw(CDC* dc, LPCRECT r);

	double p_x1, p_x2, p_y1, p_y2;
	DWORD m_grflags;
	int tipCount;
	COLORREF bkColor;

	//zoom stuff
	BOOL bZoomActive;
	CRectTracker* zoomrect;
	SFRect* undoZoomRect;
	//on-the-fly zoom rect creation
	BOOL bCreateZoomRect;
	CPoint new_zoom_1, new_zoom_2;

	CCoordinates *CurrentCoordsX, *CurrentCoordsY;
	CGraphContainer* parentPtr;
	CGraphView* mainView;
	CToolTipCtrl tooltip;

// Operations
public:
	CGraphix(CGraphContainer* parentWnd);
	CString GetToolTipString(CPoint pt);

	void ActivateToolTip(BOOL bActivate);
	void DrawAxis(CDC* pDC);
	void DrawPoints(CDC* dc);
	void DrawSquarePoint(CDC* dc, int x, int y);
	void InitCoords();
	void ApplyZoom();
	void DoFit(DWORD fitType);

	void DoGraphProps();

	void AddPoint(SGraphChange* sgc);

	//graphix attributes
	DWORD GetGraphixFlags();
	DWORD SetGraphixFlags(DWORD new_flags, BOOL bRedraw);

	double GetX1(){return p_x1;};
	double GetX2(){return p_x2;};
	double GetY1(){return p_y1;};
	double GetY2(){return p_y2;};
	void GetWorldCoords(double* x1, double* x2, double* y1, double* y2);

	void SetMinX(double x, BOOL bRedraw);
	void SetMaxX(double x, BOOL bRedraw);
	void SetMinY(double y, BOOL bRedraw);
	void SetMaxY(double y, BOOL bRedraw);
	void SetWorldCoords(double _x1, double _x2, double _y1, double _y2, BOOL bRedraw);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGraphix)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CGraphix)
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnCopyText();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CGraphContainer view

class CGraphContainer : public CScrollView
{
protected:
	CGraphContainer();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CGraphContainer)

// Attributes
public:
	FVRuler* vruler;
	FHRuler* hruler;
	CGraphix* grWnd;

	//points and coordinates
//	double x1, x2, y1, y2;

// Operations
public:
	int GetNMaxH();
	int GetNMaxV();
	double GetX1(){return grWnd->GetX1();};
	double GetX2(){return grWnd->GetX2();};
	double GetY1(){return grWnd->GetY1();};
	double GetY2(){return grWnd->GetY2();};
	void GetWorldCoords(double* x1, double* x2, double* y1, double* y2);

	CGraphView* GetMainGraphView();
	void SetWorldDim(double _x1, double _x2, double _y1, double _y2, BOOL bRedraw);
	void UpdateTitles(BOOL bXAxis, BOOL bRedraw);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGraphContainer)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CGraphContainer();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CGraphContainer)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRWND_H__6B968182_76FF_11D1_BD0B_00C04FB6D88F__INCLUDED_)
