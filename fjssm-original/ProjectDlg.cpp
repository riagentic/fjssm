// ProjectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FJSSM.h"
#include "ProjectDlg.h"


// CProjectDlg dialog

IMPLEMENT_DYNAMIC(CProjectDlg, CDialog)
CProjectDlg::CProjectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProjectDlg::IDD, pParent)
{
}

CProjectDlg::~CProjectDlg()
{
}

void CProjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX,IDC_ED_PROJECTNAME,m_pFJSS->m_strName);
	DDX_Text(pDX,IDC_ED_MAKER,m_pFJSS->m_strMaker);
	DDX_Text(pDX,IDC_ST_DATE,m_pFJSS->m_strDate);
}


BEGIN_MESSAGE_MAP(CProjectDlg, CDialog)
END_MESSAGE_MAP()


// CProjectDlg message handlers

BOOL CProjectDlg::OnInitDialog()
{
	CDialog::OnInitDialog();


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CProjectDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	UpdateData(TRUE);
	CDialog::OnOK();
}
