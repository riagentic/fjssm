/*
This file was created by Paul Barvinko (pbarvinko@yahoo.com).
This file is distributed "as is", e.g. there are no warranties 
and obligations and you could use it in your applications on your
own risk. 
Your comments and questions are welcome.
If using in your applications, please mention author in credits for your app.
*/
#include "stdafx.h"
#include "grview.h"
#include "winioctl.h"
#include "math.h"
#include "pview.h"
#include "afxole.h"

#include "grres.h"

/////////////////////////////////////////////////////////////////////////////
//class CPointsCollection
CGraphProp::CGraphProp(COLORREF newColor, char* newTitle, BOOL VisibleFlag, 
	BOOL _bResources, int _index/* = -1*/) 
	: CPointsCollection()
{
	m_grcolor = newColor;
	m_grTitle = newTitle;
	m_bIsVisible = VisibleFlag;
	m_index = _index;
	bPrivateResources = _bResources;
	if (bPrivateResources)
	{
		pen = new CPen(PS_SOLID, 1, m_grcolor);
		brush = new CBrush(m_grcolor);
	} else
	{
		pen = NULL;
		brush = NULL;
	};
}

CGraphProp::~CGraphProp()
{
	if (pen!=NULL) delete pen;
	if (brush!=NULL) delete brush;
}

CGraphProp::CGraphProp(CGraphProp* grprop, BOOL bCopyPoints /*= TRUE*/)
{
	pen = NULL;
	brush = NULL;
	if (bCopyPoints) CPointsCollection(grprop);
		else CPointsCollection();
	SetGraphProps(grprop);
}

void CGraphProp::SetGraphProps(CGraphProp* grprop)
{
	bPrivateResources = grprop->bPrivateResources;
	SetGraphColor(grprop->m_grcolor);
	m_grTitle = grprop->m_grTitle;
	m_bIsVisible = grprop->m_bIsVisible;
	m_index = grprop->m_index;
}

COLORREF CGraphProp::GetGraphColor()
{
	return m_grcolor;
}

void CGraphProp::SetGraphColor(COLORREF newColor)
{
	m_grcolor = newColor;
	//recreate pen and brush
	if (AreResourcesPrivate())
	{
		if (pen!=NULL) delete pen;
		if (brush!=NULL) delete brush;
		pen = new CPen(PS_SOLID, 1, m_grcolor);
		brush = new CBrush(m_grcolor);
	};
}

const char* CGraphProp::GetTitle()
{
	return (LPCTSTR)m_grTitle;
}

void CGraphProp::SetTitle(char* newTitle)
{
	m_grTitle = newTitle;
}

BOOL CGraphProp::IsVisible()
{
	return m_bIsVisible;
}

void CGraphProp::SetVisible(BOOL bVisibleFlag)
{
	m_bIsVisible = bVisibleFlag;
}

//CAxisProps**************
CAxisProps::CAxisProps()
{
	formatStrings[0] = formatStrings[1] = formatStrings[2] = NULL;
	m_Title = ""; 
	m_UOM = ""; 
	m_Precision = 3;
	CalculateFormatStrings();
}

CAxisProps::~CAxisProps()
{
	for (int i=0; i<3; i++) 
	{
		if (formatStrings[i]!=NULL) delete formatStrings[i];
		formatStrings[i] = NULL;
	};
}

void CAxisProps::GetFormattedOutput(double value, int formatLevel, CString* str)
{
	if (formatLevel<0 || formatLevel>2) 
	{
		*str = "";
	} else
	{
		str->Format(formatStrings[formatLevel], value);
	};
}

void CAxisProps::GetFullTitle(CString* str)
{
	*str = m_Title + ", " + m_UOM;
}

void CAxisProps::SetNewTitle(char* title)
{
	m_Title = title;
	CalculateFormatStrings();
}

void CAxisProps::SetNewUOM(char* newUOM)
{
	m_UOM = newUOM;
	CalculateFormatStrings();
}

void CAxisProps::SetNewPrecision(int newPrecision)
{
	m_Precision = newPrecision;
	CalculateFormatStrings();
}

