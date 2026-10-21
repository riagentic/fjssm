// FJSSMDoc.cpp : implementation of the CFJSSMDoc class
//

#include "stdafx.h"
#include "FJSSM.h"

#include "FJSSMDoc.h"
#include "CriteriaDlg.h"
#include "JobDlg.h"
#include "MachineDlg.h"
#include "ProcessDlg.h"
#include "ConvertDlg.h"
#include "ProjectDlg.h"
#include "ConvExpDlg.h"
#include "CritExpDlg.h"
#include "fjssmview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CFJSSMDoc, CDocument)
BEGIN_MESSAGE_MAP(CFJSSMDoc, CDocument)
	ON_COMMAND(ID_FILE_IMPORT, OnFileImport)
	ON_COMMAND(ID_FILE_EXPORT, OnFileExport)

	ON_COMMAND(ID_EDIT_PROJECT, OnEditProject)
	ON_COMMAND(ID_EDIT_JOB, OnEditJob)
	ON_COMMAND(ID_EDIT_MACHINE, OnEditMachine)
	ON_COMMAND(ID_EDIT_PROCESS, OnEditProcess)

	ON_COMMAND(ID_CONVERT_OPTION, OnConvertOption)
	ON_COMMAND(ID_CONVERT_DEFAULTDD, OnConvertDefaultDD)

	ON_COMMAND(ID_COMPUTE_CONVEXP, OnComputeConvexp)
	ON_COMMAND(ID_COMPUTE_CRITEXP, OnComputeCritexp)
	ON_COMMAND(ID_COMPUTE_CRITERION, OnComputeOptions)

	ON_UPDATE_COMMAND_UI_RANGE(ID_COMPUTE_SA,ID_COMPUTE_CRITEXP,OnUpdateCompute)

	ON_UPDATE_COMMAND_UI_RANGE (ID_RESULT_SOLVE, ID_RESULT_RUN,OnUpdateResult)
	ON_UPDATE_COMMAND_UI_RANGE(ID_RESULT_SA,ID_RESULT_GA,OnUpdateResultSAGA)
	ON_COMMAND_RANGE(ID_RESULT_SA,ID_RESULT_GA,OnResultSAGA)

	
	ON_UPDATE_COMMAND_UI(ID_EDIT_PROCESS, OnUpdateEditProcess)
	
	ON_UPDATE_COMMAND_UI(ID_RESULT_CONVEXP, OnUpdateResultConvexp)
	ON_UPDATE_COMMAND_UI_RANGE(ID_RESULT_CRIT_ITERATION,ID_RESULT_CRIT_SOARE,	OnUpdateResultCritIteration)

END_MESSAGE_MAP()


// CFJSSMDoc construction/destruction

CFJSSMDoc::CFJSSMDoc()
{
	// TODO: add one-time construction code here

}

CFJSSMDoc::~CFJSSMDoc()
{
}

BOOL CFJSSMDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	m_fjss.DeleteAll();
	UpdateAllViews(NULL,1);

	return TRUE;
}




// CFJSSMDoc serialization

void CFJSSMDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		m_fjss.Serialize(ar);
	}
	else
	{
		m_fjss.Serialize(ar);		
	}
}


// CFJSSMDoc diagnostics

#ifdef _DEBUG
void CFJSSMDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CFJSSMDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CFJSSMDoc commands

void CFJSSMDoc::OnFileImport()
{
    TCHAR szFilters[] =
	_T ("FJSS Import files (*.csv)|*.csv|All files (*.*)|*.*||");

	CFileDialog dlg(TRUE, _T("csv"), _T("*.csv"),
		OFN_FILEMUSTEXIST  | OFN_HIDEREADONLY, szFilters);

	if (dlg.DoModal () == IDOK) 
	{
		// dealokace zdroju
		m_fjss.DeleteAll();

		// import 
		m_fjss.m_strImportFile = dlg.GetPathName ();
		if (!m_fjss.Import()) SetModifiedFlag(TRUE);

		SetTitle(m_fjss.m_strName);
		SetPathName(GetTitle());
		UpdateAllViews(NULL,1);
		
	}

	// TODO: Add your command handler code here
}


void CFJSSMDoc::OnFileExport()
{
	CString strName;
	strName.Format(_T("%s.csv"),m_fjss.m_strName);
    TCHAR szFilters[] =
	_T ("FJSS Import files (*.csv)|*.csv|All files (*.*)|*.*||");

	CFileDialog dlg(FALSE, _T("csv"), strName,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilters);

	if (dlg.DoModal () == IDOK) 
	{
		// export 		
		//if (!m_fjss.Import()) SetModifiedFlag(TRUE);
		m_fjss.m_strExportFile = dlg.GetPathName();
		m_fjss.Export();

		UpdateAllViews(NULL);
		
	}

	// TODO: Add your command handler code here
}

