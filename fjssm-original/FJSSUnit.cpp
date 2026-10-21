// FJSSUnit.cpp : implementation file
//

#include "stdafx.h"
#include "FJSSM.h"
#include "FJSSUnit.h"

//---------------------------
// Job
//---------------------------
CJob::CJob()
{
	m_strName = _T("");
	m_fDD = 0;

	m_fAlfa = 0;
	m_fBeta = 0;
	m_fLambda1 = 0;
	m_fLambda2 = 0;
	m_fLambda3 = 0;

	m_nColor = 0;
}

CJob::CJob(CJob& Job)
{
	m_nColor = Job.m_nColor;

	m_strName = Job.m_strName;
	m_fDD = Job.m_fDD;

	m_fAlfa = Job.m_fAlfa;
	m_fBeta = Job.m_fBeta;
	m_fLambda1 = Job.m_fLambda1;
	m_fLambda2 = Job.m_fLambda2;
	m_fLambda3 = Job.m_fLambda3;
}

CJob::~CJob()
{
}

CJob& CJob::operator =(CJob& Job)
{
	m_nColor = Job.m_nColor;
	m_strName = Job.m_strName;
	m_fDD = Job.m_fDD;

	m_fAlfa = Job.m_fAlfa;
	m_fBeta = Job.m_fBeta;
	m_fLambda1 = Job.m_fLambda1;
	m_fLambda2 = Job.m_fLambda2;
	m_fLambda3 = Job.m_fLambda3;

	return *this;
}
//---------------------------
// JobList
//---------------------------
CJobList::CJobList()
{
}

CJobList::~CJobList()
{
}


CJobList::CJobList(CJobList& JobList)
{
	RemoveAll();
	for (int i=0;i<JobList.GetSize();i++)
	{
		Add(CJob());
		GetAt(i) = JobList[i];
	}

}


CJobList& CJobList::operator=(CJobList& JobList)
{
	RemoveAll();
	for (int i=0;i<JobList.GetSize();i++)
	{
		Add(CJob());
		GetAt(i) = JobList[i];
	}

	return *this;
	
}

//---------------------------
// Machine
//---------------------------
CMachine::CMachine()
{
	m_strName = _T("");
	m_nColor = 0;
}

CMachine::~CMachine()
{
}
CMachine& CMachine::operator=(CMachine& Machine)
{
	m_nColor = Machine.m_nColor;
	m_strName = Machine.m_strName;
	return *this;
}
//---------------------------
// MachineList
//---------------------------
CMachineList::CMachineList()
{
}

CMachineList::~CMachineList()
{
}

CMachineList& CMachineList::operator =(CMachineList& MachineList)
{
	RemoveAll();
	for (int i=0; i<MachineList.GetSize(); i++)
	{
		Add(CMachine());
		GetAt(i) = MachineList[i];
	}
	return *this;
}

//---------------------------
// StrList
//---------------------------
CStrList::CStrList()
{
}

CStrList::~CStrList()
{
}

CStrList& CStrList::operator=(CStrList& StrList)
{
	RemoveAll();
	for (int i=0; i<StrList.GetSize(); i++)
	{
		Add(CString());				
		GetAt(i) = StrList.GetAt(i);
	}

	return *this;
}

int CStrList::Add(LPCTSTR str)
{
	int nIndex = (int)CArray<CString,CString&>::Add(CString());
	GetAt(nIndex) = _T(str);	
	return nIndex;
}

//---------------------------
// StrArea
//---------------------------
CStrArea::CStrArea()
{
}

CStrArea::~CStrArea()
{
}

CStrArea& CStrArea::operator =(CStrArea& StrArea)
{
	RemoveAll();
	for (int i=0; i<StrArea.GetSize();i++)
	{
		Add(CStrList());
		GetAt(i) = StrArea[i];
	}

	return *this;
}

