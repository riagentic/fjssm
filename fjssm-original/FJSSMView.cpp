// FJSSMView.cpp : implementation of the CFJSSMView class
//

#include "stdafx.h"
#include "FJSSM.h"
#include "FJSSFN.h"


#include "FJSSMDoc.h"
#include "FJSSMView.h"
#include "GADlg.h"
#include "SADlg.h"
#include "afxdao.h"
#include "math.h"

#include <stdio.h>
#include <sys/timeb.h>
#include <time.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


const int DATACOUNT=60;
// CFJSSMView

IMPLEMENT_DYNCREATE(CFJSSMView, CView)

BEGIN_MESSAGE_MAP(CFJSSMView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)

	// View Project
	ON_COMMAND(ID_VIEW_PROJECT, OnViewProject)
	ON_COMMAND(ID_VIEW_JOB, OnViewJobs)
	ON_COMMAND(ID_VIEW_MACHINE, OnViewMachines)
	ON_COMMAND(ID_VIEW_PROCESS, OnViewProcess)
	
	// Convert
	ON_COMMAND(ID_CONVERT_CRISPDD, OnConvertCrispdd)
	ON_COMMAND(ID_CONVERT_CRISPPT, OnConvertCrispt)
	ON_COMMAND(ID_CONVERT_FUZZYDD, OnConvertFuzzydd)
	ON_COMMAND(ID_CONVERT_FUZZYPT, OnConvertFuzzypt)

	ON_UPDATE_COMMAND_UI(ID_CONVERT_CRISPDD, OnUpdateConvertCrispdd)
	ON_UPDATE_COMMAND_UI(ID_CONVERT_CRISPPT, OnUpdateConvertCrispt)
	ON_UPDATE_COMMAND_UI(ID_CONVERT_FUZZYDD, OnUpdateConvertFuzzydd)
	ON_UPDATE_COMMAND_UI(ID_CONVERT_FUZZYPT, OnUpdateConvertFuzzypt)

	ON_COMMAND(ID_COMPUTE_SA, OnComputeSA)
	ON_COMMAND(ID_COMPUTE_GA, OnComputeGA)
	ON_COMMAND(ID_RESULT_GANT, OnResultGant)
	ON_COMMAND(ID_RESULT_RUN, OnResultHistory)
	ON_COMMAND(ID_RESULT_SOLVE, OnResultSchedule)

	ON_WM_SIZE()
	ON_WM_CHAR()
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_RESULT_CONVEXP, OnResultConvexp)

	ON_COMMAND(ID_RESULT_CRIT_ITERATION, OnResultCritIteration)
	ON_COMMAND(ID_RESULT_CRIT_RE, OnResultCritRe)
	ON_COMMAND(ID_RESULT_CRIT_ARE, OnResultCritAre)
	ON_COMMAND(ID_RESULT_CRIT_SOARE, OnResultCritSoare)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, OnUpdateEditSelectAll)
END_MESSAGE_MAP()

// CFJSSMView construction/destruction

CFJSSMView::CFJSSMView()
:m_nColumn(0)
{
	m_nAction = 0;

	m_pListCtrl = NULL;
	m_pGant = NULL;
	m_pGraph = NULL;
	m_pGrid = NULL;
	
}

CFJSSMView::~CFJSSMView()
{
	
	if (m_pListCtrl) delete m_pListCtrl;
	if (m_pGant) delete m_pGant;
	if (m_pGraph) delete m_pGraph;
	if (m_pGrid) delete m_pGrid;

}

BOOL CFJSSMView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}


void CFJSSMView::OnDraw(CDC* pDC)
{
	CFJSSMDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (!m_pListCtrl && !m_pGrid && !m_pGant && !m_pGraph)
	DrawBitmap(pDC,IDB_LOGO_DESKTOP);
    UpdateWindow();

	// TODO: add draw code for native data here
	//GetDocument()->m_pView = this;
}


