// Graf 

#pragma once
#include <afxtempl.h>
#include "FJSSM.h"


#define DBITMAP_CLASSNAME    _T("MFCGDBitmap")  // Window class name

// GANTUV GRAF
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CDesktopBitmap : public CWnd

{
    DECLARE_DYNCREATE(CDesktopBitmap)
	BOOL m_bDoubleBuffer;			// buffer pro CMemDC	

// Construction
public:
	CDesktopBitmap();
    virtual ~CDesktopBitmap();
    
    BOOL Create(const RECT& rect, CWnd* parent, UINT nID,
		DWORD dwStyle =   WS_BORDER | WS_TABSTOP | WS_VISIBLE);

// Attributes
public:
	BOOL RegisterWindowClass();


    virtual void  OnDraw(CDC* pDC);

protected:
    // Generated message map functions

    DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);


};