void CFJSSMDoc::OnComputeOptions()
{
	CCriteriaDlg dlg;
	dlg.m_pFJSS = &m_fjss;
	dlg.DoModal();
}

void CFJSSMDoc::OnConvertDefaultDD()
{

	
	int nRet = AfxMessageBox(_T("Opravdu chcete provést \n\n")
							_T("AUTOMATICKÉ GENEROVÁNÍ VŠECH TERMÍNÙ DOKONÈENÍ ?     "),
							MB_OK | MB_YESNO |MB_ICONQUESTION);

    if (nRet == IDYES)
	{
		m_fjss.SetDefaultDD();
		int nRet = AfxMessageBox(_T("\n\nTERMÍNY DOKONOÈENÍ byly úspìšnì vygenerovány.   ")
									,MB_OK);

		SetModifiedFlag(TRUE);
	}

}





//---------------------------------------------------------
// On Edit Routines
//---------------------------------------------------------
void CFJSSMDoc::OnEditProject()
{
	CProjectDlg dlg;
	dlg.m_pFJSS = &m_fjss;
	if (dlg.DoModal() == IDOK)
	{
		SetTitle(m_fjss.m_strName);
		IsModified();
		SetModifiedFlag(TRUE);
	}

}
void CFJSSMDoc::OnEditJob()
{
	CJobDlg dlg;
	dlg.m_pFJSS = &m_fjss;
	if (dlg.DoModal() == IDOK) IsModified();
	if (m_fjss.m_bIsModified) SetModifiedFlag(TRUE);
}

void CFJSSMDoc::OnEditMachine()
{
	CMachineDlg dlg;
	dlg.m_pFJSS = &m_fjss;
	if (dlg.DoModal() == IDOK) IsModified();
	if (m_fjss.m_bIsModified) SetModifiedFlag(TRUE);
}

void CFJSSMDoc::OnEditProcess()
{
	CProcessDlg dlg;
	dlg.m_pFJSS = &m_fjss;
	if (dlg.DoModal() == IDOK) IsModified();
	if (m_fjss.m_bIsModified) SetModifiedFlag(TRUE);
}

void CFJSSMDoc::OnUpdateEditProcess(CCmdUI *pCmdUI)
{
	if ((m_fjss.m_nMachine > 0) && (m_fjss.m_nJob > 0))
		pCmdUI->Enable(TRUE);
	else pCmdUI->Enable(FALSE);

}

//---------------------------------------------------------
// On Convert routines
//---------------------------------------------------------
void CFJSSMDoc::OnConvertOption()
{
	CConvertDlg dlg;
	dlg.m_pFJSS = &m_fjss;
	dlg.DoModal();
}

void CFJSSMDoc::OnComputeConvexp()
{
	CConvExpDlg dlg;
	dlg.m_pFJSS = &m_fjss;
	dlg.DoModal();
}

void CFJSSMDoc::OnComputeCritexp()
{
	CCritExpDlg dlg;
	dlg.m_pFJSS = &m_fjss;
	dlg.DoModal();
}


//---------------------------------------------------------
// On Compute routines
//---------------------------------------------------------
void CFJSSMDoc::OnUpdateCompute(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_fjss.IsAbleToSolve());
}

//---------------------------------------------------------
// On Result routines
//---------------------------------------------------------
void CFJSSMDoc::OnUpdateResult(CCmdUI *pCmdUI)
{
	int n = m_fjss.IsAbleResult();
	if (m_fjss.IsAbleResult())
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CFJSSMDoc::OnUpdateResultSAGA(CCmdUI *pCmdUI)
{
	switch (pCmdUI->m_nID)
	{
	case ID_RESULT_SA:
		if ((m_fjss.IsAbleResult() == 1) || (m_fjss.IsAbleResult() == 3)) pCmdUI->Enable(TRUE);
		else pCmdUI->Enable(FALSE);
		pCmdUI->SetRadio(m_fjss.m_nSelectedResult == 0);
		break;
	case ID_RESULT_GA:
		if ((m_fjss.IsAbleResult() == 2) || (m_fjss.IsAbleResult() == 3)) pCmdUI->Enable(TRUE);
		else pCmdUI->Enable(FALSE);
		pCmdUI->SetRadio(m_fjss.m_nSelectedResult == 1);
		break;
	}	
}

// SA & GA Radiobutton
void CFJSSMDoc::OnResultSAGA(UINT nID)
{
	m_fjss.m_nSelectedResult = nID - ID_RESULT_SA;
}


void CFJSSMDoc::OnUpdateResultConvexp(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_fjss.IsAbleConvResult());
}

void CFJSSMDoc::OnUpdateResultCritIteration(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_fjss.IsAbleCritResult());
}

BOOL CFJSSMDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	// TODO:  Add your specialized creation code here
	UpdateAllViews(NULL,1);
	return TRUE;
}
