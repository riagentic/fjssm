#pragma once
#include "fjss.h"

// CConvExpDlg dialog

class CConvExpDlg : public CDialog
{
	DECLARE_DYNAMIC(CConvExpDlg)

public:
	CConvExpDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CConvExpDlg();

// Dialog Data
	enum { IDD = IDD_CONVEXPDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CFJSS* m_pFJSS;

// Running

	CProgressCtrl m_prctrlPart;
	CProgressCtrl m_prctrlAll;
	CString m_strPartProgress;
	CString m_strAllProgress;

	void SetGlobalBehaivour();
	void SetPresentBehaviour();

	void IsSolving(BOOL bIsSolving);
	BOOL m_bIsSolving;


	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
public:
	afx_msg void OnBnClickedBtParamsa();
	afx_msg void OnBnClickedBtParamga();
	afx_msg void OnBnClickedBtSolve();
	afx_msg void OnBnClickedCbAllcrit();
	afx_msg void OnBnClickedBtCriterion();
protected:
	virtual void OnCancel();
};
