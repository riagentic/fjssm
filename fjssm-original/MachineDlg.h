#pragma once
#include "FJSS.h"
#include "colorbtn.h"

// CMachineDlg dialog

class CMachineDlg : public CDialog
{
	DECLARE_DYNAMIC(CMachineDlg)

public:
	CMachineDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMachineDlg();

// Dialog Data
	enum { IDD = IDD_MACHINEDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	CFJSS* m_pFJSS;
	CBitmap m_bmBitmap;
	CStatic m_stBitmap;
// listbox
	CListBox m_lbMachine;
	CColorBtn m_crbtMachine;

	
// stroj
	CString m_strName;
	int m_nIndex;
	int m_nColor;
	
	virtual BOOL OnInitDialog();

	int UpdateListBox();
	int UpdateElement(int nIndex);

	void LoadFJSSData();
	void SaveFJSSData();

	void SetGlobalBehaviour();
	void SetPresentBehaviour();

	void UpdateDataEx(BOOL bSaveAndValidate);

	afx_msg void OnLbnSelchangeLbMachinelist();
	afx_msg void OnBnClickedBtChange();
	afx_msg void OnBnClickedBtAdd();
	afx_msg void OnBnClickedBtRemove();
protected:
	virtual void OnOK();
};

