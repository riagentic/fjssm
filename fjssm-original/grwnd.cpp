/*
This file was created by Paul Barvinko (pbarvinko@yahoo.com).
This file is distributed "as is", e.g. there are no warranties 
and obligations and you could use it in your applications on your
own risk. 
Your comments and questions are welcome.
If using in your applications, please mention author in credits for your app.
*/
// Grwnd.cpp : implementation file
//

#include "stdafx.h"
#include "Grwnd.h"
#include "grview.h"
#include "grppage.h"
#include "compages.h"
#include "f2doff.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAX_TIP_COUNT 4

/////////////////////////////////////////////////////////////////////////////
// CGraphix

IMPLEMENT_DYNCREATE(CGraphix, CWnd)

CGraphix::CGraphix()
{
    ASSERT(FALSE);
}

CGraphix::CGraphix(CGraphContainer* parentWnd)
{
	offscreen = NULL;
	parentPtr = parentWnd;
	CurrentCoordsX = NULL;
	CurrentCoordsY = NULL;
	m_grflags = (GRAPH_AUTOSCALE | GRAPH_SQUAREPOINTS | GRAPH_DRAW_AXIS);
	tipCount = MAX_TIP_COUNT;
	CRect r(10, 10, 50, 50);
	undoZoomRect = NULL;
	bZoomActive = FALSE;
	bCreateZoomRect = FALSE;
	zoomrect = new CRectTracker(&r, CRectTracker::solidLine | CRectTracker::resizeInside | CRectTracker::hatchInside);
	bkColor = RGB(255, 255, 255);
}

CGraphix::~CGraphix()
{
	if (undoZoomRect!=NULL) delete undoZoomRect;
	if (CurrentCoordsX!=NULL) delete CurrentCoordsX;
	if (CurrentCoordsY!=NULL) delete CurrentCoordsY;
	delete zoomrect;
	delete offscreen;
}


BEGIN_MESSAGE_MAP(CGraphix, CWnd)
	//{{AFX_MSG_MAP(CGraphix)
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(IDC_COPY_TEXT, OnCopyText)
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGraphix diagnostics

#ifdef _DEBUG
void CGraphix::AssertValid() const
{
	CWnd::AssertValid();
}

