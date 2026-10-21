/*
This file was created by Paul Barvinko (pbarvinko@yahoo.com).
This file is distributed "as is", e.g. there are no warranties 
and obligations and you could use it in your applications on your
own risk. 
Your comments and questions are welcome.
If using in your applications, please mention author in credits for your app.
*/
// compages.cpp : implementation file
//

#include "stdafx.h"
#include "compages.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTitlePrompt dialog


CTitlePrompt::CTitlePrompt(CWnd* pParent /*=NULL*/)
	: CDialog(CTitlePrompt::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTitlePrompt)
	m_Title = _T("");
	//}}AFX_DATA_INIT
}


void CTitlePrompt::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTitlePrompt)
	DDX_Text(pDX, IDC_TITLE_STRING, m_Title);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTitlePrompt, CDialog)
	//{{AFX_MSG_MAP(CTitlePrompt)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTitlePrompt message handlers

/////////////////////////////////////////////////////////////////////////////
// CPointsCombo

CPointsCombo::CPointsCombo()
{
}

CPointsCombo::~CPointsCombo()
{
}


BEGIN_MESSAGE_MAP(CPointsCombo, CComboBox)
	//{{AFX_MSG_MAP(CPointsCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPointsCombo message handlers
void CPointsCombo::DrawComboItem(COLORREF color, char* title, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	//prepare tools for drawing
	CDC* dc = CDC::FromHandle(lpDrawItemStruct->hDC);
	CPen pen(PS_SOLID, 1, color);
	CBrush brush(color);
	CPen* oldpen = (CPen*)dc->SelectObject(&pen);
	CBrush* oldbrush = (CBrush*)dc->SelectObject(&brush);

	//draw rectangle
	CRect r(lpDrawItemStruct->rcItem);
	r.top+=2; r.left+=2; r.bottom-=2;
	r.right = r.left+20;
	dc->Rectangle(r);
	//draw text
	CPen textpen(PS_SOLID, 1, RGB(0, 0, 0));
	dc->SelectObject(&textpen);
	int bound = r.right+3;
	r = lpDrawItemStruct->rcItem;
	r.left = bound; r.top+=3;
	dc->DrawText(title, -1, &r, DT_VCENTER | DT_LEFT);

	dc->SelectObject(oldpen);
	dc->SelectObject(oldbrush);
}

//class CGrPointsCombo ***************************
void CGrPointsCombo::CreatePointsCombo(CGraphPage* _parent)
{
	parentPtr = _parent;
	//init combobox
	for (int i=0; i<parentPtr->grprops.GetSize(); i++) AddString("");
}

void CGrPointsCombo::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (parentPtr == NULL) return;
	int index = lpDrawItemStruct->itemID;
	if (index == -1) return;
	if (parentPtr->grprops.GetSize() == 0) return;
	CGraphProp* grprop = parentPtr->grprops.GetAt(index);
	DrawComboItem(grprop->GetGraphColor(), (char*)grprop->GetTitle(), lpDrawItemStruct);	
}

/////////////////////////////////////////////////////////////////////////////
// CGraphPage property page

IMPLEMENT_DYNCREATE(CGraphPage, CPropertyPage)

CGraphPage::CGraphPage() : CPropertyPage(CGraphPage::IDD)
{
}

CGraphPage::CGraphPage(CGraphView* _parent) : CPropertyPage(CGraphPage::IDD)
{
	//{{AFX_DATA_INIT(CGraphPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	parentPtr = _parent;
	//create new collection of graph properties - for undo
	if (parentPtr != NULL && parentPtr->GetGraphCount() != 0)
	{
		//fill new array with data
		int index;
		CGraphProp* grp;
		CGraphProp* grprop = parentPtr->GetFirstGraph(&index);
		while (grprop!=NULL)
		{
			grp = new CGraphProp(grprop, FALSE);
			grp->SetIndex(index);
			grprops.Add(grp);
			grprop = parentPtr->GetNextGraph(&index);
		};
	};
}

CGraphPage::~CGraphPage()
{
	for (int i=0; i<grprops.GetSize(); i++) delete grprops.GetAt(i);
}

void CGraphPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGraphPage)
	DDX_Text(pDX, IDC_GRAPH_TITLE, m_GraphTitle);
	DDX_Control(pDX, IDC_GRAPH_CHOICE, m_GraphCombo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGraphPage, CPropertyPage)
	//{{AFX_MSG_MAP(CGraphPage)
	ON_CBN_SELCHANGE(IDC_GRAPH_CHOICE, OnSelchangeGraphChoice)
	ON_BN_CLICKED(IDC_GRAPH_CH_COLOR, OnGraphChColor)
	ON_BN_CLICKED(IDC_GRAPH_CH_TITLE, OnGraphChTitle)
	ON_BN_CLICKED(IDC_GRAPH_HIDESHOW, OnGraphHideshow)
	ON_BN_CLICKED(IDC_GRAPHS_SHOWALL, OnGraphsShowall)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGraphPage message handlers

BOOL CGraphPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	m_GraphCombo.CreatePointsCombo(this);
	SetActiveSel(0);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CGraphPage::SetActiveSel(int selnum)
{
	if (selnum<0 || selnum>=grprops.GetSize()) return;
	m_GraphCombo.SetCurSel(selnum);
	SetHideShowAttr(selnum);
	CGraphProp* grprop = grprops.GetAt(selnum);
	m_GraphTitle = grprop->GetTitle();
	UpdateData(FALSE);
}

