/*
This file was created by Paul Barvinko (pbarvinko@yahoo.com).
This file is distributed "as is", e.g. there are no warranties 
and obligations and you could use it in your applications on your
own risk. 
Your comments and questions are welcome.
If using in your applications, please mention author in credits for your app.
*/
// PView.cpp : implementation file
//

#include "stdafx.h"
#include "PView.h"
#include "grview.h"
#include "compages.h"
#include "grres.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPointEdit dialog


CPointEdit::CPointEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CPointEdit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPointEdit)
	m_XValue = 0.0;
	m_YValue = 0.0;
	//}}AFX_DATA_INIT
}


void CPointEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPointEdit)
	DDX_Text(pDX, IDC_EDIT_X, m_XValue);
	DDX_Text(pDX, IDC_EDIT_Y, m_YValue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPointEdit, CDialog)
	//{{AFX_MSG_MAP(CPointEdit)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPointEdit message handlers

/////////////////////////////////////////////////////////////////////////////
// CPointList

CPointList::CPointList() : CListCtrlEx()
{
}

CPointList::~CPointList()
{
}


BEGIN_MESSAGE_MAP(CPointList, CListCtrlEx)
	//{{AFX_MSG_MAP(CPointList)
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPointList message handlers
int CPointList::GetSelectedItem()
{
	int ret = -1;
	for (int i=0; i<GetItemCount(); i++)
	{
		if (GetItemState(i, LVIS_SELECTED)!=0)
		{
						ret = i;
						break;
		};
	};
	return ret;
}

void CPointList::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CListCtrlEx::OnRButtonDown(nFlags, point);
	if (parentPtr == NULL) return;
	if (parentPtr->m_CurrentGraph == -1) return;
	int cursel = GetSelectedItem();
	ClientToScreen(&point);
	CMenu menu;
	if (cursel == -1)
	{
		menu.LoadMenu(IDR_PVIEW_RBMENU_EMPTY);
	} else
	{
		menu.LoadMenu(IDR_PVIEW_RBMENU);
	};
	CMenu* mn = menu.GetSubMenu(0);
/*
	if (cursel == -1)
	{//gray all items except ""
		mn->EnableMenuItem(ID_POINT_EDIT, MF_BYCOMMAND | MF_GRAYED);
		mn->EnableMenuItem(ID_POINT_DELETE, MF_BYCOMMAND | MF_GRAYED);
	};
*/
	mn->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, parentPtr);
}

void CPointList::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (parentPtr == NULL) return;
	if ((long)(nFlags & MK_CONTROL)!= 0)
	{
		parentPtr->parentView->DoDragDropData(GRAPH_COPY_TEXT);
		return;
	};
	CListCtrlEx::OnLButtonDown(nFlags, point);
}

//class CPViewPointCombo
void CPViewPointCombo::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (parentPtr == NULL) return;
	int index = lpDrawItemStruct->itemID;
	if (index == -1) return;
	index = GetItemData(index);
	CGraphView* grview = parentPtr->parentView;
	if (grview == NULL) return;
	if (index<0 || index>grview->GetGraphCount()) return;
	CGraphProp* grprop = grview->GetGraph(index);
	if (grprop == NULL) return;
	DrawComboItem(grprop->GetGraphColor(), (char*)grprop->GetTitle(), lpDrawItemStruct);	
}

/////////////////////////////////////////////////////////////////////////////
// CPointsView

//defines for hiding/restoring window
#define INIT_WINDOW_MACRO DWORD oldWindowStyle_Macro;

#define HIDE_WINDOW_MACRO \
{ \
	oldWindowStyle_Macro = (DWORD)(GetStyle() & WS_VISIBLE); \
	if (oldWindowStyle_Macro != 0) ModifyStyle(WS_VISIBLE, 0, 0); \
};

#define SHOW_WINDOW_MACRO \
	if (oldWindowStyle_Macro != 0) ModifyStyle(0, WS_VISIBLE, 0);

IMPLEMENT_DYNCREATE(CPointsView, CFormView)

CPointsView::CPointsView()
	: CFormView(CPointsView::IDD)
{
	//{{AFX_DATA_INIT(CPointsView)
	//}}AFX_DATA_INIT
	m_CurrentGraph = -1;
	m_maxColSize = -1;
	parentView = NULL;
}

CPointsView::~CPointsView()
{
}