void CFJSSMView::DrawBitmap(CDC* pDC,UINT Bitmap)
{
	int cx,cy;
	RECT rect;
	GetClientRect(&rect);
	cx = rect.right;
	cy = rect.bottom;

	BITMAP bm;
	CBitmap bmFJSS;
	bmFJSS.LoadBitmap(Bitmap);
	bmFJSS.GetBitmap(&bm);

	CDC dcPamet;
	dcPamet.CreateCompatibleDC(pDC);
	
	dcPamet.SelectObject(bmFJSS);
	pDC->BitBlt((cx / 2) - (bm.bmWidth / 2),
				 (cy / 2) - (bm.bmHeight / 2),
				 bm.bmWidth,bm.bmHeight,&dcPamet,0,0,SRCCOPY);
	//pDC->StretchBlt(cx/4,cy/4,cx-cx/4,cy-cy/4,&dcPamet,0,0,bm.bmWidth,bm.bmHeight,SRCCOPY);
}


void CFJSSMView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: You may populate your ListView with items by directly accessing
	//  its list control through a call to m_pListCtrl-> ).

	// ImageList / pozdejc
    //m_ilLarge.Create (IDR_MAINFRAME, 32, 1, RGB (255, 0, 255));
    //m_ilSmall.Create (IDR_MAINFRAME, 16, 1, RGB (255, 0, 255));
    //m_pListCtrl-> ().SetImageList (&m_ilLarge, LVSIL_NORMAL);
    //m_pListCtrl-> ().SetImageList (&m_ilSmall, LVSIL_SMALL);

}


// CFJSSMView printing

BOOL CFJSSMView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CFJSSMView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CFJSSMView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CFJSSMView diagnostics

#ifdef _DEBUG
void CFJSSMView::AssertValid() const
{
	CView::AssertValid();
}

void CFJSSMView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CFJSSMDoc* CFJSSMView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CFJSSMDoc)));
	return (CFJSSMDoc*)m_pDocument;
}
#endif //_DEBUG


int CFJSSMView::DrawListView(CStrList& lstsLegend,CStrArea& areasData)
{
	SetView(0);
	
	m_pListCtrl->ModifyStyle(0,LVS_REPORT);

	CString str;
	CUIntArray lstCharNr; 

	// VYTVORENI LEGENDY

	int nColumnToAdd = (int)lstsLegend.GetSize() - m_nColumn;
	// nacteni poctu znaku z pevne legendy

	if (nColumnToAdd < 0)
		for (int i=nColumnToAdd; i<0; i++) 
			m_pListCtrl->DeleteColumn(0);
	
	if (nColumnToAdd > 0)
		for (int i=0; i<nColumnToAdd; i++) 
			m_pListCtrl->InsertColumn (i, _T(""),LVCFMT_LEFT);		

	// vymazani predchozich dat
	m_pListCtrl->DeleteAllItems();

	m_nColumn += nColumnToAdd;

	// max pocty znaku z legendy
	for (int i=0; i<lstsLegend.GetSize(); i++)
		lstCharNr.Add(m_pListCtrl->GetStringWidth(lstsLegend[i]));

	// max pocty znaku z dat
	for (int i=0; i<areasData.GetSize(); i++)
		for (int j=0; j<areasData[i].GetSize(); j++)
		{
			if (j > lstCharNr.GetSize() - 1 ) lstCharNr.Add(0);
			lstCharNr[j] = max(lstCharNr[j],(unsigned)
				m_pListCtrl->GetStringWidth(areasData[i][j]));
		}


	// style report
	m_pListCtrl->ModifyStyle(LVS_TYPEMASK,LVS_REPORT);

	// nastaveni sirky sloupcu
	for (int i=0; i<lstCharNr.GetSize();i++)
		m_pListCtrl->SetColumnWidth(i,(4*lstCharNr[i]/2)-(lstCharNr[i]/5));

	for (int i=0; i<m_nColumn; i++) lstCharNr.Add(0);

    // DRAW Legendy
	for (int i=0; i<lstsLegend.GetSize(); i++)
	{
		TCHAR szBuffer[255];
		wsprintf(szBuffer,"%s",lstsLegend[i]);		
		LVCOLUMN lv;
		lv.pszText = szBuffer;
		m_pListCtrl->SetColumn(i,&lv);
	}
	
	// DRAW Data
	for (int i=0; i<areasData.GetSize(); i++)
	{
		m_pListCtrl->InsertItem(i,_T(""));
		for (int j=0; j<areasData[i].GetSize(); j++)
		{			
			m_pListCtrl->SetItemText(i,j,areasData[i][j]);
			lstCharNr[j] = max(lstCharNr[j],(unsigned)m_pListCtrl->GetStringWidth(areasData[i][j]));
		}
	}

	lstsLegend.RemoveAll();
	areasData.RemoveAll();

	Invalidate();
	return 0;
}


