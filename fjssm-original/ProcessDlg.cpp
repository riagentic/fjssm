// ProcessDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FJSSM.h"
#include "ProcessDlg.h"


// CProcessDlg dialog

IMPLEMENT_DYNAMIC(CProcessDlg, CDialog)
CProcessDlg::CProcessDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProcessDlg::IDD, pParent)
{
	m_nJobIndex = 0;
	m_nProcessIndex = 0;
}

CProcessDlg::~CProcessDlg()
{
}

void CProcessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	// ICO Buttons
	DDX_Control(pDX,IDC_BT_UP,m_btUp);
	DDX_Control(pDX,IDC_BT_DOWN,m_btDown);
	DDX_Control(pDX,IDC_BT_LEFT,m_btLeft);
	DDX_Control(pDX,IDC_BT_RIGHT,m_btRight);

	// Job
	DDX_Text(pDX,IDC_ST_JOBINDEX,m_nJobIndex);
	DDX_Text(pDX,IDC_ST_JOBNAME,m_strJobName);

	// Process
	DDX_Text(pDX,IDC_ED_PROCESSNAME,m_strProcessName);
	DDX_Text(pDX,IDC_ED_PT1,m_fPT[0]);

	DDV_MaxChars(pDX,m_strProcessName,30);
	DDV_MinMaxInt(pDX,m_fPT[0],0,100000);
	
	if (m_pFJSS->FPT())
	{
		DDX_Text(pDX,IDC_ED_PT2,m_fPT[1]);
		DDX_Text(pDX,IDC_ED_PT3,m_fPT[2]);
		DDX_Text(pDX,IDC_ED_PT4,m_fPT[3]);

		DDV_MinMaxInt(pDX,m_fPT[1],0,100000);
		DDV_MinMaxInt(pDX,m_fPT[2],0,100000);
		DDV_MinMaxInt(pDX,m_fPT[3],0,100000);
	}
	// Control
	DDX_Control(pDX,IDC_LB_PROCESS,m_lbProcess);
	DDX_Control(pDX,IDC_ST_BITMAP,m_stBitmap);
	DDX_Control(pDX,IDC_CB_MACHINE,m_cbMachine);

}


BEGIN_MESSAGE_MAP(CProcessDlg, CDialog)
	ON_BN_CLICKED(IDC_BT_UP, OnBnClickedBtUp)
	ON_BN_CLICKED(IDC_BT_CHANGEPROCESS, OnBnClickedBtChangeprocess)
	ON_BN_CLICKED(IDC_BT_LEFT, OnBnClickedBtLeft)
	ON_BN_CLICKED(IDC_BT_RIGHT, OnBnClickedBtRight)
	ON_BN_CLICKED(IDC_BT_DOWN, OnBnClickedBtDown)
	ON_LBN_SELCHANGE(IDC_LB_PROCESS, OnLbnSelchangeLbProcess)
	ON_BN_CLICKED(IDC_BT_ADD, OnBnClickedBtAdd)
	ON_BN_CLICKED(IDC_BT_REMOVE, OnBnClickedBtRemove)
END_MESSAGE_MAP()


// CProcessDlg message handlers

BOOL CProcessDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// Bitmap Init
	m_stBitmap.SetBitmap(GetMaskedBitmap(m_bmBitmap,IDB_PROCESSDLG,RGB(255,255,255)));
	// Bitmap button init
	m_btUp.SetIcon(AfxGetApp()->LoadIcon(IDI_UP));
	m_btDown.SetIcon(AfxGetApp()->LoadIcon(IDI_DOWN));
	m_btLeft.SetIcon(AfxGetApp()->LoadIcon(IDI_LEFT));
	m_btRight.SetIcon(AfxGetApp()->LoadIcon(IDI_RIGHT));
	// Machine combobox fill
	FillMachineComboBox();

	SetGlobalJobBehaviour();
	SetPresentJobBehaviour();

	SetGlobalProcessBehaviour();
	SetPresentProcessBehaviour();

	UpdateListBox();
	LoadFJSSData();
	m_lbProcess.SetCurSel(m_nProcessIndex);
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


