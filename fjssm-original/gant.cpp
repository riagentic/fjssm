#include "stdafx.h"
#include "Gant.h"
#include "colorbtn.h"
#include "afxwin.h"


IMPLEMENT_DYNCREATE(CGant, CWnd)

// CONCRUCTORS
CGant::CGant()
{
    RegisterWindowClass();

	m_areaJobNode.RemoveAll();
	m_lstJob.RemoveAll();

// Scrollbars
//----------------------------
	m_nHScrollPos = 0;		
	m_nVScrollPos= 0;
	m_nVPageSize= 0;	
	m_nHPageSize= 0;
	m_nVSMax= 0;			
	m_nHSMax= 0;			

// Scales
//----------------------------
	m_fXScale = 0.7;
	m_fYScale = 0.7;

// Margins and space
//----------------------------
	m_nXMargin = 10;
	m_nYMargin = 10;
	m_nLLegend = 50;
	m_nTLegend = 60;

	m_nYVoid = 10;

// Area
//----------------------------
	BOOL m_bDataLoaded = FALSE;

}


CGant::~CGant()
{
	CWnd::DestroyWindow();
	CWnd::PostNcDestroy();
}


BOOL CGant::RegisterWindowClass()
{
    WNDCLASS wndcls;
    HINSTANCE hInst = AfxGetInstanceHandle();
   // HINSTANCE hInst = AfxGetResourceHandle(); 

    if (!(::GetClassInfo(hInst, GANT_CLASSNAME, &wndcls)))
    {
        // otherwise we need to register a new class
        wndcls.style            = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
        wndcls.lpfnWndProc      = ::DefWindowProc;
        wndcls.cbClsExtra       = wndcls.cbWndExtra = 0;
        wndcls.hInstance        = hInst;
        wndcls.hIcon            = NULL;
        wndcls.hCursor          = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
		wndcls.hbrBackground    = (HBRUSH)GetStockObject(WHITE_BRUSH);
        wndcls.lpszMenuName     = NULL;
        wndcls.lpszClassName    = GANT_CLASSNAME;
		

        if (!AfxRegisterClass(&wndcls)) {
            AfxThrowResourceException();
            return FALSE;
        }
    }

    return TRUE;
}

BOOL CGant::Create(const RECT& rect, CWnd* pParentWnd, UINT nID, DWORD dwStyle)
{
    ASSERT(pParentWnd->GetSafeHwnd());

	 if (!CWnd::Create(GANT_CLASSNAME, NULL, dwStyle, rect, pParentWnd, nID)) 
        return FALSE;

	
	SetDefaultScale();
	SetScroll(rect.right,rect.bottom);

    return TRUE;
}

int CGant::SetDefaultScale()
{
	CRect rect;
	GetClientRect(&rect);

	int cx = rect.right;
	int cy = rect.bottom;

	m_nVScrollPos = 0;
	m_nHScrollPos = 0;

	m_fXScale =  (double)cx/(double)GetMaxX() * 0.9;
	m_fYScale =  (double)cy/(double)GetMaxY() * 0.8;

	return 0;
}

// MESSAGE MAP
BEGIN_MESSAGE_MAP(CGant, CWnd)
    ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()

	ON_WM_CHAR()

END_MESSAGE_MAP()
// Size
//---------------------------------------------------

void CGant::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	SetScroll(cx,cy);
}


// Scroll
//---------------------------------------------------
void CGant::SetScroll(int cx,int cy)
{

	// Maximalni plocha
	m_nHSMax = (( GetMaxX() - m_nLLegend) * m_fXScale ) + m_nLLegend;  
	m_nVSMax = (( GetMaxY() - m_nTLegend) * m_fYScale ) + m_nTLegend;

	// 1. Nastaveni Posuvniku
	// Horizontalni
	SCROLLINFO si;
	si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
	si.nMin = 0;
	si.nMax = m_nHSMax-1;
	si.nPos = 0;
	si.nPage = cx;
	
	SetScrollInfo(SB_HORZ,&si,TRUE);

	// Vertikalni
	si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
	si.nMin = 0;
	si.nMax = m_nVSMax-1;
	si.nPos = 0;
	si.nPage = cy;

	SetScrollInfo(SB_VERT,&si,TRUE);
}

