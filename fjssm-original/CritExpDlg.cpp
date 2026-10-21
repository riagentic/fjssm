// CritExpDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FJSSM.h"
#include "CritExpDlg.h"
#include "math.h"


// CCritExpDlg dialog

IMPLEMENT_DYNAMIC(CCritExpDlg, CDialog)
CCritExpDlg::CCritExpDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCritExpDlg::IDD, pParent)
{
	m_bIsSolving = FALSE;
}

CCritExpDlg::~CCritExpDlg()
{
}

void CCritExpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX,IDC_ED_ITERCOUNT,m_pFJSS->m_nCREIteration);
	DDX_Radio(pDX,IDC_RB_METHOD_SA,m_pFJSS->m_nCREMethod);

	DDX_Text(pDX,IDC_ST_CRITERION,m_pFJSS->GetStrCriterion(m_pFJSS->m_nCriterion));
	DDX_Control(pDX,IDC_PR_ALL,m_prctrlProgress);
	DDX_Control(pDX,IDC_PR_PART,m_prctrlPart);
}


BEGIN_MESSAGE_MAP(CCritExpDlg, CDialog)
	ON_BN_CLICKED(ID_SOLVE, OnBnClickedSolve)
END_MESSAGE_MAP()


// CCritExpDlg message handlers


void CCritExpDlg::OnBnClickedSolve()
{

	int nAnswer = AfxMessageBox(_T("Výpoèet mùže trvat i NÌKOLIK MINUT, opravdu chcete pokraèovat ?"),
								MB_YESNO | MB_ICONQUESTION);
	if (!(nAnswer == IDYES)) return;

	IsSolving(TRUE);

	UpdateData(TRUE);
	m_pFJSS->m_nAction = 3;
	m_pFJSS->m_pCritExpDlg = this;

	// Remove Old data
	m_pFJSS->m_areafIteration.RemoveAll();
	m_pFJSS->m_areafRE.RemoveAll();
	m_pFJSS->m_areafAverageRE.RemoveAll();

	// Process...
	m_prctrlProgress.SetRange(0,m_pFJSS->m_nAbleCriterion);
	m_prctrlPart.SetRange(0,m_pFJSS->m_nCREIteration);
	
	for (int i=0; i<m_pFJSS->m_nAbleCriterion; i++)
		//for (int j=0; j<m_pFJSS->m_nCREIteration; j++)
		{
			m_pFJSS->m_nCREActIter = 0;
			m_pFJSS->m_nCriterion = m_pFJSS->m_lstnAbleCriterion[i];
			UpdateData(FALSE);

			switch (m_pFJSS->m_nCREMethod)
			{
				case 0:			// SA
					m_pFJSS->SolveSA();
					break;
				case 1:			// GA
					m_pFJSS->SolveGA();
					break;
			}
		
			m_prctrlProgress.SetPos(i+1);
	
	}

	SolveRelativeError();
	SolveAverageRE();

	m_pFJSS->IsAbleCritResult(1);

	IsSolving(FALSE);
	SetPresentBehaviour();
}

void CCritExpDlg::SolveRelativeError()
{
	// Vypocet relativni chyby ER
	// Zjisteni maxima a minima
	
	BOOL bFirst = TRUE;
	CDoubleList lstBest;
	CDoubleList lstWorst;
	CDoubleList lstSuma;
	double nSuma = 0;
	int nIter = m_pFJSS->m_nCREIteration;

	m_pFJSS->m_areafRE = m_pFJSS->m_areafIteration;

	
	for (int i=0; i<m_pFJSS->m_nAbleCriterion; i++)
	{
		lstBest.RemoveAll();
		lstWorst.RemoveAll();
		lstSuma.RemoveAll();
		bFirst = TRUE;

		for (int j=0; j<nIter; j++)
		{
			for (int k=0; k<m_pFJSS->m_nAbleCriterion; k++)
			{
				if (bFirst)
				{
					lstBest.Add(m_pFJSS->m_areafRE[i*nIter+j][k]);
					lstWorst.Add(m_pFJSS->m_areafRE[i*nIter+j][k]);
				}
				else 
				{
					lstBest[k] = min(lstBest[k],m_pFJSS->m_areafRE[i*nIter+j][k]);
					lstWorst[k] = max(lstWorst[k],m_pFJSS->m_areafRE[i*nIter+j][k]);
				}
			}
			bFirst = FALSE;
		}
		
		for (int j=0; j<nIter; j++)
			for (int k=0; k<m_pFJSS->m_nAbleCriterion; k++)
			{
				m_pFJSS->m_areafRE[i*nIter+j][k] = GetRE(m_pFJSS->m_areafRE[i*nIter+j][k],
					lstBest[k],lstWorst[k]);				
				TRACE(_T("Uprava radku %d , sloupce %d\n"),i*nIter+j,k);
			}

	}
}

