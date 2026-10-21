// ConvExpDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FJSSM.h"
#include "ConvExpDlg.h"
#include "SAdlg.h"
#include "GAdlg.h"
#include "CriteriaDlg.h"


// CConvExpDlg dialog

IMPLEMENT_DYNAMIC(CConvExpDlg, CDialog)
CConvExpDlg::CConvExpDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConvExpDlg::IDD, pParent)
{
	m_bIsSolving = FALSE;
}

CConvExpDlg::~CConvExpDlg()
{
}

void CConvExpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
// Init
	DDX_Text(pDX,IDC_ED_SOLVECOUNT, m_pFJSS->m_nCOESolveCount);
	DDX_Text(pDX,IDC_ED_SOLVETIME, m_pFJSS->m_nCOESolveTime);
	DDX_Check(pDX,IDC_CB_ALLCRIT, m_pFJSS->m_bCOEAllCriterion);

// Running
	DDX_Text(pDX,IDC_ST_CRITERION,m_pFJSS->GetStrCriterion(m_pFJSS->m_nCriterion));
	DDX_Control(pDX,IDC_PR_PART,m_prctrlPart);
	DDX_Control(pDX,IDC_PR_ALL,m_prctrlAll);

	DDV_MinMaxInt(pDX,m_pFJSS->m_nCOESolveCount,1,100);
	DDV_MinMaxInt(pDX,m_pFJSS->m_nCOESolveTime,1,1000);

}


BEGIN_MESSAGE_MAP(CConvExpDlg, CDialog)
	ON_BN_CLICKED(IDC_BT_PARAMSA, OnBnClickedBtParamsa)
	ON_BN_CLICKED(IDC_BT_PARAMGA, OnBnClickedBtParamga)
	ON_BN_CLICKED(IDC_BT_SOLVE, OnBnClickedBtSolve)
	ON_BN_CLICKED(IDC_CB_ALLCRIT, OnBnClickedCbAllcrit)
	ON_BN_CLICKED(IDC_BT_CRITERION, OnBnClickedBtCriterion)
END_MESSAGE_MAP()


// CConvExpDlg message handlers

BOOL CConvExpDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	SetGlobalBehaivour();
	SetPresentBehaviour();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CConvExpDlg::SetGlobalBehaivour()
{
	UpdateData(FALSE);
}

void CConvExpDlg::SetPresentBehaviour()
{
	if (!m_bIsSolving)
	switch (m_pFJSS->m_bCOEAllCriterion)
	{
		case FALSE:
			GetDlgItem(IDC_BT_CRITERION)->EnableWindow(TRUE);
			break;
		case TRUE:
			GetDlgItem(IDC_BT_CRITERION)->EnableWindow(FALSE);
			break;
	}

	switch (m_pFJSS->m_nSubAction)
	{
	case -1:		// NONE
		GetDlgItem(IDC_ST_ARROW1)->EnableWindow(FALSE);
		GetDlgItem(IDC_ST_ARROW2)->EnableWindow(FALSE);
		break;
	case 0:			// SA
		GetDlgItem(IDC_ST_ARROW1)->EnableWindow(TRUE);
		GetDlgItem(IDC_ST_ARROW2)->EnableWindow(FALSE);
		break;
	case 1:			// GA
		GetDlgItem(IDC_ST_ARROW1)->EnableWindow(FALSE);
		GetDlgItem(IDC_ST_ARROW2)->EnableWindow(TRUE);
		break;
	}

	switch (m_pFJSS->m_nCritMaxOrSum)
	{
	case 0:			// MAX
		GetDlgItem(IDC_ST_ARROW3)->EnableWindow(TRUE);
		GetDlgItem(IDC_ST_ARROW4)->EnableWindow(FALSE);
		break;
	case 1:			// SUMA
		GetDlgItem(IDC_ST_ARROW3)->EnableWindow(FALSE);
		GetDlgItem(IDC_ST_ARROW4)->EnableWindow(TRUE);
		break;
	}

}

void CConvExpDlg::OnOK()
{
	UpdateData(TRUE);
	CDialog::OnOK();
}

void CConvExpDlg::OnBnClickedBtParamsa()
{
	CSADlg dlg;
	dlg.m_pFJSS = m_pFJSS;
	dlg.m_nDlgType = 1;		// only params
	dlg.DoModal();
}

void CConvExpDlg::OnBnClickedBtParamga()
{
	CGADlg dlg;
	dlg.m_pFJSS = m_pFJSS;
	dlg.m_nDlgType = 1;		// only params
	dlg.DoModal();
}