void CPointsView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();

	parentView = GetMainGraphView();

	m_GraphCombo.CreatePointsCombo(this);
	m_PointList.CreatePointList(this);

	//insert columns for point view
	CRect r;
	m_PointList.GetClientRect(&r);
	CString s;
	s = "X";
	m_PointList.InsertColumn(0, s, LVCFMT_LEFT, (r.right-r.left)/2);
	s = "Y";
	m_PointList.InsertColumn(1, s, LVCFMT_LEFT, (r.right-r.left)/2);
//	m_PointList.SetColumnTypes(_gnLogColTypes, NUM_LOGCOLUMNS);
	m_PointList.SetHighlightType(HIGHLIGHT_ROW);
	//calc m_maxColSize
	CWnd* wnd = GetDlgItem(IDC_POINT_LIST);
	if (wnd != NULL)
	{
		CRect r;
		wnd->GetClientRect(&r);
		m_maxColSize = (r.right-r.left)+25;
	};
	SendMessage(CM_GRAPH_NOTIFICATION, GRAPH_PVIEW_UPDATE, 0);
}

void CPointsView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPointsView)
	DDX_Control(pDX, IDC_POINT_LIST, m_PointList);
	DDX_Control(pDX, IDC_GRAPH_COMBO, m_GraphCombo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPointsView, CFormView)
	//{{AFX_MSG_MAP(CPointsView)
	ON_WM_SIZE()
	ON_CBN_SELCHANGE(IDC_GRAPH_COMBO, OnSelchangeGraphCombo)
	ON_COMMAND(ID_POINT_DELETE, OnPointDelete)
	ON_COMMAND(ID_POINT_EDIT, OnPointEdit)
	ON_COMMAND(ID_POINT_INSERT, OnPointInsert)
	ON_COMMAND(ID_POINT_APPEND, OnPointAppend)
	ON_COMMAND(ID_PVIEW_HIDE, OnPviewHide)
	ON_COMMAND(ID_PVIEW_PROPERTIES, OnPviewProperties)
	ON_COMMAND(ID_PVIEW_REFRESH, OnPviewRefresh)
	ON_COMMAND(IDC_COPY_ASTEXT, OnCopyAstext)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPointsView diagnostics

#ifdef _DEBUG
void CPointsView::AssertValid() const
{
	CFormView::AssertValid();
}

void CPointsView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPointsView message handlers

void CPointsView::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);
	CWnd* wnd = GetDlgItem(IDC_POINT_LIST);
	if (wnd == NULL) return;
	CRect r, r1;
	GetClientRect(&r);
	r.bottom -= 5;
	r.right -= 5;
	ClientToScreen(&r);
	wnd->GetWindowRect(&r1);
	int shift = r.bottom-r1.bottom;
	if (r1.bottom-r1.top+shift<30) shift = 30;
		else shift = r1.bottom-r1.top+shift;
	wnd->SetWindowPos(NULL, 0, 0, r1.right-r1.left, shift, SWP_NOZORDER | SWP_NOMOVE);
//	int shift1 = r.right-r1.right;
//	if (r1.right-r1.left+shift1<80) shift1 = 80;
//		else shift1 = r1.right-r1.left+shift1;
//	wnd->SetWindowPos(NULL, 0, 0, shift1, shift, SWP_NOZORDER | SWP_NOMOVE);
/*
	//adjust horizontal size of the pane
	if (cx>r1.right-r1.left+20)
	{
		CWnd* parentWnd = GetOwner();
		if (parentWnd!=NULL && parentWnd->IsKindOf(RUNTIME_CLASS(CSplitterWnd)))
		{
			CSplitterWnd* splitter = (CSplitterWnd*)parentWnd;
			//find correct pane for this window
			for (int i=0; i<splitter->GetRowCount(); i++)
			{
				for(int j=0; j<splitter->GetColumnCount(); j++)
				{
					CWnd* pane = splitter->GetPane(i, j);
					if (pane->m_hWnd == this->m_hWnd)
					{
						//pane is found - resize it to new size
//						splitter->SetColumnInfo(j, r1.right-r1.left+20, 0);
						splitter->SetColumnInfo(j, 40, 0);
						splitter->RecalcLayout();
						break;
					};
				};
				if (j<splitter->GetColumnCount()) break;
			};
		};
	};
*/
}

