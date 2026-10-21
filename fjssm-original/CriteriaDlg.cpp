// CriteriaDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FJSSM.h"
#include "CriteriaDlg.h"


// CCriteriaDlg dialog

IMPLEMENT_DYNAMIC(CCriteriaDlg, CDialog)
CCriteriaDlg::CCriteriaDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCriteriaDlg::IDD, pParent)
{	
}

CCriteriaDlg::~CCriteriaDlg()
{
}

void CCriteriaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	m_nCriterion = m_pFJSS->m_nCriterion;
	m_nCritMaxOrSum = m_pFJSS->m_nCritMaxOrSum;
	m_fw1m = m_pFJSS->m_fw1m;
	m_fw2s = m_pFJSS->m_fw2s;
	m_fw1 = m_pFJSS->m_fw1;
	m_fw2 = m_pFJSS->m_fw2;
	m_fw3 = m_pFJSS->m_fw3;
	m_fw4 = m_pFJSS->m_fw4;

	DDX_Text(pDX,IDC_ED_W1_M,m_fw1m);
	DDX_Text(pDX,IDC_ED_W2_S,m_fw2s);
	DDX_Text(pDX,IDC_ED_W1,m_fw1);
	DDX_Text(pDX,IDC_ED_W2,m_fw2);
	DDX_Text(pDX,IDC_ED_W3,m_fw3);
	DDX_Text(pDX,IDC_ED_W4,m_fw4);

	DDX_Radio(pDX,IDC_RD_CRIT0,m_nCriterion);
	DDX_Radio(pDX,IDC_RD_FMAX,m_nCritMaxOrSum);

	DDV_MinMaxDouble(pDX,m_fw1m,0,100);
	DDV_MinMaxDouble(pDX,m_fw2s,0,100);
	DDV_MinMaxDouble(pDX,m_fw1,0,100);
	DDV_MinMaxDouble(pDX,m_fw2,0,100);
	DDV_MinMaxDouble(pDX,m_fw3,0,100);
	DDV_MinMaxDouble(pDX,m_fw4,0,100);
}


BEGIN_MESSAGE_MAP(CCriteriaDlg, CDialog)

	ON_BN_CLICKED(IDC_RD_CRIT1, OnBnClickedRdCrit)
	ON_BN_CLICKED(IDC_RD_CRIT2, OnBnClickedRdCrit)
	ON_BN_CLICKED(IDC_RD_CRIT3, OnBnClickedRdCrit)
	ON_BN_CLICKED(IDC_RD_CRIT4, OnBnClickedRdCrit)
	ON_BN_CLICKED(IDC_RD_CRIT5, OnBnClickedRdCrit)
	ON_BN_CLICKED(IDC_RD_CRIT6, OnBnClickedRdCrit)


END_MESSAGE_MAP()


// CCriteriaDlg message handlers


void CCriteriaDlg::SetGlobalBehaviour()
{
	for (int i=0; i<CRIT_COUNT; i++)
		SetDlgItemText(IDC_ST_CRIT0 + i,m_pFJSS->GetStrCriterion(i,1));
}


BOOL CCriteriaDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	for (int i=0; i<CRIT_COUNT;i++)
	{
		CWnd* pWnd = GetDlgItem(IDC_RD_CRIT0 + i);
		pWnd->EnableWindow(m_pFJSS->m_bCrit[i]);		
	}

	SetGlobalBehaviour();

	OnBnClickedRdCrit();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CCriteriaDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	UpdateData();
	m_pFJSS->m_nCriterion = m_nCriterion;
	m_pFJSS->m_nCritMaxOrSum = m_nCritMaxOrSum;

	m_pFJSS->m_fw1m =m_fw1m;
	m_pFJSS->m_fw2s = m_fw2s;
	m_pFJSS->m_fw1 = m_fw1;
	m_pFJSS->m_fw2 = m_fw2;
	m_pFJSS->m_fw3 = m_fw3;
	m_pFJSS->m_fw4 = m_fw4;

	m_pFJSS->SetModified(TRUE);

	CDialog::OnOK();
}


void CCriteriaDlg::OnBnClickedRdCrit()
{
	UpdateData();
	CWnd* pWnd;

	for (int i=0; i<6; i++)
	{
	    pWnd = GetDlgItem(IDC_ED_W1_M + i);		
		pWnd->EnableWindow(FALSE);
	}


	if ((m_nCriterion == 2) || (m_nCriterion == 3)) 
	{
		for (int i=0; i<2; i++)
		{
		    pWnd = GetDlgItem(IDC_ED_W1_M + i);		
			pWnd->EnableWindow(TRUE);
		}
	}

	if (m_nCriterion == 4)
	{
		for (int i=0; i<4; i++)
		{
		    pWnd = GetDlgItem(IDC_ED_W1 + i);		
			pWnd->EnableWindow(TRUE);
		}
	}
	
}
