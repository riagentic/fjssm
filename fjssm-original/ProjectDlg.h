#pragma once
#include "fjss.h"

// CProjectDlg dialog

class CProjectDlg : public CDialog
{
	DECLARE_DYNAMIC(CProjectDlg)

public:
	CProjectDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CProjectDlg();

// Dialog Data
	enum { IDD = IDD_PROJECTDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CFJSS* m_pFJSS;

	void GetFJSSData(BOOL bFrom=TRUE);
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
};
