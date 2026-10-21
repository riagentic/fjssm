// ConvertDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FJSSM.h"
#include "ConvertDlg.h"


// CConvertDlg dialog

IMPLEMENT_DYNAMIC(CConvertDlg, CDialog)
CConvertDlg::CConvertDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConvertDlg::IDD, pParent)
{
}

CConvertDlg::~CConvertDlg()
{
	
}

void CConvertDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX,IDC_ED_DDCOEF,m_pFJSS->m_fDDCoef);
	DDX_Text(pDX,IDC_ED_PTCOEF,m_pFJSS->m_fPTCoef);
	DDX_Text(pDX,IDC_ED_JOBCOEF,m_pFJSS->m_fJobCoef);
	DDX_Text(pDX,IDC_ED_MACHINECOEF,m_pFJSS->m_fMachineCoef);

	DDV_MinMaxDouble(pDX,m_pFJSS->m_fDDCoef,0,1);
	DDV_MinMaxDouble(pDX,m_pFJSS->m_fPTCoef,0,1);
	DDV_MinMaxDouble(pDX,m_pFJSS->m_fJobCoef,0,1);
	DDV_MinMaxDouble(pDX,m_pFJSS->m_fMachineCoef,0,1);

}


BEGIN_MESSAGE_MAP(CConvertDlg, CDialog)
END_MESSAGE_MAP()


// CConvertDlg message handlers

void CConvertDlg::OnOK()
{
	CDialog::OnOK();
}

BOOL CConvertDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