void CGraphix::Dump(CDumpContext& dc) const
{
	CWnd::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGraphix message handlers

void CGraphix::OnRButtonUp(UINT nFlags, CPoint point) 
{
	CWnd::OnRButtonUp(nFlags, point);
	ClientToScreen(&point);
	CMenu menu;
	menu.LoadMenu(IDR_GRAPH_PROPS);
	CMenu* mn = menu.GetSubMenu(0);
	//check and uncheck items
	if ((m_grflags & GRAPH_DRAW_AXIS) == GRAPH_DRAW_AXIS) 
	{
		menu.CheckMenuItem(ID_GRAPH_SHOW_AXIS, MF_CHECKED | MF_BYCOMMAND);
	};
	if ((m_grflags & GRAPH_SHOW_TOOLTIP) == GRAPH_SHOW_TOOLTIP) 
	{
		menu.CheckMenuItem(ID_MOUSECOORDS, MF_CHECKED | MF_BYCOMMAND);
	};
	if ((m_grflags & GRAPH_AUTOSCALE) == GRAPH_AUTOSCALE) 
	{
		menu.CheckMenuItem(ID_GRAPH_AUTOSCALE, MF_CHECKED | MF_BYCOMMAND);
	};
	if ((m_grflags & GRAPH_SQUAREPOINTS) == GRAPH_SQUAREPOINTS) 
	{
		menu.CheckMenuItem(ID_POINTMARKS, MF_CHECKED | MF_BYCOMMAND);
	};
	//zoom section
	if (undoZoomRect == NULL) menu.EnableMenuItem(ID_GRAPH_UNDO_ZOOM, MF_GRAYED  | MF_BYCOMMAND);
	if (!bZoomActive) menu.EnableMenuItem(ID_GRAPH_APPLYZOOM, MF_GRAYED | MF_BYCOMMAND);
		else menu.CheckMenuItem(ID_GRAPH_ZOOM, MF_CHECKED | MF_BYCOMMAND);
	if (mainView != NULL) 
	{
		//fit stuff
		double x1, x2, y1, y2;
		if (!mainView->GetBoundRect(&x1, &x2, &y1, &y2))
		{
			menu.EnableMenuItem(ID_GRAPH_FITX, MF_GRAYED  | MF_BYCOMMAND);
			menu.EnableMenuItem(ID_GRAPH_FITY, MF_GRAYED  | MF_BYCOMMAND);
			menu.EnableMenuItem(ID_GRAPH_FITPAGE, MF_GRAYED  | MF_BYCOMMAND);
		};
		//hide/show points window
		if (mainView->GetPointViewState()) 
			menu.CheckMenuItem(ID_GRAPH_SHOWPOINTSVIEW, MF_CHECKED | MF_BYCOMMAND);
		//call parent handler
		if (!mainView->OnGraphRBMenu(&menu)) return;
	};
	mn->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
}

BOOL CGraphix::PreTranslateMessage(MSG* pMsg) 
{
	tooltip.RelayEvent(pMsg);
	return CWnd::PreTranslateMessage(pMsg);
}

void CGraphix::OnMouseMove(UINT nFlags, CPoint point) 
{
	tipCount -= 1;
	if (tipCount<=0)
	{
		tooltip.Activate((m_grflags & GRAPH_SHOW_TOOLTIP) == GRAPH_SHOW_TOOLTIP);
		CString str = GetToolTipString(point);
		tooltip.UpdateTipText(str, this);
		tipCount = MAX_TIP_COUNT;
	};

	//if on-the-fly zoom is active - redraw new rectangle
	if (bCreateZoomRect)
	{
		CRect old_rect(new_zoom_1.x, new_zoom_1.y, new_zoom_2.x, new_zoom_2.y);
		old_rect.NormalizeRect();

		new_zoom_2 = point;
		CRect rect(new_zoom_1.x, new_zoom_1.y, new_zoom_2.x, new_zoom_2.y);
		rect.NormalizeRect();

		zoomrect->m_rect = rect;

		old_rect.UnionRect(&old_rect, &rect);
		old_rect.InflateRect(2, 2);
		InvalidateRect(&old_rect, FALSE);
	};

	CWnd::OnMouseMove(nFlags, point);
}

void CGraphix::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (bCreateZoomRect)
	{
		CRect old_rect(new_zoom_1.x, new_zoom_1.y, new_zoom_2.x, new_zoom_2.y);
		old_rect.NormalizeRect();

		new_zoom_2 = point;
		bCreateZoomRect = FALSE;

		CRect rect(new_zoom_1.x, new_zoom_1.y, new_zoom_2.x, new_zoom_2.y);
		rect.NormalizeRect();
		zoomrect->m_rect = rect;

		if (new_zoom_1 != new_zoom_2)
		{
			bZoomActive = TRUE;
		};

		old_rect.UnionRect(&old_rect, &rect);
		old_rect.InflateRect(2, 2);
		InvalidateRect(&old_rect, FALSE);
	};
	CWnd::OnLButtonUp(nFlags, point);
}

void CGraphix::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (!bZoomActive)
	{
		bCreateZoomRect = TRUE;
		new_zoom_1 = new_zoom_2 = point;
	};
	
	if (bZoomActive)
	{
		CRect r1, r2;
		zoomrect->GetTrueRect(&r1);
		if (zoomrect->Track(this, point))
		{
			zoomrect->GetTrueRect(&r2);
			InvalidateRect(&r1, FALSE);
			InvalidateRect(&r2, FALSE);
		};
	};

	CWnd::OnLButtonDown(nFlags, point);
}

void CGraphix::SetWorldCoords(double _x1, double _x2, double _y1, double _y2, BOOL bRedraw)
{
	if (_x1 != p_x1 || _x2 != p_x2 || _y1 != p_y1 || _y2 != p_y2)
	{
		p_x1 =_x1; p_y1 = _y1;
		p_x2 = _x2; p_y2 = _y2;
		InitCoords();
		if (parentPtr->vruler != NULL)
		{
			parentPtr->vruler->SetMinMax(_y1, _y2, FALSE);
			parentPtr->vruler->RedrawWindow();
		};
		if (parentPtr->hruler != NULL)
		{
			parentPtr->hruler->SetMinMax(_x1, _x2, FALSE);
			parentPtr->hruler->RedrawWindow();
		};
		UpdateGraphWindow(NULL, bRedraw);
	};
}

CString CGraphix::GetToolTipString(CPoint pt)
{
	if (CurrentCoordsX == NULL || CurrentCoordsY == NULL) return "";
	double v1, v2;
	v1 = CurrentCoordsX->XtoW(pt.x);
	v2 = CurrentCoordsY->XtoW(pt.y);

	if (mainView == NULL) return "";
	CString str;
	mainView->FormatAxisOutput(v1, TRUE, 1, str);
	CString str1;
	mainView->FormatAxisOutput(v2, FALSE, 1, str1);
	CString str2;
	str2.Format("x=%s, y=%s", str, str1);
	return str2;
}

