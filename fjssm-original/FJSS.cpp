// FJSS.cpp : implementation file
//

#include "stdafx.h"
#include "FJSSM.h"
#include "FJSS.h"
#include <stdio.h>
#include <time.h>
#include <math.h>


// CFJSS

CFJSS::CFJSS()
{
	DeleteAll();
}

CFJSS::~CFJSS()
{
}

//-----------------------------------------------
// Project
//-----------------------------------------------
// vynulovani projektu a dealokace pameti
int CFJSS::DeleteAll()
{
	DeleteAllBase();
	DeleteAllSolving(3);		// vsechna reseni
	return 0;
}

CString CFJSS::GetStrDate()
{
	CString str;

	CTime time;
	time = CTime::GetCurrentTime();
	int nDay = time.GetDay();
	int nMonth = time.GetMonth();
	int nYear = time.GetYear();

	str.Format(_T("%d.%d.%d"),nDay,nMonth,nYear);

	return str;
}
// Delete BASE
//------------------------------------------------	
int CFJSS::DeleteAllBase()
{
	// Project
	m_strName = _T("");
	m_strImportFile = _T("");
	m_strExportFile = _T("");
	m_strDate = GetStrDate();
	m_strMaker = _T("");

	m_nJob = 0;
	m_nMachine =0;

	m_nType = 0;
	SetTypeProp(m_nType);

	m_bFuzzyDD = false;
	m_bFuzzyPT = false;

	m_bIsModified = FALSE;

	m_areaJobNode.RemoveAll();
	m_lstJob.RemoveAll();
	m_lstMachine.RemoveAll();


	// Criterion
	m_nCriterion = 0;
	m_nCritMaxOrSum = 0;

	// Weight
	m_fw1m = 1.0;
	m_fw2s = 0.5;
	m_fw1 = 1.0;
	m_fw2 = 0.5;
	m_fw3 = 0.25;
	m_fw4 = 0.1;

	// Konverze
	m_fDefaultDDCoef = 2.2;
	m_fDDCoef	= 0.4;
	m_fPTCoef	= 0.2;
	m_fJobCoef	= 0.04;
	m_fMachineCoef	= 0.02;	


	// SA
	SetDefaultSAParam();					// paramety SA
	// SA
	SetDefaultGAParam();					// paramety GA
	SetCriterionFlag();

	// Running solving
	m_fMinDelta = 0.1;
	m_areaAxis.RemoveAll();

	// Convergence solving
	m_nCOESolveTime = 10;
	m_nCOESolveCount = 5;
	m_bCOEAllCriterion = TRUE;
	m_bCOESaveToFile = TRUE;

	// Criteria solving
	m_nCREIteration = 100;
	m_nCREActIter = 0;
	m_bCRESaveToFile = TRUE;
	m_nCREMethod = 0;		

	// Runtime (nic se neprovadi)
	m_nAction = -1;			
	m_nSubAction = -1;

	// Result flag
	IsAbleResult(0);
	IsAbleConvResult(0);
	IsAbleCritResult(0);
	
	return 0;
}

// Delete SOLVING
//------------------------------------------------	
int CFJSS::DeleteAllSolving(int nMethod)
// vymaze vsechny zdroje, ktere patri do kategorie vypoctove
// (ne vysledky)
{
	// Uzly a hrany + 0 * 
	m_pLastCritNode = NULL;
	SetDefaultSpecialNode();
	m_lstpNode.RemoveAll();
	m_lstpEdge.RemoveAll();
	m_areaConEdge.RemoveAll();
	m_areaDisEdge.RemoveAll();

	// CPM + CRIT Block
	for (int i=0; i<FN_PART; i++) 
	{
		m_lstpCriticalPath[i].RemoveAll();
		m_lstnBlockMachine[i].RemoveAll();
		m_lstnBlockBegin[i].RemoveAll();
		m_lstnBlockEnd[i].RemoveAll();
	}


	m_lstsLegend.RemoveAll();						
	m_areasData.RemoveAll();

	return 0;
}

// Delete RESULTS
//------------------------------------------------	



int CFJSS::SetTypeMask(int nCase)
{
	if ((m_nType == 1) || (m_nType == 3)) m_bFuzzyDD = TRUE;
	else m_bFuzzyDD = FALSE;
	if ((m_nType == 2) || (m_nType == 3)) m_bFuzzyPT = TRUE;
	else m_bFuzzyPT = FALSE;

	switch (nCase)
	{
		case 0:  m_bFuzzyDD = FALSE; break;
		case 1:  m_bFuzzyPT = FALSE; break;
		case 2:  m_bFuzzyDD = TRUE; break;
		case 3:  m_bFuzzyPT = TRUE; break;
		default: break;
	}
	
	return SetTypeProp(GetTypeFromMask());
}

int CFJSS::GetTypeFromMask()
{
	// 1. posle priznaku nastavit typ problemu
	// 2. Updatovat moznosti kriterii

	if ((!m_bFuzzyDD) && (!m_bFuzzyPT)) return  0; 
	if ((m_bFuzzyDD) && (!m_bFuzzyPT))	return  1;
	if ((!m_bFuzzyDD) && (m_bFuzzyPT))	return  2;
	if ((m_bFuzzyDD) && (m_bFuzzyPT))	return  3;

	return -1;
}

int CFJSS::SetTypeProp(int nType)
{
	switch (nType)
	{
		case 0:m_nDDPart =1; m_nPTPart = 1;break;
		case 1:m_nDDPart = FN_PART; m_nPTPart = 1;break;
		case 2:m_nDDPart =1; m_nPTPart = FN_PART;break;
		case 3:m_nDDPart = FN_PART; m_nPTPart = FN_PART;break;
		default: break;
	}
	m_nType = nType;

	// 2.
	SetCriterionFlag();

	return m_nType;
}

//-----------------------------------------------
// Adding function
//-----------------------------------------------

int CFJSS::AddJob(LPCTSTR strName,CFN& fDD,double fAlfa,double fBeta,
							double fLambda1,double fLambda2,double fLambda3)
//#need m_lstJob
{
	// Kroky pridani Jobu
	// 1. pridani jobu do joblistu
	// 2. pridani NodeListu v NodeArea
	// 3. inkrementace pocitadla

	//1.
	int nJobIndex = (int)m_lstJob.Add(CJob());
	m_lstJob[nJobIndex].m_strName = strName;

	// C/F = OK
	for (int i=0; i<m_nDDPart; i++)
		m_lstJob[nJobIndex].m_fDD[i] = fDD[i];

	m_lstJob[nJobIndex].m_fAlfa = fAlfa;
	m_lstJob[nJobIndex].m_fBeta = fBeta;	
	m_lstJob[nJobIndex].m_fLambda1 = fLambda1;
	m_lstJob[nJobIndex].m_fLambda2 = fLambda2;
	m_lstJob[nJobIndex].m_fLambda3 = fLambda3;
	m_lstJob[nJobIndex].m_nColor = (m_nJob % 20);

	//2.
	int nNodeListIndex = (int)m_areaJobNode.Add(CNodeList());

	//3.
	m_nJob++;

	SetModified(TRUE);

	return 0;
}


int CFJSS::AddMachine(LPCTSTR strName)
//#need m_lstMachine
{
	// Kroky pro pridani Machine
	// 1. Pridani stroje do MachineListu
	// 2. Zvyseni pocitadla stroju

	// 1.
	int nMachineIndex = (int)m_lstMachine.Add(CMachine());
	m_lstMachine[nMachineIndex].m_strName = strName;
	m_lstMachine[nMachineIndex].m_nColor = (m_nMachine % 20);


	//2.
	m_nMachine++;

	SetModified(TRUE);
	return 0;
}

int CFJSS::MoveProcessUp(int nJob,int nProcess)
{
	// presunuti 
	// prepsani poradi operace v uzlu

	CNode Node;
	Node = m_areaJobNode[nJob][nProcess];
	m_areaJobNode[nJob].RemoveAt(nProcess);
	m_areaJobNode[nJob].InsertAt(nProcess-1,CNode());
	m_areaJobNode[nJob][nProcess-1] = Node;
	m_areaJobNode[nJob][nProcess-1].m_nProcess = nProcess-1;
	m_areaJobNode[nJob][nProcess].m_nProcess = nProcess;
	return 0;
}
int CFJSS::MoveProcessDown(int nJob,int nProcess)
{
	CNode Node;
	Node = m_areaJobNode[nJob][nProcess];
	m_areaJobNode[nJob].RemoveAt(nProcess);
	m_areaJobNode[nJob].InsertAt(nProcess+1,CNode());
	m_areaJobNode[nJob][nProcess+1] = Node;
	m_areaJobNode[nJob][nProcess+1].m_nProcess = nProcess+1;
	m_areaJobNode[nJob][nProcess].m_nProcess = nProcess;

	return 0;
}

int CFJSS::AddProcess(int nJobIndex, int nProcessIndex, int nMachineIndex,CFN& fPT)
//#m_areaJobNode
{
	// Kroky v pridani operace:
	// 1. pridani operace do area nodu

	CString strName = _T("Operace ");

	//1.
	int nNodeIndex = (int)m_areaJobNode[nJobIndex].Add(CNode());
	strName.AppendFormat(_T("%d-%d"),nJobIndex,nNodeIndex);

	m_areaJobNode[nJobIndex][nNodeIndex].m_strName = strName;
	m_areaJobNode[nJobIndex][nNodeIndex].m_nJob = nJobIndex;
	m_areaJobNode[nJobIndex][nNodeIndex].m_nProcess = nProcessIndex;
	m_areaJobNode[nJobIndex][nNodeIndex].m_nMachine = nMachineIndex;
	
	
	// C/F = OK
	for (int i=0; i<m_nPTPart; i++)
		m_areaJobNode[nJobIndex][nNodeIndex].m_fPT[i] = fPT[i];

	SetModified(TRUE);
	return 0;
}


int CFJSS::RemoveMachine(int nMachine)
{
	// 1. zruseni v lst_machine
	// 2. zruseni v areaJobnode
	// - uprava indexu masin v nodech
	// - precislovani indexu operaci v nodech

	// 1. 
	m_lstMachine.RemoveAt(nMachine);

	// 2.
	for (int i=m_areaJobNode.GetSize()-1; i> -1; i--)
		for (int j=m_areaJobNode[i].GetSize()-1; j>-1; j--)
		{
			if (m_areaJobNode[i][j].m_nMachine == nMachine)
				m_areaJobNode[i].RemoveAt(j);
			else
				if (m_areaJobNode[i][j].m_nMachine > nMachine)
					m_areaJobNode[i][j].m_nMachine --; 

		}

		// Precislovani indexu procesu
		for (int i=0; i<m_areaJobNode.GetSize(); i++)
			for (int j=0; j<m_areaJobNode[i].GetSize(); j++)
				m_areaJobNode[i][j].m_nProcess = j;


	m_nMachine--;
	
	SetModified(TRUE);

	return 0;
}


int CFJSS::RemoveJob(int nJob)
{
	// 1. zruseni v lst_job
	// 2. zruseni v areaJobnode
	// - uprava indexu v nasledujicich jobech --

	// 1. 
	m_lstJob.RemoveAt(nJob);

	// 2.
	m_areaJobNode.RemoveAt(nJob);
	for (int i=nJob; i<m_areaJobNode.GetSize(); i++)
		for (int j=0; j<m_areaJobNode[i].GetSize();j++)
			m_areaJobNode[i][j].m_nJob--;

	m_nJob--;
	
	SetModified(TRUE);

	return 0;
}

int CFJSS::RemoveProcess(int nJob, int nProcess)
{
	// 1. Vymazani opearce
	// 2. nasledujici operace upravit poradi

	m_areaJobNode[nJob].RemoveAt(nProcess);
	
	for (int i=nProcess; i<m_areaJobNode[nJob].GetSize(); i++)
		m_areaJobNode[nJob][i].m_nProcess--;

	return 0;
}
//-----------------------------------------------
// Counter Functvion
//-----------------------------------------------

int CFJSS::MaxJobProcess()
{
	int nMax = 0;

	for (int i=0; i<m_nJob; i++)
		nMax = max(nMax,(int)m_areaJobNode[i].GetSize());

	return nMax;
}
int CFJSS::MaxMachineProcess(CScheduleArea& areaSchedule)
{
	int nMax = 0;

	for (int i=0; i<m_nMachine; i++)
		nMax = max(nMax,(int)areaSchedule[i].GetSize());

	return nMax;
}

int CFJSS::MaxCPEdges()
{
	int nMax = 0; 
	// C/F = OK
	for (int i=0; i<m_nPTPart; i++)
		nMax = max(nMax,(int)m_lstpCriticalPath[i].GetSize());

	return nMax;

}


//-----------------------------------------------
// Convert Function
//-----------------------------------------------

int CFJSS::ConvertToCrispDD()
{
	int nHi = 0;	// a3 - a3
	int nLo = 0;	// a4 - a1

	for (int i=0; i<m_nJob; i++)
	{
		nHi = (m_lstJob[i].m_fDD.m_a[2] + m_lstJob[i].m_fDD.m_a[1]) / 2;
		nLo = (m_lstJob[i].m_fDD.m_a[3] + m_lstJob[i].m_fDD.m_a[0]) / 2;
		m_lstJob[i].m_fDD[0] = (nHi + nLo) / 2;
	}

	SetTypeMask(0);	
	return 0;
}
int CFJSS::ConvertToCrispPT()
{
	int nHi = 0;	// a3 - a3
	int nLo = 0;	// a4 - a1

	for (int i=0; i<m_nJob; i++)
		for (int j=0; j<m_areaJobNode[i].GetSize();j++)
		{
			nHi = (m_areaJobNode[i][j].m_fPT.m_a[2] + m_areaJobNode[i][j].m_fPT.m_a[1]) / 2;
			nLo = (m_areaJobNode[i][j].m_fPT.m_a[3] + m_areaJobNode[i][j].m_fPT.m_a[0]) / 2;
			
			m_areaJobNode[i][j].m_fPT[0] = (nHi + nLo) / 2;
		}

	SetTypeMask(1);
	return 1;
}

int CFJSS::ConvertToFuzzyDD()
{
	PrepareSrand();	

	int na = 0;	//stredni hodnota cisla	 |	      ------------------
	int nb = 0;	//a2 = a-c ,a3 = a+c	 |	     /|<--c--> |        
	int nc = 0; //a1 = a-b, a4 = a+b	 |   	/              
	//   							     |      |<----b--->|
	//									 |<-------a------->|
	double fResultL = 0;
	double fResultR = 0;

	int nDD = 0;

	for (int i=0; i<m_nJob; i++)
	{
		nDD = m_lstJob[i].m_fDD[0];

		fResultL = m_fDDCoef * nDD * RandF();
		fResultR = m_fDDCoef * nDD * RandF();

		m_lstJob[i].m_fDD.m_a[0] = nDD - (int)fResultL;
		m_lstJob[i].m_fDD.m_a[3] = nDD + (int)fResultR;

		fResultL = fResultL * RandF();
		fResultR = fResultR * RandF();

		m_lstJob[i].m_fDD.m_a[1] =   nDD - (int)fResultL;
		m_lstJob[i].m_fDD.m_a[2] =   nDD + (int)fResultR;

		m_lstJob[i].m_fDD.Check();

		
	}
	
	// nastavit 0 - crisp DD, 1 - crisp PT, 2 - fuzzy DD, 3 - fuzzy PT
	SetTypeMask(2);
	return 2;
}

int CFJSS::ConvertToFuzzyPT()
{

	PrepareSrand();

	int na = 0;	//stredni hodnota cisla	 |	      ------------------
	int nb = 0;	//a2 = a-c ,a3 = a+c	 |	     /|<--c--> |        
	int nc = 0; //a1 = a-b, a4 = a+b	 |   	/              
	//   							     |      |<----b--->|
	
	double fResultL = 0;
	double fResultR = 0;

	int nPT = 0;

	for (int i=0; i<m_nJob; i++)
		for (int j=0; j<m_areaJobNode[i].GetSize();j++)
		{
			nPT = m_areaJobNode[i][j].m_fPT[0];

			fResultL = m_fPTCoef * nPT * RandF();			
			fResultR = m_fPTCoef * nPT * RandF();			

			m_areaJobNode[i][j].m_fPT.m_a[0] = nPT - (int)fResultL;
			m_areaJobNode[i][j].m_fPT.m_a[3] = nPT + (int)fResultR;

			fResultL = fResultL * RandF();
			fResultR = fResultR * RandF();

			m_areaJobNode[i][j].m_fPT.m_a[1] = nPT  - (int)fResultL;
			m_areaJobNode[i][j].m_fPT.m_a[2] = nPT + (int)fResultR;

			m_areaJobNode[i][j].m_fPT.Check();
		}
	
	// nastavit 0 - crisp DD, 1 - crisp PT, 2 - fuzzy DD, 3 - fuzzy PT
	SetTypeMask(3);
	return 3;
}

// nastavi casy dokonceni u operaci na Ci poslednich operaci 

int CFJSS::SetDefaultDD()
{
	SolvePureEndTime();
	
	for (int i=0; i<m_nJob; i++)
	{
		int nLastIndex  = (int)m_areaJobNode[i].GetSize()-1;
		if (nLastIndex > -1)
		{
			double fCi;
			CFN fPT;
			fPT = m_areaJobNode[i][nLastIndex].m_fC;
			if (FPT())
				fCi = (double) m_areaJobNode[i][nLastIndex].m_fC.Center();
			if (!FPT()) 
				fCi = (double) m_areaJobNode[i][nLastIndex].m_fC[0];

				fCi = fCi + (m_fJobCoef*(double)m_nJob*fCi) + 
							(m_fMachineCoef*(double)m_nMachine*fCi);
				
				m_lstJob[i].m_fDD[0] = (int)fCi;
		}
	}

	if (FDD())
		ConvertToFuzzyDD();

	return 0;
}


//---------------------------------------------------------
// Flag area
//---------------------------------------------------------
BOOL CFJSS::IsAbleResult(int nType)
{
	switch (nType)
	{
	case 0:	
		m_nAbleResult = 0;
		m_nSelectedResult = 0;
		break;
	case 1:		// + SA
		if ((m_nAbleResult == 2) || (m_nAbleResult == 3)) 
			m_nAbleResult = 3;
		else 
		{
			m_nAbleResult = 1;
			m_nSelectedResult = 0;
		}
		break;
	case 2:		// + GA
		if ((m_nAbleResult == 1) || (m_nAbleResult == 3)) 
			m_nAbleResult = 3;
		else 
		{
			m_nAbleResult = 2;
			m_nSelectedResult = 1;
		}
		break;

	case 10: return m_nAbleResult;
	}

	return m_nAbleResult;
}
BOOL CFJSS::IsAbleToSolve()
{
	if ((m_nJob > 0) && (m_nMachine > 0))
		for(int i=0; i<m_nJob; i++)
			if (m_areaJobNode[i].GetSize() > 0)
					return TRUE;
	
	return FALSE;
}

BOOL CFJSS::IsAbleConvResult(int nType)
{
	switch (nType)
	{
	case 0:
		m_bAbleConvResult = FALSE;
		break;
	case 1:
		m_bAbleConvResult = TRUE;
		break;
	}

	return m_bAbleConvResult;
}

BOOL CFJSS::IsAbleCritResult(int nType)
{
	switch (nType)
	{
	case 0:
		m_bAbleCritResult = FALSE;
		break;
	case 1:
		m_bAbleCritResult = TRUE;
		break;
	}

	return m_bAbleCritResult;
}

