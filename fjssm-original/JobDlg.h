#pragma once
#include "colorbtn.h"
#include "fjss.h"

// CJobDlg dialog

class CJobDlg : public CDialog
{
	DECLARE_DYNAMIC(CJobDlg)

public:
	CJobDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CJobDlg();

// Dialog Data
	enum { IDD = IDD_JOBDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	CFJSS* m_pFJSS;

	CColorBtn m_crbtJob;
	CListBox m_lbJob;

	CStatic m_stBitmap;
	CBitmap m_bmBitmap;

	int m_nIndex;
	int m_nColor;
	CString m_strName;
	CFN m_fDD;
	double m_fAlfa;
	double m_fBeta;
	double m_fLambda1;
	double m_fLambda2;
	double m_fLambda3;
	
	void SetGlobalBehaviour();
	void SetPresentBehaviour();
	void LoadFJSSData();
	void SaveFJSSData();
	int UpdateListBox();
	void UpdateDataEx(BOOL bSaveAndValidate);

	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedBtChange();
	afx_msg void OnLbnSelchangeLbJobs();
	afx_msg void OnBnClickedBtAdd();
	afx_msg void OnBnClickedBtRemove();
};