//---------------------------
// Node
//---------------------------
CNode::CNode()
{
	m_bChecked = FALSE;

	m_strName = _T("");

	m_nPrice = 0;
	m_nJob = 0;
	m_nProcess = 0;
	m_nMachine = 0;

	m_fC = 0;
	m_fr = 0;
	m_fPT = 0;
}
CNode::CNode(CNode& Node)
{
	m_bChecked = Node.m_bChecked;

	m_strName = Node.m_strName;

	m_nJob = Node.m_nJob;
	m_nProcess = Node.m_nProcess;
	m_nMachine = Node.m_nMachine;	
	m_nPrice = Node.m_nPrice;

	m_fPT = Node.m_fPT;
	m_fr = Node.m_fr;
	m_fC = Node.m_fC;

	for (int i=0; i<FN_PART; i++) m_pPrevEdge[i] = Node.m_pPrevEdge[i];
}


CNode::~CNode()
{
}

CNode::Void()
{	
	m_bChecked = FALSE;

	m_strName = _T("");

	m_nJob = 0;
	m_nProcess = 0;
	m_nMachine = 0;
	m_nPrice = 0;

	m_fC = 0;
	m_fr = 0;
	m_fPT = 0;

	for (int i=0; i<FN_PART; i++) m_pPrevEdge[i] = NULL;

	return 0;
}

// CNode member functions
CNode& CNode::operator =(CNode& Node)
{
	m_bChecked = Node.m_bChecked;

	m_strName = Node.m_strName;

	m_nJob = Node.m_nJob;
	m_nProcess = Node.m_nProcess;
	m_nMachine = Node.m_nMachine;	
	m_nPrice = Node.m_nPrice;

	m_fPT = Node.m_fPT;
	m_fr = Node.m_fr;
	m_fC = Node.m_fC;

	for (int i=0; i<FN_PART; i++) m_pPrevEdge[i] = Node.m_pPrevEdge[i];

	return *this;
}

//---------------------------
// NodeList
//---------------------------
CNodeList::CNodeList()
{
}

CNodeList::~CNodeList()
{
}

// CNodeList member function
CNodeList& CNodeList::operator =(CNodeList& NodeList)
{
	RemoveAll();
	for (int i=0; i<NodeList.GetSize();i++)
	{
		Add(CNode());				
		GetAt(i) = NodeList.GetAt(i);
	}
	return *this;
}

//---------------------------
// PtrNodeList
//---------------------------
CPtrNodeList::CPtrNodeList()
{
}

CPtrNodeList::~CPtrNodeList()
{
}

CPtrNodeList& CPtrNodeList::operator =(CPtrNodeList& PtrNodeList)
{

    RemoveAll();

	for (int i=0; i<PtrNodeList.GetSize(); i++)
		Add(PtrNodeList[i]);


	return *this;
}
//---------------------------
// NodeArea
//---------------------------
CNodeArea::CNodeArea()
{
}

CNodeArea::~CNodeArea()
{
}

CNodeArea& CNodeArea::operator =(CNodeArea& NodeArea)
{
	RemoveAll();
	for (int i=0; i<NodeArea.GetSize(); i++)
	{
        Add(CNodeList());
		GetAt(i) = NodeArea[i];
	}
	return *this;
}
//---------------------------
// NodePtrArea
//---------------------------
CPtrNodeArea::CPtrNodeArea()
{
	m_fMakespan = 0;
	m_fVOOF = 0;
}

CPtrNodeArea::CPtrNodeArea(CPtrNodeArea& PtrNodeArea)
{

	m_fMakespan = PtrNodeArea.m_fMakespan;	// Makespan = cas dokonceni posledni operace
	m_fVOOF = PtrNodeArea.m_fVOOF;			// Value of objective function

	RemoveAll();
	for (int i=0; i<PtrNodeArea.GetSize(); i++)
	{
		Add(CPtrNodeList());
		GetAt(i) = PtrNodeArea[i];
	}

}

CPtrNodeArea::~CPtrNodeArea()
{
}


CPtrNodeArea& CPtrNodeArea::operator =(CPtrNodeArea& PtrNodeArea)
{

	m_fMakespan = PtrNodeArea.m_fMakespan;
	m_fVOOF = PtrNodeArea.m_fVOOF;


	RemoveAll();
	for (int i=0; i<PtrNodeArea.GetSize(); i++)
	{
		Add(CPtrNodeList());
		GetAt(i) = PtrNodeArea[i];
	}

	return *this;
}