void CAxisProps::CalculateFormatStrings()
{
	for (int i=0; i<3; i++) 
	{
		if (formatStrings[i]!=NULL) delete formatStrings[i];
		formatStrings[i] = NULL;
	};
	CString str;

	str.Format("%%0.%df", m_Precision);
	formatStrings[0] = new char[str.GetLength()+1];
	memcpy(formatStrings[0], (LPCTSTR)str, str.GetLength()+1);

	str.Format("%%0.%df %s", m_Precision, m_UOM);
	formatStrings[1] = new char[str.GetLength()+1];
	memcpy(formatStrings[1], (LPCTSTR)str, str.GetLength()+1);

	str.Format("%s: %%0.%df %s", m_Title, m_Precision, m_UOM);
	formatStrings[2] = new char[str.GetLength()+1];
	memcpy(formatStrings[2], (LPCTSTR)str, str.GetLength()+1);
}

/////////////////////////////////////////////////////////////////////////////
// CGraphSplitter

IMPLEMENT_DYNCREATE(CGraphSplitter, CSplitterWnd)

CGraphSplitter::CGraphSplitter()
{
	m_MaxSecondColSize = 200;
}

CGraphSplitter::~CGraphSplitter()
{
}


BEGIN_MESSAGE_MAP(CGraphSplitter, CSplitterWnd)
	//{{AFX_MSG_MAP(CGraphSplitter)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGraphSplitter message handlers

void CGraphSplitter::TrackColumnSize(int x, int col)
{
	CRect r;
	GetClientRect(&r);
	if (col == 0 && x<r.right-r.left-m_MaxSecondColSize) x = r.right-r.left-m_MaxSecondColSize;
	CSplitterWnd::TrackColumnSize(x, col);
}

void CGraphSplitter::OnSize(UINT nType, int cx, int cy) 
{
	CSplitterWnd::OnSize(nType, cx, cy);
	//check if panes are already created
	if (GetPane(0, 1))
	{
		int cursize, minsize;
		GetColumnInfo(0, cursize, minsize);
		CRect r;
		GetClientRect(&r);
		
		if (cursize<r.right-r.left-m_MaxSecondColSize)
		{
			TrackColumnSize(0, 0);
			RecalcLayout();
		};
	};
}

void CGraphSplitter::SetMaxSecondColSize(int newMax, BOOL bRedraw /* = TRUE*/)
{
	m_MaxSecondColSize = newMax;
	if (bRedraw)
	{
		TrackColumnSize(0, 0);
		RecalcLayout();
	};
}

CGraphView* CGraphSplitter::GetMainGraphView()
{
	CWnd* wnd = GetOwner();
	if (wnd == NULL) return NULL;
	if (wnd->IsKindOf(RUNTIME_CLASS(CGraphView)))
	{
		return (CGraphView*)wnd;
	};
	return NULL;
}

BOOL CGraphSplitter::OnEraseBkgnd(CDC* pDC) 
{
	return FALSE;
//	return CSplitterWnd::OnEraseBkgnd(pDC);
}

// CGraphView

IMPLEMENT_DYNCREATE(CGraphView, CScrollView)

CGraphView::CGraphView()
{
	OleInitialize(NULL);
	bSplitterCreated = FALSE;
	grafix = NULL;
	pointsview = NULL;
	m_graphEnumCount = -1;
	m_graphCount = 0;
	for (int i=0; i<MAX_GRAPHS; i++) m_graphs[i] = NULL;
	bShowPointView = TRUE;
}

CGraphView::~CGraphView()
{
	for (int i=0; i<MAX_GRAPHS; i++) 
	{
		if (m_graphs[i] != NULL) 
		{
			delete m_graphs[i];
			m_graphs[i] = NULL;
		};
	};
	OleUninitialize();
}


BEGIN_MESSAGE_MAP(CGraphView, CScrollView)
	//{{AFX_MSG_MAP(CGraphView)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGraphView drawing

BOOL CGraphView::Create( LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	BOOL b = CScrollView::Create( lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);

	m_splitter.CreateStatic(this, 1, 2, WS_CHILD | WS_VISIBLE);
	m_splitter.SetRowInfo(0, 100, 0);
	m_splitter.SetColumnInfo(0, 100, 0);
	m_splitter.SetColumnInfo(1, 100, 0);
	m_splitter.CreateView(0, 0, RUNTIME_CLASS(CGraphContainer), CSize(200, 200), pContext);
	m_splitter.CreateView(0, 1, RUNTIME_CLASS(CPointsView), CSize(200, 200), pContext);
	pointsview = (CPointsView*)m_splitter.GetPane(0, 1);
	grafix = (CGraphContainer*)m_splitter.GetPane(0, 0);
	//set minimum size for CPointsView
	m_splitter.SetScrollStyle(0);
	m_splitter.RecalcLayout();
	bSplitterCreated = TRUE;

	//no scrolling at all - at least for now
	CSize sizeTotal;
	sizeTotal.cx = sizeTotal.cy = 0;
	SetScrollSizes(MM_TEXT, sizeTotal);

	return b;
}

