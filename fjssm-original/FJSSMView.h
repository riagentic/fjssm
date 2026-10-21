// FJSSMView.h : interface of the CFJSSMView class
//

#pragma once
#include "Gant.h"
//#include "Graph.h"
#include "mschart.h"
#include "dbitmap.h"
#include "grid/gridctrl.h"
#include "listctrlstyled.h"

class CFJSSMView : public CView
{
protected: // create from serialization only
	CFJSSMView();
	DECLARE_DYNCREATE(CFJSSMView)

// Attributes
public:
	CFJSSMDoc* GetDocument() const;
	BOOL		m_bGraphComplete;

// Operations
public:

// Overrides
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CFJSSMView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnViewJobs();

public:
	//ImageList
	CImageList m_ilLarge;
	CImageList m_ilSmall;


	afx_msg void OnViewProject();
	afx_msg void OnViewMachines();
	afx_msg void OnViewProcess();
	afx_msg void OnViewEdges();
	afx_msg void OnViewCpm();

	void SetViewCPM();



   // pocet sloupcu v ListView
   int m_nColumn;

   // priprava ListView - rutini zalezitosti, legendy, sirka, style, atd...
   int DrawListView(CStrList& lstsLegend,CStrArea& areasData);
   int DrawGrid(CStrArea& areasData,int nFixRow = 1,int nFixCol = 1);

	afx_msg void OnConvertCrispdd();
	afx_msg void OnConvertCrispt();
	afx_msg void OnConvertFuzzydd();
	afx_msg void OnConvertFuzzypt();
	afx_msg void OnUpdateConvertMenu(CCmdUI* pCmdUI);
   int ShowConvertReport(int nConvert);
   int AskForConvert(int nType);

   int UpdateMenu();
protected:
	
public:
	afx_msg void OnUpdateConvertCrispdd(CCmdUI *pCmdUI);
	afx_msg void OnUpdateConvertCrispt(CCmdUI *pCmdUI);
	afx_msg void OnUpdateConvertFuzzydd(CCmdUI *pCmdUI);
	afx_msg void OnUpdateConvertFuzzypt(CCmdUI *pCmdUI);

	afx_msg void OnComputeSA();
	afx_msg void OnComputeGA();
	afx_msg void OnTestTest();
	
	afx_msg void OnResultGant();
	afx_msg void OnResultHistory();
	afx_msg void OnResultSchedule();

	int SetView(int nViewType);

	CListCtrl* m_pListCtrl;
	CMschart1* m_pGraph;
	CGant* m_pGant;
	CGridCtrl* m_pGrid;
		

		afx_msg void OnSize(UINT nType, int cx, int cy);

		afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
		afx_msg void OnUpdateEditCopy(CCmdUI *pCmdUI);
		afx_msg void OnEditCopy();
		afx_msg void OnResultConvexp();
		afx_msg void OnResultCritIteration();
		afx_msg void OnResultCritRe();
		afx_msg void OnResultCritAre();
		afx_msg void OnResultCritSoare();
		virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);

	void DrawBitmap(CDC* pDC,UINT Bitmap);
	afx_msg void OnEditSelectAll();
	afx_msg void OnUpdateEditSelectAll(CCmdUI *pCmdUI);
protected:

public:
	int m_nAction;	// akce, kterou vykonava pohled
protected:
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
};

#ifndef _DEBUG  // debug version in FJSSMView.cpp
inline CFJSSMDoc* CFJSSMView::GetDocument() const
   { return reinterpret_cast<CFJSSMDoc*>(m_pDocument); }
   
#endif