void CGraphix::GetWorldCoords(double* x1, double* x2, double* y1, double* y2)
{
	*x1 = GetX1();
	*x2 = GetX2();
	*y1 = GetY1();
	*y2 = GetY2();
}

DWORD CGraphix::GetGraphixFlags()
{
	return m_grflags;
}

DWORD CGraphix::SetGraphixFlags(DWORD new_flags, BOOL bRedraw)
{
	DWORD old_flags = m_grflags;
	if (old_flags != new_flags)
	{
		m_grflags = new_flags;
		UpdateGraphWindow(NULL, bRedraw);
	};
	return old_flags;
}

void CGraphix::ActivateToolTip(BOOL bActivate)
{
	tooltip.Activate(bActivate);
}

void CGraphix::DrawAxis(CDC* pDC)
{
	//draw axis and zero line first
	CRect r;
	GetClientRect(&r);

	CPen BlackDashPen(PS_DOT, 1, RGB(0, 0, 0));
	CPen bkPen(PS_SOLID, 1, bkColor);
	CPen* OldPen = (CPen*)pDC->SelectObject(&BlackDashPen);
	pDC->SetBkMode( TRANSPARENT);
		//horizontal
	double Delta;
	double* wp;
	double* sp;
	int count, maxexp, NMax;
	if (CurrentCoordsX!=NULL)
	{
		NMax = parentPtr->GetNMaxH();
		CurrentCoordsX->DivideInterval(NMax, &maxexp, &Delta, &count, &sp, &wp);
		for (int i=0; i<count; i++)
		{
			int x = (int)sp[i];
			pDC->MoveTo(x, r.bottom);	
			pDC->LineTo(x, r.top);
		};
		if (sp!=NULL) delete sp;
		if (wp!=NULL) delete wp;
	};
	//vertical
	if (CurrentCoordsY!=NULL)
	{
		NMax = parentPtr->GetNMaxV();
		CurrentCoordsY->DivideInterval(NMax, &maxexp, &Delta, &count, &sp, &wp);
		for (int i=0; i<count; i++)
		{
			int x = (int)sp[i];
			pDC->MoveTo(r.right,x);
			pDC->LineTo(r.left,x);
		};
		if (sp!=NULL) delete sp;
		if (wp!=NULL) delete wp;
	};

	pDC->SelectObject(OldPen);
}

void CGraphix::OnSize(UINT nType, int cx, int cy) 
{
	InitCoords();
	CWnd::OnSize(nType, cx, cy);
	if(offscreen)
	{// update offscreen 
	    CRect r;	
	    GetClientRect(&r);
	    CSize s = r.Size();
	    offscreen->ResizeDib( s.cx, s.cy );
	}
}

void CGraphix::InitCoords()
{
	if (CurrentCoordsX!=NULL)
	{
		delete CurrentCoordsX;
		CurrentCoordsX = NULL;
	};
	if (CurrentCoordsY!=NULL)
	{
		delete CurrentCoordsY;
		CurrentCoordsY = NULL;
	};
	CRect r;
	GetClientRect(&r);
	CurrentCoordsX = new CCoordinates(r.left, r.right, GetX1(), GetX2());
	CurrentCoordsY = new CCoordinates(r.bottom, r.top, GetY1(), GetY2());
}

int CGraphix::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
	    return -1;
    EnableToolTips(TRUE);
    //create tool tip object
    tooltip.Create(this);
    tooltip.Activate(TRUE);
    tooltip.AddTool(this,  (LPCTSTR)"");
    tooltip.SendMessage(TTM_SETMAXTIPWIDTH, 0, SHRT_MAX);
    tooltip.SendMessage(TTM_SETDELAYTIME, TTDT_AUTOPOP, SHRT_MAX);
    tooltip.SendMessage(TTM_SETDELAYTIME, TTDT_INITIAL, 100);
    tooltip.SendMessage(TTM_SETDELAYTIME, TTDT_RESHOW, 100);

    mainView = parentPtr->GetMainGraphView();

    CRect r;
    GetClientRect(&r);
    offscreen=new FOffScreen(r.Width(), r.Height());

    return 0;
}

