#pragma once
#include "fjss.h"

// CConvertDlg dialog

class CConvertDlg : public CDialog
{
	DECLARE_DYNAMIC(CConvertDlg)

public:
	CConvertDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CConvertDlg();

// Dialog Data
	enum { IDD = IDD_CONVERTDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CFJSS* m_pFJSS;

protected:
	virtual void OnOK();

public:
	virtual BOOL OnInitDialog();
	
    
};