void CGant::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// 1. Kroky 

	CRect rect;
	GetClientRect(&rect);

	int nDelta = 0;
	int nStep = 10;
	
	switch (nSBCode)
	{
		case SB_LINELEFT:
			nDelta -= nStep;
			break;
		case SB_LINERIGHT:
			nDelta += nStep;
			break;
		case SB_PAGELEFT:
			nDelta -=m_nHPageSize;
			break;
		case SB_PAGERIGHT:
			nDelta += m_nHPageSize;
			break;
		case SB_THUMBTRACK:
			nDelta = (int) nPos - m_nHScrollPos;
			break;

	}

	//2. Validace mezi
	int nScrollPos = m_nHScrollPos + nDelta;

	if (nScrollPos < 0)
	{
		nDelta = -m_nHScrollPos;
	}
	else if (nScrollPos > m_nHSMax - rect.right )
		nDelta = m_nHSMax - m_nHScrollPos - rect.right;

	// 3. Scroll a uprava m_nVSCroll
	if (nDelta != 0)
	{
		m_nHScrollPos+= nDelta;
		SetScrollPos(SB_HORZ,m_nHScrollPos,TRUE);
		ScrollWindow(-nDelta,0);
		
	}
	
	
}

void CGant::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	
	// 1. Kroky 
	CRect rect;
	GetClientRect(&rect);

	int nDelta = 0;
	int nStep = 10;
	
	switch (nSBCode)
	{
		case SB_LINEUP:
			nDelta -= nStep;
			break;
		case SB_LINEDOWN:
			nDelta += nStep;
			break;
		case SB_PAGEUP:
			nDelta -=m_nVPageSize;
			break;
		case SB_PAGEDOWN:
			nDelta += m_nVPageSize;
			break;
		case SB_THUMBTRACK:
			nDelta = (int) nPos - m_nVScrollPos;
        break;
	}

	//2. Validace mezi
	int nScrollPos = m_nVScrollPos + nDelta;

	if (nScrollPos < 0)
	{
		nDelta = -m_nVScrollPos;
	}
	else if (nScrollPos > m_nVSMax - rect.bottom)
		nDelta = m_nVSMax - m_nVScrollPos - rect.bottom;

	// 3. Scroll a uprava m_nVSCroll
	if (nDelta != 0)
	{
		m_nVScrollPos+= nDelta;
		SetScrollPos(SB_VERT,m_nVScrollPos,TRUE);
		ScrollWindow(0,-nDelta);
		
	}
	
}

// JSS Data
//---------------------------------------------------
int CGant::SetData(CFJSS* pFJSS)
{

	CMachineList ml;

	m_lstJob = pFJSS->m_lstJob;
	m_lstMachine = pFJSS->m_lstMachine;
	m_areaJobNode = pFJSS->m_areaJobNode;
	
	if (pFJSS->m_nSelectedResult == 0)		// SA
		m_areaSchedule = pFJSS->m_areaSASchedule;
	if (pFJSS->m_nSelectedResult == 1)		// GA
		m_areaSchedule = pFJSS->m_areaGASchedule;

	m_strName = pFJSS->m_strName;
	m_strType = pFJSS->GetStrType();
	m_strMethod = pFJSS->GetStrMethod(pFJSS->m_nSelectedResult);

	m_bFDD = pFJSS->FDD();
	m_bFPT = pFJSS->FPT();

	m_bDataLoaded = TRUE;

	return 0;
}

// Axis
//---------------------------------------------------
int CGant::PTX(int nX)
{
	int nXCoord = 0;
	// Typ vsechno flex
	nXCoord = (m_nXMargin + nX) * m_fXScale + m_nLLegend;
	return nXCoord;
}

int CGant::PTY(int nJob,int nPart)
{
	int nCoordY = 0;

	nCoordY = (( m_nYMargin + ( 7 * nJob * m_nYVoid ) + ((7 - nPart) * m_nYVoid ) )
			   * m_fYScale) + m_nTLegend;

	return nCoordY;
}

int CGant::DDY(int nJob,int nPart)
{
	int nCoordY = 0;
	nCoordY = (( m_nYMargin + ( 7 * nJob * m_nYVoid ) + ((3 - nPart) * m_nYVoid ) )
			   * m_fYScale) + m_nTLegend;


	return nCoordY;
}