//---------------------------
// PtrListAreaNode (3D)
//---------------------------
CPtrNodeAreaList::CPtrNodeAreaList()
{
}

CPtrNodeAreaList::~CPtrNodeAreaList()
{
}


//---------------------------
// Edge
//---------------------------
CEdge::CEdge()
{
	m_bCon = TRUE;

	m_nItem = 0;
	m_nList = 0;

	m_pNode1 = NULL;
	m_pNode2 = NULL;
}

CEdge::~CEdge()
{
}

// CEdge member function
CEdge& CEdge::operator=(CEdge& Edge)
{
	m_bCon = Edge.m_bCon;

	m_nList = Edge.m_nList;
	m_nItem = Edge.m_nItem;

	m_pNode1 =  Edge.m_pNode1;
	m_pNode2 =  Edge.m_pNode2;

	return *this;
}


//---------------------------
// EdgeList
//---------------------------
CEdgeList::CEdgeList()
{
}

CEdgeList::~CEdgeList()
{
}

CEdgeList& CEdgeList::operator =(CEdgeList& EdgeList)
{
	RemoveAll();
	for (int i=0; i<EdgeList.GetSize();i++)
	{
		Add(CEdge());				
		GetAt(i) = EdgeList.GetAt(i);
	}

	return *this;
}
//---------------------------
// PtrEdgeList
//---------------------------
CPtrEdgeList::CPtrEdgeList()
{
}

CPtrEdgeList::~CPtrEdgeList()
{
}

CPtrEdgeList& CPtrEdgeList::operator =(CPtrEdgeList& PtrEdgeList)
{
	RemoveAll();
	for (int i=0; i<PtrEdgeList.GetSize(); i++) 
		Add(PtrEdgeList[i]);
	return *this;
}
//---------------------------
// EdgeArea
//---------------------------
CEdgeArea::CEdgeArea()
{
}

CEdgeArea::~CEdgeArea()
{
}

CEdgeArea& CEdgeArea::operator =(CEdgeArea& EdgeArea)
{
	RemoveAll();
	for (int i=0; i<EdgeArea.GetSize();i++)
	{
		Add(CEdgeList());				
		GetAt(i) = EdgeArea.GetAt(i);
	}

	return *this;
}

//---------------------------
// PtrEdgeArea
//---------------------------
CPtrEdgeArea::CPtrEdgeArea()
{
}

CPtrEdgeArea::~CPtrEdgeArea()
{
}
//---------------------------
// CFuzzyList
//---------------------------
CFNList::CFNList()
{
}

CFNList::~CFNList()
{
}

CFNList& CFNList::operator =(CFNList& FNList)
{
	RemoveAll();
	for (int i=0; i< FNList.GetSize(); i++)
	{
		Add(CFN());
		GetAt(i) = FNList[i];
	}
	return *this;
}
//---------------------------
// UintList
//---------------------------
CUintList::CUintList()
{
}

CUintList::~CUintList()
{
}

CUintList& CUintList::operator =(CUintList& UintList)
{
	RemoveAll();
	for (int i=0; i<UintList.GetSize();i++)
		Add(UintList[i]);
	return *this;
}
//---------------------------
// UintArea
//---------------------------
CUintArea::CUintArea()
{
}

CUintArea::~CUintArea()
{
}

CUintArea& CUintArea::operator =(CUintArea& UintArea)
{
	RemoveAll();
	for (int i=0; i<UintArea.GetSize(); i++)
	{
		int nIndex = (int)Add(CUintList());
		GetAt(i) = UintArea[i];
	}
	return *this;

}

//---------------------------
// CDoubleList
//---------------------------
CDoubleList::CDoubleList()
{
}

CDoubleList::~CDoubleList()
{
}

CDoubleList& CDoubleList::operator =(CDoubleList& DoubleList)
{
	RemoveAll();
	for (int i=0; i<DoubleList.GetSize();i++)
	{
		Add(DoubleList[i]);
	}
	return *this;
}
//---------------------------
// CDoubleArea
//---------------------------
CDoubleArea::CDoubleArea()
{
}