void CGraphView::ShowPointView(BOOL bShow)
{
	if (bShow)
	{
		if (pointsview!=NULL) 
		{
			m_splitter.SetMaxSecondColSize(pointsview->GetMaxColSize());
			bShowPointView = TRUE;
			UpdatePointView();
		};
	} else
	{
		m_splitter.SetMaxSecondColSize(0);
		bShowPointView = FALSE;
	};
}

BOOL CGraphView::GetPointViewState()
{
	return bShowPointView;
}

void CGraphView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CGraphView diagnostics

#ifdef _DEBUG
void CGraphView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CGraphView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGraphView message handlers

void CGraphView::OnSize(UINT nType, int cx, int cy) 
{
	if (bSplitterCreated && ::IsWindow(m_splitter.m_hWnd)) m_splitter.MoveWindow(0, 0, cx, cy, FALSE);
	CScrollView::OnSize(nType, cx, cy);
}

//********************* graphs and points operations ******************
int CGraphView::AddGraph(COLORREF newColor/* = 0*/, char* title/* = ""*/, BOOL bInvalidate/* = TRUE*/)
{
	int index = -1;
	//find first non-NULL element in m_graphCount array
	for (int i=0; i<MAX_GRAPHS; i++)
	{
		if (m_graphs[i]==NULL)
		{
			index = i;
			break;
		};
	};
	if (index == -1) return -1;
	m_graphs[index] = new CGraphProp(newColor, title);
	m_graphCount+=1;
	SGraphChange sgc;
	sgc.graphnum = index;
	sgc.bRedraw = bInvalidate;
	if (pointsview!=NULL) pointsview->SendMessage(CM_GRAPH_NOTIFICATION, GRAPH_GRAPH_ADDED, (DWORD)&sgc);
	if (grafix!=NULL) grafix->SendMessage(CM_GRAPH_NOTIFICATION, GRAPH_GRAPH_ADDED, (DWORD)&sgc);
//	if (bInvalidate) Invalidate(TRUE);
	return index;
}

int CGraphView::RemoveGraph(int index, BOOL bInvalidate/* = TRUE*/)
{
	if (index<0 || index>MAX_GRAPHS) return -1;
	if (m_graphs[index] == NULL) return -1;
	delete m_graphs[index];
	m_graphs[index] = NULL;
	m_graphCount-=1;
	SGraphChange sgc;
	sgc.graphnum = index;
	sgc.bRedraw = bInvalidate;
	if (pointsview!=NULL) pointsview->SendMessage(CM_GRAPH_NOTIFICATION, GRAPH_GRAPH_REMOVED, (DWORD)&sgc);
	if (grafix!=NULL) grafix->SendMessage(CM_GRAPH_NOTIFICATION, GRAPH_GRAPH_REMOVED, (DWORD)&sgc);
//	if (bInvalidate) Invalidate(TRUE);
	return index;
}

int CGraphView::UpdateGraph(int index, BOOL bInvalidate /*= TRUE*/)
{
	SGraphChange sgc;
	sgc.graphnum = index;
	sgc.bRedraw = bInvalidate;
	if (pointsview!=NULL && bShowPointView) pointsview->SendMessage(CM_GRAPH_NOTIFICATION, GRAPH_PROP_CHANGED, (DWORD)&sgc);
	if (grafix!=NULL) grafix->SendMessage(CM_GRAPH_NOTIFICATION, GRAPH_PROP_CHANGED, (DWORD)&sgc);
	return -1;
}

CGraphProp* CGraphView::GetGraph(int index)
{
	if (index<0 || index>MAX_GRAPHS) return NULL;
	return m_graphs[index];
}

int CGraphView::GetGraphCount()
{
	return m_graphCount;
}

CGraphProp* CGraphView::GetFirstGraph(int* index)
{
	m_graphEnumCount = -1;
	for (int i=0; i<MAX_GRAPHS; i++)
	{
		if (m_graphs[i]!=NULL)
		{
			m_graphEnumCount = i;
			break;
		};
	};
	if (m_graphEnumCount == -1) return NULL;
	*index = m_graphEnumCount;
	return m_graphs[m_graphEnumCount];
}