int CPointsView::GetMaxColSize()
{
	if (m_maxColSize == -1)
	{
		CWnd* wnd = GetDlgItem(IDC_POINT_LIST);
		if (wnd != NULL)
		{
			CRect r;
			wnd->GetClientRect(&r);
			m_maxColSize = (r.right-r.left)+25;
		} else
		{
			m_maxColSize = 0;
		};
	};
	return m_maxColSize;
}

CGraphView* CPointsView::GetMainGraphView()
{
	CWnd* wnd = GetOwner();
	if (wnd == NULL) return NULL;
	if (!wnd->IsKindOf(RUNTIME_CLASS(CGraphSplitter))) return NULL;
	CGraphView* grview = ((CGraphSplitter*)wnd)->GetMainGraphView();
	return grview;
}

LRESULT CPointsView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	INIT_WINDOW_MACRO;
	if (message == CM_GRAPH_NOTIFICATION)
	{//this is graph's internal notification message
//		CGraphView* grview = GetMainGraphView();
		if (parentView != NULL)
		{
			switch (wParam)
			{
				case GRAPH_POINT_REMOVED:
					{
						SGraphChange* sgc = (SGraphChange*)lParam;
						if (!sgc->bRedraw) HIDE_WINDOW_MACRO;
						if (m_CurrentGraph == sgc->graphnum) DeletePoint(sgc);
						if (!sgc->bRedraw) SHOW_WINDOW_MACRO;
					}; break;
				case GRAPH_POINT_CHANGED:
					{
						SGraphChange* sgc = (SGraphChange*)lParam;
						if (!sgc->bRedraw) HIDE_WINDOW_MACRO;
						if (m_CurrentGraph == sgc->graphnum) ChangePoint(sgc);
						if (!sgc->bRedraw) SHOW_WINDOW_MACRO;
					}; break;
				case GRAPH_POINT_ADDED:
					{
						SGraphChange* sgc = (SGraphChange*)lParam;
						if (!sgc->bRedraw) HIDE_WINDOW_MACRO;
						if (m_CurrentGraph == sgc->graphnum) AddPoint(sgc);
						if (!sgc->bRedraw) SHOW_WINDOW_MACRO;
					}; break;
				case GRAPH_GRAPH_ADDED:
					{
						SGraphChange* sgc = (SGraphChange*)lParam;
						if (!sgc->bRedraw) HIDE_WINDOW_MACRO;
						InitGlobalVariables();
						if (m_CurrentGraph == -1) SetCurrentGraph(sgc->graphnum, sgc->bRedraw);
						if (!sgc->bRedraw) SHOW_WINDOW_MACRO;
					}; break;
				case GRAPH_PROP_CHANGED:
					{
						SGraphChange* sgc = (SGraphChange*)lParam;
						if (m_CurrentGraph == -1 || m_CurrentGraph!=sgc->graphnum) break;
						if (!sgc->bRedraw) HIDE_WINDOW_MACRO;
						UpdateGraph(sgc);
						if (!sgc->bRedraw) SHOW_WINDOW_MACRO;
					}; break;
				case GRAPH_GRAPH_REMOVED:
				{
					SGraphChange* sgc = (SGraphChange*)lParam;
					if (m_CurrentGraph == -1 || m_CurrentGraph!=sgc->graphnum) break;
					if (!sgc->bRedraw) HIDE_WINDOW_MACRO;
					RemoveGraph(sgc);
					if (!sgc->bRedraw) SHOW_WINDOW_MACRO;
				}; break;
				case GRAPH_GRAPH_CLEAR:
				{
					SGraphChange* sgc = (SGraphChange*)lParam;
					if (m_CurrentGraph == -1 || m_CurrentGraph!=sgc->graphnum) break;
					if (!sgc->bRedraw) HIDE_WINDOW_MACRO;
					m_PointList.DeleteAllItems();
					if (!sgc->bRedraw) SHOW_WINDOW_MACRO;
				}; break;
				case GRAPH_GRAPHIX_AXISPROPS:
				{
					SAxisPropsChange* sapc = (SAxisPropsChange*)lParam;
					if (!sapc->bRedraw) HIDE_WINDOW_MACRO;
					InitListView();
					if (!sapc->bRedraw) SHOW_WINDOW_MACRO;
				}; break;
				case GRAPH_PVIEW_UPDATE:
				{//update view with info from main view and redraw it
						if (InitGlobalVariables()!=-1)
						{//set new focus
							SetCurrentGraph(m_CurrentGraph, TRUE);
						} else
						{
							SetCurrentGraph(0, TRUE);
						};
				}; break;
				default:
					{
						return CFormView::WindowProc(message, wParam, lParam);
					};
			};
		};
	};
	return CFormView::WindowProc(message, wParam, lParam);
}

