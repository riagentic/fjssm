// JobDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FJSSM.h"
#include "JobDlg.h"


// CJobDlg dialog

IMPLEMENT_DYNAMIC(CJobDlg, CDialog)
CJobDlg::CJobDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CJobDlg::IDD, pParent)
{
	m_nColor = 0;
	m_nIndex = 0;
	m_fAlfa = 0;
	m_fBeta = 0;
	m_fLambda1 = 0;
	m_fLambda2 = 0;
	m_fLambda3 = 0;
}

CJobDlg::~CJobDlg()
{
}

void CJobDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	
	DDX_Text(pDX,IDC_ST_INDEX,m_nIndex);
	DDX_Text(pDX,IDC_ED_NAME, m_strName);
	DDX_Text(pDX,IDC_ED_DD1,m_fDD[0]);
	DDX_Control(pDX,IDC_LB_JOB,m_lbJob);
	DDX_Control(pDX,IDC_ST_BITMAP,m_stBitmap);

	DDV_MaxChars(pDX,m_strName,30);
	DDV_MinMaxInt(pDX,m_fDD[0],0,100000);

	// C/F
	if (m_pFJSS->FDD())
	{
		DDX_Text(pDX,IDC_ED_DD2,m_fDD[1]);
		DDX_Text(pDX,IDC_ED_DD3,m_fDD[2]);
		DDX_Text(pDX,IDC_ED_DD4,m_fDD[3]);
		DDX_Text(pDX,IDC_ED_ALFA,m_fAlfa);
		DDX_Text(pDX,IDC_ED_BETA,m_fBeta);
		
		DDX_Text(pDX,IDC_ED_LAMBDA1,m_fLambda1);
		DDX_Text(pDX,IDC_ED_LAMBDA2,m_fLambda2);
		DDX_Text(pDX,IDC_ED_LAMBDA3,m_fLambda3);
				
		DDV_MinMaxInt(pDX,m_fDD[1],0,100000);
		DDV_MinMaxInt(pDX,m_fDD[2],0,100000);
		DDV_MinMaxInt(pDX,m_fDD[3],0,100000);	
		
		DDV_MinMaxDouble(pDX,m_fAlfa,0,100);
		DDV_MinMaxDouble(pDX,m_fBeta,0,100);
		DDV_MinMaxDouble(pDX,m_fLambda1,0,100);
		DDV_MinMaxDouble(pDX,m_fLambda2,0,100);
		DDV_MinMaxDouble(pDX,m_fLambda3,0,100);
	}
}


BEGIN_MESSAGE_MAP(CJobDlg, CDialog)
	ON_LBN_SELCHANGE(IDC_LB_JOB, OnLbnSelchangeLbJobs)
	ON_BN_CLICKED(IDC_BT_CHANGE, OnBnClickedBtChange)
	ON_BN_CLICKED(IDC_BT_ADD, OnBnClickedBtAdd)
	ON_BN_CLICKED(IDC_BT_REMOVE, OnBnClickedBtRemove)
END_MESSAGE_MAP()


// CJobDlg message handlers

int CJobDlg::UpdateListBox()
{
	// vymazat
	for (int i=m_lbJob.GetCount()-1; i>-1; i--) m_lbJob.DeleteString(i);
	// naplnit
	for (int i=0; i<m_pFJSS->m_nJob;i++)
		m_lbJob.AddString(m_pFJSS->m_lstJob[i].m_strName);
	//index
	if ( m_nIndex>m_lbJob.GetCount()-1 ) m_nIndex = m_lbJob.GetCount()-1;
	
	return m_lbJob.GetCount();
}

