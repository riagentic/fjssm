/*
This file was created by Paul Barvinko (pbarvinko@yahoo.com).
This file is distributed "as is", e.g. there are no warranties 
and obligations and you could use it in your applications on your
own risk. 
Your comments and questions are welcome.
If using in your applications, please mention author in credits for your app.
*/
// Changed from ruler.cpp (created by Pasha)
// fruler.cpp : implementation file
//

#include "stdafx.h"
#include "gruler.h"
#include "afxtempl.h"
//

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// FRuler

IMPLEMENT_DYNCREATE(FRuler, CWnd);

FRuler::FRuler() : CWnd()
{
	MinShift=FRulerWidth;
	MaxShift=0;
	MinValue=1.0f; MaxValue=100.0f;
	MainFontRec.lfHeight = 14;
	MainFontRec.lfWidth = 8;
	MainFontRec.lfEscapement = 0;
	MainFontRec.lfOrientation = 0;
	MainFontRec.lfWeight = 100;
	MainFontRec.lfItalic = 0;
	MainFontRec.lfUnderline = 0;
	MainFontRec.lfStrikeOut = 0;
	MainFontRec.lfCharSet = ANSI_CHARSET;
	MainFontRec.lfOutPrecision = OUT_TT_PRECIS;
	MainFontRec.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	MainFontRec.lfQuality = PROOF_QUALITY;
	MainFontRec.lfPitchAndFamily =FF_MODERN;
	strcpy(MainFontRec.lfFaceName,"");
	sUnit = "";
}

FRuler::~FRuler()
{
}

void FRuler::SetMinMax(double minv, double maxv, BOOL bRedraw)
{
	MinValue = minv;
	MaxValue = maxv;
	if (bRedraw) RedrawWindow();
}

BEGIN_MESSAGE_MAP(FRuler, CWnd)
	//{{AFX_MSG_MAP(FRuler)
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int FRuler::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	EnableToolTips(TRUE);
	//create tool tip object
	tooltip.Create(this, TTS_ALWAYSTIP);
	tooltip.Activate(TRUE);
	tooltip.AddTool(this,  (LPCTSTR)"");
  tooltip.SendMessage(TTM_SETMAXTIPWIDTH, 0, SHRT_MAX);
  tooltip.SendMessage(TTM_SETDELAYTIME, TTDT_AUTOPOP, SHRT_MAX);
  tooltip.SendMessage(TTM_SETDELAYTIME, TTDT_INITIAL, 100);
  tooltip.SendMessage(TTM_SETDELAYTIME, TTDT_RESHOW, 100);
	return 0;
}

BOOL FRuler::PreTranslateMessage(MSG* pMsg) 
{
	tooltip.RelayEvent(pMsg);
	return CWnd::PreTranslateMessage(pMsg);
}

void FRuler::SetNewTitles(char* newUOM, char* newTitle)
{
	sUnit = newUOM;
	sTitle = newTitle;
	tooltip.UpdateTipText(sTitle, this);
}