// CFJSSMView message handlers
//-----------------------------------------------
// ON View
//-----------------------------------------------
// PROJECT
void CFJSSMView::OnViewProject()
{
	CFJSS* pFJSS = &GetDocument()->m_fjss;
	pFJSS->GenerateProjectReport();
	
	//DrawListView(pFJSS->m_lstsLegend,pFJSS->m_areasData);
	SetView(3);
	DrawGrid(pFJSS->m_areasData,1,0);
	m_pGrid->AutoSize();	

	m_nAction = 1;
}
// JOBS
void CFJSSMView::OnViewJobs()
{
	CFJSS* pFJSS = &GetDocument()->m_fjss;
	pFJSS->GenerateJobReport();

//	DrawListView(pFJSS->m_lstsLegend,pFJSS->m_areasData);
	SetView(3);
	DrawGrid(pFJSS->m_areasData,1,0);
	m_pGrid->AutoSize();	

	
}

void CFJSSMView::OnViewMachines()
{
	CFJSS* pFJSS = &GetDocument()->m_fjss;
	pFJSS->GenerateMachineReport();

//	DrawListView(pFJSS->m_lstsLegend,pFJSS->m_areasData);
	SetView(3);
	DrawGrid(pFJSS->m_areasData,1,0);
	m_pGrid->AutoSize();	

}

// JOB PROCESS & MACHINE PROCESS
void CFJSSMView::OnViewProcess()
{
	CFJSS* pFJSS = &GetDocument()->m_fjss;
	pFJSS->GenerateProcessReport();

//	DrawListView(pFJSS->m_lstsLegend,pFJSS->m_areasData);
	SetView(3);
	DrawGrid(pFJSS->m_areasData,1,0);
	m_pGrid->AutoSize();	

}

// HRANY
void CFJSSMView::OnViewEdges()
{
}

// POCATECNI ROZVRH A CPM
void CFJSSMView::SetViewCPM()
{
}

void CFJSSMView::OnViewCpm()
{

}
//-----------------------------------------------
// ON Convert 
//-----------------------------------------------

void CFJSSMView::OnConvertCrispdd()
{
	CFJSS* pFJSS = &GetDocument()->m_fjss;
	int nRet = AskForConvert(0);
    if (nRet == IDYES) ShowConvertReport(pFJSS->ConvertToCrispDD());
	if (pFJSS->m_bIsModified) GetDocument()->SetModifiedFlag(TRUE);
}

void CFJSSMView::OnConvertCrispt()
{
	CFJSS* pFJSS = &GetDocument()->m_fjss;
	int nRet = AskForConvert(1);
    if (nRet == IDYES) ShowConvertReport(pFJSS->ConvertToCrispPT());
	if (pFJSS->m_bIsModified) GetDocument()->SetModifiedFlag(TRUE);
}

void CFJSSMView::OnConvertFuzzydd()
{
	CFJSS* pFJSS = &GetDocument()->m_fjss;
	int nRet = AskForConvert(2);
    if (nRet == IDYES) ShowConvertReport(pFJSS->ConvertToFuzzyDD());
	if (pFJSS->m_bIsModified) GetDocument()->SetModifiedFlag(TRUE);
}