CGraphProp* CGraphView::GetNextGraph(int* index)
{
	if (m_graphEnumCount == -1) return NULL;
	int oldValue = m_graphEnumCount;
	for (int i=m_graphEnumCount+1; i<MAX_GRAPHS; i++)
	{
		if (m_graphs[i]!=NULL)
		{
			m_graphEnumCount = i;
			break;
		};
	};
	if (m_graphEnumCount == oldValue) 
	{
		m_graphEnumCount = -1;
		return NULL;
	};
	*index = m_graphEnumCount;
	return m_graphs[m_graphEnumCount];
}

int CGraphView::AddPoint(int graphnum, double x, double y, BOOL bRedraw, BOOL bUpdatePView, int index/* = -1*/)
{
	int ind, res;
	CGraphProp* graph = GetGraph(graphnum);
	if (graph == NULL) return -1;
	if (index == -1) ind = graph->AddPoint(x, y, TRUE, &res);
		else ind = graph->InsertPoint(index, x, y, TRUE);

	SGraphChange sgc;
	sgc.graphnum = graphnum;
	sgc.index = ind;
	sgc.bRedraw = bRedraw;
	if (bUpdatePView)
		if (pointsview!=NULL && bShowPointView) pointsview->SendMessage(CM_GRAPH_NOTIFICATION, GRAPH_POINT_ADDED, (DWORD)&sgc);
	if (grafix!=NULL) grafix->SendMessage(CM_GRAPH_NOTIFICATION, GRAPH_POINT_ADDED, (DWORD)&sgc);
	
	return index;
}

int CGraphView::EditPoint(int graphnum, int index, double x, double y, BOOL bRedraw)
{
	CGraphProp* graph = GetGraph(graphnum);
	if (graph == NULL) return -1;
	int res = graph->EditPoint(index, x, y, TRUE);
	SGraphChange sgc;
	sgc.graphnum = graphnum;
	sgc.index = index;
	sgc.index_ex = res;
	sgc.bRedraw = bRedraw;
	if (pointsview!=NULL && bShowPointView) pointsview->SendMessage(CM_GRAPH_NOTIFICATION, GRAPH_POINT_CHANGED, (DWORD)&sgc);
	if (grafix!=NULL) grafix->SendMessage(CM_GRAPH_NOTIFICATION, GRAPH_POINT_CHANGED, (DWORD)&sgc);
	return res;
}

int CGraphView::RemovePoint(int graphnum, int index, BOOL bRedraw)
{
	CGraphProp* graph = GetGraph(graphnum);
	if (graph == NULL) return -1;
	graph->RemovePoint(index, TRUE);
	SGraphChange sgc;
	sgc.graphnum = graphnum;
	sgc.index = index;
	sgc.bRedraw = bRedraw;
	if (pointsview!=NULL && bShowPointView) pointsview->SendMessage(CM_GRAPH_NOTIFICATION, GRAPH_POINT_REMOVED, (DWORD)&sgc);
	if (grafix!=NULL) grafix->SendMessage(CM_GRAPH_NOTIFICATION, GRAPH_POINT_REMOVED, (DWORD)&sgc);
	return index;
}

void CGraphView::ClearGraph(int graphnum, BOOL bRedraw, BOOL bUpdatePView)
{
	CGraphProp* graph;
	SGraphChange sgc;
	if (graphnum != -1)
	{
		graph = GetGraph(graphnum);
		if (graph == NULL) return;
		sgc.graphnum = graphnum;
		sgc.bRedraw = bRedraw;
		graph->RemoveAll();
		if (bUpdatePView) 
		{
		   if (pointsview!=NULL && bShowPointView) pointsview->SendMessage(CM_GRAPH_NOTIFICATION, GRAPH_GRAPH_CLEAR, (DWORD)&sgc);
		};
		if (grafix!=NULL) grafix->SendMessage(CM_GRAPH_NOTIFICATION, GRAPH_GRAPH_CLEAR, (DWORD)&sgc);
	} else
	{
		int index;
		graph = GetFirstGraph(&index);
		while (graph!=NULL)
		{
			sgc.graphnum = index;
			sgc.bRedraw = FALSE;
			graph->RemoveAll();
			if (bUpdatePView)
			{
			    if (pointsview!=NULL && bShowPointView) pointsview->SendMessage(CM_GRAPH_NOTIFICATION, GRAPH_GRAPH_CLEAR, (DWORD)&sgc);
			};
			graph = GetNextGraph(&index);
		};
		sgc.graphnum = index;
		sgc.bRedraw = bRedraw;
		if (grafix != NULL) grafix->SendMessage(CM_GRAPH_NOTIFICATION, GRAPH_GRAPH_CLEAR, (DWORD)&sgc);
		if (bRedraw) 
		{
			if (bUpdatePView) 
			{
			    if (pointsview!=NULL) pointsview->RedrawWindow();
			};
		};
	};
}