void CConvExpDlg::OnBnClickedBtSolve()
{
	// Akce:
	// 1. vymazat m_areaConvergence
	// 2. nastaveni hlavni akce a dialog
	// 3. Update dat
	// 4. ctrl
	// 5. vsechna pouzitelna kriteria
	// 6. Akce
	

	int nAnswer = AfxMessageBox(_T("Výpoèet mùže trvat i NÌKOLIK MINUT, opravdu chcete pokraèovat ?"),
								MB_YESNO | MB_ICONQUESTION);
	if (!(nAnswer == IDYES)) return;

	IsSolving(TRUE);

	int nIndex;
	// 1.
	m_pFJSS->m_areafConvergence.RemoveAll();
	// 2.
	m_pFJSS->m_nAction = 2;		// Convergence Experiment
	m_pFJSS->m_pConvExpDlg = this;
	// 3.
	UpdateData(TRUE);
	m_pFJSS->m_nSATime = m_pFJSS->m_nCOESolveTime;
	m_pFJSS->m_nGATime = m_pFJSS->m_nCOESolveTime;

	m_prctrlPart.SetRange(0,m_pFJSS->m_nCOESolveTime);

	// 2. Samotny vypocet
	if (m_pFJSS->m_bCOEAllCriterion)			// All able criterion
	{
		m_prctrlAll.SetRange(0,m_pFJSS->m_nAbleCriterion * m_pFJSS->m_nCOESolveCount * 	2);

		for (int i=0; i<m_pFJSS->m_nAbleCriterion; i++)	// AbleCriteria
			for (int k = 0; k < m_pFJSS->m_nCOESolveCount; k++)			// SolveCount
			{
				nIndex = m_pFJSS->m_areafConvergence.Add(CDoubleList());
				m_pFJSS->m_areafConvergence[nIndex].Add(m_pFJSS->m_lstnAbleCriterion[i]);
				for (int j=0; j<2; j++)						// MAX || SUM
				{

					m_pFJSS->m_nCritMaxOrSum = j;
					m_pFJSS->m_nCriterion = m_pFJSS->m_lstnAbleCriterion[i];

					m_pFJSS->m_nSubAction = 0;
					SetPresentBehaviour();
					m_pFJSS->SolveSA();
					m_pFJSS->m_nSubAction = 1;
					SetPresentBehaviour();
					m_pFJSS->SolveGA();
					
					m_prctrlAll.SetPos(i * m_pFJSS->m_nCOESolveCount * 2 +	k * 2  + j + 1);

				}
	
			}
	}
	else
	{
		m_prctrlAll.SetRange(0,m_pFJSS->m_nCOESolveCount * 	2);

		for (int k = 0; k < m_pFJSS->m_nCOESolveCount; k++)			// SolveCount
		{
			nIndex = m_pFJSS->m_areafConvergence.Add(CDoubleList());
			m_pFJSS->m_areafConvergence[nIndex].Add(m_pFJSS->m_nCriterion);
			for (int j=0; j<2; j++)						// MAX || SUM
			{
				m_pFJSS->m_nCritMaxOrSum = j;
				m_pFJSS->m_nSubAction = 0;
				SetPresentBehaviour();
				m_pFJSS->SolveSA();
				m_pFJSS->m_nSubAction = 1;
				SetPresentBehaviour();
				m_pFJSS->SolveGA();
					
				m_prctrlAll.SetPos(k * 2  + j + 1);
			}
	
		}
	}

	m_pFJSS->IsAbleConvResult(1);

	IsSolving(FALSE);
	SetPresentBehaviour();

}

void CConvExpDlg::OnBnClickedCbAllcrit()
{
	CButton* pButton;
	pButton = (CButton*)GetDlgItem(IDC_CB_ALLCRIT);
	m_pFJSS->m_bCOEAllCriterion = !m_pFJSS->m_bCOEAllCriterion;
	pButton->SetCheck(m_pFJSS->m_bCOEAllCriterion);
	SetPresentBehaviour();
}

void CConvExpDlg::OnBnClickedBtCriterion()
{
	CCriteriaDlg dlg;
	dlg.m_pFJSS = m_pFJSS;
	if (dlg.DoModal()==IDOK) 
	{
		UpdateData(FALSE);
	}
}

void CConvExpDlg::IsSolving(BOOL bIsSolving)
{
	m_bIsSolving = bIsSolving;
	BOOL bEnabled = !bIsSolving;

	GetDlgItem(IDC_ED_SOLVECOUNT)->EnableWindow(bEnabled);
	GetDlgItem(IDC_ED_SOLVETIME)->EnableWindow(bEnabled);
	GetDlgItem(IDC_CB_ALLCRIT)->EnableWindow(bEnabled);
	GetDlgItem(IDC_BT_CRITERION)->EnableWindow(bEnabled);
	GetDlgItem(IDC_BT_PARAMSA)->EnableWindow(bEnabled);
	GetDlgItem(IDC_BT_PARAMGA)->EnableWindow(bEnabled);
	GetDlgItem(IDC_BT_SOLVE)->EnableWindow(bEnabled);
	GetDlgItem(IDOK)->EnableWindow(bEnabled);
	GetDlgItem(IDCANCEL)->EnableWindow(bEnabled);
}
void CConvExpDlg::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class
	if (!m_bIsSolving)
		CDialog::OnCancel();
}