void CGraphix::UpdateGraphWindow(CRect* rect, BOOL Redraw)
{
    if(!offscreen)
    {
		return;
    };
    CRect r;
    if (rect==NULL) GetClientRect(&r);
	else r.CopyRect(rect);

    //do paint here
    CDC* dc = offscreen->GetDibCDC();
    if (dc != NULL)
    {
		//fill background
		CBrush bkbrush(bkColor);
		CBrush* oldbrush = (CBrush*)dc->SelectObject(&bkbrush);

		CRect bkrect;
		GetClientRect(&bkrect);
		bkrect.bottom+=1; bkrect.right+=1; bkrect.left-=1; bkrect.top -= 1;
		dc->Rectangle(bkrect);
		dc->SelectObject(oldbrush);

		//draw points
		DrawPoints(dc);

		//axis
		if ((m_grflags & GRAPH_DRAW_AXIS) == GRAPH_DRAW_AXIS) DrawAxis(dc);

		offscreen->SetReady(TRUE);
    };

    if (Redraw) 
    {
		CDC* cdc = GetDC();
		DoRedraw(cdc, &r);
		ReleaseDC(cdc);
    };
}

void CGraphix::DrawPoints(CDC* dc)
{
	CBrush* oldbrush;
	//draw all the points
	SSinglePoint ssp, ssp1;
	int x, y, x1, y1, grnum;
	if (CurrentCoordsX == NULL || CurrentCoordsY == NULL) return;
	if (mainView == NULL) return;
	CGraphProp* grprop = mainView->GetFirstGraph(&grnum);
	while (grprop!=NULL)
	{
		if (grprop->GetSize() ==0 || !grprop->IsVisible()) 
		{
			grprop = mainView->GetNextGraph(&grnum);
			continue;
		};
		//set color
		CPen* pen;
		CBrush* brush;
		if (grprop->AreResourcesPrivate()) 
		{
			pen = grprop->GetPen();
			brush = grprop->GetBrush();
		} else 
		{
			pen = new CPen(PS_SOLID, 1, grprop->GetGraphColor());
			brush = new CBrush(grprop->GetGraphColor());
		};

		CPen* oldpen = (CPen*)dc->SelectObject(pen);
		oldbrush = (CBrush*)dc->SelectObject(brush);
		grprop->GetPoint(0, &ssp);
		x = (int)CurrentCoordsX->WtoX(ssp.x);
		y = (int)CurrentCoordsY->WtoX(ssp.y);
		if ((long)(m_grflags & GRAPH_SQUAREPOINTS)!=0) DrawSquarePoint(dc, x, y);
		for (int i=1; i<grprop->GetSize(); i++)
		{
			grprop->GetPoint(i, &ssp);
			x1 = (int)CurrentCoordsX->WtoX(ssp.x);
			y1 = (int)CurrentCoordsY->WtoX(ssp.y);
			if ((long)(m_grflags & GRAPH_SQUAREPOINTS)!=0) DrawSquarePoint(dc, x1, y1);
			dc->MoveTo(x, y);
			dc->LineTo(x1, y1);
			x = x1; y = y1;
		};
		dc->SelectObject(oldpen);
		dc->SelectObject(oldbrush);
		if (!grprop->AreResourcesPrivate()) 
		{
			delete brush;
			delete pen;
		};
		grprop = mainView->GetNextGraph(&grnum);
	};
}

void CGraphix::DoRedraw(CDC* dc, LPCRECT r)
{
    if (!offscreen)
    {
	return;
    };

    CRect rect;
    if (r == NULL)
    {
		GetClientRect(&rect);
    } else
    {
		rect.CopyRect(r);
    };
    if(rect.IsRectEmpty()) return;

    if( !offscreen->IsDIBReady() )
	    UpdateGraphWindow(NULL, FALSE);

    offscreen->DrawDib(dc->m_hDC, &rect);
    //draw zoom rect
    if (bZoomActive || bCreateZoomRect) zoomrect->Draw(dc);
}

void CGraphix::OnPaint() 
{
    CPaintDC dc(this); // device context for painting
    PAINTSTRUCT *ps=&dc.m_ps;
    CRect rect(ps->rcPaint);

    DoRedraw(&dc, rect);
}

void CGraphix::DrawSquarePoint(CDC* dc, int x, int y)
{
	dc->Rectangle(x-2, y-2, x+3, y+3);
}