void CFJSSMView::OnConvertFuzzypt()
{
	CFJSS* pFJSS = &GetDocument()->m_fjss;
	int nRet = AskForConvert(3);
    if (nRet == IDYES) ShowConvertReport(pFJSS->ConvertToFuzzyPT());	
	if (pFJSS->m_bIsModified) GetDocument()->SetModifiedFlag(TRUE);
}

int CFJSSMView::AskForConvert(int nType)
{
	CString str;
	str.Format(_T("Opravdu chcete provést konverzi projektu na typ \n\ns  "));
	switch (nType)
	{
	case 0:
		str.AppendFormat(_T("DETERMINISTICKÝMI TERMÍNY DOKONÈENÍ ?"));
		break;
	case 1:
		str.AppendFormat(_T("DETERMINISTICKÝMI DOBAMI TRVÁNÍ OPERACÍ ?"));
		break;
	case 2:
		str.AppendFormat(_T("FUZZY TERMÍNY DOKONÈENÍ ?"));
		break;
	case 3:
		str.AppendFormat(_T("FUZZY DOBAMI TRVÁNÍ OPERACÍ ?"));
		break;
	}

	return AfxMessageBox(str,MB_YESNO,MB_ICONQUESTION);

}

int CFJSSMView::ShowConvertReport(int nConvert)
{
	CFJSS* pFJSS = &GetDocument()->m_fjss;
	CString str[5] = {_T("Hotovo !!! \n\nAktuální typ projektu:   "),
  					  _T("Hotovo !!! \n\nAktuální typ projektu:   "),
  					  _T("Hotovo !!! \n\nAktuální typ projektu:   "),
  					  _T("Hotovo !!! \n\nAktuální typ projektu:   "),					
					  _T("Operace probìhla neúspìšnì, neznámá chyba !!!")};
	
	str[nConvert].AppendFormat(_T("%s\t"),pFJSS->GetStrType());
	AfxMessageBox(str[nConvert],MB_OK | MB_ICONINFORMATION);

	// Update Menu
	
	return 0;
}

//-----------------------------------------------
// ON Menu Update Convert
//-----------------------------------------------

void CFJSSMView::OnUpdateConvertCrispdd(CCmdUI *pCmdUI)
{
	CFJSS* pFJSS = &GetDocument()->m_fjss;
	if ((pFJSS->m_nType == 0) || (pFJSS->m_nType == 2))
		pCmdUI->Enable(FALSE);
	else 
		pCmdUI->Enable(TRUE);
}

void CFJSSMView::OnUpdateConvertCrispt(CCmdUI *pCmdUI)
{
	CFJSS* pFJSS = &GetDocument()->m_fjss;
	if ((pFJSS->m_nType == 0) || (pFJSS->m_nType == 1))
		pCmdUI->Enable(FALSE);
	else 
		pCmdUI->Enable(TRUE);

}

void CFJSSMView::OnUpdateConvertFuzzydd(CCmdUI *pCmdUI)
{
	CFJSS* pFJSS = &GetDocument()->m_fjss;
	if ((pFJSS->m_nType == 1) || (pFJSS->m_nType == 3))
		pCmdUI->Enable(FALSE);
	else 
		pCmdUI->Enable(TRUE);
}

void CFJSSMView::OnUpdateConvertFuzzypt(CCmdUI *pCmdUI)
{
	CFJSS* pFJSS = &GetDocument()->m_fjss;
	if ((pFJSS->m_nType == 2) || (pFJSS->m_nType == 3))
		pCmdUI->Enable(FALSE);
	else 
		pCmdUI->Enable(TRUE);
}

//-----------------------------------------------
// Simulated Annealing
//-----------------------------------------------

void CFJSSMView::OnComputeSA()
{

	CFJSS* pFJSS = &GetDocument()->m_fjss;
	CSADlg dlg;

	dlg.m_pFJSS = pFJSS;
	dlg.DoModal();
}