void CGraphView::SetGraphWorldCoords(double x1, double x2, double y1, double y2, BOOL bRedraw)
{
	if (grafix!=NULL)
	{
		SGraphixProps sgp;
		sgp.x1 = x1; sgp.x2 = x2;
		sgp.y1 = y1; sgp.y2 = y2;
		sgp.flags = GetGraphFlags();
		sgp.bRedraw = bRedraw;
		grafix->SendMessage(CM_GRAPH_NOTIFICATION, GRAPH_GRAPHIX_PROP, (DWORD)&sgp);
	};
}

void CGraphView::SetGraphFlags(DWORD new_flags, BOOL bRedraw)
{
	if (grafix != NULL)
	{
		SGraphixProps sgp;
		GetGraphWorldCoords(&sgp.x1, &sgp.x2, &sgp.y1, &sgp.y2);
		sgp.flags = new_flags;
		sgp.bRedraw = bRedraw;
		grafix->SendMessage(CM_GRAPH_NOTIFICATION, GRAPH_GRAPHIX_PROP, (DWORD)&sgp);
	};
}

DWORD CGraphView::GetGraphFlags()
{
	if (grafix!=NULL)
	{
		return grafix->grWnd->GetGraphixFlags();
	};
	return 0;
}

void CGraphView::GetGraphWorldCoords(double* x1, double* x2, double* y1, double* y2)
{
	grafix->GetWorldCoords(x1, x2, y1, y2);
}

void CGraphView::SetAxisProps(char* _title, char* _UOM, int _precision, BOOL bXAxis, BOOL bRedraw)
{
	CAxisProps* props;
	if (bXAxis) props = &x_axis;
		else props = &y_axis;
	props->SetNewTitle(_title);
	props->SetNewUOM(_UOM);
	props->SetNewPrecision(_precision);
	SAxisPropsChange sapc;
	sapc.bXAxis = bXAxis;
	sapc.bRedraw = bRedraw;
	if (pointsview!=NULL && bShowPointView) pointsview->SendMessage(CM_GRAPH_NOTIFICATION, GRAPH_GRAPHIX_AXISPROPS, (DWORD)&sapc);
	if (grafix!=NULL) grafix->SendMessage(CM_GRAPH_NOTIFICATION, GRAPH_GRAPHIX_AXISPROPS, (DWORD)&sapc);
}

void CGraphView::FormatAxisOutput(double value, BOOL bXAxis, int format_level, CString& res_str)
{
	CString str;
	CAxisProps* props;
	if (bXAxis) props = &x_axis;
		else props = &y_axis;
	props->GetFormattedOutput(value, format_level, &res_str);
}

BOOL CGraphView::OnEraseBkgnd(CDC* pDC) 
{
	return FALSE;
//	return CScrollView::OnEraseBkgnd(pDC);
}

BOOL CGraphView::OnGraphRBCommand(DWORD commandId)
{
	return TRUE;
}

BOOL CGraphView::OnGraphRBMenu(CMenu* menu)
{
	return TRUE;
}

BOOL CGraphView::GetBoundRect(double* minx, double* maxx, double* miny, double* maxy)
{
	int grcount = GetGraphCount();
	if (grcount == 0) return FALSE;
	int index;
	BOOL b = FALSE;
	CGraphProp* grprop = GetFirstGraph(&index);
	while(grprop!=NULL)
	{
		if (grprop->GetSize() == 0 || !grprop->IsVisible()) 
		{
			grprop = GetNextGraph(&index);
			continue;
		};
		if (!b)
		{//first time
			*minx = grprop->min_x;
			*maxx = grprop->max_x;
			*miny = grprop->min_y;
			*maxy = grprop->max_y;
			b = TRUE;
		} else
		{
			if (grprop->min_x<*minx) *minx = grprop->min_x;
			if (grprop->max_x>*maxx) *maxx = grprop->max_x;
			if (grprop->min_y<*miny) *miny = grprop->min_y;
			if (grprop->max_y>*maxy) *maxy = grprop->max_y;
		};
		grprop = GetNextGraph(&index);
	};
	return b;
}

