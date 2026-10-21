// MachineDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FJSSM.h"
#include "MachineDlg.h"


// CMachineDlg dialog

IMPLEMENT_DYNAMIC(CMachineDlg, CDialog)
CMachineDlg::CMachineDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMachineDlg::IDD, pParent)
{
	m_nIndex = 0;
	m_nColor = 0;
}

CMachineDlg::~CMachineDlg()
{
}

void CMachineDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX,IDC_ST_INDEX,m_nIndex);
	DDX_Text(pDX,IDC_ED_NAME,m_strName);
	
	DDX_Control(pDX,IDC_LB_MACHINE,m_lbMachine);
	DDX_Control(pDX,IDC_ST_BITMAP,m_stBitmap);

	DDV_MaxChars(pDX,m_strName,30);
}


BEGIN_MESSAGE_MAP(CMachineDlg, CDialog)
	ON_LBN_SELCHANGE(IDC_LB_MACHINE, OnLbnSelchangeLbMachinelist)
	ON_BN_CLICKED(IDC_BT_CHANGE, OnBnClickedBtChange)
	ON_BN_CLICKED(IDC_BT_ADD, OnBnClickedBtAdd)
	ON_BN_CLICKED(IDC_BT_REMOVE, OnBnClickedBtRemove)
END_MESSAGE_MAP()


// CMachineDlg message handlers

int CMachineDlg::UpdateListBox()
{

	for (int i=m_lbMachine.GetCount()-1; i>-1; i--)
		m_lbMachine.DeleteString(i);

	for (int i=0; i<m_pFJSS->m_lstMachine.GetSize();i++)
		m_lbMachine.AddString(m_pFJSS->m_lstMachine[i].m_strName);

	if ( m_nIndex>m_lbMachine.GetCount()-1 ) m_nIndex = m_lbMachine.GetCount()-1;

	return m_lbMachine.GetCount();
}


BOOL CMachineDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Bitmap inint
	m_stBitmap.SetBitmap(GetMaskedBitmap(m_bmBitmap,IDB_MACHINEDLG,RGB(255,255,255)));
	// Bitmap Button init
	m_crbtMachine.SubclassDlgItem(IDC_CRBT_MACHINE,this);
	// Vyplneni control 
	SetGlobalBehaviour();
	SetPresentBehaviour();
	UpdateListBox();
	LoadFJSSData();
	m_lbMachine.SetCurSel(m_nIndex);
	UpdateDataEx(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CMachineDlg::OnLbnSelchangeLbMachinelist()
{
	m_nIndex = m_lbMachine.GetCurSel();
	LoadFJSSData();
	UpdateDataEx(FALSE);
	
}

void CMachineDlg::OnBnClickedBtChange()
{
	UpdateDataEx(TRUE);
	SaveFJSSData();
	UpdateListBox();
	SetPresentBehaviour();

	m_pFJSS->SetModified(TRUE);
}

void CMachineDlg::OnBnClickedBtAdd()
{
	CString str;
	str.Format(_T("Stroj%d"),m_pFJSS->m_lstMachine.GetSize()+1);
	m_pFJSS->AddMachine(str);

	m_nIndex = UpdateListBox() - 1;
	SetPresentBehaviour();
	LoadFJSSData();
	UpdateDataEx(FALSE);

	m_pFJSS->SetModified(TRUE);
}


void CMachineDlg::OnBnClickedBtRemove()
{
	int nAnswer;
	nAnswer = AfxMessageBox(_T("Opravdu chcete odstranit stroj vèetnì")
							_T("všech jemu pøiøazených operací?"),MB_YESNO,MB_ICONQUESTION);

	if (nAnswer == IDYES)
	{
		m_pFJSS->RemoveMachine(m_nIndex);
		int nLast = UpdateListBox();
		m_nIndex = min(m_nIndex,nLast);
		SetPresentBehaviour();
		LoadFJSSData();
		UpdateDataEx(FALSE);

		m_pFJSS->SetModified(TRUE);
	}
	// TODO: Add your control notification handler code here
}

void CMachineDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	CDialog::OnOK();
}

void CMachineDlg::LoadFJSSData()
{

	if ((m_nIndex > -1 ) && (m_nIndex < m_pFJSS->m_nMachine))
	{
		m_strName = m_pFJSS->m_lstMachine[m_nIndex].m_strName;
		m_nColor = m_pFJSS->m_lstMachine[m_nIndex].m_nColor;			
	}
	else		// Zadny stroj
	{
		m_strName = _T("");
		m_nIndex = 0;
		m_nColor = 0;
	}
}

void CMachineDlg::SaveFJSSData()
{
	m_pFJSS->m_lstMachine[m_nIndex].m_strName = m_strName;
	m_pFJSS->m_lstMachine[m_nIndex].m_nColor = m_nColor;
}
	

void CMachineDlg::SetGlobalBehaviour()
{
}

void CMachineDlg::SetPresentBehaviour()
{
	BOOL bEnabled = FALSE;
	if (m_pFJSS->m_nMachine == 0)	// Zadne stroje
	{
		GetDlgItem(IDC_ST_INDEX)->EnableWindow(bEnabled);
		GetDlgItem(IDC_ED_NAME)->EnableWindow(bEnabled);
		GetDlgItem(IDC_CRBT_MACHINE)->EnableWindow(bEnabled);
		GetDlgItem(IDC_BT_CHANGE)->EnableWindow(bEnabled);
		GetDlgItem(IDC_BT_REMOVE)->EnableWindow(bEnabled);
		GetDlgItem(IDC_LB_MACHINE)->EnableWindow(bEnabled);
	}
	else							// existuji stroje
	{
		bEnabled = !bEnabled;
		GetDlgItem(IDC_ST_INDEX)->EnableWindow(bEnabled);
		GetDlgItem(IDC_ED_NAME)->EnableWindow(bEnabled);
		GetDlgItem(IDC_CRBT_MACHINE)->EnableWindow(bEnabled);
		GetDlgItem(IDC_BT_CHANGE)->EnableWindow(bEnabled);
		GetDlgItem(IDC_BT_REMOVE)->EnableWindow(bEnabled);
		GetDlgItem(IDC_LB_MACHINE)->EnableWindow(bEnabled);
		m_lbMachine.SetCurSel(m_nIndex);

	}

}

void CMachineDlg::UpdateDataEx(BOOL bSaveAndValidate)
{
	// Update colobtn
	if (bSaveAndValidate)
	{
		m_nColor = m_crbtMachine.dlg.colorindex;
	}
	else
	{
		m_crbtMachine.dlg.colorindex = m_nColor;
		m_crbtMachine.currentcolor = CColorBtnDlg::colors[m_nColor];
		m_crbtMachine.Invalidate();
	}

	UpdateData(bSaveAndValidate);
}