void CFJSSMView::OnComputeGA()
{

	CFJSS* pFJSS = &GetDocument()->m_fjss;
	CGADlg dlg;

	dlg.m_pFJSS = pFJSS;
	dlg.DoModal();

//	SetViewCPM();
//	DrawListView();
}

void CFJSSMView::OnTestTest()
{
	CFN fTest;
	CFN fTest2;
	CFN fTest3;
	CFN fTest4;
	CFN fTest5;
	CFN fTest6;
	CFN fTest7;

	fTest[0] = 2;
	fTest[1] = 3;
	fTest[2] = 4;
	fTest[3] = 10;

	fTest2[0] = 6;
	fTest2[1] = 7;
	fTest2[2] = 8;
	fTest2[3] = 9;

	fTest3[0] = 1;
	fTest3[1] = 10;
	fTest3[2] = 12;
	fTest3[3] = 13;

	fTest4[0] = 3;
	fTest4[1] = 5;
	fTest4[2] = 8;
	fTest4[3] = 11;

	fTest5[0] = 1;
	fTest5[1] = 2;
	fTest5[2] = 2;
	fTest5[3] = 3;

	fTest6[0] = 2;
	fTest6[1] = 3;
	fTest6[2] = 3;
	fTest6[3] = 4;


	fTest7[0] = 2;
	fTest7[1] = 4;
	fTest7[2] = 5;
	fTest7[3] = 6;

	for (int i=0; i<100; i++)
	{
		TRACE("x = %d, fx = %f\n",i,FNFx(fTest,i));
		
	}
	
	//Spillover(fTest,fTest2);

	//Spillover(fTest3,fTest4);

	double Result = FNSpillover(fTest5,fTest6);

	double fMean = fTest7.Mean();
	double fDevi = fTest7.Devi();

}

void CFJSSMView::OnResultGant()
{
	CFJSS* pFJSS = &GetDocument()->m_fjss;

	switch (pFJSS->m_nSelectedResult)
	{
		case 0: pFJSS->SolveScheduleForResult(pFJSS->m_areaSASchedule);break;
		case 1: pFJSS->SolveScheduleForResult(pFJSS->m_areaGASchedule);break;
	}

	SetView(2);		// GANT


}


int CFJSSMView::SetView(int nViewType)
{
	//	0. ListView
	//  1. Graph
	//  2. Gant
	//  3. Grid

	CFJSS* pFJSS = &GetDocument()->m_fjss;

	CRect rect;
	GetClientRect(rect);
	
	if (m_pListCtrl) { delete m_pListCtrl; m_pListCtrl = NULL; }
	if (m_pGant) {delete m_pGant; m_pGant = NULL;}
	if (m_pGraph) {delete m_pGraph; m_pGraph = NULL;}
	if (m_pGrid) {delete m_pGrid;m_pGrid = NULL;}

	switch (nViewType)
	{
		case 0:					// ListView, delaocate m_pGraph and Series

				if (!m_pListCtrl)
				{
					m_pListCtrl = new CListCtrl();//Styled();
					m_pListCtrl->Create(WS_CHILD|WS_VISIBLE|WS_BORDER|LVS_REPORT,
						rect,this,ID_LISTCTRL);
					m_nColumn = 0;
				}

				break;

		case 1:	
				// Graph
				if (!m_pGraph)
				{
					m_pGraph = new CMschart1();//CGraph();
					m_pGraph->Create(_T("Chart"),WS_TABSTOP | WS_VISIBLE,rect,this,ID_GRAPH);
				}

				break;

		case 2:					// Gant
				if (!m_pGant)
				{
					m_pGant = new CGant();
					m_pGant->SetData(pFJSS);
					m_pGant->Create(rect,this,ID_VIEW_GANT);
				}
				break;

		case 3:					// Graph
				if (!m_pGrid)
				{
					m_pGrid = new CGridCtrl;
					m_pGrid->Create(rect,this,ID_VIEW_GRID);
				}
				break;
		

	}

	return 0;
}