LRESULT CGraphix::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (message == WM_COMMAND && (wParam>=ID_RBMENU_GRAPHIX_BASE && wParam<=ID_RBMENU_GRAPHIX_END))
	{//this is right button menu commands
		switch (wParam)
		{
			case ID_GRAPHIX_PROPERTIES:
			{
				DoGraphProps();
			}; break;
			case ID_GRAPH_SHOW_AXIS:
			{
				DWORD win_flags = GetGraphixFlags();
				if ((win_flags & GRAPH_DRAW_AXIS) == GRAPH_DRAW_AXIS)
				{
					win_flags = win_flags & (~GRAPH_DRAW_AXIS);
				} else
				{
					win_flags = win_flags | GRAPH_DRAW_AXIS;
				};
				SetGraphixFlags(win_flags, TRUE);
			}; break;
			case ID_MOUSECOORDS:
			{
				DWORD win_flags = GetGraphixFlags();
				if ((win_flags & GRAPH_SHOW_TOOLTIP) == GRAPH_SHOW_TOOLTIP)
				{
					win_flags = win_flags & (~GRAPH_SHOW_TOOLTIP);
				} else
				{
					win_flags = win_flags | GRAPH_SHOW_TOOLTIP;
				};
				SetGraphixFlags(win_flags, FALSE); //will be updated on the fly by mouse movement
			}; break;
			case ID_GRAPH_AUTOSCALE:
			{
				DWORD win_flags = GetGraphixFlags();
				if ((win_flags & GRAPH_AUTOSCALE) == GRAPH_AUTOSCALE)
				{
					win_flags = win_flags & (~GRAPH_AUTOSCALE);
				} else
				{
					win_flags = win_flags | GRAPH_AUTOSCALE;
				};
				SetGraphixFlags(win_flags, FALSE); //will be updated on the next data redraw
			}; break;
			case ID_POINTMARKS:
			{
				DWORD win_flags = GetGraphixFlags();
				if ((win_flags & GRAPH_SQUAREPOINTS) == GRAPH_SQUAREPOINTS)
				{
					win_flags = win_flags & (~GRAPH_SQUAREPOINTS);
				} else
				{
					win_flags = win_flags | GRAPH_SQUAREPOINTS;
				};
				SetGraphixFlags(win_flags, TRUE);
			}; break;
			case ID_GRAPH_ZOOM:
			{
				if (!bZoomActive)
				{
					bZoomActive = TRUE;
					zoomrect->m_rect.left = 5;
					zoomrect->m_rect.top = 5;
					zoomrect->m_rect.right = 50;
					zoomrect->m_rect.bottom = 50;

				} else
				{
					bZoomActive = FALSE;
				};
				CRect r;
				zoomrect->GetTrueRect(&r);
				InvalidateRect(&r, FALSE);
			}; break;
			case ID_GRAPH_APPLYZOOM:
			{
				ApplyZoom();
			}; break;
			case ID_GRAPH_UNDO_ZOOM:
			{
				if (undoZoomRect!=NULL)
				{
					parentPtr->SetWorldDim(undoZoomRect->x1, undoZoomRect->x2, undoZoomRect->y1, undoZoomRect->y2, TRUE);
					delete undoZoomRect;
					undoZoomRect = NULL;
				};
			}; break;
			case ID_GRAPH_SHOWPOINTSVIEW:
			{
				if (mainView!=NULL)
				{
					mainView->ShowPointView(!mainView->GetPointViewState());
				};
			}; break;
			case ID_GRAPH_FITX:
			case ID_GRAPH_FITY:
			case ID_GRAPH_FITPAGE:
			{
				DoFit(wParam);
			}; break;
			default:
			{//call parent view with wParam
				if (mainView != NULL) mainView->OnGraphRBCommand(wParam);
			};
		};
		
	};
	if (message == CM_GRAPH_NOTIFICATION)
	{
		switch (wParam)
		{
			case GRAPH_GRAPH_CLEAR:
			case GRAPH_PROP_CHANGED:
			case GRAPH_POINT_REMOVED:
			case GRAPH_POINT_CHANGED:
				{
					SGraphChange* sgc = (SGraphChange*)lParam;
					UpdateGraphWindow(NULL, sgc->bRedraw);
				}; break;
			case GRAPH_POINT_ADDED:
				{
					AddPoint((SGraphChange*)lParam);
					return NULL;
				}; break;
			case GRAPH_GRAPH_ADDED:
				{
				}; break;
			case GRAPH_GRAPHIX_PROP:
				{
						SGraphixProps* sgp = (SGraphixProps*)lParam;
						SetGraphixFlags(sgp->flags, sgp->bRedraw);
						SetWorldCoords(sgp->x1, sgp->x2, sgp->y1, sgp->y2, sgp->bRedraw);
				}; break;
			case GRAPH_GRAPHIX_AXISPROPS:
				{
					SAxisPropsChange* sapc = (SAxisPropsChange*)lParam;
					parentPtr->UpdateTitles(sapc->bXAxis, sapc->bRedraw);
				}; break;
		};
	};	
	return CWnd::WindowProc(message, wParam, lParam);
}