// upravuje momentalni chovani nekterych veci podle soucasneho stavu dlg
void CJobDlg::SetPresentBehaviour()
{
	BOOL bEnabled = FALSE;
	if (m_pFJSS->m_nJob == 0)	// Zadne joby
	{
		GetDlgItem(IDC_BT_REMOVE)->EnableWindow(bEnabled);
		GetDlgItem(IDC_ST_INDEX)->EnableWindow(bEnabled);
		GetDlgItem(IDC_ED_NAME)->EnableWindow(bEnabled);
		GetDlgItem(IDC_CRBT_JOB)->EnableWindow(bEnabled);
		GetDlgItem(IDC_ST_BITMAP)->EnableWindow(bEnabled);
		GetDlgItem(IDC_BT_CHANGE)->EnableWindow(bEnabled);
		GetDlgItem(IDC_LB_JOB)->EnableWindow(bEnabled);
		GetDlgItem(IDC_ED_DD1)->EnableWindow(bEnabled);		
		if (m_pFJSS->FDD())
		{
			GetDlgItem(IDC_ED_DD2)->EnableWindow(bEnabled);
			GetDlgItem(IDC_ED_DD3)->EnableWindow(bEnabled);
			GetDlgItem(IDC_ED_DD4)->EnableWindow(bEnabled);
			GetDlgItem(IDC_ED_ALFA)->EnableWindow(bEnabled);
			GetDlgItem(IDC_ED_BETA)->EnableWindow(bEnabled);
			GetDlgItem(IDC_ED_LAMBDA1)->EnableWindow(bEnabled);
			GetDlgItem(IDC_ED_LAMBDA2)->EnableWindow(bEnabled);
			GetDlgItem(IDC_ED_LAMBDA3)->EnableWindow(bEnabled);
		}

	}
	else							// existuji joby
	{
		bEnabled = !bEnabled;
		GetDlgItem(IDC_BT_REMOVE)->EnableWindow(bEnabled);
		GetDlgItem(IDC_ST_INDEX)->EnableWindow(bEnabled);
		GetDlgItem(IDC_ED_NAME)->EnableWindow(bEnabled);
		GetDlgItem(IDC_CRBT_JOB)->EnableWindow(bEnabled);
		GetDlgItem(IDC_ST_BITMAP)->EnableWindow(bEnabled);
		GetDlgItem(IDC_BT_CHANGE)->EnableWindow(bEnabled);
		GetDlgItem(IDC_LB_JOB)->EnableWindow(bEnabled);
		GetDlgItem(IDC_ED_DD1)->EnableWindow(bEnabled);		
		if (m_pFJSS->FDD())
		{
			GetDlgItem(IDC_ED_DD2)->EnableWindow(bEnabled);
			GetDlgItem(IDC_ED_DD3)->EnableWindow(bEnabled);
			GetDlgItem(IDC_ED_DD4)->EnableWindow(bEnabled);
			GetDlgItem(IDC_ED_ALFA)->EnableWindow(bEnabled);
			GetDlgItem(IDC_ED_BETA)->EnableWindow(bEnabled);
			GetDlgItem(IDC_ED_LAMBDA1)->EnableWindow(bEnabled);
			GetDlgItem(IDC_ED_LAMBDA2)->EnableWindow(bEnabled);
			GetDlgItem(IDC_ED_LAMBDA3)->EnableWindow(bEnabled);
		}
		m_lbJob.SetCurSel(m_nIndex);
	}
}

void CJobDlg::SetGlobalBehaviour()
{
	if (!m_pFJSS->FDD())
	{
		BOOL bEnabled = FALSE;
		GetDlgItem(IDC_ED_DD2)->EnableWindow(bEnabled);
		GetDlgItem(IDC_ED_DD3)->EnableWindow(bEnabled);
		GetDlgItem(IDC_ED_DD4)->EnableWindow(bEnabled);
		GetDlgItem(IDC_ED_ALFA)->EnableWindow(bEnabled);
		GetDlgItem(IDC_ED_BETA)->EnableWindow(bEnabled);
		GetDlgItem(IDC_ED_LAMBDA1)->EnableWindow(bEnabled);
		GetDlgItem(IDC_ED_LAMBDA2)->EnableWindow(bEnabled);
		GetDlgItem(IDC_ED_LAMBDA3)->EnableWindow(bEnabled);
		GetDlgItem(IDC_ST_BITMAP)->EnableWindow(bEnabled);
	}

}


