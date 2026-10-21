#pragma once
#include "fjss.h"


// CCritExpDlg dialog

class CCritExpDlg : public CDialog
{
	DECLARE_DYNAMIC(CCritExpDlg)

public:
	CCritExpDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCritExpDlg();

// Dialog Data
	enum { IDD = IDD_CRITEXP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CFJSS* m_pFJSS;

// Runtime
	CProgressCtrl m_prctrlProgress;
	CProgressCtrl m_prctrlPart;

// Method
	void SetGlobalBehaviour();
	void SetPresentBehaviour();
	
// Events
	afx_msg void OnBnClickedSolve();
	virtual BOOL OnInitDialog();

	double GetBest(CDoubleArea& DoubleArea,int nCol);
	double GetWorst(CDoubleArea& DoubleArea,int nCol);
	double GetRE(double fVOOF,double fBest,double fWorst);
	double GetAverage(CDoubleArea& DoubleArea,int nCol);
	void SolveRelativeError();
	void SolveAverageRE();

	void IsSolving(BOOL bIsSolving);
	BOOL m_bIsSolving;
};