//-----------------------------------------------
// Dialog behavour
//-----------------------------------------------
void CProcessDlg::SetGlobalProcessBehaviour()
{
	if (m_pFJSS->FPT())
	{
	}
	else
	{
		BOOL bEnabled = FALSE;
		GetDlgItem(IDC_ED_PT2)->EnableWindow(bEnabled);
		GetDlgItem(IDC_ED_PT3)->EnableWindow(bEnabled);
		GetDlgItem(IDC_ED_PT4)->EnableWindow(bEnabled);
		GetDlgItem(IDC_ST_BITMAP)->EnableWindow(bEnabled);
	}
}

void CProcessDlg::SetPresentProcessBehaviour()
{
	BOOL bEnabled;

	if (m_nJobIndex > -1) 
	{
		if (!m_pFJSS->m_areaJobNode[m_nJobIndex].GetSize()) m_nProcessIndex = -1;
	}
	else m_nProcessIndex = -1;

	if (m_nProcessIndex == -1)				// NO PROCESS
	{
		bEnabled = FALSE;
		GetDlgItem(IDC_LB_PROCESS)->EnableWindow(bEnabled);
		GetDlgItem(IDC_BT_UP)->EnableWindow(bEnabled);
		GetDlgItem(IDC_BT_DOWN)->EnableWindow(bEnabled);
		GetDlgItem(IDC_BT_REMOVE)->EnableWindow(bEnabled);
		GetDlgItem(IDC_BT_CHANGEPROCESS)->EnableWindow(bEnabled);

		GetDlgItem(IDC_ST_PROCESSINDEX)->EnableWindow(bEnabled);
		GetDlgItem(IDC_ED_PROCESSNAME)->EnableWindow(bEnabled);
		GetDlgItem(IDC_CB_MACHINE)->EnableWindow(bEnabled);
		GetDlgItem(IDC_ED_PT1)->EnableWindow(bEnabled);
		
		if (m_pFJSS->FPT())
		{
			GetDlgItem(IDC_ED_PT2)->EnableWindow(bEnabled);
			GetDlgItem(IDC_ED_PT3)->EnableWindow(bEnabled);
			GetDlgItem(IDC_ED_PT4)->EnableWindow(bEnabled);
		}

	}
	else
	{
		bEnabled = TRUE;
		GetDlgItem(IDC_LB_PROCESS)->EnableWindow(bEnabled);
		GetDlgItem(IDC_BT_UP)->EnableWindow(bEnabled);
		GetDlgItem(IDC_BT_DOWN)->EnableWindow(bEnabled);
		GetDlgItem(IDC_BT_REMOVE)->EnableWindow(bEnabled);
		GetDlgItem(IDC_BT_CHANGEPROCESS)->EnableWindow(bEnabled);

		GetDlgItem(IDC_ST_PROCESSINDEX)->EnableWindow(bEnabled);
		GetDlgItem(IDC_ED_PROCESSNAME)->EnableWindow(bEnabled);
		GetDlgItem(IDC_CB_MACHINE)->EnableWindow(bEnabled);
		GetDlgItem(IDC_ED_PT1)->EnableWindow(bEnabled);
		if (m_pFJSS->FPT())
		{
			GetDlgItem(IDC_ED_PT2)->EnableWindow(bEnabled);
			GetDlgItem(IDC_ED_PT3)->EnableWindow(bEnabled);
			GetDlgItem(IDC_ED_PT4)->EnableWindow(bEnabled);
		}
		
		if (m_nProcessIndex == 0)							// UP
			GetDlgItem(IDC_BT_UP)->EnableWindow(FALSE);
		else 
			GetDlgItem(IDC_BT_UP)->EnableWindow(TRUE);		

															// DOWN
		if (m_nProcessIndex == m_pFJSS->m_areaJobNode[m_nJobIndex].GetSize()-1)
			GetDlgItem(IDC_BT_DOWN)->EnableWindow(FALSE);
		else
			GetDlgItem(IDC_BT_DOWN)->EnableWindow(TRUE);

		m_lbProcess.SetCurSel(m_nProcessIndex);
	}
}

