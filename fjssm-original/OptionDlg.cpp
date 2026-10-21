// OptionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FJSSM.h"
#include "OptionDlg.h"


// OptionDlg dialog

IMPLEMENT_DYNAMIC(OptionDlg, CDialog)
OptionDlg::OptionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(OptionDlg::IDD, pParent)
{
}

OptionDlg::~OptionDlg()
{
}

void OptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(OptionDlg, CDialog)
END_MESSAGE_MAP()


// OptionDlg message handlers