void CGraphix::DoGraphProps()
{
	if (mainView == NULL) return;
	CPropertySheet psheet("Graph window properties");
	//pages
	CGraphGenPage genpage(this);
	CGraphPage grpage(mainView);
	CAxisPage axispage(mainView);
	psheet.AddPage(&genpage);
	psheet.AddPage(&grpage);
	psheet.AddPage(&axispage);
	if (psheet.DoModal() == IDOK)
	{
		mainView->RedrawWindow();
	};
}

void CGraphix::DoFit(DWORD fitType)
{
	if (mainView == NULL) return;
	double x1, x2, y1, y2;
	if (!mainView->GetBoundRect(&x1, &x2, &y1, &y2)) return;
	if (x1 == x2) x1 = x2 = x1*0.9;
	if (y1 == y2) y1 = y2 = y1*0.9;
	if (undoZoomRect == NULL) undoZoomRect = new SFRect;
	undoZoomRect->x1 = p_x1; undoZoomRect->x2 = p_x2;
	undoZoomRect->y1 = p_y1; undoZoomRect->y2 = p_y2;
	switch (fitType)
	{
		case ID_GRAPH_FITX:
		{
			y1 = p_y1; y2 = p_y2;
		}; break;
		case ID_GRAPH_FITY:
		{
			x1 = p_x1; x2 = p_x2;
		}; break;
		case ID_GRAPH_FITPAGE:
		{
		}; break;
	};
	parentPtr->SetWorldDim(x1, x2, y1, y2, TRUE);
}

void CGraphix::AddPoint(SGraphChange* sgc)
{
	//get main view pointer
	if (mainView == NULL) return;
	SSinglePoint ssp;
	CGraphProp* sgp = mainView->GetGraph(sgc->graphnum);
	if (!sgp->IsVisible())
	{
	    //if graph is not visible - do not do anything here
	    return;
	};
	sgp->GetPoint(sgc->index, &ssp);
	BOOL bNeedRedraw = FALSE;
	if ((unsigned long)(m_grflags & GRAPH_AUTOSCALE))
	{
		//if grafix is autoscaled - just recalc coordinates and redraw the window
		if (ssp.x<CCoordinates::fmin(p_x1, p_x2)) 
		{
			SetMinX(ssp.x, FALSE);
			bNeedRedraw = TRUE;
		};
		if (ssp.x>CCoordinates::fmax(p_x1, p_x2)) 
		{
			SetMaxX(ssp.x, FALSE);
			bNeedRedraw = TRUE;
		};
		if (ssp.y<CCoordinates::fmin(p_y1, p_y2)) 
		{
			SetMinY(ssp.y, FALSE);
			bNeedRedraw = TRUE;
		};
		if (ssp.y>CCoordinates::fmax(p_y1, p_y2)) 
		{
			SetMaxY(ssp.y, FALSE);
			bNeedRedraw = TRUE;
		};
	};
	//draw new point 
	if (bNeedRedraw) 
	{
//		parentPtr->SetWorldDim(p_x1, p_x2, p_y1, p_y2, sgc->bRedraw);
		UpdateGraphWindow(NULL, sgc->bRedraw);
		return;
	};
	if (offscreen == NULL) return;

	//if point is inside the graph - we need invalidate the whole picture
	if (sgc->index!=0 && sgc->index!=sgp->GetSize()-1)
	{
		UpdateGraphWindow(NULL, sgc->bRedraw );
		return;
	};

	int x, y, x1, y1;
	x = (int)CurrentCoordsX->WtoX(ssp.x);
	y = (int)CurrentCoordsY->WtoX(ssp.y);

	CDC* cdc = GetDC();
	CDC* dc = offscreen->GetDibCDC();
	CPen* pen;
	CBrush* brush;
	if (sgp->AreResourcesPrivate()) 
	{
	    pen = sgp->GetPen();
	    brush = sgp->GetBrush();
	} else
	{
	    pen = new CPen(PS_SOLID, 1, sgp->GetGraphColor());
	    brush = new CBrush(sgp->GetGraphColor());
	};
	CPen* oldpen = (CPen*)dc->SelectObject(pen);
	CBrush* oldbrush = (CBrush*)dc->SelectObject(brush);
	CPen* oldpen_cdc = (CPen*)cdc->SelectObject(pen);
	CBrush* oldbrush_cdc = (CBrush*)cdc->SelectObject(brush);

	if (sgp->GetSize()>1)
	{
		if (sgc->index == 0) sgp->GetPoint(1, &ssp);
		if (sgc->index == sgp->GetSize()-1) sgp->GetPoint(sgc->index-1, &ssp);
		x1 = (int)CurrentCoordsX->WtoX(ssp.x);
		y1 = (int)CurrentCoordsY->WtoX(ssp.y);
		dc->MoveTo(x, y);
		dc->LineTo(x1, y1);
		if (sgc->bRedraw) 
		{
			cdc->MoveTo(x, y);
			cdc->LineTo(x1, y1);
		};
	};
	if ((m_grflags & GRAPH_SQUAREPOINTS) != 0) 
	{
		DrawSquarePoint(dc, x, y);
		if (sgc->bRedraw) 
		{
			DrawSquarePoint(cdc, x, y);
		};

	};

	dc->SelectObject(oldpen);
	dc->SelectObject(oldbrush);
	cdc->SelectObject(oldpen_cdc);
	cdc->SelectObject(oldbrush_cdc);
	if (!sgp->AreResourcesPrivate()) 
	{
	    delete pen;
	    delete brush;
	};
	ReleaseDC(cdc);
}