void CFJSSMView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	CRect rect;
	GetClientRect(rect);
	
	if (m_pListCtrl)	m_pListCtrl->MoveWindow(rect,TRUE);
	if (m_pGant)		m_pGant->MoveWindow(rect);
	if (m_pGraph)		m_pGraph->MoveWindow(rect);
	if (m_pGrid)		m_pGrid->MoveWindow(rect);

}

void CFJSSMView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default

	if (m_pGant)
		m_pGant->OnChar(nChar,nRepCnt,nFlags);


	CView::OnChar(nChar, nRepCnt, nFlags);
}

void CFJSSMView::OnUpdateEditCopy(CCmdUI *pCmdUI)
{
	if (m_pGraph || m_pGant || m_pGrid) pCmdUI->Enable(TRUE);
	else pCmdUI->Enable(FALSE);
}

void CFJSSMView::OnEditCopy()
{
	if (m_pGant) m_pGant->CopyToClipboard();
	if (m_pGraph) m_pGraph->EditCopy();
	if (m_pGrid) m_pGrid->OnEditCopy();
	
	
}


int CFJSSMView::DrawGrid(CStrArea& areasData, int nFixRow, int nFixCol)
{
	CFJSS* pFJSS = &GetDocument()->m_fjss;


	int nMaxColumn = 0;
	for (int i=0; i<areasData.GetSize();i++)
		nMaxColumn = max(nMaxColumn, areasData[i].GetSize());
	
	m_pGrid->SetRowCount(areasData.GetSize());
	m_pGrid->SetColumnCount(nMaxColumn);

	m_pGrid->SetFixedColumnCount(nFixCol);
	m_pGrid->SetFixedRowCount(nFixRow);

	for (int i=0; i<areasData.GetSize();i++)
		for (int j=0; j<areasData[i].GetSize();j++)
			m_pGrid->SetItemText(i,j,areasData[i][j]);

	areasData.RemoveAll();

	return 0;
	
}


BOOL CFJSSMView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: Add your specialized code here and/or call the base class
	return CView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

//-------------------------------------------------
// On Result events
//-------------------------------------------------
void CFJSSMView::OnResultSchedule()
{
	SetView(3);

	CFJSS* pFJSS = &GetDocument()->m_fjss;
	switch (pFJSS->m_nSelectedResult)
	{
	case 0: pFJSS->GenerateResultReport(pFJSS->m_areaSASchedule);break;
	case 1: pFJSS->GenerateResultReport(pFJSS->m_areaGASchedule);break;
	}

	DrawGrid(pFJSS->m_areasData,0,0);
	COLORREF crBkColor = RGB(255,255,180);
	// Legend bkcolor
	for (int i=0; i<6; i++)
		for (int j=0; j<m_pGrid->GetColumnCount();j++)
			m_pGrid->SetItemBkColour(i,j,crBkColor);

	m_pGrid->SetItemBkColour(m_pGrid->GetRowCount()-2,0,crBkColor);
	m_pGrid->SetItemBkColour(m_pGrid->GetRowCount()-2,1,crBkColor);
	m_pGrid->SetItemBkColour(m_pGrid->GetRowCount()-1,0,crBkColor);
	m_pGrid->SetItemBkColour(m_pGrid->GetRowCount()-1,1,crBkColor);

	m_pGrid->AutoSize();

}