void CProcessDlg::SetGlobalJobBehaviour()
{
}

void CProcessDlg::SetPresentJobBehaviour()
{
	BOOL bEnabled = FALSE;

	if (!m_pFJSS->m_nJob) m_nJobIndex = -1;					// NO JOB
	if (m_nJobIndex == -1)
	{
		bEnabled = FALSE;
		GetDlgItem(IDC_ST_JOBINDEX)->EnableWindow(bEnabled);
		GetDlgItem(IDC_ST_JOBNAME)->EnableWindow(bEnabled);
	}
	else
	{
		bEnabled = TRUE;
		GetDlgItem(IDC_ST_JOBINDEX)->EnableWindow(bEnabled);
		GetDlgItem(IDC_ST_JOBNAME)->EnableWindow(bEnabled);
	}

	if (m_nJobIndex < 1)									// LEFT
	{
		bEnabled = FALSE;
		GetDlgItem(IDC_BT_LEFT)->EnableWindow(bEnabled);
	}
	else
	{
		bEnabled = TRUE;
		GetDlgItem(IDC_BT_LEFT)->EnableWindow(bEnabled);
	}

	if (m_nJobIndex == m_pFJSS->m_nJob-1)					// RIGHT
	{
		bEnabled = FALSE;
		GetDlgItem(IDC_BT_RIGHT)->EnableWindow(bEnabled);
	}
	else
	{
		bEnabled = TRUE;
		GetDlgItem(IDC_BT_RIGHT)->EnableWindow(bEnabled);
	}
		

}

int CProcessDlg::UpdateListBox()
{
	// vymazat
	
	for (int i=m_lbProcess.GetCount()-1; i>-1; i--) m_lbProcess.DeleteString(i);
	// naplnit
	if (m_nJobIndex > -1)
		for (int i=0; i<m_pFJSS->m_areaJobNode[m_nJobIndex].GetSize();i++)
			m_lbProcess.AddString(m_pFJSS->m_areaJobNode[m_nJobIndex][i].m_strName);

	m_nProcessIndex = min(m_nProcessIndex,m_lbProcess.GetCount()-1);

	return m_lbProcess.GetCount();

}

//-----------------------------------------------
// Load and save FJSS data
//-----------------------------------------------

void CProcessDlg::LoadFJSSData()
{
	// Job
	if (m_nJobIndex > -1)
		m_strJobName = m_pFJSS->m_lstJob[m_nJobIndex].m_strName;
	else 
		m_strJobName = _T("");

	// Process
	if (m_nProcessIndex > -1)
	{
		m_strProcessName = m_pFJSS->m_areaJobNode[m_nJobIndex][m_nProcessIndex].m_strName;
		m_cbMachine.SetCurSel(m_pFJSS->m_areaJobNode[m_nJobIndex][m_nProcessIndex].m_nMachine);
		m_fPT[0] = m_pFJSS->m_areaJobNode[m_nJobIndex][m_nProcessIndex].m_fPT[0];

		if (m_pFJSS->FPT())
		{
			m_fPT[1] = m_pFJSS->m_areaJobNode[m_nJobIndex][m_nProcessIndex].m_fPT[1];
			m_fPT[2] = m_pFJSS->m_areaJobNode[m_nJobIndex][m_nProcessIndex].m_fPT[2];
			m_fPT[3] = m_pFJSS->m_areaJobNode[m_nJobIndex][m_nProcessIndex].m_fPT[3];
		}
	}
	else
	{
		m_strProcessName = _T("");
		m_fPT[0]=0;
	}
}

