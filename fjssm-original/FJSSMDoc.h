// FJSSMDoc.h : interface of the CFJSSMDoc class
//

#pragma once
#include "fjss.h"

class CFJSSMView;
class CFJSSMDoc : public CDocument
{
protected: // create from serialization only
	CFJSSMDoc();
	DECLARE_DYNCREATE(CFJSSMDoc)

// Attributes
public:

// Operations
public:

// Overrides
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CFJSSMDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFileImport();

public:
	//instance CFJSS 
	CFJSS m_fjss;
	//CFJSSMView* m_pView;

	afx_msg void OnFileExport();
	afx_msg void OnComputeOptions();
	afx_msg void OnConvertDefaultDD();

	afx_msg void OnUpdateResult(CCmdUI *pCmdUI);
	afx_msg void OnUpdateResultSA(CCmdUI *pCmdUI);
	afx_msg void OnUpdateResultGA(CCmdUI *pCmdUI);
	afx_msg void OnUpdateResultSAGA(CCmdUI *pCmdUI);
	afx_msg void OnResultSAGA(UINT nID);

	afx_msg void OnEditProject();
	afx_msg void OnEditJob();
	afx_msg void OnEditMachine();
	afx_msg void OnEditProcess();

	afx_msg void OnConvertOption();
	afx_msg void OnComputeConvexp();
	afx_msg void OnComputeCritexp();
	
	afx_msg void OnUpdateEditProcess(CCmdUI *pCmdUI);
	afx_msg void OnUpdateCompute(CCmdUI *pCmdUI);
	afx_msg void OnUpdateComputeConvexp(CCmdUI *pCmdUI);
	afx_msg void OnUpdateResultConvexp(CCmdUI *pCmdUI);
	afx_msg void OnUpdateResultCritIteration(CCmdUI *pCmdUI);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
};