CDoubleArea::~CDoubleArea()
{
}

CDoubleArea& CDoubleArea::operator =(CDoubleArea& DoubleArea)
{
	RemoveAll();
	for (int i=0; i<DoubleArea.GetSize();i++)
	{
		Add(CDoubleList());
		GetAt(i) = DoubleArea[i];
	}
	return *this;
}

//---------------------------
// DoubleAreaList
//---------------------------
CDoubleAreaList::CDoubleAreaList()
{
}

CDoubleAreaList::~CDoubleAreaList()
{
}

CDoubleAreaList& CDoubleAreaList::operator =(CDoubleAreaList& DoubleAreaList)
{
	RemoveAll();
	for (int i=0; i<DoubleAreaList.GetSize();i++)
	{
		Add(CDoubleArea());
		GetAt(i) = DoubleAreaList[i];
	}
	return *this;
}

//---------------------------
// CSchedule
//---------------------------
CSchedule::CSchedule()
{
}

CSchedule::~CSchedule()
{
}


CSchedule& CSchedule::operator =(CSchedule& Schedule)
{
	m_nJob = Schedule.m_nJob;
	m_nProcess = Schedule.m_nProcess;

	return *this;
}

CNode& CSchedule::Node(CNodeArea& areaNode)
{
	return areaNode[m_nJob][m_nProcess];
}

CSchedule& CSchedule::operator =(CNode& Node)
{
	m_nJob = Node.m_nJob;
	m_nProcess = Node.m_nProcess;

	return *this;
}


//---------------------------
// CScheduleList
//---------------------------
CScheduleList::CScheduleList()
{
}

CScheduleList::~CScheduleList()
{
}

CScheduleList& CScheduleList::operator =(CScheduleList& ScheduleList)
{
	RemoveAll();
	for (int i=0; i<ScheduleList.GetSize();i++)
	{
		Add(CSchedule());
		GetAt(i) = ScheduleList[i];
	}
	return *this;	
}

CScheduleList& CScheduleList::operator =(CPtrNodeList& PtrNodeList)
{
	RemoveAll();
	for (int i=0; i<PtrNodeList.GetSize();i++)
	{
		Add(CSchedule());
		GetAt(i) = *PtrNodeList[i];
	}
	return *this;	
}
//---------------------------
// CScheduleArea
//---------------------------
CScheduleArea::CScheduleArea()
{
}

CScheduleArea::~CScheduleArea()
{
}

CScheduleArea& CScheduleArea::operator =(CScheduleArea& ScheduleArea)
{
	m_fMakespan = ScheduleArea.m_fMakespan;
	m_fVOOF = ScheduleArea.m_fVOOF;

	RemoveAll();
	for (int i=0; i<ScheduleArea.GetSize();i++)
	{
		Add(CScheduleList());
		GetAt(i) = ScheduleArea[i];
	}
	return *this;	
}

CScheduleArea& CScheduleArea::operator =(CPtrNodeArea& PtrNodeArea)
{
	m_fMakespan = PtrNodeArea.m_fMakespan;
	m_fVOOF = PtrNodeArea.m_fVOOF;

	RemoveAll();
	for (int i=0; i<PtrNodeArea.GetSize();i++)
	{
		Add(CScheduleList());
		GetAt(i) = PtrNodeArea[i];
	}
	return *this;	
}

//---------------------------
// CAxis
//---------------------------
CAxis::CAxis()
{
	m_fX = 0;
	m_fY = 0;
}

CAxis::~CAxis()
{
}

CAxis& CAxis::operator =(CAxis& Axis)
{
	m_fX = Axis.m_fX;
	m_fY = Axis.m_fY;

	return *this;
}
//---------------------------
// CAxisList
//---------------------------
CAxisList::CAxisList()
{
	m_strName = _T("");
}

CAxisList::~CAxisList()
{
}