void CPointsView::SetCurrentGraph(int grnum, BOOL bRedraw)
{
//	CGraphView* grview = GetMainGraphView();
	if (parentView == NULL) return;
	if (grnum<0 || grnum>parentView->GetGraphCount()) return;
	m_CurrentGraph = grnum;
	//update all controls
//	CGraphProp* grprop = parentView->GetGraph(grnum);
		//graph combo box
	m_GraphCombo.SetCurSel(m_CurrentGraph);
	//points
	InitListView();
}

void CPointsView::InitListView()
{
	INIT_WINDOW_MACRO;

	HIDE_WINDOW_MACRO;
	m_PointList.DeleteAllItems();
	if (m_CurrentGraph != -1 && parentView != NULL) 
	{
		CGraphProp* grprop = parentView->GetGraph(m_CurrentGraph);
		if (grprop != NULL) 
		{
			//point list
			SSinglePoint ssp;
			CString str;
			for (int i=0; i<grprop->GetSize(); i++)
			{
				grprop->GetPoint(i, &ssp);
				parentView->FormatAxisOutput(ssp.x, TRUE, 1, str);
				m_PointList.InsertItem(i, str);
				//add second column
				parentView->FormatAxisOutput(ssp.y, FALSE, 1, str);
				m_PointList.SetItem(i, 1, LVIF_TEXT, str, 0, 0, 0, NULL);
			};
		};
	};
	SHOW_WINDOW_MACRO;
	RedrawWindow();
}

int CPointsView::InitGlobalVariables()
{
	int ind = m_CurrentGraph;
	if (parentView == NULL) return -1;
	int index, k;
	k = -1;
	//update all controls
		//graph combo box
	if (::IsWindow(m_GraphCombo.m_hWnd))
	{
		m_GraphCombo.ResetContent();
		CGraphProp* grprop = parentView->GetFirstGraph(&index);
		while (grprop!=NULL)
		{
			int i = m_GraphCombo.AddString("");
			m_GraphCombo.SetItemData(i, index);
			if (index == ind) k = i;
			grprop = parentView->GetNextGraph(&index);
		};
		//make active previous choice
		if (k!=-1) m_GraphCombo.SetCurSel(k);
	};
	return k;
}

void CPointsView::OnSelchangeGraphCombo() 
{
	int cursel = m_GraphCombo.GetCurSel();
	if (cursel == -1) return;
	int index = m_GraphCombo.GetItemData(cursel);
	if (index!=m_CurrentGraph) SetCurrentGraph(index, TRUE);
}

void CPointsView::AddPoint(SGraphChange* sgc)
{
	if (parentView == NULL) return;
	if (m_CurrentGraph == -1 || m_CurrentGraph!=sgc->graphnum) return;
	CGraphProp* grprop = parentView->GetGraph(sgc->graphnum);
	if (grprop == NULL) return;
	SSinglePoint ssp;
	grprop->GetPoint(sgc->index, &ssp);
		CString str;
		parentView->FormatAxisOutput(ssp.x, TRUE, 1, str);
		m_PointList.InsertItem(sgc->index, str);
		//add second column
		parentView->FormatAxisOutput(ssp.y, FALSE, 1, str);
		m_PointList.SetItem(sgc->index, 1, LVIF_TEXT, str, 0, 0, 0, NULL);
}

void CPointsView::UpdateGraph(SGraphChange* sgc)
{
	if (parentView == NULL) return;
//	CGraphProp* grprop = parentView->GetGraph(sgc->graphnum);
	//redraw combobox
	m_GraphCombo.RedrawWindow();
}

void CPointsView::RemoveGraph(SGraphChange* sgc)
{
	if (m_CurrentGraph == -1 || m_CurrentGraph!=sgc->graphnum) return;
	if (parentView == NULL) return;
	InitGlobalVariables();
	if (parentView->GetGraphCount() == 0)
	{//that was last graph
		m_CurrentGraph = -1;
		InitListView();
	} else
	{//set next graph as active
		int index;
		CGraphProp* grprop = parentView->GetFirstGraph(&index);
		SetCurrentGraph(index, sgc->bRedraw);
	};
}

