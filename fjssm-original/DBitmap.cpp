#include "stdafx.h"
#include "DBitmap.h"



IMPLEMENT_DYNCREATE(CDesktopBitmap, CWnd)

// CONCRUCTORS
CDesktopBitmap::CDesktopBitmap()
{
    RegisterWindowClass();
}


CDesktopBitmap::~CDesktopBitmap()
{
	CWnd::DestroyWindow();
	CWnd::PostNcDestroy();
}


BOOL CDesktopBitmap::RegisterWindowClass()
{
    WNDCLASS wndcls;
    HINSTANCE hInst = AfxGetInstanceHandle();
   // HINSTANCE hInst = AfxGetResourceHandle(); 

	if (!(::GetClassInfo(hInst, DBITMAP_CLASSNAME, &wndcls)))
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
        wndcls.lpszClassName    = DBITMAP_CLASSNAME;
		

        if (!AfxRegisterClass(&wndcls)) {
            AfxThrowResourceException();
            return FALSE;
        }
    }

    return TRUE;
}

BOOL CDesktopBitmap::Create(const RECT& rect, CWnd* pParentWnd, UINT nID, DWORD dwStyle)
{
    ASSERT(pParentWnd->GetSafeHwnd());

	 if (!CWnd::Create(DBITMAP_CLASSNAME, NULL, dwStyle, rect, pParentWnd, nID)) 
        return FALSE;
    return TRUE;
}


// MESSAGE MAP
BEGIN_MESSAGE_MAP(CDesktopBitmap, CWnd)
    ON_WM_PAINT()
	ON_WM_SIZE()
END_MESSAGE_MAP()


void CDesktopBitmap::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
}


void CDesktopBitmap::OnPaint() 
{
    CPaintDC dc(this);      // device context for painting
	OnDraw(&dc);
}


void CDesktopBitmap::OnDraw(CDC* pDC)
{
	pDC->MoveTo(0,0);
	pDC->LineTo(100,100);
}