void CFJSSMView::OnResultHistory()
{
	CFJSS* pFJSS = &GetDocument()->m_fjss;
	SetView(1);	// GRAPH


	// pripady
	// 1. SA
	// 2. GA
	// 3. GA + SA

	int nMaxAxis = 0;
	int nSeriesCount = pFJSS->m_areaAxis.GetSize();
	// nastaveni velikosti SaveArray
	for (int i=0; i<nSeriesCount;i++)
		nMaxAxis = max(nMaxAxis,pFJSS->m_areaAxis[i].GetSize());

	COleSafeArray saRet;
	//						MaxElement, Series*2 pro XY graf
	DWORD numElements[] = {nMaxAxis,nSeriesCount*2}; 
	// Create the safe-array...
	saRet.Create(VT_R8, 2, numElements);

	// meritko pro X, odstranim, az sezenu dokumentaci k mschart
	double fMaxY = 0;
	double fMinY = 0;
	double fMaxX = 0;
	for (int i=0;i<nSeriesCount;i++)
	{
		int nLast = pFJSS->m_areaAxis[i].GetSize()-1;
		fMaxY = max(fMaxY,pFJSS->m_areaAxis[i][0].m_fY);
		fMinY = min(fMinY,pFJSS->m_areaAxis[i][nLast].m_fY);
		fMaxX = max(fMaxX,pFJSS->m_areaAxis[i][nLast].m_fX);
	}

	fMaxY = fabs(fMaxY) + fabs(fMinY);
	double fXScale = fMaxY / fMaxX;

	// Graph data
	long index[2];
	for (int i=0; i<nMaxAxis;i++)
		for (int j=0; j<nSeriesCount;j++)
		{
			index[0] = i;
			double fDataX;
			double fDataY;
			int k;
			index[1] = 2*j;

			k = min(pFJSS->m_areaAxis[j].GetSize()-1,i);
       		fDataX = pFJSS->m_areaAxis[j][k].m_fX*fXScale;
			saRet.PutElement(index,&fDataX);

			index[1] = 2*j+1;
			k = min(pFJSS->m_areaAxis[j].GetSize()-1,i);
			fDataY = pFJSS->m_areaAxis[j][k].m_fY;
			saRet.PutElement(index,&fDataY);


		}
	// Return the safe-array encapsulated in a VARIANT...
	m_pGraph->put_ChartData(saRet.Detach());
	m_pGraph->put_chartType(CMschart1::VtChChartType2dXY);

	m_pGraph->put_TitleText(_T("Prubeh hledani optima ucelove funkce"));
	m_pGraph->put_ShowLegend(TRUE);
	m_pGraph->put_RowLabel(_T("Hodnota ucelove funkce"));
	CString str;
	str.Format(_T("cas vypoctu [s / %3.2f]"),fXScale);
	m_pGraph->put_ColumnLabel(str);

	
	m_pGraph->Refresh();

	for (int i=0; i<nSeriesCount;i++)
	{
		m_pGraph->put_Column((i*2)+1);
		m_pGraph->put_ColumnLabel(pFJSS->m_areaAxis[i].m_strName);

	}

}

void CFJSSMView::OnResultConvexp()
{
	CFJSS* pFJSS = &GetDocument()->m_fjss;
	SetView(3);		// GRID

	pFJSS->GenerateConvergenceReport();
	DrawGrid(pFJSS->m_areasData,0,0);
		
	m_pGrid->AutoSize();

	COLORREF crBkColor = RGB(255,255,180);
	for (int i=0; i<2; i++)
		for (int j=0; j<m_pGrid->GetColumnCount();j++)
			m_pGrid->SetItemBkColour(i,j,crBkColor);

	for (int i=0; i<m_pGrid->GetRowCount(); i++)
		for (int j=0; j<1;j++)
			m_pGrid->SetItemBkColour(i,j,crBkColor);

	m_pGrid->AutoSize();

}

//-------------------------------------------------
// On Criteria Result events
//-------------------------------------------------
void CFJSSMView::OnResultCritIteration()
{
	CFJSS* pFJSS = &GetDocument()->m_fjss;
	SetView(3);

	pFJSS->GenerateIterationReport();
	DrawGrid(pFJSS->m_areasData,0,0);

	COLORREF crBkColor = RGB(255,255,180);
	for (int i=0; i<1; i++)
		for (int j=0; j<m_pGrid->GetColumnCount();j++)
			m_pGrid->SetItemBkColour(i,j,crBkColor);

	for (int i=0; i<m_pGrid->GetRowCount(); i++)
		m_pGrid->SetItemBkColour(i,m_pGrid->GetColumnCount()-1,crBkColor);


	m_pGrid->AutoSize();

}