// Vrati maximalni pocet bodu v Xove ose, ktere se budou vykreslovat
// = okraj + legenda + max(DD,PT)
int CGant::GetMaxX()
{

	int nMaxPTOrDD = 0;

	if (m_bFPT) nMaxPTOrDD = m_areaSchedule.m_fMakespan[3];
	if (!m_bFPT) nMaxPTOrDD = m_areaSchedule.m_fMakespan[0];

	for (int i = 0; i<m_lstJob.GetSize();i++)
	{
		if (m_bFDD)  nMaxPTOrDD = max(nMaxPTOrDD,m_lstJob[i].m_fDD[3]);
		if (!m_bFDD) nMaxPTOrDD = max(nMaxPTOrDD , m_lstJob[i].m_fDD[0]);
	}

	int nMaxX = 2*m_nXMargin + nMaxPTOrDD + m_nLLegend;

	return nMaxX;

}

int CGant::GetMaxY()
{
		int nMaxY = (7 * m_areaJobNode.GetSize() * m_nYVoid) + 2*m_nYMargin + m_nTLegend;


		return nMaxY;
}

// Draw and Paint
//---------------------------------------------------

void CGant::OnPaint() 
{
    CPaintDC dc(this);      // device context for painting

	dc.SetWindowOrg (m_nHScrollPos, m_nVScrollPos);

	OnDraw(&dc);

}


void CGant::OnDraw(CDC* pDC)
{
	// Vykresleni PT

	if (m_bDataLoaded) 
	{
		DrawMainLegend(pDC);
		DrawJobLegend(pDC);
		DrawGrid(pDC);
		DrawPT(pDC);	
	}

}

void CGant::DrawPT(CDC* pDC)
{
	
	CBrush brMachine[20];

	for (int i=0; i<20; i++)
		brMachine[i].CreateSolidBrush(CColorBtnDlg::colors[i]);

	CPen penBlack;
	penBlack.CreatePen(PS_SOLID,1,RGB(0,0,0));
	CBrush brushBlue;
	brushBlue.CreateSolidBrush(RGB(100,100,255));

	pDC->SelectObject(&penBlack);
	pDC->SelectObject(&brushBlue);

	for (int i=0; i<m_areaJobNode.GetSize();i++)
	{
		CPoint ptDD[4];
        for (int k=0; k<2; k++)
		{
			if (m_bFDD)
			{
				ptDD[k] = CPoint(PTX(m_lstJob[i].m_fDD[k]), DDY(i,k));
				ptDD[k+2] = CPoint(PTX(m_lstJob[i].m_fDD[k+2]),DDY(i,1-k));
			}
			else
			{
				ptDD[k] = CPoint(PTX(0), DDY(i,k));
				ptDD[k+2] = CPoint(PTX(m_lstJob[i].m_fDD[0]),DDY(i,1-k));
			}
		}

		int nJobColor = m_lstJob[i].m_nColor;
		pDC->SelectObject(&brMachine[nJobColor]);
		pDC->MoveTo(ptDD[3]);
		//pDC->Polyline(pt,8);
		pDC->Polygon(ptDD,4);


		for (int j=0; j<m_areaJobNode[i].GetSize();j++)
		{
			// Fuzzy Procesing TIME
			int nMachine = m_areaJobNode[i][j].m_nMachine;
			pDC->SelectObject(&brMachine[nMachine]);
			CPoint pt[8];
			for (int k=0; k<FN_PART;k++)
			{
				if (m_bFPT)
				{
					pt[k] = CPoint(PTX(m_areaJobNode[i][j].m_fr[k]),PTY(i,k));
					pt[k+FN_PART] = CPoint(PTX(m_areaJobNode[i][j].m_fC[FN_PART-k-1]),PTY(i,FN_PART-k-1));
				}
				else
				{
					pt[k] = CPoint(PTX(m_areaJobNode[i][j].m_fr[0]),PTY(i,k));
					pt[k+FN_PART] = CPoint(PTX(m_areaJobNode[i][j].m_fC[0]),PTY(i,FN_PART-k-1));
				}
			}
			
			int nColorIndex = m_lstMachine[m_areaJobNode[i][j].m_nMachine].m_nColor;			
			pDC->SelectObject(&brMachine[nColorIndex]);
			pDC->MoveTo(pt[7]);
			//pDC->Polyline(pt,8);
			pDC->Polygon(pt,8);

		}
	}


}


