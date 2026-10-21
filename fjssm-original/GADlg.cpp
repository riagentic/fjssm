// FJSSGAdlg.cpp : implementation file
//

#include "stdafx.h"
#include "FJSSM.h"
#include "GAdlg.h"
#include "CriteriaDlg.h"


// CGADlg dialog

IMPLEMENT_DYNAMIC(CGADlg, CDialog)
CGADlg::CGADlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGADlg::IDD, pParent)
{
	m_bIsSolving = FALSE;
	m_nDlgType = 0;
}

CGADlg::~CGADlg()
{
}

void CGADlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

// Init
	DDX_Text(pDX,IDC_GA_POPULATION,m_pFJSS->m_nGAPopulation);
	DDX_Text(pDX,IDC_GA_TIME,m_pFJSS->m_nGATime);
	DDX_Text(pDX,IDC_GA_CYCLE,m_pFJSS->m_nGACycle);
	DDX_Text(pDX,IDC_GA_SELCOEF,m_pFJSS->m_nGASelCoef);	
	DDX_Text(pDX,IDC_GA_MINMSX,m_pFJSS->m_nGAMinMSX);	
	DDX_Text(pDX,IDC_GA_MSMCYCLE,m_pFJSS->m_nGA_MSMCycle);
	DDX_Text(pDX,IDC_GA_LSCYCLE,m_pFJSS->m_nGA_LSCycle);
	DDX_Text(pDX,IDC_ST_ACTCRIT,m_pFJSS->GetStrCriterion(m_pFJSS->m_nCriterion));

// Runtime
	
	DDX_Text(pDX,IDC_ST_BESTVOOF,m_strXBest);
	DDX_Text(pDX,IDC_ST_LASTVOOF,m_strX0);
	DDX_Text(pDX,IDC_ST_ACTTIME,m_strTimeProgress);
	DDX_Text(pDX,IDC_ST_ACTCYCLE,m_strCycleProgress);
	DDX_Control(pDX,IDC_PROG_CYCLE,m_prctrlCycle);
	DDX_Control(pDX,IDC_PROG_TIME,m_prctrlTime);



	DDV_MinMaxInt(pDX,m_pFJSS->m_nGAPopulation,2,1000);
	DDV_MinMaxInt(pDX,m_pFJSS->m_nGATime,1,10000);
	DDV_MinMaxInt(pDX,m_pFJSS->m_nGACycle,1,10000);
	DDV_MinMaxInt(pDX,m_pFJSS->m_nGASelCoef,1,100);
	DDV_MinMaxInt(pDX,m_pFJSS->m_nGAMinMSX,1,1000);
	DDV_MinMaxInt(pDX,m_pFJSS->m_nGA_MSMCycle,1,1000);
	DDV_MinMaxInt(pDX,m_pFJSS->m_nGA_LSCycle,1,10000);
}

BEGIN_MESSAGE_MAP(CGADlg, CDialog)
	ON_BN_CLICKED(IDC_BT_SOLVEGA, OnBnClickedBtSolve)
	ON_BN_CLICKED(IDC_BT_CRITERION, OnBnClickedBtCriterion)
END_MESSAGE_MAP()


// CGADlg message handlers

BOOL CGADlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	UpdateOperation(-1);

	SetGlobalBehaviour();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CGADlg::OnOK()
{
	UpdateData(TRUE);
	CDialog::OnOK();
}

void CGADlg::OnBnClickedBtSolve()
{
	m_pFJSS->m_nAction = 1;
	m_pFJSS->m_pGADlg = this;

	UpdateData(TRUE);

	IsSolving(TRUE);

	m_prctrlTime.SetRange(0,m_pFJSS->m_nGATime);
	m_prctrlCycle.SetRange(0,m_pFJSS->m_nGACycle);
	m_prctrlTime.SetPos(0);
	m_prctrlCycle.SetPos(0);

	m_pFJSS->SolveGA();

	IsSolving(FALSE);
}


int CGADlg::UpdateOperation(int nOperation)
{
	for (int i=0; i<6; i++)
		GetDlgItem(IDC_ST_ARROW1+i)->EnableWindow(FALSE);

	if (nOperation != -1) GetDlgItem(IDC_ST_ARROW1+nOperation)->EnableWindow(TRUE);

	return 0;
}


void CGADlg::OnBnClickedBtCriterion()
{
	CCriteriaDlg dlg;
	dlg.m_pFJSS = m_pFJSS;
	if (dlg.DoModal()==IDOK)
		UpdateData(FALSE);
}


void CGADlg::SetGlobalBehaviour()
{
	BOOL bEnabled = FALSE;

	switch (m_nDlgType)
	{
	case 0:
		break;
	case 1:
		GetDlgItem(IDC_BT_SOLVEGA)->EnableWindow(bEnabled);
		GetDlgItem(IDC_BT_CRITERION)->EnableWindow(bEnabled);
		GetDlgItem(IDC_GA_TIME)->EnableWindow(bEnabled);
		break;
	}

	m_prctrlTime.SetRange(0,m_pFJSS->m_nGATime);
	m_prctrlCycle.SetRange(0,m_pFJSS->m_nGACycle);

	UpdateData(FALSE);
}

void CGADlg::IsSolving(BOOL bIsSolving)
{
	BOOL bEnabled = !bIsSolving;
	m_bIsSolving = bIsSolving;

	GetDlgItem(IDC_GA_TIME)->EnableWindow(bEnabled);
	GetDlgItem(IDC_GA_POPULATION)->EnableWindow(bEnabled);
	GetDlgItem(IDC_GA_CYCLE)->EnableWindow(bEnabled);
	GetDlgItem(IDC_GA_SELCOEF)->EnableWindow(bEnabled);
	GetDlgItem(IDC_GA_MINMSX)->EnableWindow(bEnabled);
	GetDlgItem(IDC_GA_MSMCYCLE)->EnableWindow(bEnabled);
	GetDlgItem(IDC_GA_LSCYCLE)->EnableWindow(bEnabled);
	GetDlgItem(IDC_BT_SOLVEGA)->EnableWindow(bEnabled);
	GetDlgItem(IDC_BT_CRITERION)->EnableWindow(bEnabled);
	GetDlgItem(IDOK)->EnableWindow(bEnabled);
	GetDlgItem(IDCANCEL)->EnableWindow(bEnabled);
}
void CGADlg::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class
	if (!m_bIsSolving)
		CDialog::OnCancel();
}
