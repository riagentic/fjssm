// Gantuv graf

#pragma once
#include <afxtempl.h>
#include "FJSSM.h"
#include "FJSS.h"
#include "FJSSUnit.h"


#define GANT_CLASSNAME    _T("MFCGant")  // Window class name

// GANTUV GRAF
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CGant : public CWnd
{
    DECLARE_DYNCREATE(CGant)
	BOOL m_bDoubleBuffer;			// buffer pro CMemDC	

// Construction
public:
	CGant();
    virtual ~CGant();

    BOOL Create(const RECT& rect, CWnd* parent, UINT nID,
		DWORD dwStyle =   WS_VSCROLL| WS_HSCROLL | WS_BORDER | WS_TABSTOP | WS_VISIBLE);

// Attributes
public:
	BOOL RegisterWindowClass();
    virtual void  OnDraw(CDC* pDC);
	void SetScroll(int cx,int cy);
	
// OVERRIDES
	virtual bool CopyToClipboard();

protected:
    // Generated message map functions

    DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);

public:
//--------------------------------------------------------
// DATA
//--------------------------------------------------------

// Area
//----------------------------

	CRect m_rectMaxArea;
// Text
//----------------------------
CString m_strName;
CString m_strType;
CString m_strMethod;


// Scrollbars
//----------------------------
	int m_nHScrollPos;		
	int m_nVScrollPos;
	int m_nVPageSize;	
	int m_nHPageSize;
	int m_nVSMax;			// maximalni y souradnice 
	int m_nHSMax;			// maximalni xova souradnice

// Scales
//----------------------------
	double m_fXScale;
	double m_fYScale;

// Margins and space
//----------------------------
	int m_nXMargin;
	int m_nYMargin;

	int m_nLLegend;
	int m_nTLegend;

	int m_nYVoid;		// mezera mezi jednotlivymi slozkami

// Coordinates
//----------------------------
// Xsove souradnice i-teho jobu prislusne polozky
	int JTX();
	int LTX();
	int JTY(int nJob);
	int LTY(int nRow);

	int PTX(int nX);					// souradnice X pro PT
	int PTY(int nJob,int nPart);		// souradnice Y pro PT i DD

	int DDY(int nJob,int nPart);		// souradnice y pro dD

	int GetMaxX();						// absolutni maximalni pocet bodu, ktere se maji vykreslit
	int GetMaxY();					

// JSS Data
//----------------------------
	CJobList m_lstJob;				// data jobu
	CMachineList m_lstMachine;		// data stroju
	CNodeArea m_areaJobNode;		// area nodu
	CScheduleArea m_areaSchedule;	// area rozvrhu

	BOOL m_bFDD;
	BOOL m_bFPT;

	int SetData(CFJSS* pFJSS);
	int SetDefaultScale();
	

	BOOL m_bDataLoaded;

// Draw
//----------------------------

	void DrawPT(CDC* pDC);
	void DrawJobLegend(CDC* pDC);
	void DrawMainLegend(CDC* pDC);
	void DrawGrid(CDC* pDC);

// Events
//----------------------------

public:
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

};