int CAxisList::Add(double fX,double fY,double nSec)
{
	 
	if (GetSize())
		if (fX < GetAt(GetSize()-1).m_fX+nSec) return GetSize();

	int nPos = (int)CArray<CAxis,CAxis&>::Add(CAxis());

	GetAt(GetSize()-1).m_fX = fX;
	GetAt(GetSize()-1).m_fY = fY;

	return nPos;
}

CAxisList& CAxisList::operator =(CAxisList& AxisList)
{
	m_strName = AxisList.m_strName;

	RemoveAll();
	for (int i=0;i<AxisList.GetSize();i++)
	{
		CArray<CAxis,CAxis&>::Add(CAxis());
		GetAt(i) = AxisList[i];
	}

	return *this;
}

//---------------------------
// CAxisArea
//---------------------------
CAxisArea::CAxisArea()
{
}

CAxisArea::~CAxisArea()
{
}

CAxisArea& CAxisArea::operator =(CAxisArea& AxisArea)
{
	RemoveAll();
	for (int i=0; i<AxisArea.GetSize(); i++)
	{
		Add(CAxisList());
		GetAt(i) = AxisArea[i];
	}
	return *this;
}

void CJob::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{	// storing code
		// 1. Nazev jobu
		// 2. DD
		// 3. alfa
		// 4. beta
		ar << m_strName;
		ar << m_fAlfa;
		ar << m_fBeta;
		ar << m_fLambda1;
		ar << m_fLambda2;
		ar << m_fLambda3;
		ar << m_nColor;
		m_fDD.Serialize(ar);
	}
	else
	{	// loading code
		ar >> m_strName;
		ar >> m_fAlfa;
		ar >> m_fBeta;
		ar >> m_fLambda1;
		ar >> m_fLambda2;
		ar >> m_fLambda3;
		ar >> m_nColor;
		m_fDD.Serialize(ar);
	}
}

void CJobList::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{	// storing code
		int nSize;
		nSize = GetSize();
		ar << nSize;
		for (int i=0; i<GetSize();i++)
			GetAt(i).Serialize(ar);
	}
	else
	{	// loading code
		RemoveAll();
		int nSize;
		ar >> nSize;
		for (int i=0; i<nSize;i++)
		{
			Add(CJob());
			GetAt(i).Serialize(ar);
		}
	}
}

void CMachine::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{	
		// 1. Nazev stroje
		ar << m_strName;
		ar << m_nColor;
	}
	else
	{	// loading code
		ar >> m_strName;
		ar >> m_nColor;

	}
}

void CMachineList::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{	// storing code
		int nSize = GetSize();
		ar << nSize;
		for (int i=0; i<nSize; i++)
			GetAt(i).Serialize(ar);
	}
	else
	{	// loading code
		RemoveAll();
		int nSize;
		ar >> nSize;
		for (int i=0; i<nSize; i++)
		{
			Add(CMachine());
			GetAt(i).Serialize(ar);
		}
	}
}

void CNodeArea::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{	// storing code
		int nSize;
		nSize = GetSize();
		ar << nSize;
		for (int i=0; i<GetSize(); i++)
			GetAt(i).Serialize(ar);
	}

	else
	{	// loading code
		RemoveAll();
		int nSize;
		ar >> nSize;
		for (int i=0; i<nSize; i++)
		{
			Add(CNodeList());
			GetAt(i).Serialize(ar);
		}
	}
}

void CNodeList::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{	// storing code
		int nSize;
		nSize = GetSize();
		ar << nSize;
		for (int i=0; i<GetSize();i++)
			GetAt(i).Serialize(ar);
	}
	else
	{	// loading code
		RemoveAll();
		int nSize;
		ar >> nSize;
		for (int i=0; i<nSize;i++)
		{
			Add(CNode());
			GetAt(i).Serialize(ar);
		}
	}
}

void CNode::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{	// storing code
		// 1. Nazev nodu, job, process, machine, PT
		ar << m_strName ;
		ar << m_nJob;
		ar << m_nProcess;
		ar << m_nMachine;
		m_fPT.Serialize(ar);

	}
	else
	{	// loading code
		ar >> m_strName ;
		ar >> m_nJob;
		ar >> m_nProcess;
		ar >> m_nMachine;
		m_fPT.Serialize(ar);

	}
}