void CGraphPage::OnSelchangeGraphChoice() 
{
	SetActiveSel(m_GraphCombo.GetCurSel());
}

void CGraphPage::SetHideShowAttr(int selnum)
{
	if (selnum == -1) return;
	CGraphProp* grprop = grprops.GetAt(selnum);
	CWnd* wnd = GetDlgItem(IDC_GRAPH_HIDESHOW);
	if (wnd == NULL) return;
	if (grprop->IsVisible()) wnd->SetWindowText("Hide");
		else wnd->SetWindowText("Show");
}

void CGraphPage::OnGraphChColor() 
{
	int index = m_GraphCombo.GetCurSel();
	if (index == -1) return;
	CGraphProp* grprop = grprops.GetAt(index);
	CColorDialog dlg(grprop->GetGraphColor());
	if (dlg.DoModal() == IDOK)
	{
		grprop->SetGraphColor(dlg.GetColor());
		m_GraphCombo.RedrawWindow();
	};
}

void CGraphPage::OnGraphChTitle() 
{
	int index = m_GraphCombo.GetCurSel();
	if (index == -1) return;
	CGraphProp* grprop = grprops.GetAt(index);
	CTitlePrompt dlg;
	dlg.m_Title = grprop->GetTitle();
	if (dlg.DoModal() == IDOK)
	{
		grprop->SetTitle((char*)(LPCTSTR)dlg.m_Title);
		m_GraphCombo.RedrawWindow();
		m_GraphTitle = grprop->GetTitle();
		UpdateData(FALSE);
	};
}

void CGraphPage::OnGraphHideshow() 
{
	int index = m_GraphCombo.GetCurSel();
	if (index == -1) return;
	CGraphProp* grprop = grprops.GetAt(index);
	grprop->SetVisible(!grprop->IsVisible());
	SetHideShowAttr(index);
}

void CGraphPage::OnGraphsShowall() 
{
	int index = m_GraphCombo.GetCurSel();
	if (index == -1) return;
	CGraphProp* grprop;
	for (int i=0; i<grprops.GetSize(); i++)
	{
		grprop = grprops.GetAt(i);
		grprop->SetVisible(TRUE);
	};
	SetHideShowAttr(index);
}

void CGraphPage::OnOK() 
{
	CPropertyPage::OnOK();
	//update graph with info - but do not redraw! - this will be done in main module
	for (int i=0; i<grprops.GetSize(); i++)
	{
		CGraphProp* grprop = grprops.GetAt(i); 
		int index = grprop->GetIndex();
		CGraphProp* grp = parentPtr->GetGraph(index);
		grp->SetGraphProps(grprop);
		parentPtr->UpdateGraph(index, FALSE);
	};
}
/////////////////////////////////////////////////////////////////////////////
// CAxisPage property page

IMPLEMENT_DYNCREATE(CAxisPage, CPropertyPage)

CAxisPage::CAxisPage() : CPropertyPage(CAxisPage::IDD)
{
}

CAxisPage::CAxisPage(CGraphView* parent) : CPropertyPage(CAxisPage::IDD)
{
	//{{AFX_DATA_INIT(CAxisPage)
	m_XTitle = _T("");
	m_XUOM = _T("");
	m_XAxisPrec = 0;
	m_YAxisPrec = 0;
	m_YTitle = _T("");
	m_YUOM = _T("");
	//}}AFX_DATA_INIT
	parentPtr = parent;
	//fill data
	m_XTitle = parentPtr->x_axis.GetTitle();
	m_XUOM = parentPtr->x_axis.GetUOM();
	m_XAxisPrec = parentPtr->x_axis.GetPrecision();
	m_YTitle = parentPtr->y_axis.GetTitle();
	m_YUOM = parentPtr->y_axis.GetUOM();
	m_YAxisPrec = parentPtr->y_axis.GetPrecision();
}

CAxisPage::~CAxisPage()
{
}

void CAxisPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAxisPage)
	DDX_Text(pDX, IDC_XTITLE, m_XTitle);
	DDX_Text(pDX, IDC_XUOM, m_XUOM);
	DDX_Text(pDX, IDC_XAXIS_PRECISION, m_XAxisPrec);
	DDV_MinMaxInt(pDX, m_XAxisPrec, 0, 10);
	DDX_Text(pDX, IDC_YAXIS_PRECISION, m_YAxisPrec);
	DDV_MinMaxInt(pDX, m_YAxisPrec, 0, 10);
	DDX_Text(pDX, IDC_YTITLE, m_YTitle);
	DDX_Text(pDX, IDC_YUOM, m_YUOM);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAxisPage, CPropertyPage)
	//{{AFX_MSG_MAP(CAxisPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAxisPage message handlers

void CAxisPage::OnOK() 
{
	CPropertyPage::OnOK();
	parentPtr->SetAxisProps((char*)(LPCTSTR)m_XTitle, (char*)(LPCTSTR)m_XUOM, m_XAxisPrec, TRUE, FALSE);
	parentPtr->SetAxisProps((char*)(LPCTSTR)m_YTitle, (char*)(LPCTSTR)m_YUOM, m_YAxisPrec, FALSE, FALSE);
}