void FRuler::OnMouseMove(UINT nFlags, CPoint point) 
{
	tooltip.Activate(TRUE);
	CWnd::OnMouseMove(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
// FRuler drawing

/////////////////////////////////////////////////////////////////////////////
// FRuler diagnostics

#ifdef _DEBUG
void FRuler::AssertValid() const
{
	CWnd::AssertValid();
}

void FRuler::Dump(CDumpContext& dc) const
{
	CWnd::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// FHRuler

IMPLEMENT_DYNAMIC(FHRuler, FRuler);

//FHRuler object
FHRuler::FHRuler() : FRuler()
{
}

int FHRuler::GetNMax()
{
	CRect R;
	GetClientRect( &R);

	CString s;
	s = "9.99 ";

	TEXTMETRIC TM;
	int Nmax;
	tagSIZE Sz;
	CFont* newfont;
	newfont=new CFont();
	MainFontRec.lfEscapement = 0;
	MainFontRec.lfHeight = 14;
	MainFontRec.lfWidth = 8;
	newfont->CreateFontIndirect(&MainFontRec);
	CDC* PaintDC = GetDC();
	CFont* OldFont=(CFont*)PaintDC->SelectObject(newfont);
	PaintDC->GetTextMetrics(&TM);
	GetTextExtentPoint32(PaintDC->m_hDC,LPCTSTR(s),s.GetLength(),&Sz);

	double p;
	p=(double)(R.right-R.left-MinShift-MaxShift)/Sz.cx;
	if (p==(int)p) Nmax=(int)p; else Nmax=(int)p+1;

	PaintDC->SelectObject(OldFont);
	ReleaseDC(PaintDC);
	delete newfont;
	return Nmax;
}

void FHRuler::OnPaint()
{
	CRect R;
	GetClientRect( &R);
			// Filling Rectangle
	CPaintDC PaintDC(this);
	CPen* OldPen = (CPen*)PaintDC.SelectStockObject(NULL_PEN);
	CBrush* OldBrush = (CBrush*) PaintDC.SelectStockObject(LTGRAY_BRUSH);
	R.right+=1;
	R.bottom+=1;
	PaintDC.Rectangle(R);
	R.right-=2;
	R.bottom-=2;
	PaintDC.SelectStockObject(BLACK_PEN);
	PaintDC.MoveTo(R.left+MinShift,R.bottom);
	PaintDC.LineTo(R.right,R.bottom);
	PaintDC.LineTo(R.right,R.top);
	PaintDC.SelectStockObject(WHITE_PEN);
	PaintDC.LineTo(R.left+MinShift,R.top);
	PaintDC.LineTo(R.left+MinShift,R.bottom);
	if (R.bottom>=R.top+Width()-1 && R.right-R.left>MinShift+MaxShift+5)
	{
		 // Counting grid value
		CString s;
		s = "9.99 ";
		CString UnitName = sUnit+"-10";
		//UnitName.Format("%s",StrUnits[GblOpt.m_MeasurementUnit()]);
		TEXTMETRIC TM;
		int Nmax;
		tagSIZE Sz,UnitNameSize;
		CFont* newfont;
		newfont=new CFont();
		MainFontRec.lfEscapement = 0;
		MainFontRec.lfHeight = 14;
		MainFontRec.lfWidth = 8;
		newfont->CreateFontIndirect(&MainFontRec);
		CFont* OldFont=(CFont*)PaintDC.SelectObject(newfont);
		PaintDC.GetTextMetrics(&TM);
		GetTextExtentPoint32(PaintDC.m_hDC,LPCTSTR(s),s.GetLength(),&Sz);
		GetTextExtentPoint32(PaintDC.m_hDC,LPCTSTR(UnitName),UnitName.GetLength(),&UnitNameSize);
		UnitNameSize.cx+=4;
		CCoordinates coord(R.left+MinShift, R.right-MaxShift, MinValue, MaxValue);

//		double p;
//		p=(double)(R.right-R.left-MinShift-MaxShift)/Sz.cx;
//		if (p==(int)p) Nmax=(int)p; else Nmax=(int)p+1;
		Nmax = GetNMax();
		double Delta;//=(float)CountGrid(minv,maxv,Nmax);
		double* wp;
		double* sp;
		int count, maxexp;
		coord.DivideInterval(Nmax, &maxexp, &Delta, &count, &sp, &wp);
		// drawing grid points
		int CurrentX;
		double CurrentValue;
		//char ch[4];
		PaintDC.SelectStockObject(BLACK_PEN);
		PaintDC.SetBkMode( TRANSPARENT);
		for (int i=0; i<count; i++)
		{
			CurrentX = (int)sp[i];
			CurrentValue = wp[i];
			double v = CurrentValue*exp(-maxexp*log(10));
			//big marks
			PaintDC.MoveTo(CurrentX,R.bottom);	
			PaintDC.LineTo(CurrentX,R.bottom-3);
			//write text above big marks
			s.Format("%.1f", v);
			GetTextExtentPoint32(PaintDC.m_hDC,LPCTSTR(s),s.GetLength(),&Sz);
			if (CurrentX-(Sz.cx >> 1)>R.left+MinShift  && CurrentX+(Sz.cx >> 1)< R.right-UnitNameSize.cx)
				PaintDC.TextOut(CurrentX-(Sz.cx >> 1),R.bottom-TM.tmHeight,s);
			//little marks
			v = (CurrentValue*exp(-maxexp*log(10))-Delta/2)*exp(maxexp*log(10));
			int x = (int)coord.WtoX(v);
			if (coord.IsPointInRangeX(x))
			{
				PaintDC.MoveTo(x, R.bottom);	
				PaintDC.LineTo(x, R.bottom-2);
			};
			v = (CurrentValue*exp(-maxexp*log(10))+Delta/2)*exp(maxexp*log(10));
			x = (int)coord.WtoX(v);
			if (coord.IsPointInRangeX(x))
			{
				PaintDC.MoveTo(x, R.bottom);	
				PaintDC.LineTo(x, R.bottom-2);
			};
		};		
		//  Draw current measuring unit
		if (maxexp!=0) 
		{
			s.Format("%d", maxexp);
			UnitName = sUnit+"*"+s;
		} else UnitName = sUnit;
		PaintDC.TextOut(R.right-(UnitNameSize.cx+1),R.bottom-TM.tmHeight,UnitName);

		if (sp!=NULL) delete sp;
		if (wp!=NULL) delete wp;

		PaintDC.SelectObject(OldFont);
		delete newfont;
	};
}

int FRuler::Width()
{
	//if (IsWindowVisible())
		return FRulerWidth;
	//else return 0;
}

BEGIN_MESSAGE_MAP(FHRuler, FRuler)
	//{{AFX_MSG_MAP(FRuler)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// FVRuler

IMPLEMENT_DYNAMIC(FVRuler, FRuler);

FVRuler::FVRuler() : FRuler()
{
}

int FVRuler::GetNMax()
{
	CRect R;
	GetClientRect( &R);

	CString s;
	s = "9.99 ";

	TEXTMETRIC TM;
	int Nmax;
	tagSIZE Sz;
	CFont* newfont;
	newfont=new CFont();
	MainFontRec.lfEscapement = 900;
	MainFontRec.lfHeight = 12;
//	MainFontRec.lfHeight = 10;
	MainFontRec.lfWidth = 8;
	newfont->CreateFontIndirect(&MainFontRec);
	CDC* PaintDC = GetDC();
	CFont* OldFont=(CFont*)PaintDC->SelectObject(newfont);
	PaintDC->GetTextMetrics(&TM);
	GetTextExtentPoint32(PaintDC->m_hDC,LPCTSTR(s),s.GetLength(),&Sz);

	double p;
	p=(double)(R.bottom-R.top-MinShift-MaxShift)/Sz.cx;
	if (p==(int)p) Nmax=(int)p; else Nmax=(int)p+1;

	PaintDC->SelectObject(OldFont);
	ReleaseDC(PaintDC);
	delete newfont;

	return Nmax;
}

void FVRuler::OnPaint()
{
			 // Get current picture size
	CRect R;
	GetClientRect( &R);
				   // Filling rectangle
	CPaintDC PaintDC(this);
	CPen* OldPen = (CPen*)PaintDC.SelectStockObject(NULL_PEN);
	CBrush* OldBrush = (CBrush*) PaintDC.SelectStockObject(LTGRAY_BRUSH);
	R.right+=1;
	R.bottom+=1;
	PaintDC.Rectangle(R);
	R.right-=2;
	R.bottom-=2;
	PaintDC.SelectStockObject(BLACK_PEN);
	PaintDC.MoveTo(R.left,R.bottom);
	PaintDC.LineTo(R.right,R.bottom);
	PaintDC.LineTo(R.right,R.top+MinShift);
	PaintDC.SelectStockObject(WHITE_PEN);
	PaintDC.LineTo(R.left,R.top+MinShift);
	PaintDC.LineTo(R.left,R.bottom);
	if (R.bottom-R.top>MinShift+MaxShift+5 && R.right>=R.left+Width()-1)
	{
		// Counting grid value
		CString s;
		s = "9.99 ";
		CString UnitName = sUnit+"-10";
//		CString UnitName(StrUnits(ID_UNIT_Feet));
		TEXTMETRIC TM;
		int Nmax;
		tagSIZE Sz,UnitNameSize;
		CFont* newfont;
		newfont=new CFont();
		MainFontRec.lfEscapement = 900;
		MainFontRec.lfHeight = 12;
		MainFontRec.lfWidth = 8;
		newfont->CreateFontIndirect(&MainFontRec);
		CFont* OldFont=(CFont*)PaintDC.SelectObject(newfont);
		PaintDC.GetTextMetrics(&TM);
		GetTextExtentPoint32(PaintDC.m_hDC,LPCTSTR(s),s.GetLength(),&Sz);
		GetTextExtentPoint32(PaintDC.m_hDC,LPCTSTR(UnitName),UnitName.GetLength(),&UnitNameSize);
		UnitNameSize.cx+=4;
		CCoordinates coord(R.bottom-MaxShift, R.top+MinShift, MinValue, MaxValue);

//		float p;
//		p=(float)(R.bottom-R.top-MinShift-MaxShift)/Sz.cx;
//		if (p==(int)p) Nmax=(int)p; else Nmax=(int)p+1;
		Nmax = GetNMax();
		double Delta;//=(float)CountGrid(minv,maxv,Nmax);
		double* wp;
		double* sp;
		int count, maxexp;
		coord.DivideInterval(Nmax, &maxexp, &Delta, &count, &sp, &wp);

//		float Delta=(float)CountGrid(minv,maxv,Nmax);
		  // drawing grid points
		int CurrentY;
		double CurrentValue;
		PaintDC.SelectStockObject(BLACK_PEN);
		PaintDC.SetBkMode( TRANSPARENT);
		for (int i=0; i<count; i++)
		{
			CurrentY = (int)sp[i];
			CurrentValue = wp[i];
			double v = CurrentValue*exp(-maxexp*log(10));
			//big marks
			PaintDC.MoveTo(R.right,CurrentY);
			PaintDC.LineTo(R.right-3,CurrentY);
			//write text above big marks
			s.Format("%.1f", v);
			GetTextExtentPoint32(PaintDC.m_hDC,LPCTSTR(s),s.GetLength(),&Sz);
			if (CurrentY+(Sz.cx >> 1)<R.bottom && CurrentY-(Sz.cx >> 1)>R.top+MinShift+UnitNameSize.cx)
			PaintDC.TextOut(R.right-TM.tmHeight,CurrentY+(Sz.cx >> 1),s);
			//little marks
			v = (CurrentValue*exp(-maxexp*log(10))-Delta/2)*exp(maxexp*log(10));
			int x = (int)coord.WtoX(v);
			if (coord.IsPointInRangeX(x))
			{
				PaintDC.MoveTo(R.right,x);
				PaintDC.LineTo(R.right-2,x);
			};
			v = (CurrentValue*exp(-maxexp*log(10))+Delta/2)*exp(maxexp*log(10));
			x = (int)coord.WtoX(v);
			if (coord.IsPointInRangeX(x))
			{
				PaintDC.MoveTo(R.right,x);
				PaintDC.LineTo(R.right-2,x);
			};
		};		
		//  Draw current measuring unit
		if (maxexp!=0) 
		{
			s.Format("%d", maxexp);
			UnitName = sUnit+"*"+s;
		} else UnitName = sUnit;
		PaintDC.TextOut(R.right-TM.tmHeight,R.top+FRulerWidth+(UnitNameSize.cx+1),UnitName);

		if (sp!=NULL) delete sp;
		if (wp!=NULL) delete wp;

		PaintDC.SelectObject(OldFont);
		delete newfont;
	};
 }

BEGIN_MESSAGE_MAP(FVRuler, FRuler)
	//{{AFX_MSG_MAP(FRuler)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

