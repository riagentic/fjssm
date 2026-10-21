#pragma once
#include "fjss.h"


// CCriteriaDlg dialog

class CCriteriaDlg : public CDialog
{
	DECLARE_DYNAMIC(CCriteriaDlg)

public:
	CCriteriaDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCriteriaDlg();

// Dialog Data
	enum { IDD = IDD_CRITERIADLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:

	CFJSS* m_pFJSS;

	double m_fw1m;
	double m_fw2s;
	double m_fw1;
	double m_fw2;
	double m_fw3;
	double m_fw4;

	int m_nCriterion;
	int m_nCritMaxOrSum;

	BOOL m_bCrit[CRIT_COUNT];
    
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();

public:
	afx_msg void OnBnClickedRdCrit();

	void SetEditBehaviour();
	void SetGlobalBehaviour();
	afx_msg void OnStnClickedStaticFddfpt2();
};
