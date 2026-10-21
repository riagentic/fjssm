// FJSSM.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "FJSSM.h"
#include "MainFrm.h"

#include "FJSSMDoc.h"
#include "FJSSMView.h"
#include "mschart.h"
#include "CreditStatic.h"
#include "fjssfn.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFJSSMApp

BEGIN_MESSAGE_MAP(CFJSSMApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()


// CFJSSMApp construction

CFJSSMApp::CFJSSMApp()
{
	EnableHtmlHelp();

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CFJSSMApp object

CFJSSMApp theApp;

// CFJSSMApp initialization

BOOL CFJSSMApp::InitInstance()
{
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinApp::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CFJSSMDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CFJSSMView));
	AddDocTemplate(pDocTemplate);
	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);
	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	  CMenu* pMenu = m_pMainWnd->GetMenu();
	  if (pMenu)pMenu->DestroyMenu();
	  HMENU hMenu = ((CMainFrame*) m_pMainWnd)->NewMenu();
	  pMenu = CMenu::FromHandle( hMenu );
	  m_pMainWnd->SetMenu(pMenu);
	  ((CMainFrame*)m_pMainWnd)->m_hMenuDefault = hMenu;


	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();
	return TRUE;

}


// AboutDialog
//=========================================================
// AboutDialog
//=========================================================
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	CCreditStatic m_static;
	UINT bitmapIDs[2];
	UINT TimerOn;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	
public:
	afx_msg void OnStnClickedDisplayStatic();

	CBitmap m_bmBitmap;
	CStatic m_stBitmap;
};

// CAboutDlg dialog used for App About
void CFJSSMApp::OnAppAbout()
{
	CAboutDlg dlg;
	dlg.DoModal();
}


CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
//	bitmapIDs[0] = IDB_BITMAP1;
	//bitmapIDs[1] = IDB_BITMAP1;
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX,IDC_ST_BITMAP,m_stBitmap);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_STN_CLICKED(IDC_DISPLAY_STATIC, OnStnClickedDisplayStatic)
END_MESSAGE_MAP()


#define  DISPLAY_TIMER_ID		151

void CAboutDlg::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent != DISPLAY_TIMER_ID)
	{
		CDialog::OnTimer(nIDEvent);
		return;
	}

	static int index = 0;

	index = ++index % 2;
	
	m_static.SetBkImage(bitmapIDs[index]);
	
	CDialog::OnTimer(nIDEvent);
}

void CAboutDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	if(TimerOn)
		ASSERT(KillTimer(DISPLAY_TIMER_ID));
}


BOOL CAboutDlg::OnInitDialog() 
{
	// nazvy obrazku musi byt stringy

	CDialog::OnInitDialog();
	m_stBitmap.SetBitmap(GetMaskedBitmap(m_bmBitmap,IDB_LOGO_ABOUT,RGB(255,255,255)));
	UpdateData(FALSE);

	char *pArrCredit = { 
		"IDB_LOGO_ABOUT_CREDIT^|"    // this is a quoted bitmap resource 		
		"Program pro fuzzy rozvrhování|"		
		"zakázkové výroby|||||"		
		"Na vývoji se podíleli:\r||"		
		"Programátor\r|riagentic||"        
		"Testování\r| riagentic||"
        "IDB_VUTLOGO_ABOUT_CREDITS^|"  // this is a quoted bitmap resource   
		"IDB_BIRD_ABOUT_CREDITS^||"  // this is a quoted bitmap resource   
		"All right reserved|Copyright (c) 2002||||||||||||||||||||||||"        
        };


	m_static.SetColor(4,RGB(0,0,255));
	m_static.SetColor(3,RGB(0,0,0));
	m_static.SetColor(2,RGB(0,0,0));
	m_static.SetColor(1,RGB(0,0,0));
	m_static.SetColor(0,RGB(0,0,0));

		
	m_static.SubclassDlgItem(IDC_DISPLAY_STATIC,this);
	m_static.SetCredits(pArrCredit,'|');
	m_static.SetSpeed(DISPLAY_MEDIUM);
	m_static.SetColor(BACKGROUND_COLOR, GetSysColor(COLOR_3DFACE));
	m_static.SetTransparent();
	//m_static.SetGradient(GRADIENT_RIGHT_DARK);
	//m_static.SetBkImage(bitmapIDs[0]);
	TimerOn = SetTimer(DISPLAY_TIMER_ID,200000000,NULL);	
	m_static.StartScrolling();
	
    ASSERT(TimerOn != 0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CAboutDlg::OnStnClickedDisplayStatic()
{
	// TODO: Add your control notification handler code here
}