void CGraphix::SetMinX(double x, BOOL bRedraw)
{
	if (CCoordinates::fmin(p_x1, p_x2) == p_x1) 
	{
	    SetWorldCoords(x, p_x2, p_y1, p_y2, bRedraw);
	} else
	{
	    SetWorldCoords(p_x1, x, p_y1, p_y2, bRedraw);
	};
}

void CGraphix::SetMaxX(double x, BOOL bRedraw)
{
	if (CCoordinates::fmax(p_x1, p_x2) == p_x1) 
	{
	    SetWorldCoords(x, p_x2, p_y1, p_y2, bRedraw);
	} else
	{
	    SetWorldCoords(p_x1, x, p_y1, p_y2, bRedraw);
	};
}

void CGraphix::SetMinY(double y, BOOL bRedraw)
{
	if (CCoordinates::fmin(p_y1, p_y2) == p_y1) 
	{
	    SetWorldCoords(p_x1, p_x2, y, p_y2, bRedraw);
	} else
	{
	    SetWorldCoords(p_x1, p_x2, y, p_y2, bRedraw);
	};
}

void CGraphix::SetMaxY(double y, BOOL bRedraw)
{
	if (CCoordinates::fmax(p_y1, p_y2) == p_y1) 
	{
	    SetWorldCoords(p_x1, p_x2, y, p_y2, bRedraw);
	} else
	{
	    SetWorldCoords(p_x1, p_x2, y, p_y2, bRedraw);
	};
}

BOOL CGraphix::OnEraseBkgnd(CDC* pDC) 
{
	return FALSE;
//	return CWnd::OnEraseBkgnd(pDC);
}

BOOL CGraphix::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (bZoomActive)
		if (zoomrect->SetCursor(this, nHitTest)) return TRUE;
	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

void CGraphix::ApplyZoom()
{
	if (!bZoomActive) return;
	bZoomActive = FALSE;
	CRect r;
	zoomrect->GetTrueRect(&r);
	//convert this rectangle to world coords
	double x1 = CurrentCoordsX->XtoW(r.left);
	double x2 = CurrentCoordsX->XtoW(r.right);
	double y1 = CurrentCoordsY->XtoW(r.bottom);
	double y2 = CurrentCoordsY->XtoW(r.top);
	//fill in undo structure
	if (undoZoomRect == NULL) undoZoomRect = new SFRect;
	undoZoomRect->x1 = p_x1; undoZoomRect->x2 = p_x2;
	undoZoomRect->y1 = p_y1; undoZoomRect->y2 = p_y2;

	parentPtr->SetWorldDim(x1, x2, y1, y2, TRUE);
}

void CGraphix::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if (bZoomActive) ApplyZoom();
	CWnd::OnLButtonDblClk(nFlags, point);
}

void CGraphix::OnCopyText() 
{
	if (mainView == NULL) return;
	mainView->DoCopyData(GRAPH_COPY_TEXT);
}

/////////////////////////////////////////////////////////////////////////////
// CGraphContainer

IMPLEMENT_DYNCREATE(CGraphContainer, CScrollView)

CGraphContainer::CGraphContainer()
{
	hruler = NULL;
	vruler = NULL;
	grWnd = NULL;
}

CGraphContainer::~CGraphContainer()
{
	if (vruler!=NULL) delete vruler;
	if (hruler!=NULL) delete hruler;
	if (grWnd!=NULL) delete grWnd;
}


BEGIN_MESSAGE_MAP(CGraphContainer, CScrollView)
	//{{AFX_MSG_MAP(CGraphContainer)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGraphContainer drawing