void CGant::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{

	switch (nChar)
	{
		case _T('x'):    m_fXScale -= 0.1;  break;
		case _T('y'):	 m_fYScale -= 0.1;  break;
		case _T('X'):    m_fXScale += 0.1;  break;
		case _T('Y'):	 m_fYScale += 0.1;  break;
		case _T('r'):	 SetDefaultScale();  break;
	}


	if (m_fXScale < 0.1)   m_fXScale = 0.1;
	if (m_fYScale < 0.1)   m_fYScale = 0.1;

	CRect rect;
	GetClientRect(&rect);
	SetScroll(rect.right,rect.bottom);
	Invalidate();

	CWnd::OnChar(nChar, nRepCnt, nFlags);
}


void CGant::DrawGrid(CDC* pDC)
{

	CPen penBlack;
	CPen penGray;
	penBlack.CreatePen(PS_SOLID,1,RGB(0,0,0));
	penGray.CreatePen(PS_SOLID,1,RGB(200,200,200));

	for (int i=0; i<m_lstJob.GetSize();i++)
	{
		pDC->SelectObject(&penBlack);
		pDC->MoveTo(PTX(0),PTY(i,0));
		pDC->LineTo(PTX(GetMaxX()-m_nXMargin-m_nLLegend),PTY(i,0));

		pDC->MoveTo(PTX(0),PTY(i,3));
		pDC->LineTo(PTX(GetMaxX()-m_nXMargin-m_nLLegend),PTY(i,3));
	}

	// GRAY
	for (int i=0; i<m_lstJob.GetSize();i++)
	{
		pDC->SelectObject(&penGray);
		pDC->MoveTo(PTX(0),PTY(i,1));
		pDC->LineTo(PTX(GetMaxX()-m_nXMargin-m_nLLegend),PTY(i,1));

		pDC->MoveTo(PTX(0),PTY(i,2));
		pDC->LineTo(PTX(GetMaxX()-m_nXMargin-m_nLLegend),PTY(i,2));

		pDC->MoveTo(PTX(0),DDY(i,0));
		pDC->LineTo(PTX(GetMaxX()-m_nXMargin-m_nLLegend),DDY(i,0));
		pDC->MoveTo(PTX(0),DDY(i,1));
		pDC->LineTo(PTX(GetMaxX()-m_nXMargin-m_nLLegend),DDY(i,1));


	}

}

void CGant::DrawMainLegend(CDC* pDC)
{
	CString str;
	for (int i=0; i<m_lstJob.GetSize(); i++)
	{
		str.Format(_T("Job %d"),i+1);
		pDC->TextOut(JTX(),JTY(i),str);
	}
}

void CGant::DrawJobLegend(CDC* pDC)
{
	pDC->TextOut(LTX(),LTY(0),m_strName);
	pDC->TextOut(LTX(),LTY(1),m_strType);
	pDC->TextOut(LTX(),LTY(2),m_strMethod);
}

int CGant::JTX()
{
	return m_nXMargin * m_fXScale;
}

int CGant::JTY(int nJob)
{
	return PTY(nJob,4);
}

int CGant::LTX()
{
	return (m_nXMargin * m_fXScale) + m_nLLegend;
}

int CGant::LTY(int nRow)
{
	return (m_nYMargin * m_fYScale) + (20*nRow);
}


bool CGant::CopyToClipboard()
{
	//
	// Special thanks to Zafir Anjum for a large part of following code.
	//
	CBitmap         bitmap;
	CClientDC       dc(this);
	CDC             memDC;

	BeginWaitCursor();

	memDC.CreateCompatibleDC(&dc);

	bitmap.CreateCompatibleBitmap(&dc, m_nHSMax,m_nVSMax);

	CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);

	CBrush brushBlack;
	brushBlack.CreateSolidBrush(RGB(255,255,255));
	memDC.SelectObject(brushBlack);
	memDC.Rectangle(0,0,m_nHSMax,m_nVSMax);
	DrawPT(&memDC);

	DrawJobLegend(&memDC);
	DrawMainLegend(&memDC);
	DrawGrid(&memDC);


	this->OpenClipboard() ;
	EmptyClipboard() ;
	SetClipboardData (CF_BITMAP, bitmap.GetSafeHandle() ) ;
	CloseClipboard () ;

	bitmap.Detach();

	EndWaitCursor();

	return true;
}
