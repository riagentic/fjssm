// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "FJSSM.h"
#include "splash.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


namespace
{
	LONG GetRegKey(HKEY key, LPCTSTR subkey, LPTSTR retdata)
	{
		HKEY hkey;
		LONG retval = RegOpenKeyEx(key, subkey, 0, KEY_QUERY_VALUE, &hkey);

		if (retval == ERROR_SUCCESS) 
		{
			long datasize = MAX_PATH;
			TCHAR data[MAX_PATH];
			RegQueryValue(hkey, NULL, data, &datasize);
			lstrcpy(retdata,data);
			RegCloseKey(hkey);
		}

		return retval;
	}
}

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	// Global help commands
	ON_COMMAND(ID_HELP_FINDER, CFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_HELP, CFrameWnd::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, CFrameWnd::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, CFrameWnd::OnHelpFinder)
	ON_WM_MEASUREITEM()
	ON_WM_MENUCHAR()
	ON_WM_INITMENUPOPUP()

// LOGO SUPPORT
	ON_REGISTERED_MESSAGE(ON_LOGO_CLICK,OnLogoClick)
// LOGO SUPPORT

END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
	m_menu.DestroyMenu();
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	// MSChart OCX licence register, if not installed
	//[HKEY_CLASSES_ROOT\Licenses\12B142A4-BD51-11d1-8C08-0000F8754DA1]
	//@ = "aadhgafabafajhchnbchehfambfbbachmfmb"

    HKEY hk; 
    DWORD dwData; 
    TCHAR szBuf[80]; 
	LONG lRet;

	// Existuje licence
	if(RegOpenKeyEx(HKEY_CLASSES_ROOT,
		_T("Licenses\\12B142A4-BD51-11d1-8C08-0000F8754DA1"),
                   0, KEY_QUERY_VALUE,&hk) != ERROR_SUCCESS) 
	{						// NE
		RegCreateKey(HKEY_CLASSES_ROOT,
			_T("Licenses\\12B142A4-BD51-11d1-8C08-0000F8754DA1"),&hk);
		
		strcpy(szBuf, "aadhgafabafajhchnbchehfambfbbachmfmb");

		RegSetValueEx(hk,			  // subkey handle 
	        "",						  // value name  - "" default
		    0,                        // must be zero 
			REG_SZ,					  // value type 
			(LPBYTE) szBuf,           // pointer to value data 
			strlen(szBuf) + 1);       // length of value data 
   
		RegCloseKey(hk); 
	}
	else
		RegCloseKey(hk);	// ANO


	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	//CSplashWnd::
		//ShowSplashScreen(this);
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndToolBar2.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar2.LoadToolBar(IDR_MAINFRAME2))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}


	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	// TODO: Delete these three lines if you don't want the toolbar to be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	m_wndToolBar2.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar2);

	// LOGO SUPPORT
	CControlBar* pTopDockBar = GetControlBar(AFX_IDW_DOCKBAR_TOP);
	if (pTopDockBar 
	&& m_wndLogo.Create(NULL,
						WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
						CRect(0,0,0,0),
						pTopDockBar,
						IDS_LOGO))
	{	
		
		m_wndLogo.LoadBitmaps(IDB_LOGO_TOOLBAR, IDB_LOGO_TOOLBAR);
		m_wndLogo.SizeToContent();
	}
	// LOGO SUPPORT


	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame message handlers

HMENU CMainFrame::NewMenu()
{
  m_menu.LoadMenu(IDR_MAINFRAME);  
  m_menu.LoadToolbar(IDR_MAINFRAME);
  return(m_menu.Detach());
}

void CMainFrame::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	// TODO: Add your message handler code here and/or call default

	CFrameWnd::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}

LRESULT CMainFrame::OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu)
{
	// TODO: Add your message handler code here and/or call default

	return CFrameWnd::OnMenuChar(nChar, nFlags, pMenu);
}

void CMainFrame::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	CFrameWnd::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);

	// TODO: Add your message handler code here
}


// LOGO SUPPORT
LRESULT CMainFrame::OnLogoClick(WPARAM, LPARAM)
{
	CString URL(_T(""));

	HINSTANCE result = ::ShellExecute(NULL, _T("open"), URL, NULL,NULL, SW_SHOW);

	if ((UINT)result <= HINSTANCE_ERROR)
	{
		TCHAR key[MAX_PATH + MAX_PATH] = {_T('\0')};
		TCHAR ext[_MAX_EXT] = {_T('\0')};

		_splitpath(URL,NULL,NULL,NULL,ext);
			
		if (GetRegKey(HKEY_CLASSES_ROOT, ext, key) == ERROR_SUCCESS) 
		{
			lstrcat(key, _T("\\shell\\open\\command"));

			if (GetRegKey(HKEY_CLASSES_ROOT,key,key) == ERROR_SUCCESS) 
			{
				TCHAR *pos;
				pos = _tcsstr(key, _T("\"%1\""));
				if (pos == NULL) 
				{                       
					pos = strstr(key, _T("%1"));
					if (pos == NULL)            
						pos = key+lstrlen(key)-1;
					else
						*pos = '\0';            
				}
				else
					*pos = '\0';                

				lstrcat(pos, _T(" "));
				lstrcat(pos, URL);

				::WinExec(key,SW_SHOW);
			}
		}
	}

	return 0;
}
// LOGO SUPPORT