void CGraphView::UpdatePointView()
{
	if (pointsview!=NULL && bShowPointView) pointsview->SendMessage(CM_GRAPH_NOTIFICATION, GRAPH_PVIEW_UPDATE, 0);
}

void CGraphView::DoCopyData(unsigned char dataFormat)
{
	CWaitCursor wait;
	HANDLE h = GetDataHandle(dataFormat);
//	COleDataSource olesrc;
	COleDataSource * olesrc = new COleDataSource;
	olesrc->CacheGlobalData(CF_TEXT, h);
	olesrc->SetClipboard();
}

void CGraphView::DoDragDropData(unsigned char dataFormat)
{
	CWaitCursor* wait = new CWaitCursor();
	HANDLE h = GetDataHandle(dataFormat);
	delete wait;
//	COleDataSource olesrc;
	COleDataSource * olesrc = new COleDataSource;
	olesrc->CacheGlobalData(CF_TEXT, h);
	olesrc->DoDragDrop(DROPEFFECT_COPY | DROPEFFECT_SCROLL);
}

HANDLE CGraphView::GetDataHandle(unsigned char dataFormat)
{
	CMemFile memfile;
	if (!WriteDataToFile(&memfile)) return NULL;
	int length = memfile.GetLength();
	void* src = memfile.Detach();
	HANDLE h = GlobalAlloc(GMEM_DDESHARE | GMEM_MOVEABLE, length);
	void* lp = GlobalLock(h);
	memcpy(lp, src, length);
	GlobalUnlock(h);
	delete src;
	return h;
}

void WriteLineToFile(CFile* file, char* line)
{
	static char* crlf = "\r\n";
	file->Write(line, strlen(line));
	file->Write(crlf, strlen(crlf));
}

BOOL CGraphView::WriteDataToFile(CFile* file)
{
	CString buffer;
	int index, i;
	int GraphCount = GetGraphCount();
	if (GraphCount == 0) return FALSE;
	PCGraphProp* graphs = new PCGraphProp[GraphCount];
	PCGraphProp grprop;

	grprop = GetFirstGraph(&index);
	i = 0;
	while (grprop!=NULL)
	{
		if (grprop->IsVisible())
		{
			graphs[i] = grprop;
			i+=1;
		};
		grprop = GetNextGraph(&index);
	};
	GraphCount = i;
	if (GraphCount == 0)
	{
		delete graphs;
		return FALSE;
	};
	//write header to file
	buffer = "";
	for (i=0; i<GraphCount; i++)
	{
		buffer+=graphs[i]->GetTitle();
		if (i!=GraphCount-1)
		{
			buffer+="\t\t";
		} else
		{
			buffer+="\t";
		};
	};
	WriteLineToFile(file, (char*)(LPCTSTR)buffer);
	//subtitles
	buffer = "";
	CString xtitle, ytitle;
	x_axis.GetFullTitle(&xtitle);
	y_axis.GetFullTitle(&ytitle);
	for (i=0; i<GraphCount; i++)
	{
		buffer+=xtitle;
		buffer+="\t";
		buffer+=ytitle;
		if (i!=GraphCount-1) buffer+="\t";
	};
	WriteLineToFile(file, (char*)(LPCTSTR)buffer);
	//write points to file
	BOOL bExit;
	int pcount = 0, res;
	SSinglePoint ssp;
	CString temp_str;
	do
	{
		bExit = TRUE;
		buffer = "";
		for (i=0; i<GraphCount; i++)
		{
			res = graphs[i]->GetPoint(pcount, &ssp);
			if (res!=-1)
			{
				bExit = FALSE;
				FormatAxisOutput(ssp.x, TRUE, 0, temp_str);
				buffer += temp_str;
				buffer+="\t";
				FormatAxisOutput(ssp.y, FALSE, 0, temp_str);
				buffer+=temp_str;
				if (i!=GraphCount-1) buffer+="\t";
			} else
			{
				buffer+="\t";
				if (i!=GraphCount-1) buffer+="\t";
			};
		};
		WriteLineToFile(file, (char*)(LPCTSTR)buffer);
		pcount+=1;
	}while(!bExit);
	delete graphs;
	return TRUE;
}