void CPointsView::ChangePoint(SGraphChange* sgc)
{
	if (m_CurrentGraph == -1) return;
	if (parentView == NULL) return;
	if (m_CurrentGraph != sgc->graphnum) return;
	CGraphProp* grprop = parentView->GetGraph(m_CurrentGraph);
	if (grprop == NULL) return;
	SSinglePoint ssp;
	grprop->GetPoint(sgc->index_ex, &ssp);
	CString str, str1;
	parentView->FormatAxisOutput(ssp.x, TRUE, 1, str);
	parentView->FormatAxisOutput(ssp.y, FALSE, 1, str1);
	//delete old point
	m_PointList.DeleteItem(sgc->index);
	//insert new one
	m_PointList.InsertItem(sgc->index_ex, str);
	//add second column
	m_PointList.SetItem(sgc->index_ex, 1, LVIF_TEXT, str1, 0, 0, 0, NULL);
}

void CPointsView::DeletePoint(SGraphChange* sgc)
{
	if (m_CurrentGraph == -1) return;
	if (parentView == NULL) return;
	if (m_CurrentGraph != sgc->graphnum) return;
	m_PointList.DeleteItem(sgc->index);
}

//these commands came from list control
void CPointsView::OnPointDelete() 
{
	if (m_CurrentGraph == -1) return;
	if (parentView == NULL) return;
	int index = m_PointList.GetSelectedItem();
	if (index == -1) return;
	if (AfxMessageBox("Delete selected point?", MB_YESNO | MB_DEFBUTTON2) != IDYES) return;
	parentView->RemovePoint(m_CurrentGraph, index, TRUE);
}

void CPointsView::OnPointEdit() 
{
	if (m_CurrentGraph == -1) return;
	if (parentView == NULL) return;
	CGraphProp* grprop = parentView->GetGraph(m_CurrentGraph);
	if (grprop == NULL) return;
	SSinglePoint ssp;
	int index = m_PointList.GetSelectedItem();
	if (index == -1) return;
	grprop->GetPoint(index, &ssp);

	CString str;
	CPointEdit dlg;
	parentView->FormatAxisOutput(ssp.x, TRUE, 0, str);
	dlg.m_XValue = atof(str);

	parentView->FormatAxisOutput(ssp.y, FALSE, 0, str);
	dlg.m_YValue = atof(str);

	if (dlg.DoModal() == IDOK)
	{
		parentView->EditPoint(m_CurrentGraph, index, dlg.m_XValue, dlg.m_YValue, TRUE);
	};
}

void CPointsView::OnPointAdd(BOOL bInsert)
{
	if (m_CurrentGraph == -1) return;
	if (parentView == NULL) return;
	CGraphProp* grprop = parentView->GetGraph(m_CurrentGraph);
	if (grprop == NULL) return;
	SSinglePoint ssp;
	int index = m_PointList.GetSelectedItem();
	int ind;
	if (index == -1) ind = grprop->GetSize();
		else
		{
			if (bInsert) ind = index;
				else ind = index+1;
		};
	CPointEdit dlg;
	dlg.m_XValue = 0;
	dlg.m_YValue = 0;
	if (dlg.DoModal() == IDOK)
	{
		parentView->AddPoint(m_CurrentGraph, dlg.m_XValue, dlg.m_YValue, TRUE, ind);
	};
}

void CPointsView::OnPointInsert() 
{
	OnPointAdd(TRUE);
}

void CPointsView::OnPointAppend() 
{
	OnPointAdd(FALSE);
}

void CPointsView::OnPviewHide() 
{
	if (parentView == NULL) return;
	parentView->ShowPointView(FALSE);
}

void CPointsView::OnPviewProperties() 
{
//	CGraphView* grview = GetMainGraphView();
	if (parentView == NULL) return;
	CPropertySheet psheet("Points window properties");
	//pages
	CGraphPage grpage(parentView);
	CAxisPage axispage(parentView);
	psheet.AddPage(&grpage);
	psheet.AddPage(&axispage);
	if (psheet.DoModal() == IDOK)
	{
		parentView->RedrawWindow();
	};
}

void CPointsView::OnPviewRefresh() 
{
	SendMessage(CM_GRAPH_NOTIFICATION, GRAPH_PVIEW_UPDATE, 0);
}


void CPointsView::OnCopyAstext() 
{
	if (parentView == NULL) return;
	parentView->DoCopyData(GRAPH_COPY_TEXT);
}