void CProcessDlg::SaveFJSSData()
{
	// Process only
	m_pFJSS->m_areaJobNode[m_nJobIndex][m_nProcessIndex].m_strName = m_strProcessName;
	m_pFJSS->m_areaJobNode[m_nJobIndex][m_nProcessIndex].m_nMachine = m_cbMachine.GetCurSel();
	m_pFJSS->m_areaJobNode[m_nJobIndex][m_nProcessIndex].m_fPT[0] = m_fPT[0];

	if (m_pFJSS->FPT())
	{
		m_pFJSS->m_areaJobNode[m_nJobIndex][m_nProcessIndex].m_fPT[1] = m_fPT[1];
		m_pFJSS->m_areaJobNode[m_nJobIndex][m_nProcessIndex].m_fPT[2] = m_fPT[2];
		m_pFJSS->m_areaJobNode[m_nJobIndex][m_nProcessIndex].m_fPT[3] = m_fPT[3];
	}
}


//-----------------------------------------------
// Button events
//-----------------------------------------------

void CProcessDlg::OnBnClickedBtChangeprocess()
{
	UpdateData(TRUE);
	SaveFJSSData();
	UpdateListBox();
	SetPresentProcessBehaviour();
}

void CProcessDlg::OnBnClickedBtLeft()
{
	m_nJobIndex--;
	if (m_nProcessIndex == -1) m_nProcessIndex = 0;
	m_nProcessIndex = min(m_nProcessIndex,m_pFJSS->m_areaJobNode[m_nJobIndex].GetSize()-1);
	LoadFJSSData();
	UpdateListBox();
	UpdateData(FALSE);
	SetPresentJobBehaviour();
	SetPresentProcessBehaviour();

}

void CProcessDlg::OnBnClickedBtRight()
{
	m_nJobIndex++;
	if (m_nProcessIndex == -1) m_nProcessIndex = 0;
	m_nProcessIndex = min(m_nProcessIndex,m_pFJSS->m_areaJobNode[m_nJobIndex].GetSize()-1);
	LoadFJSSData();
	UpdateListBox();
	UpdateData(FALSE);
	SetPresentJobBehaviour();
	SetPresentProcessBehaviour();

}


void CProcessDlg::OnBnClickedBtUp()
{
	m_pFJSS->MoveProcessUp(m_nJobIndex,m_nProcessIndex);
	m_nProcessIndex--;
	UpdateListBox();
	SetPresentJobBehaviour();
	SetPresentProcessBehaviour();

	m_pFJSS->SetModified(TRUE);
}

void CProcessDlg::OnBnClickedBtDown()
{
	m_pFJSS->MoveProcessDown(m_nJobIndex,m_nProcessIndex);
	m_nProcessIndex++;
	UpdateListBox();
	SetPresentJobBehaviour();
	SetPresentProcessBehaviour();

	m_pFJSS->SetModified(TRUE);
}

void CProcessDlg::OnLbnSelchangeLbProcess()
{
	m_nProcessIndex = m_lbProcess.GetCurSel();
	SetPresentJobBehaviour();
	SetPresentProcessBehaviour();
	LoadFJSSData();
	UpdateData(FALSE);
}

void CProcessDlg::OnBnClickedBtAdd()
{
	CString str;
	CFN fn;
	fn = 0;
    str.Format(_T("Operace%d-%d"),m_nJobIndex,m_pFJSS->m_areaJobNode[m_nJobIndex].GetSize());

	m_pFJSS->AddProcess(m_nJobIndex,m_pFJSS->m_areaJobNode[m_nJobIndex].GetSize(),0,fn);
	m_nProcessIndex = UpdateListBox() - 1;
	SetPresentProcessBehaviour();
	LoadFJSSData();
	UpdateData(FALSE);

	m_pFJSS->SetModified(TRUE);
}

void CProcessDlg::OnBnClickedBtRemove()
{
	m_pFJSS->RemoveProcess(m_nJobIndex,m_nProcessIndex);
	int nLast = UpdateListBox();
	m_nProcessIndex = min(m_nProcessIndex,nLast);
	SetPresentProcessBehaviour();
	LoadFJSSData();
	UpdateData(FALSE);

	m_pFJSS->SetModified(TRUE);
}

void CProcessDlg::FillMachineComboBox()
{
	CString str;
	for (int i=0; i<m_pFJSS->m_nMachine; i++)
	{
		str.Format(_T("%d - %s"),i,m_pFJSS->m_lstMachine[i].m_strName);
		m_cbMachine.AddString(str);
	}
}