void CGraphContainer::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: calculate the total size of this view
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);
	SetScrollSizes( MM_TEXT, CSize(0, 0));
	//tool tips
	EnableToolTips(TRUE);
}

void CGraphContainer::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CGraphContainer diagnostics

#ifdef _DEBUG
void CGraphContainer::AssertValid() const
{
	CScrollView::AssertValid();
}

void CGraphContainer::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGraphContainer message handlers

void CGraphContainer::OnSize(UINT nType, int cx, int cy) 
{
	DWORD hStyle=hruler->GetStyle();
	DWORD vStyle=vruler->GetStyle();
	int rwx= (hStyle & WS_VISIBLE)==0 ? 0 : vruler->Width();
	int rwy= (vStyle & WS_VISIBLE)==0 ? 0 : hruler->Width();
//	Rel->MoveWindow(rwx,rwy,cx-rwx,cy-rwy);
	vruler->MoveWindow(0,0,rwx,cy);
	hruler->MoveWindow(0,0,cx,rwy);
	grWnd->MoveWindow(rwx, rwy, cx-rwx, cy-rwy);
	CRect r;
	GetClientRect(&r);
	CScrollView::OnSize(nType, cx, cy);
}

int CGraphContainer::GetNMaxH()
{
	if (hruler==NULL) return 0;
	return hruler->GetNMax();
}

int CGraphContainer::GetNMaxV()
{
	if (vruler==NULL) return 0;
	return vruler->GetNMax();
}

int CGraphContainer::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CScrollView::OnCreate(lpCreateStruct) == -1)
		return -1;
	//create rulers
	CRect R(0, 0, 100, 100);
	hruler = new FHRuler();
	hruler->Create(NULL, "", WS_CHILD | WS_VISIBLE, R, this, 0, NULL);
	vruler = new FVRuler();
	vruler->Create(NULL, "", WS_CHILD | WS_VISIBLE, R, this, 0, NULL);
	//set initial values for rulers
	double x1 = 0, x2 = 10, y1 = 0, y2 = 10;
	hruler->SetMinMax(x1, x2, TRUE);
	vruler->SetMinMax(y1, y2, TRUE);
	grWnd = new CGraphix(this);
	grWnd->Create(NULL, "", WS_CHILD | WS_VISIBLE, R, this, 0, NULL);
	grWnd->SetWorldCoords(x1, x2, y1, y2, TRUE);
	return 0;
}

LRESULT CGraphContainer::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (message == CM_GRAPH_NOTIFICATION)
	{//this is graph's internal notification message
		if (grWnd!=NULL) grWnd->SendMessage(message, wParam, lParam);
		return NULL;
	};
	return CScrollView::WindowProc(message, wParam, lParam);
}

CGraphView* CGraphContainer::GetMainGraphView()
{
	CWnd* wnd = GetOwner();
	if (wnd == NULL) return NULL;
	if (!wnd->IsKindOf(RUNTIME_CLASS(CGraphSplitter))) return NULL;
	CGraphView* grview = ((CGraphSplitter*)wnd)->GetMainGraphView();
	return grview;
}

void CGraphContainer::SetWorldDim(double _x1, double _x2, double _y1, double _y2, BOOL bRedraw)
{
//	hruler->SetMinMax(_x1, _x2, FALSE);
//	vruler->SetMinMax(_y1, _y2, FALSE);
	grWnd->SetWorldCoords(_x1, _x2, _y1, _y2, bRedraw);
//	if (bRedraw) RedrawWindow();
}

void CGraphContainer::UpdateTitles(BOOL bXAxis, BOOL bRedraw)
{
	CString str;
	CGraphView* grview = GetMainGraphView();
	if (grview == NULL) return;
	if (bXAxis)
	{
		grview->x_axis.GetFullTitle(&str);
		hruler->SetNewTitles((char*)(LPCTSTR)grview->x_axis.GetUOM(),
			(char*)(LPCTSTR)str);
		if (bRedraw) hruler->RedrawWindow();
	} else
	{
		grview->y_axis.GetFullTitle(&str);
		vruler->SetNewTitles((char*)(LPCTSTR)grview->y_axis.GetUOM(),
			(char*)(LPCTSTR)str);
		if (bRedraw) vruler->RedrawWindow();
	};
}

BOOL CGraphContainer::OnEraseBkgnd(CDC* pDC) 
{
	return FALSE;
//	return CScrollView::OnEraseBkgnd(pDC);
}

void CGraphContainer::GetWorldCoords(double* x1, double* x2, double* y1, double* y2)
{
	*x1 = GetX1();
	*x2 = GetX2();
	*y1 = GetY1();
	*y2 = GetY2();
}