BOOL CJobDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// Color Button Init
	m_crbtJob.SubclassDlgItem(IDC_CRBT_JOB,this);
	// Bitmap Init
	m_stBitmap.SetBitmap(GetMaskedBitmap(m_bmBitmap,IDB_JOBDLG,RGB(255,255,255)));

	SetGlobalBehaviour();
	SetPresentBehaviour();
	UpdateListBox();
	LoadFJSSData();
	m_lbJob.SetCurSel(m_nIndex);
	UpdateDataEx(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CJobDlg::OnLbnSelchangeLbJobs()
{
	m_nIndex = m_lbJob.GetCurSel();
	LoadFJSSData();
	UpdateDataEx(FALSE);
}

void CJobDlg::OnBnClickedBtChange()
{
	UpdateDataEx(TRUE);
	SaveFJSSData();
	UpdateListBox();
	SetPresentBehaviour();

	m_pFJSS->SetModified(TRUE);
}

void CJobDlg::LoadFJSSData()
{
	if ((m_nIndex > -1 ) && (m_nIndex < m_pFJSS->m_nJob))
	{
		m_strName = m_pFJSS->m_lstJob[m_nIndex].m_strName;
		m_nColor = m_pFJSS->m_lstJob[m_nIndex].m_nColor;
		m_fDD[0] = m_pFJSS->m_lstJob[m_nIndex].m_fDD[0];
	
		if (m_pFJSS->FDD())
		{
			m_fDD[1] = m_pFJSS->m_lstJob[m_nIndex].m_fDD[1];
			m_fDD[2] = m_pFJSS->m_lstJob[m_nIndex].m_fDD[2];
			m_fDD[3] = m_pFJSS->m_lstJob[m_nIndex].m_fDD[3];
			m_fAlfa =  m_pFJSS->m_lstJob[m_nIndex].m_fAlfa;
			m_fBeta =  m_pFJSS->m_lstJob[m_nIndex].m_fBeta;
			m_fLambda1 = m_pFJSS->m_lstJob[m_nIndex].m_fLambda1;
			m_fLambda2 = m_pFJSS->m_lstJob[m_nIndex].m_fLambda2;
			m_fLambda3 = m_pFJSS->m_lstJob[m_nIndex].m_fLambda3;
		}
	}
	else
	{
		m_strName = _T("");
		m_nIndex = 0;
		m_nColor = 0;
		m_fDD = 0;
		m_fAlfa = 0;
		m_fBeta = 0;
		m_fLambda1 = 0;
		m_fLambda2 = 0;
		m_fLambda3 = 0;
	}
}

void CJobDlg::SaveFJSSData()
{
	CString str;

	m_pFJSS->m_lstJob[m_nIndex].m_strName = m_strName;
	m_pFJSS->m_lstJob[m_nIndex].m_nColor = m_nColor;
	m_pFJSS->m_lstJob[m_nIndex].m_fDD[0] = m_fDD[0];
	
	if (m_pFJSS->FDD())
	{
		m_pFJSS->m_lstJob[m_nIndex].m_fDD[1] = m_fDD[1];
		m_pFJSS->m_lstJob[m_nIndex].m_fDD[2] = m_fDD[2];
		m_pFJSS->m_lstJob[m_nIndex].m_fDD[3] = m_fDD[3];
		m_pFJSS->m_lstJob[m_nIndex].m_fAlfa= m_fAlfa;
		m_pFJSS->m_lstJob[m_nIndex].m_fBeta = m_fBeta;
		m_pFJSS->m_lstJob[m_nIndex].m_fLambda1 = m_fLambda1;
		m_pFJSS->m_lstJob[m_nIndex].m_fLambda2 = m_fLambda2;
		m_pFJSS->m_lstJob[m_nIndex].m_fLambda3 = m_fLambda3;
	}
}

void CJobDlg::UpdateDataEx(BOOL bSaveAndValidate)
{
	// Update colobtn
	if (bSaveAndValidate)
	{
		m_nColor = m_crbtJob.dlg.colorindex;
	}
	else
	{
		m_crbtJob.dlg.colorindex = m_nColor;
		m_crbtJob.currentcolor = CColorBtnDlg::colors[m_nColor];
		m_crbtJob.Invalidate();
	}
	
	UpdateData(bSaveAndValidate);
}
void CJobDlg::OnBnClickedBtAdd()
{
	CString str;
	CFN fn;
	fn = 0;
	str.Format(_T("Job%d"),m_pFJSS->m_lstJob.GetSize()+1);

	m_pFJSS->AddJob(str,fn,0,0,0,0,0);

	m_nIndex = UpdateListBox() - 1;
	SetPresentBehaviour();
	LoadFJSSData();
	UpdateDataEx(FALSE);

	m_pFJSS->SetModified(TRUE);

}

void CJobDlg::OnBnClickedBtRemove()
{
	int nAnswer;
	nAnswer = AfxMessageBox(_T("Opravdu chcete odstranit job vèetnì")
							_T("všech jeho operací?"),MB_YESNO,MB_ICONQUESTION);

	if (nAnswer == IDYES)
	{
		m_pFJSS->RemoveJob(m_nIndex);
		int nLast = UpdateListBox();
		m_nIndex = min(m_nIndex,nLast);
		SetPresentBehaviour();
		LoadFJSSData();
		UpdateDataEx(FALSE);

		m_pFJSS->SetModified(TRUE);
	}

}
