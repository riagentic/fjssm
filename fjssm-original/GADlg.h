#pragma once
#include "FJSS.h"


// CGADlg dialog

class CGADlg : public CDialog
{
	DECLARE_DYNAMIC(CGADlg)

public:
	CGADlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CGADlg();

// Dialog Data
	enum { IDD = IDD_GADLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
	virtual void OnOK();

public:
	virtual BOOL OnInitDialog();
	

public:

// Trace
	int m_nDlgType; 

	CFJSS* m_pFJSS;

	CString m_strActCrit;


// Running
	CString m_strTimeProgress;
	CString m_strCycleProgress;
	CString m_strX0;
	CString m_strXBest;

	CProgressCtrl m_prctrlCycle;
	CProgressCtrl m_prctrlTime;

// Method

	afx_msg void OnBnClickedBtSolve();
	afx_msg void OnBnClickedBtCriterion();

	void IsSolving(BOOL bIsSolving);
	BOOL m_bIsSolving;
	void SetGlobalBehaviour();
	int UpdateOperation(int nOperation);
protected:
	virtual void OnCancel();
};

