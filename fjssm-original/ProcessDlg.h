#pragma once
#include "fjss.h"

// CProcessDlg dialog

class CProcessDlg : public CDialog
{
	DECLARE_DYNAMIC(CProcessDlg)

public:
	CProcessDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CProcessDlg();

// Dialog Data
	enum { IDD = IDD_PROCESSDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();

public:

	CFJSS* m_pFJSS;

	CButton m_btUp;
	CButton m_btDown;
	CButton m_btRight;
	CButton m_btLeft;

	CListBox m_lbProcess;
	CBitmap m_bmBitmap;
	CStatic m_stBitmap;

	// Job
	int m_nJobIndex;
	CString m_strJobName;

	// Process
	int m_nProcessIndex;
	CString m_strProcessName;
	int m_nMachine;
	CComboBox m_cbMachine;
	CFN m_fPT;

	void LoadFJSSData();
	void SaveFJSSData();

	void FillMachineComboBox();
	void SetGlobalProcessBehaviour();
	void SetPresentProcessBehaviour();
	void SetGlobalJobBehaviour();
	void SetPresentJobBehaviour();
	int UpdateListBox();

	afx_msg void OnBnClickedBtUp();
	afx_msg void OnBnClickedBtLeft();
	afx_msg void OnBnClickedBtRight();
	afx_msg void OnBnClickedBtDown();
	afx_msg void OnBnClickedBtChangeprocess();

	afx_msg void OnLbnSelchangeLbProcess();
	afx_msg void OnBnClickedBtAdd();
	afx_msg void OnBnClickedBtRemove();
};
