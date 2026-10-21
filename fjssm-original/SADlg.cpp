// SAdlg.cpp : implementation file
//

#include "stdafx.h"
#include "SAdlg.h"
#include "FJSSM.h"
#include "CriteriaDlg.h"



// CSADlg dialog

IMPLEMENT_DYNAMIC(CSADlg, CDialog)
CSADlg::CSADlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSADlg::IDD, pParent)
		
{
	m_bIsSolving = FALSE;

	m_nDlgType = 0;

	m_fActCycleCount = 0;
	m_fActCycle = 0;
	m_fXBest = 0;
}

CSADlg::~CSADlg()
{
}

void CSADlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

// Input 
	DDX_Text(pDX,IDC_SA_TIME,m_pFJSS->m_nSATime);
	DDX_Text(pDX,IDC_SA_MINTEMP,m_pFJSS->m_nSAMinTemp);
	DDX_Text(pDX,IDC_SA_TEMP,m_pFJSS->m_fSATemp);
	DDX_Text(pDX,IDC_SA_CYCLE,m_pFJSS->m_nSACycle);
	DDX_Text(pDX,IDC_SA_TEMPCOEF,m_pFJSS->m_fSATempCoef);
	DDX_Text(pDX,IDC_SA_CYCLECOEF,m_pFJSS->m_nSACycleCoef);
	DDX_Text(pDX,IDC_ST_ACTCRIT,m_pFJSS->GetStrCriterion(m_pFJSS->m_nCriterion));

// Runtime
	DDX_Text(pDX,IDC_ST_ACTTEMP,m_strActTemp);
	DDX_Text(pDX,IDC_ST_BESTVOOF,m_strfXBest);	
	DDX_Text(pDX,IDC_ST_ACTCYCLE,m_strCycleProgress);
	DDX_Text(pDX,IDC_ST_ACTTIME,m_strTimeProgress);

	DDX_Control(pDX,IDC_SA_PROG_CYCLE,m_prctrlCycle);
	DDX_Control(pDX,IDC_SA_PROG_TIME,m_prctrlTime);

// DDV
	DDV_MinMaxLong(pDX,m_pFJSS->m_nSATime,1,10000);
	DDV_MinMaxInt(pDX,m_pFJSS->m_nSAMinTemp,0,10000);
	DDV_MinMaxDouble(pDX,m_pFJSS->m_fSATemp,1,10000);
	DDV_MinMaxInt(pDX,m_pFJSS->m_nSACycle,1,10000);
	DDV_MinMaxDouble(pDX,m_pFJSS->m_fSATempCoef,0,0.999999);
	DDV_MinMaxInt(pDX,m_pFJSS->m_nSACycleCoef,0,100);
}


BEGIN_MESSAGE_MAP(CSADlg, CDialog)
	ON_BN_CLICKED(IDC_BT_SOLVESA, OnBnClickedSaSolve)
	ON_BN_CLICKED(IDC_BT_CRITERION, OnBnClickedSaCriterion)
END_MESSAGE_MAP()


// CSADlg message handlers

BOOL CSADlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetGlobalBehaviour();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}



void CSADlg::OnOK()
{
	UpdateData(TRUE);
	CDialog::OnOK();
}

void CSADlg::OnBnClickedSaSolve()
{	
	m_pFJSS->m_nAction = 0;

	UpdateData(TRUE);

	IsSolving(TRUE);

	m_prctrlTime.SetRange(0,m_pFJSS->m_nSATime);
	m_prctrlCycle.SetRange(0,m_pFJSS->m_nSACycle);
	m_pFJSS->m_pSADlg = this;
	m_pFJSS->SolveSA();

	IsSolving(FALSE);
}



void CSADlg::OnBnClickedSaCriterion()
{
	CCriteriaDlg dlg;
	dlg.m_pFJSS = m_pFJSS;
	if (dlg.DoModal()==IDOK);
	{
		UpdateData(FALSE);
	}
}

void CSADlg::SetGlobalBehaviour()
{
	switch(m_nDlgType)
	{
	case 0:							// standart dialog behaviour
		break;
	case 1:							// only set params
		BOOL bEnabled = FALSE;
		GetDlgItem(IDC_BT_SOLVESA)->EnableWindow(bEnabled);
		GetDlgItem(IDC_BT_CRITERION)->EnableWindow(bEnabled);
		GetDlgItem(IDC_SA_TIME)->EnableWindow(bEnabled);
		break;
	}

	UpdateData(FALSE);

}

void CSADlg::IsSolving(BOOL bIsSolving)
{
	
	m_bIsSolving = bIsSolving;
	BOOL bEnabled = !bIsSolving;
	GetDlgItem(IDC_SA_TIME)->EnableWindow(bEnabled);
	GetDlgItem(IDC_SA_MINTEMP)->EnableWindow(bEnabled);
	GetDlgItem(IDC_SA_TEMP)->EnableWindow(bEnabled);
	GetDlgItem(IDC_SA_CYCLE)->EnableWindow(bEnabled);
	GetDlgItem(IDC_SA_TEMPCOEF)->EnableWindow(bEnabled);
	GetDlgItem(IDC_SA_CYCLECOEF)->EnableWindow(bEnabled);
	GetDlgItem(IDOK)->EnableWindow(bEnabled);
	GetDlgItem(IDCANCEL)->EnableWindow(bEnabled);
	GetDlgItem(IDC_BT_SOLVESA)->EnableWindow(bEnabled);
	GetDlgItem(IDC_BT_CRITERION)->EnableWindow(bEnabled);
	
}


void CSADlg::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class
	if (!m_bIsSolving)
		CDialog::OnCancel();
}