void CCritExpDlg::SolveAverageRE()
{
	BOOL bFirst;	
	// Spocteni prumernych hodnot
	bFirst = TRUE;
	int nIter = m_pFJSS->m_nCREIteration;

	for (int i=0; i<m_pFJSS->m_nAbleCriterion; i++)
	{
		bFirst = TRUE;
		m_pFJSS->m_areafAverageRE.Add(CDoubleList());
		for (int j=0; j<nIter; j++)
		{
			for (int k=0; k<m_pFJSS->m_nAbleCriterion; k++)
			{
				if (bFirst)
					m_pFJSS->m_areafAverageRE[i].Add(m_pFJSS->m_areafRE[i*nIter+j][k]);
				else
					m_pFJSS->m_areafAverageRE[i][k] = m_pFJSS->m_areafAverageRE[i][k] +
												m_pFJSS->m_areafRE[i*nIter+j][k];
			}
			bFirst = FALSE;
		}

		for (int k=0; k<m_pFJSS->m_nAbleCriterion; k++)
			m_pFJSS->m_areafAverageRE[i][k] = (m_pFJSS->m_areafAverageRE[i][k] / ((double)nIter));

	}

	// Vypocet celkovych prumeru RE
	for (int i= 0; i<m_pFJSS->m_areafAverageRE.GetSize(); i++)
	{
		double nSuma = 0;
		for (int j=0; j<m_pFJSS->m_areafAverageRE[i].GetSize(); j++)
		{
			nSuma += m_pFJSS->m_areafAverageRE[i][j];
		}
		int nIndex = m_pFJSS->m_areafAverageRE[i].Add(nSuma / (double)m_pFJSS->m_nAbleCriterion);
	}

}



double CCritExpDlg::GetRE(double fVOOF,double fBest,double fWorst)
{
	double fResult;

	fResult = (100.0) * ((fabs((fVOOF - fBest))) / (fabs((fWorst - fBest))));
	if (fabs(fWorst-fBest) == 0) return 100;

	return fResult;
}


void CCritExpDlg::SetGlobalBehaviour()
{
	m_prctrlProgress.SetRange(0,m_pFJSS->m_nCREIteration);
}

void CCritExpDlg::SetPresentBehaviour()
{
}

BOOL CCritExpDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	UpdateData(FALSE);
	SetGlobalBehaviour();
	SetPresentBehaviour();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CCritExpDlg::IsSolving(BOOL bIsSolving)
{
	BOOL bEnabled = !bIsSolving;

	GetDlgItem(IDC_ED_ITERCOUNT)->EnableWindow(bEnabled);
	GetDlgItem(IDC_RB_METHOD_SA)->EnableWindow(bEnabled);
	GetDlgItem(IDC_RB_METHOD_GA)->EnableWindow(bEnabled);
	GetDlgItem(ID_SOLVE)->EnableWindow(bEnabled);
	GetDlgItem(IDOK)->EnableWindow(bEnabled);
	GetDlgItem(IDCANCEL)->EnableWindow(bEnabled);

	m_bIsSolving = bIsSolving;
}