void CFJSSMView::OnResultCritRe()
{
	CFJSS* pFJSS = &GetDocument()->m_fjss;
	SetView(3);

	pFJSS->GenerateREReport();
	DrawGrid(pFJSS->m_areasData,0,0);

	COLORREF crBkColor = RGB(255,255,180);
	for (int i=0; i<1; i++)
		for (int j=0; j<m_pGrid->GetColumnCount();j++)
			m_pGrid->SetItemBkColour(i,j,crBkColor);

	for (int i=0; i<m_pGrid->GetRowCount(); i++)
		m_pGrid->SetItemBkColour(i,m_pGrid->GetColumnCount()-1,crBkColor);

//	for (int i=0; i<m_pGrid->GetRowCount(); i++)
		//m_pGrid->SetItemBkColour(i,0,crBkColor);

	m_pGrid->AutoSize();

}

void CFJSSMView::OnResultCritAre()
{
	CFJSS* pFJSS = &GetDocument()->m_fjss;
	SetView(3);

	pFJSS->GenerateAREReport();
	DrawGrid(pFJSS->m_areasData,0,0);

	COLORREF crBkColor = RGB(255,255,180);
	for (int i=0; i<1; i++)
		for (int j=0; j<m_pGrid->GetColumnCount();j++)
			m_pGrid->SetItemBkColour(i,j,crBkColor);

	for (int i=0; i<m_pGrid->GetRowCount(); i++)
		m_pGrid->SetItemBkColour(i,m_pGrid->GetColumnCount()-1,crBkColor);

	m_pGrid->AutoSize();

}

void CFJSSMView::OnResultCritSoare()
// Sum Of Average Relative Error
{
	CFJSS* pFJSS = &GetDocument()->m_fjss;
	SetView(1);	// GRAPH

	int nSeriesCount = pFJSS->m_areafAverageRE.GetSize();

	COleSafeArray saRet;
	DWORD numElements[] = {1,nSeriesCount}; //MaxElement, Series*2 pro XY graf
	saRet.Create(VT_R8, 2, numElements);

	// Graph data
	long index[2];
	for (int i=0; i<nSeriesCount; i++)
	{
		index[0] = 0;
		index[1] = i;

		double fSOARE=pFJSS->m_areafAverageRE[i][pFJSS->m_areafAverageRE[i].GetSize()-1];

		saRet.PutElement(index,&fSOARE);
	}
	// Return the safe-array encapsulated in a VARIANT...
	m_pGraph->put_ChartData(saRet.Detach());
	m_pGraph->put_chartType(CMschart1::VtChChartType2dBar);
	m_pGraph->put_TitleText(_T("Srovnani kvality pouzitych kriterii"));
	m_pGraph->put_RowLabel(_T("Jednotliva kriteria"));
	m_pGraph->put_ShowLegend(TRUE);
	m_pGraph->Refresh();

	
	for (int i=0; i<nSeriesCount;i++)
	{
		m_pGraph->put_Column(i+1);
		m_pGraph->put_ColumnLabel(pFJSS->GetStrCriterion(pFJSS->m_lstnAbleCriterion[i],3));
	}
	
}


void CFJSSMView::OnEditSelectAll()
{
	if (m_pGrid) m_pGrid->OnEditSelectAll();
}

void CFJSSMView::OnUpdateEditSelectAll(CCmdUI *pCmdUI)
{
	if (m_pGrid) 
		pCmdUI->Enable(TRUE);
	else pCmdUI->Enable(FALSE);
}


void CFJSSMView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	switch (lHint)
	{
		case 1:	// NEW, OPEN, IMPORT
			SetView(10);
			break;
		
	}
}
