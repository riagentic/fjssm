/*
This file was created by Paul Barvinko (pbarvinko@yahoo.com).
This file is distributed "as is", e.g. there are no warranties 
and obligations and you could use it in your applications on your
own risk. 
Your comments and questions are welcome.
If using in your applications, please mention author in credits for your app.
*/
#ifndef _GRVIEW_H_
#define _GRVIEW_H_
////////////////////////////////////////////////////////////////////////////

#include "grwnd.h"
#include "points.h"

//notification messages
#define CM_GRAPH_NOTIFICATION 50000

//notification constants
#define GRAPH_POINT_ADDED 0x00000001
#define GRAPH_POINT_REMOVED 0x00000002
#define GRAPH_POINT_CHANGED 0x00000003
#define GRAPH_POINT_OTHER 0x00000004

//graph notify constants
#define GRAPH_GRAPH_ADDED 0x00000005
#define GRAPH_GRAPH_REMOVED 0x00000006
#define GRAPH_GRAPH_CLEAR 0x00000007
#define GRAPH_GRAPH_QUICK_ERASE 0x00000008
#define GRAPH_PROP_CHANGED 0x00000009

//graph window notifications
#define GRAPH_GRAPHIX_PROP 0x0000000A
#define GRAPH_GRAPHIX_AXISPROPS 0x0000000B

//point window update
#define GRAPH_PVIEW_UPDATE 0x0000000C

//AXIS definition for notification commands
#define GRAPH_X_AXIS	TRUE
#define GRAPH_Y_AXIS	FALSE

//clipboard copy formats
#define GRAPH_COPY_TEXT 0x01

typedef struct SGraphixProps
{
	double x1, x2, y1, y2;
	DWORD flags;
	BOOL bRedraw;
}SGraphixProps;

typedef struct SAxisPropsChange
{
	BOOL bXAxis;
	BOOL bRedraw;
}SAxisPropsChange;

#define MAX_GRAPHS 32

class CAxisProps
{
	public:
		CAxisProps();
		~CAxisProps();

		void GetFormattedOutput(double value, int formatLevel, CString* str);
		void GetFullTitle(CString* str);

		void SetNewTitle(char* title);
		void SetNewUOM(char* newUOM);
		void SetNewPrecision(int newPrecision);

		const char* GetTitle(){return m_Title;};
		const char* GetUOM(){return m_UOM;};
		int GetPrecision(){return m_Precision;};

	protected:
		CString m_Title;
		CString m_UOM;
		int m_Precision;
		//format string for different formatLevel
		void CalculateFormatStrings();
		char* formatStrings[3];
};

/////////////////////////////////////////////////////////////////////////////
//graph properties
class CGraphProp : public CPointsCollection
{
	public:
		CGraphProp(COLORREF newColor, char* newTitle, BOOL VisibleFlag = TRUE, 
			BOOL _bResources = TRUE, int _index = -1);
		CGraphProp(CGraphProp* grprop, BOOL bCopyPoints = TRUE);
		virtual ~CGraphProp();

		void SetGraphProps(CGraphProp* grprop);
		
		COLORREF GetGraphColor();
		void SetGraphColor(COLORREF newColor);

		const char* GetTitle();
		void SetTitle(char* newTitle);

		BOOL IsVisible();
		void SetVisible(BOOL bVisibleFlag);

		int GetIndex(){return m_index;};
		void SetIndex(int _index){m_index = _index;};

		BOOL AreResourcesPrivate(){return bPrivateResources;};
		CPen* GetPen(){return pen;};
		CBrush* GetBrush(){return brush;};

	protected:
		BOOL bPrivateResources;
		CPen* pen;
		CBrush* brush;
		COLORREF m_grcolor;
		CString m_grTitle;
		BOOL m_bIsVisible;
		int m_index;
};

typedef CGraphProp* PCGraphProp;

// CGraphSplitter window

class CGraphView;

class CGraphSplitter : public CSplitterWnd
{
// Construction
public:
	CGraphSplitter();
	DECLARE_DYNCREATE(CGraphSplitter)

// Attributes
public:

	int m_MaxSecondColSize;

// Operations
public:

	virtual void TrackColumnSize(int x, int col);
	void SetMaxSecondColSize(int newMax, BOOL bRedraw = TRUE);

	CGraphView* GetMainGraphView();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGraphSplitter)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGraphSplitter();

	// Generated message map functions
protected:
	//{{AFX_MSG(CGraphSplitter)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CGraphView view

class CPointsView;

class CGraphView : public CScrollView
{
protected:
	CGraphView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CGraphView)

// Attributes
public:

	CGraphSplitter m_splitter;
	BOOL bSplitterCreated;
	//pointers to views
	CPointsView* pointsview;
	CGraphContainer* grafix;
	//points coolections
	CGraphProp* m_graphs[MAX_GRAPHS];
	int m_graphCount, m_graphEnumCount;
	BOOL bShowPointView;
	//axis
	CAxisProps y_axis;
	CAxisProps x_axis;

// Operations
public:
	//operations with graphs
	int AddGraph(COLORREF newColor = 0, char* title = "", BOOL bInvalidate = TRUE);
	int RemoveGraph(int index, BOOL bInvalidate = TRUE);
	int UpdateGraph(int index, BOOL bInvalidate = TRUE);
	CGraphProp* GetGraph(int index);
	int GetGraphCount();
	//enumeration functions
	CGraphProp* GetFirstGraph(int* index);
	CGraphProp* GetNextGraph(int* index);

	//points operations
	int AddPoint(int graphnum, double x, double y, BOOL bRedraw, BOOL bUpdatePView, int index = -1);
	int EditPoint(int graphnum, int index, double x, double y, BOOL bRedraw);
	int RemovePoint(int graphnum, int index, BOOL bRedraw);
	void ClearGraph(int graphnum, BOOL bRedraw, BOOL bUpdatePView);
	//graph properties
	BOOL GetBoundRect(double* minx, double* maxx, double* miny, double* maxy);
	void SetGraphTitle(int grnum, char* newTitle);

	void SetGraphFlags(DWORD new_flags, BOOL bRedraw);
	DWORD GetGraphFlags();
		
	void SetGraphWorldCoords(double x1, double x2, double y1, double y2, BOOL bRedraw);
	void GetGraphWorldCoords(double* x1, double* x2, double* y1, double* y2);

	void SetAxisProps(char* _title, char* _UOM, int _precision, BOOL bXAxis, BOOL bRedraw);
	void FormatAxisOutput(double value, BOOL bXAxis, int format_level, CString& res_str);
	//point view operation
	void ShowPointView(BOOL bShow);
	BOOL GetPointViewState();
	void UpdatePointView();
	//data operations (clipboard, file...)
	void DoCopyData(unsigned char dataFormat);
	void DoDragDropData(unsigned char dataFormat);
	BOOL WriteDataToFile(CFile* file);
	HANDLE GetDataHandle(unsigned char dataFormat);

	virtual BOOL Create( LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);

// Overrides
	virtual BOOL OnGraphRBMenu(CMenu* menu);
	virtual BOOL OnGraphRBCommand(DWORD commandId);

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGraphView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CGraphView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CGraphView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif
