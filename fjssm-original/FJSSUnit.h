#pragma once
#include "afxtempl.h"
#include "FJSSFN.h"

//---------------------------
// StrList
//---------------------------

// CStrList target
class CStrList: public CArray<CString,CString&>
{
public:
	CStrList();
	virtual ~CStrList();

	virtual INT_PTR Add(LPCTSTR str);

	CStrList& operator=(CStrList& StrList);
};

//---------------------------
// StrArea
//---------------------------
class CStrArea: public CArray<CStrList,CStrList&>
{
public:
	CStrArea();
	virtual ~CStrArea();

	CStrArea& operator =(CStrArea& StrArea);

};

//---------------------------
// Job
//---------------------------
class CJob : public CObject
{
public:
	CJob();
	CJob(CJob& Job);

	virtual ~CJob();
	CString m_strName;			// nazev jobu
	CFN		m_fDD;				// termin dokonceni jobu
	int		m_nColor;			// index barvy

	double m_fAlfa;				// koeficienty rozsirene 
	double m_fBeta;				// funkce prislusnosti DD
	double m_fLambda1;			// koef pro krit 9
	double m_fLambda2;	
	double m_fLambda3;

	CJob& operator=(CJob& Job);

	virtual void Serialize(CArchive& ar);
};

//---------------------------
// JobList
//---------------------------
class CJobList : public CArray<CJob,CJob&>
{
public:
    CJobList();
	CJobList(CJobList& JobList);
	virtual ~CJobList();

	CJobList& operator=(CJobList& joblist);
	CJobList& Equal(CJobList& Joblist);

	virtual void Serialize(CArchive& ar);
};


//---------------------------
// Machine
//---------------------------
class CMachine: public CObject
{
public:
	CMachine();
	virtual ~CMachine();
	int m_nColor;				// index barvy v tabulce colorbtn
	CString m_strName;							// oznaceni stroje
	CMachine& operator=(CMachine& Machine);
	virtual void Serialize(CArchive& ar);
};
//---------------------------
// MachineList
//---------------------------
class CMachineList : public CArray <CMachine,CMachine&>
{
public:
	CMachineList();
	virtual ~CMachineList();
	virtual void Serialize(CArchive& ar);

	CMachineList& operator=(CMachineList& MachineList);
};

//---------------------------
// Node
//---------------------------
class CEdge;
class CNode : public CObject
{
public:
	CNode();
	CNode(CNode& Node);
	virtual ~CNode();

	int m_nJob;						// index jobu
	int m_nProcess;					// index operace
	int m_nMachine;					// index stroje
	CString m_strName;				// nazev operace

	BOOL m_bChecked;					// priznak provedeni operace

	CFN m_fPT;						// cas operace
	CFN m_fr;						// nejdrive mozny zacatek operace
	CFN m_fC;						// konecny cas operace

	int m_nPrice;					// cena uzlu 

	CEdge* m_pPrevEdge[FN_PART];	// ukazatel na predchozi hranu
	int Void();						// vynuluje vsechny udaje v uzlu

	CNode& operator =(CNode& Node);
	virtual void Serialize(CArchive& ar);
};


//---------------------------
// NodeList
//---------------------------
class CNodeList : public CArray<CNode,CNode&>
{
public:
	CNodeList();
	virtual ~CNodeList();
	CNodeList& operator=(CNodeList& NodeList);
	virtual void Serialize(CArchive& ar);
};

//---------------------------
// NodePtrList (1D)
//---------------------------
class CPtrNodeList : public CTypedPtrArray<CObArray,CNode*>
{
public:
	CPtrNodeList(); 
	virtual ~CPtrNodeList();

	CPtrNodeList& operator=(CPtrNodeList& PtrNodeList);

};
//---------------------------
// NodeArea
//---------------------------
class CNodeArea : public CArray<CNodeList,CNodeList&>
{
public:
	CNodeArea();
	virtual ~CNodeArea();
	virtual void Serialize(CArchive& ar);

	CNodeArea& operator=(CNodeArea& NodeArea);
};
//---------------------------
// PtrNodeArea (2D)
//---------------------------
class CPtrNodeArea : public CArray<CPtrNodeList,CPtrNodeList&>
{
public:
	CFN m_fMakespan;		// Makespan = cas dokonceni posledni operace
	double m_fVOOF;			// VOFF value of objective function
							// Makespan (u Crisp)
							// Stredni hodnota (u Fuzzy)
							// ...

	CPtrNodeArea();
	CPtrNodeArea(CPtrNodeArea& PtrNodeArea);
	virtual ~CPtrNodeArea();

	CPtrNodeArea& operator=(CPtrNodeArea& PtrNodeArea);
};
//---------------------------
// PtrListAreaNode (3D)
//---------------------------
class CPtrNodeAreaList : public CArray<CPtrNodeArea,CPtrNodeArea&>
{
public:
	CPtrNodeAreaList();
	virtual ~CPtrNodeAreaList();

};


//---------------------------
// Edge
//---------------------------
class CEdge : public CObject
{
public:
	CEdge();
	virtual ~CEdge();
	
