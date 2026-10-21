/*
This file was created by Paul Barvinko (pbarvinko@yahoo.com).
This file is distributed "as is", e.g. there are no warranties 
and obligations and you could use it in your applications on your
own risk. 
Your comments and questions are welcome.
If using in your applications, please mention author in credits for your app.
*/
// grppage.cpp : implementation file
//

#include "stdafx.h"
#include "grppage.h"
#include "grwnd.h"
#include "grview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGraphGenPage property page

IMPLEMENT_DYNCREATE(CGraphGenPage, CPropertyPage)

CGraphGenPage::CGraphGenPage() : CPropertyPage(CGraphGenPage::IDD)
{
}

CGraphGenPage::CGraphGenPage(CGraphix* _parent) : CPropertyPage(CGraphGenPage::IDD)
{
	//{{AFX_DATA_INIT(CGraphGenPage)
	m_x1 = 0.0;
	m_x2 = 0.0;
	m_y1 = 0.0;
	m_y2 = 0.0;
	m_Autoscale = FALSE;
	m_MouseCoords = FALSE;
	m_PointMarks = FALSE;
	m_ShowAxis = FALSE;
	//}}AFX_DATA_INIT
	parentPtr = _parent;
	//fill data
	CString str;

	parentPtr->mainView->FormatAxisOutput(parentPtr->GetX1(), TRUE, 0, str);
	m_x1 = atof(str);

	parentPtr->mainView->FormatAxisOutput(parentPtr->GetX2(), TRUE, 0, str);
	m_x2 = atof(str);

	parentPtr->mainView->FormatAxisOutput(parentPtr->GetY1(), FALSE, 0, str);
	m_y1 = atof(str);

	parentPtr->mainView->FormatAxisOutput(parentPtr->GetY2(), FALSE, 0, str);
	m_y2 = atof(str);

	m_Autoscale = (parentPtr->m_grflags & GRAPH_AUTOSCALE)!=0;
	m_PointMarks = (parentPtr->m_grflags & GRAPH_SQUAREPOINTS)!=0;
	m_MouseCoords = (parentPtr->m_grflags & GRAPH_SHOW_TOOLTIP)!=0;
	m_ShowAxis = (parentPtr->m_grflags & GRAPH_DRAW_AXIS)!=0;
}

CGraphGenPage::~CGraphGenPage()
{
}

void CGraphGenPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGraphGenPage)
	DDX_Text(pDX, IDC_X1, m_x1);
	DDX_Text(pDX, IDC_X2, m_x2);
	DDX_Text(pDX, IDC_Y1, m_y1);
	DDX_Text(pDX, IDC_Y2, m_y2);
	DDX_Check(pDX, IDC_GRAPH_AUTOSCALE, m_Autoscale);
	DDX_Check(pDX, IDC_GRAPH_MOUSECOORDS, m_MouseCoords);
	DDX_Check(pDX, IDC_GRAPH_POINTMARKS, m_PointMarks);
	DDX_Check(pDX, IDC_GEN_GRAPH_PROPS_AXIS, m_ShowAxis);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGraphGenPage, CPropertyPage)
	//{{AFX_MSG_MAP(CGraphGenPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGraphGenPage message handlers

BOOL CGraphGenPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CGraphGenPage::OnOK() 
{
	CPropertyPage::OnOK();
	//coords
	parentPtr->parentPtr->SetWorldDim(m_x1, m_x2, m_y1, m_y2, FALSE);
	//attributes
	DWORD win_flags = parentPtr->GetGraphixFlags();
	if (m_MouseCoords)
	{
		win_flags = win_flags | GRAPH_SHOW_TOOLTIP;
	} else
	{
		win_flags = win_flags & (~GRAPH_SHOW_TOOLTIP);
	};
	if (m_Autoscale)
	{
		win_flags = win_flags | GRAPH_AUTOSCALE;
	} else
	{
		win_flags = win_flags & (~GRAPH_AUTOSCALE);
	};
	if (m_PointMarks)
	{
		win_flags = win_flags | GRAPH_SQUAREPOINTS;
	} else
	{
		win_flags = win_flags & (~GRAPH_SQUAREPOINTS);
	};
	if (m_ShowAxis)
	{
		win_flags = win_flags | GRAPH_DRAW_AXIS;
	} else
	{
		win_flags = win_flags & (~GRAPH_DRAW_AXIS);
	};
	parentPtr->SetGraphixFlags(win_flags, FALSE);
}
