#pragma once
#include "stdafx.h"
#include "FJSS.h"
#include "resource.h"

// CSADlg dialog

class CSADlg : public CDialog
{
	DECLARE_DYNAMIC(CSADlg)

public:
	CSADlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSADlg();

// Dialog Data
	enum { IDD = IDD_SADLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
	virtual void OnOK();

public:
	virtual BOOL OnInitDialog();

public:

// Trace value
	int m_nDlgType;						// 0 - standard, 1 - pouze uprava parametru

	CFJSS*	m_pFJSS;

// Runtime value

	CString m_strActTemp;
	CString m_strfXBest;

	double m_fActCycleCount;
	double m_fActCycle;
	double m_fXBest;

	CString m_strTimeProgress;
	CString m_strCycleProgress;

	CProgressCtrl m_prctrlCycle;
	CProgressCtrl m_prctrlTime;


// Methods 
	void SetGlobalBehaviour();
	void SetPresentBehaviour();

	void IsSolving(BOOL bIsSolving);
	BOOL m_bIsSolving;
	afx_msg void OnBnClickedSaSolve();
	afx_msg void OnBnClickedSaCriterion();
protected:
	virtual void OnCancel();
};