	// DATA
	int m_nList;			// index stroje(CON_EDGE), stroje (DIS_EDGE)
	int m_nItem;			// index polozky v seznamu polozek jobu(CON_EDGE), stroje(DIS_EDGE)

	BOOL m_bCon;			// konjunktivni hrana

	CNode* m_pNode1;		// ukazatel na uzel 1 hrany
	CNode* m_pNode2;		// ukazatel na uzel 2 hrany

	CEdge& operator=(CEdge& Edge);
};

//---------------------------
// EdgeList
//---------------------------
class CEdgeList : public CArray <CEdge,CEdge&>
{
public:
	CEdgeList();
	virtual ~CEdgeList();
	
	CEdgeList& operator =(CEdgeList& EdgeList);
};


//---------------------------
// PtrEdgeList
//---------------------------
class CPtrEdgeList: public CTypedPtrArray<CObArray,CEdge*>
{
public:
	CPtrEdgeList();
	virtual ~CPtrEdgeList();

	CPtrEdgeList& operator=(CPtrEdgeList& PtrEdgeList);

};

//---------------------------
// EdgeArea
//---------------------------
class CEdgeArea: public CArray<CEdgeList,CEdgeList&>
{
public:
	CEdgeArea();
	virtual ~CEdgeArea();
	
	CEdgeArea& operator=(CEdgeArea& EdgeArea);

};

//---------------------------
// PtrEdgeArea
//---------------------------
class CPtrEdgeArea: public CArray<CPtrEdgeList,CPtrEdgeList&>
{
public:
	CPtrEdgeArea();
	virtual ~CPtrEdgeArea();
	
	
};
//---------------------------
// CFuzzyList
//---------------------------
class CFNList : public CArray<CFN, CFN&>
{
public:
	CFNList();
	virtual ~CFNList();
	CFNList& operator=(CFNList& FNList);
};

//---------------------------
// UintList
//---------------------------
class CUintList : public CUIntArray
{
public:
	CUintList();
	virtual ~CUintList();
	CUintList& operator=(CUintList& UintList);
};

//---------------------------
// UintArea
//---------------------------
class CUintArea : public CArray<CUintList, CUintList&>
{
public:
	CUintArea();
	virtual ~CUintArea();

	CUintArea& operator=(CUintArea& UintArea);
};

//---------------------------
// DoubleList
//---------------------------
class CDoubleList : public CArray<double, double>
{
public:
	CDoubleList();
	virtual ~CDoubleList();

	CDoubleList& operator=(CDoubleList& DoubleList);
};

//---------------------------
// DoubleArea
//---------------------------
class CDoubleArea: public CArray<CDoubleList,CDoubleList&>
{
public:
	CDoubleArea();
	virtual ~CDoubleArea();

	CDoubleArea& operator =(CDoubleArea& DoubleArea);
};
//---------------------------
// DoubleAreaList
//---------------------------
class CDoubleAreaList: public CArray<CDoubleArea,CDoubleArea&>
{
public:
	CDoubleAreaList();
	virtual ~CDoubleAreaList();

	CDoubleAreaList& operator =(CDoubleAreaList& DoubleAreaList);
};

//---------------------------
// CSchedule
//---------------------------
class CSchedule : public CObject
{
public:
	CSchedule();
	virtual ~CSchedule();

	int m_nJob;			// index jobu
	int m_nProcess;		// index operace v jobu

	CSchedule& operator=(CSchedule& Schedule);
	CSchedule& operator=(CNode& Node);

	CNode& Node(CNodeArea& areaNode);
};

//---------------------------
// CScheduleList
//---------------------------
class CScheduleList : public CArray<CSchedule,CSchedule&>
{
public:
	CScheduleList();
	virtual ~CScheduleList();

	CScheduleList& operator=(CPtrNodeList& PtrNodeList);
	CScheduleList& operator=(CScheduleList& ScheduleList);
};

//---------------------------
// CScheduleArea
//---------------------------
class CScheduleArea : public CArray<CScheduleList,CScheduleList&>
{
public:
	CScheduleArea();
	virtual ~CScheduleArea();
   	
	// Aktual
	double m_fVOOF;			// nejlepsi hodnota ucelove funkce
	CFN	   m_fMakespan;		// makespan nejlepsiho reseni

	CScheduleArea& operator=(CPtrNodeArea& PtrNodeArea);
	CScheduleArea& operator=(CScheduleArea& ScheduleArea);

};
//---------------------------
// CAxis
//---------------------------
class CAxis:public CObject
{
public:
	CAxis();
	virtual ~CAxis();

	double m_fX;		// s,msec
	double m_fY;		// VOOF

	CAxis& operator=(CAxis& Axis);
};
//---------------------------
// CAxisList
//---------------------------

class CAxisList: public CArray<CAxis,CAxis&>
{
public:
	CAxisList();
	virtual ~CAxisList();
    CString m_strName;			// Legenda funkce

	virtual int Add(double fX,double fY,double nSec);

	CAxisList& operator=(CAxisList& AxisList);
};

//---------------------------
// CAxisArea
//---------------------------
class CAxisArea:public CArray<CAxisList,CAxisList&>
{
public:
	CAxisArea();
	virtual ~CAxisArea();

	CAxisArea& operator=(CAxisArea& AxisArea);
};