// urci pouzitelna kriteria pro dany pripad
int CFJSS::SetCriterionFlag()
{
	//A. Kriteria postaven a makespanu
	// CPT/*
	// 0. Makespan
	// FPT/*
	// 1. m(A)
	// 2. w1m + w2s
	// 3. Suma(wiai)
	// 4. Nove kriterium w1M(A) + w2sR(a)

	//B. Kriteria pro FDD & CPT
	// 5. Gi(Ci) = miD(Ci)
		
	//C. Kriteria pro CDD & FPT
	
	//D. Kriteria pro FDD & FPT
	// 6. Sakawa & Mori Gi(Ci) = area(Ci n Di) / area (Ci)
	// 7. Gi(Ci) = area(Ci n Di) / area (Ci u Di)
	// 8. kriterium
	// 9. Nove kriterium Gi(Ci) = wi1 agr(Ci,Di) + wi2(1-dist(Ci,Di))

	for (int i=0; i<CRIT_COUNT; i++) m_bCrit[i]=FALSE;

	if (FPT())
	{
		m_bCrit[1] = TRUE;
		m_bCrit[2] = TRUE;
		m_bCrit[3] = TRUE;
		m_bCrit[4] = TRUE;

		if (FDD())
		{
			m_bCrit[6] = TRUE;
			m_bCrit[7] = TRUE;
			m_bCrit[8] = TRUE;
			m_bCrit[9] = TRUE;
		}
		else
		{
			;
		}
	}
	else
	{
		m_bCrit[0] = TRUE;
		if (FDD())
		{
			m_bCrit[5] = TRUE;
		}
		else
		{
			;
		}
	}

	// Nastav nove aktualni kriterium
	for (int i=0; i<CRIT_COUNT;i++)
	{
		if (m_bCrit[i]) 
		{
			m_nCriterion = i;
			break;
		}
	}

	// vypocitat pocet moznych kriterii
	m_nAbleCriterion = 0;
	for (int i=0; i<CRIT_COUNT; i++)
		if (m_bCrit[i]) 
		{
			m_lstnAbleCriterion[m_nAbleCriterion] = i;
			m_nAbleCriterion++;
		}

	return 0;

}

//-----------------------------------------------
// Removing Resources
//-----------------------------------------------
int CFJSS::SetDefaultSpecialNode()
{
	m_ndStart.Void();
	m_ndStart.m_nJob = -1;
	m_ndStart.m_nProcess = -1;
	m_ndStart.m_nMachine = -1; 

	m_ndEnd.Void();
	m_ndEnd.m_nJob = -2;
	m_ndEnd.m_nProcess = -2;
	m_ndEnd.m_nMachine = -2; 

	return 0;
}



void CFJSS::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{	// storing code
		// Ukládat se bude
		// 1. Název projektu, datum, provedl
		// 2. Typ, jobu, stroju
		// 3. Nazvy vsech stroju 
		// 4. Nazvy vsech jobu
		// 5. dd, alfa, beta
		// 6. nazev operace, machine, dd
		ar << m_strName;
		ar << m_strDate;
		ar << m_strMaker;

		ar << m_nType;
		ar << m_nJob;
		ar << m_nMachine;

		m_lstMachine.Serialize(ar);
		m_lstJob.Serialize(ar);
		m_areaJobNode.Serialize(ar);
		
	}
	else
	{	
		DeleteAll();
		// loading code
		ar >> m_strName;
		ar >> m_strDate;
		ar >> m_strMaker;

		ar >> m_nType;
		ar >> m_nJob;
		ar >> m_nMachine;

		m_lstMachine.Serialize(ar);
		m_lstJob.Serialize(ar);
		m_areaJobNode.Serialize(ar);

		// Nastaveni prislusnych flagu
		SetTypeProp(m_nType);		// nastaveni m_nPTpart, a DDPart
		SetCriterionFlag();			// nastaveni moznych kriterii

	}
}

BOOL CFJSS::SetModified(BOOL bModified)
{
	// Pri modifikaci zakladu problemu, tzn. uprava jakychkoli vstupnich dat
	// 1. vyusit udaje o prubehu 

	if (bModified)
	{
		m_areaAxis.RemoveAll();
		IsAbleResult(0);
		IsAbleConvResult(0);
		IsAbleCritResult(0);
	}

	m_bIsModified = bModified;
	return bModified;
}