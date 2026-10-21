// FJSSCompute.cpp : implementation file
//

#include "stdafx.h"
#include "FJSS.h"
#include "math.h"
#include "SAdlg.h"
#include "GAdlg.h"
#include "ConvExpDlg.h"
#include "CritExpDlg.h"

#include <stdio.h>
#include <sys/timeb.h>
#include <time.h>

int CFJSS::GenerateBeginSchedule(CPtrNodeArea& areapMachineNode)
{
	// Generovani nahodneho rozvrhu algoritmem G&T
	SolvePureEndTime();									
    ActiveScheduleAlgoritm(areapMachineNode);			

	return 0;
}

double CFJSS::GetVOOF(CPtrNodeArea& areapMachineNode)
{

	double fResult;

start:
    // Generovani uzlu a hran
	GenerateNodePtrList(m_lstpNode);								
	GenerateConEdges(m_areaConEdge);				
	GenerateDisEdges(areapMachineNode,m_areaDisEdge);						
	GenerateEdgePtrList(m_areaConEdge,m_areaDisEdge,m_lstpEdge);		
	
	// Test acyklicnosti a setrideni uzlu i < j, nemelo by to nastat, ale co kdyby
	if (OrderAndCheckCycle(m_lstpNode,m_lstpEdge) == -1) 
	{
		GenerateBeginSchedule(areapMachineNode);
		goto start;
	}

	// CPM a urceni kriticke cesty
	SolveCPM(m_lstpNode,m_lstpEdge,areapMachineNode);
	
	fResult = SolveObjectiveFunction(areapMachineNode);
	GenerateCriticalPath();

	return fResult;
}

int CFJSS::SolvePureEndTime()
// #using m_areaJobNode
{
	for (int i=0; i<m_nJob; i++)
		for (int j=0; j<m_areaJobNode[i].GetSize();j++)
		{
			// Hrana ij
			// rj = Ci;		// mimo prvniho uzlu
			// Cj = rj + PTj;
									
			// C/F = OK
			for (int k=0; k<m_nPTPart;k++)
			{
				if (j)	m_areaJobNode[i][j].m_fr[k] = m_areaJobNode[i][j-1].m_fC[k];
				else	m_areaJobNode[i][j].m_fr[k] = 0;

				m_areaJobNode[i][j].m_fC[k] = m_areaJobNode[i][j].m_fr[k] + m_areaJobNode[i][j].m_fPT[k];
			}
			
		}
	return 0;
}


int CFJSS::ActiveScheduleAlgoritm(CPtrNodeArea& areapMachineNode)
// #using m_areaJobNode
{
	// Alg:
	// 
	// nachystat random
	// vynulovat machinelist

	// 1. pridani do kofliktni mnoziny vsechy rozvrhovatelne operace v technologickem sledu
	// 2. vypocitat casy dokonceni operaci v rezu C
	// 3. najit operaci s nejmensim casem dokonceni a najit konfliktni mnozinu
	// 4. nahodne vybrat operaci z konfl mnoziny
	// 5. upravit casy dokonceni v konfliktni mnozine

	PrepareSrand();

	areapMachineNode.RemoveAll();

	for (int i=0; i<m_nMachine; i++)
		areapMachineNode.Add(CPtrNodeList());

	CTypedPtrArray <CObArray,CNode*> lstpCCut;
	CUIntArray lstnConflictSet;
	CUIntArray lstnNextProcess;

	for (int i=0; i<m_nJob; i++) lstnNextProcess.Add(0);	// indexy na dalsi operaci v m_areaJobNode

	// 1. 
	for (int i=0; i<m_nJob; i++)						// pridani prvnich operaci do rezu C
		if (m_areaJobNode[i].GetSize())
			lstpCCut.Add(&m_areaJobNode[i][0]);

	// 2. 
	for (int i=0; i<lstpCCut.GetSize(); i++)				// urceni casu dokonceni operaci v rezu C
		for (int j=0; j<m_nPTPart;j++)					// C/F = OK
			lstpCCut[i]->m_fC[j] = lstpCCut[i]->m_fr[j] + lstpCCut[i]->m_fPT[j];

	// CYKLUS
	while (lstpCCut.GetSize())
	{
		CFN fCmin;
		fCmin = -1;
		int nMachineIndex = 0;

		for (int i=0; i<lstpCCut.GetSize();i++)				// vybrani operace s nejmensim C[0]
			if ((lstpCCut[i]->m_fC[0] < fCmin[0]) || (fCmin[0] == -1))
			{
				for (int k=0; k<m_nPTPart; k++)			// C/F = OK
					fCmin[k] = lstpCCut[i]->m_fC[k];

				nMachineIndex = lstpCCut[i]->m_nMachine;
			}			

		lstnConflictSet.RemoveAll();						
		

		for (int i=0; i<lstpCCut.GetSize();i++)
		{
			if (lstpCCut[i]->m_nMachine == nMachineIndex)
				for (int j=0; j<m_nPTPart;j++)			// C/F = OK
				{
					if (lstpCCut[i]->m_fr[j] <= fCmin[j])
						lstnConflictSet.Add(i);
						break;
				}
			
		}

		// 4.
		int nRandMax = (int) (lstnConflictSet.GetSize() -1);
		//if (nRandMax < 0 ) return 0;	// TEST
		int nRand = RandI(nRandMax);
		int nConflictIndex = lstnConflictSet[nRand];

		CFN fC;

		for (int k=0; k<m_nPTPart; k++)								// C/F = OK
			fC[k] = lstpCCut[nConflictIndex]->m_fC[k];				// C vybrane operace
		int nConflictJob = lstpCCut[nConflictIndex]->m_nJob;
		
		// index na dalsi operaci ve vybranem jobu
		lstnNextProcess[nConflictJob]++;
		lstnConflictSet.RemoveAt(nRand);

		// 5.
		for (int i=0; i<lstnConflictSet.GetSize();i++)
		{
			for (int j=0; j<m_nPTPart;j++)
			{
				lstpCCut[lstnConflictSet[i]]->m_fr[j] = 
					max( lstpCCut[lstnConflictSet[i]]->m_fr[j] , fC[j] );
				lstpCCut[lstnConflictSet[i]]->m_fC[j] = lstpCCut[lstnConflictSet[i]]->m_fr[j] + 
										 lstpCCut[lstnConflictSet[i]]->m_fPT[j];
			}

		}

		// *** pridani vybrane operace do areapMachineNode, urcuje hranu		
		areapMachineNode[nMachineIndex].Add(lstpCCut[nConflictIndex]);
		

		lstpCCut.RemoveAt(nConflictIndex);
		// pridani do CCutu dalsi operace
		if ((int)m_areaJobNode[nConflictJob].GetSize() > (int)lstnNextProcess[nConflictJob])		
		{
			
			int nIndex = (int)lstpCCut.Add(&m_areaJobNode[nConflictJob][lstnNextProcess[nConflictJob]]);
			for (int i=0; i<m_nPTPart; i++)
				lstpCCut[nIndex]->m_fC[i] = lstpCCut[nIndex]->m_fr[i] + lstpCCut[nIndex]->m_fPT[i];
		}
	}

	return 0;
}

int CFJSS::GenerateNodePtrList(CPtrNodeList& lstpNode)
{
	m_lstpNode.RemoveAll();

	// prvni uzel 0
	lstpNode.Add(&m_ndStart);

	// ostatni uzly
	for (int i=0; i<m_nJob; i++)
		for (int j=0; j<m_areaJobNode[i].GetSize();j++)
			lstpNode.Add(&m_areaJobNode[i][j]);

	// posledni uzel *
	lstpNode.Add(&m_ndEnd);

	return 0;
}



int CFJSS::GenerateEdgePtrList(CEdgeArea& areaConEdge,CEdgeArea& areaDisEdge,CPtrEdgeList& lstpEdge)
{
	m_lstpEdge.RemoveAll();

	// konjunktivni hrany
	for (int i=0; i<areaConEdge.GetSize(); i++)
		for (int j=0; j<areaConEdge[i].GetSize(); j++)
			lstpEdge.Add(&areaConEdge[i][j]);

	// disjunktivni hrany
	for (int i=0; i<areaDisEdge.GetSize(); i++)
		for (int j=0; j<areaDisEdge[i].GetSize(); j++)
			lstpEdge.Add(&areaDisEdge[i][j]);

	return 0;
}

int CFJSS::GenerateConEdges(CEdgeArea& areaConEdge)
{	
	m_areaConEdge.RemoveAll();
	// EdgeList
	for (int i=0; i<m_nJob; i++)
		areaConEdge.Add(CEdgeList());	

	// 0 - ..
	for (int i=0; i<m_nJob; i++)
	{
		int nIndex = (int)areaConEdge[i].Add(CEdge());
		areaConEdge[i][nIndex].m_pNode1 = &m_ndStart;

		if (!m_areaJobNode[i].GetSize())
			areaConEdge[i][nIndex].m_pNode2 = &m_ndEnd;
		else
		{
			areaConEdge[i][nIndex].m_nList = i;
			areaConEdge[i][nIndex].m_nItem = -1;
			areaConEdge[i][nIndex].m_pNode2 = &m_areaJobNode[i][0];
		}
	}
	// 11 - 12
	for (int i=0; i<m_nJob; i++)
		for (int j=0; j<m_areaJobNode[i].GetSize()-1; j++)
		{
			int nIndex = (int)areaConEdge[i].Add(CEdge());
			areaConEdge[i][nIndex].m_nList = i;
			areaConEdge[i][nIndex].m_nItem = j;
			areaConEdge[i][nIndex].m_pNode1 = &m_areaJobNode[i][j];
			areaConEdge[i][nIndex].m_pNode2 = &m_areaJobNode[i][j+1];
		}

	// .. - *
	for (int i=0; i<m_nJob; i++)
	{
		int nLast = (int)m_areaJobNode[i].GetSize() - 1;
		if (nLast > -1)
		{
			int nIndex = (int)areaConEdge[i].Add(CEdge());
			areaConEdge[i][nIndex].m_nList = i;
			areaConEdge[i][nIndex].m_nItem = -2;
			areaConEdge[i][nIndex].m_pNode1 = &m_areaJobNode[i][nLast];
			areaConEdge[i][nIndex].m_pNode2 = &m_ndEnd;
		}
	}
		
	return 0;
}

int CFJSS::GenerateDisEdges(CPtrNodeArea& areapMachineNode,CEdgeArea& areaDisEdge)
{

	m_areaDisEdge.RemoveAll();

	for (int i=0; i<m_nMachine; i++)
		areaDisEdge.Add(CEdgeList());

	for (int i=0; i<areapMachineNode.GetSize(); i++)
		for (int j=0; j<areapMachineNode[i].GetSize()-1; j++)
		{			
			int nIndex = (int)areaDisEdge[i].Add(CEdge());
			areaDisEdge[i][nIndex].m_bCon = FALSE;
			areaDisEdge[i][nIndex].m_nList = i;
			areaDisEdge[i][nIndex].m_nItem = j;
			areaDisEdge[i][nIndex].m_pNode1 = areapMachineNode[i][j];
			areaDisEdge[i][nIndex].m_pNode2 = areapMachineNode[i][j+1];
		}

	return 0;
}


int CFJSS::OrderAndCheckCycle(CPtrNodeList& r_lstpNode,CPtrEdgeList& r_lstpEdge)
{
	// Topologicke usporadani acyklickeho grafu - prevzato z Jiri Deml - Topologie grafu
	//
	// 1. Vypocet vstupich stupnu, vsechny V -> C(x) = 0,
	//    pro vsechny hrany C(KV(e)) = C(KV(e)) +1
	// 2. Inicializace mnoziny M dame vsechny vrcholy x takove, ze C(x) = 0
	// 3. Vyber vrcholu, je-li M prazdna mnozina, konec vypoctu
	//    jinak odstranime z M nektery vrchol, oznacime jej x a zaradime na konec posloupnosti vrch.
	// 4. Zpracovani hran vychazejicich z vrcholu x - pro kazdou hranu vychazejici z x provedeme 5.
	//    a pak pokracujeme 3.
	// 5. Zpracovani hrany e. Zaradime hranu e na konec posloupnosti hran, polozime 
	//    C(KV(e)) --; a jestlize nyni C(KV(e)) = 0; zaradime KV(e) do M
	// 
	// casove naroky O(m+n), kde n je pocet vrcholu a m pocet hran

	// vynulovani setridenych seznamu
	CPtrNodeList						lstpNode;
	CPtrEdgeList						lstpEdge;

	// vytvoreni kopie
	lstpNode = r_lstpNode;
	lstpEdge = r_lstpEdge;

	// vymazani 
	r_lstpNode.RemoveAll();
	r_lstpEdge.RemoveAll();


	//1.
	// C(x) = 0; vytvoreni pole ukazatelu na uzly
	for (int i=0; i<lstpNode.GetSize();i++)
		lstpNode[i]->m_nPrice = 0;
	

	// C(KV(e)) ++
	for (int i=0; i<lstpEdge.GetSize();i++)
			lstpEdge[i]->m_pNode2->m_nPrice++;

	//2.
	// Mnozina M vrcholu s bez vstupu
	CTypedPtrArray<CObArray,CNode*> lstpMSet;
	lstpMSet.RemoveAll();

	for (int i=0; i<lstpNode.GetSize();i++)
			if (!lstpNode[i]->m_nPrice)
				lstpMSet.Add(lstpNode[i]);

	//3.
	while (lstpMSet.GetSize())
	{
		//4.
		// pro kazdou hranu provedeme
		for (int i=0; i<lstpEdge.GetSize();i++)

			//5.
			if  ((lstpMSet[0]->m_nJob == lstpEdge[i]->m_pNode1->m_nJob ) &&
				(lstpMSet[0]->m_nProcess == lstpEdge[i]->m_pNode1->m_nProcess ) &&
				(lstpMSet[0]->m_nMachine == lstpEdge[i]->m_pNode1->m_nMachine ))
			{
			  // zaradime hranu ktera ma vstupni uzel MSet[0] na konec posloupnosti hran
			  r_lstpEdge.Add(lstpEdge[i]);			  
			  // snizime hodnotu C(KV(e)) - 1
			  lstpEdge[i]->m_pNode2->m_nPrice--;
			  // pokud uz pote 0, pridame vrchol do M
			  if (lstpEdge[i]->m_pNode2->m_nPrice == 0 ) 
				  lstpMSet.Add(lstpEdge[i]->m_pNode2);	  						
			}

		// zaradit uzal na konec posloupnosti vrcholu
		r_lstpNode.Add(lstpMSet[0]);
		// vyjmout z mnoziny M
		lstpMSet.RemoveAt(0);
	}
	
	// TestAcyklicnosti
	int nOrderNode = (int)r_lstpNode.GetSize();
	int nNode = (int)lstpNode.GetSize();

	int nOrderEdge = (int)r_lstpNode.GetSize();
	int nEdge = (int)lstpNode.GetSize();
	
	int nDelta = nNode - nOrderNode;

	if (nDelta > 0) 
	{
		/*
		for (int i=0; i<r_lstpNode.GetSize();i++)
			TRACE("%d, %d, %d\n",r_lstpNode[i]->m_nJob,r_lstpNode[i]->m_nProcess,r_lstpNode[i]->m_nMachine);
	

		CString str;
		str.Format(_T("Pøi poslední operaci s Disjunktivnim grafem\t\n")
				   _T("vznikly v grafu cykly\n\n ")
				   _T("Setridene uzly =  %d\n")
				   _T("Setridene hrany =  %d\n")
				   _T("Nesetridene  uzly =  %d\n")
				   _T("Nesetridene hrany =  %d\n")
				   _T("Rozdil uzlu =  %d\n"),

				   nOrderNode, nOrderEdge,nNode,nEdge,nDelta);

			   
		AfxMessageBox(str,MB_OK | MB_ICONERROR);
	*/
		return -1;
	}
	return 0;
}



int CFJSS::SolveCPM(CPtrNodeList& lstpNode,CPtrEdgeList& lstpEdge,CPtrNodeArea& areapMachineNode)
{
	// Postup:
	// 1. vynulovani pocatecnich a koncovych casu
	// 2. prepocitani konfliktnich casu zahajeni na strojich
	// 3. CPM vypocet pocatecnich a koncovych casu
	// 4. Zapsani dosazeneho makespanu do Rozvrhu

	// 1.
	// C/F = OK
	m_ndStart.m_fC = 0;
	m_ndStart.m_fr = 0;
	m_ndEnd.m_fC = 0;
	m_ndEnd.m_fr = 0;
	for (int i=0; i<lstpNode.GetSize();i++) 
		for (int j=0; j<m_nPTPart; j++)
		{
			lstpNode[i]->m_pPrevEdge[j] = NULL;
			lstpNode[i]->m_fr[j] = 0;
			lstpNode[i]->m_fC[j] = 0;
		}



	// 2.
	// 3.
	// C/F = OK

	for (int i=0; i<lstpEdge.GetSize(); i++)
	{

		CNode& node1 = *lstpEdge[i]->m_pNode1;
		CNode& node2 = *lstpEdge[i]->m_pNode2;
		
		for (int j=0; j<m_nPTPart; j++)
		{
			node1.m_fC[j] = node1.m_fr[j] + node1.m_fPT[j];
			// zmena predchudce, je treba <= pro prepsani hran 0-...
			if (node2.m_fr[j] <= node1.m_fC[j]) 
				node2.m_pPrevEdge[j] = lstpEdge[i];

				node2.m_fr[j] = max (node1.m_fC[j],node2.m_fr[j]);
				node2.m_fC[j] = node2.m_fr[j] + node2.m_fPT[j];
		}
		
	}

	// 4 .Ulozit Makespan do rozvrhu
	for (int i=0; i<m_nPTPart; i++)
		areapMachineNode.m_fMakespan[i] = m_ndEnd.m_fC[i];
	
	TRACE("CPM Makespan = %d\n",m_ndEnd.m_fC[0]);


	return 0;
}

int CFJSS::GenerateCriticalPath()
{
	TRACE("Critical Path");
	// 1. Od uzlu * k 0 ukladat hrany -->   * ---> 0
	// 2. Prohodit poradi					0 ---> *

	// 1.
	// C/F = OK	, vyreseno m_nPTPart
	for (int i=0; i<FN_PART; i++) m_lstpCriticalPath[i].RemoveAll();
	// zapsani kriticke cesty do lstpCriticalPath
	CNode* pNode[FN_PART];
	
	for (int i=0; i<m_nPTPart; i++)
	{
		pNode[i] = m_pLastCritNode;
		//if (!pNode[i]) return 0;	// TEST
		while (pNode[i]->m_pPrevEdge[i])
		{
			m_lstpCriticalPath[i].Add(pNode[i]->m_pPrevEdge[i]);
			pNode[i] = pNode[i]->m_pPrevEdge[i]->m_pNode1;	
		}
	}

	// 2. kriticka cesta je od konce k zacatku, prevratim tedy poradi
	for (int i=0; i<m_nPTPart; i++)
	{
		for (int j = (int)m_lstpCriticalPath[i].GetSize();j>0;j--)
			m_lstpCriticalPath[i].Add(m_lstpCriticalPath[i][j-1]);

		m_lstpCriticalPath[i].RemoveAt(0,m_lstpCriticalPath[i].GetSize()/2);
	}



	return 0;
}


//*********************************************************
// Ucelove funkce (jednotliva kriteria)
//*********************************************************

double CFJSS::ObjectiveFunction0(int nCi)
{
	// Crisp Makespan
    // CPT/*
	// snazime se min(max Makespan)
	// nebo       min(sum Makespan)
	// posleni kriticky uzel = uzel *

	// Max/Sum
	// Ucelova funkce je termin dokonceni posledni operaci

	return (double)nCi;
}

double CFJSS::ObjectiveFunction1(CFN& fCi)
{

	// m(A)
	// Fuzzy Makespan
    // FPT/*
	// snazime se min(max m(A))
	// nebo       min(sum m(A))
	// posledni uzel na kriticke ceste, uzel s Max(m(A))

	// Max/Sum
	// Ucelova funkce je termin dokonceni posledni operaci
	
	// Max/Suma
	double fResult;
		
	fResult = fCi.Mean();

    return fResult;

}

double CFJSS::ObjectiveFunction2(CFN& fCi)
{
	// w1m(A)+w2s(A)
	// Fuzzy Makespan
    // FPT/*
	// snazime se min(max w1m+w2s)
	// nebo       min(sum w1m+w2s)

	// Max/Sum
	// Ucelova funkce je termin dokonceni posledni operaci
	
	// Max/Suma
	double fResult;

	fResult = (double) ( (m_fw1m * fCi.Mean()) + (m_fw2s * fCi.Devi()) );

	return fResult;

}

double CFJSS::ObjectiveFunction3(CFN& fCi)
{
	// w1m(A)+w2sR(A)
	// SR(A) = Int(xmiA(x)dx,x=m(a)..a(4))

	// Fuzzy Makespan
    // FPT/*
	// snazime se min(max w1m+w2s)
	// nebo       min(sum w1m+w2s)

	// Max/Sum
	// Ucelova funkce je termin dokonceni posledni operaci
	
	// Max/Suma


	double fResult = 0;

	fResult = (m_fw1m * fCi.Mean()) + (m_fw2s * fCi.RDevi());

	return fResult; 
}

double CFJSS::ObjectiveFunction4(CFN& fCi)
{
	// w1a(4)+w2a(2)+w3a(2)+w4a(1)
	// Fuzzy Makespan
    // FPT/*
	// snazime se min(max (w1a(4)+w2a(2)+w3a(2)+w4a(1))
	// nebo       min(sum (w1a(4)+w2a(2)+w3a(2)+w4a(1))

	// posledni uzel na kriticke ceste = uzel s max(F(ci))

	// Max/Sum
	// Ucelova funkce je termin dokonceni posledni operaci
	
	// Max/Suma
	double fResult;

	fResult = (double) (m_fw1 * fCi[3] + m_fw2 * fCi[2] + 
						m_fw3 * fCi[1] + m_fw4 * fCi[0]);

	return fResult;

}

double CFJSS::ObjectiveFunction5(int nCi,CFN& fDi,double fAlfai,double fBetai)
{
	// Ucelova funkce pro Fuzzy DD / Crisp PT, 
	// snazime se max(min Spokojenost)
	// nebo       max(sum Spokojenost)
	// Prevedni na min. problem je min(-(min Spokojenost))
	// resp.					   min(-(sum Spokojenost))

	// Je treba pouzit rozsirenou fci prislusnosti
	//            alfai(t-di(1))
	//  vDi  =    miD(t)
	//            betai(di(4)-t))

	// Max/Suma
	double fResult;

	fResult = (-1.0)*ExtendedFNFx(fDi,nCi,fAlfai,fBetai);

	return fResult;
}

double CFJSS::ObjectiveFunction6(CFN& fCi,CFN& fDi)
{
	// area (Ci n Di) / area (Ci), FuzzyPT / FuzzyDD
	// snazime se max(min Spokojenost)
	// nebo       max(sum Spokojenost)
	// Prevedni na min. problem je min(-(min Spokojenost))
	// resp.					   min(-(sum Spokojenost))

	double fResult = 0;
	double fTest = FNArea(fCi);
	if (fTest  == 0) AfxMessageBox(_T("Cybla "),MB_OK);

	fResult = (-1.0) * ( FNSpillover(fCi,fDi) / FNArea(fCi) );

	return fResult;
}

double CFJSS::ObjectiveFunction7(CFN& fCi,CFN& fDi)
{
	// area (Ci n Di) / area (Ci u Di), FuzzyPT / FuzzyDD
	// snazime se max(min Spokojenost)
	// nebo       max(sum Spokojenost)
	// Prevedni na min. problem je min(-(min Spokojenost))
	// resp.					   min(-(sum Spokojenost))
	
	double fResult;

	fResult = (-1.0)*( FNSpillover(fCi,fDi) / (FNArea(fCi) + FNArea(fDi) - FNSpillover(fCi,fDi)));
	
	return fResult;

}

double CFJSS::ObjectiveFunction8(CFN& fCi,CFN& fDi,double fAlfai,double fBetai)
{
	// Kriterium 8
	//	F(Ci) = area(CiE) + area(CiT) - area(CiD) + alfai*suma(max(0,di(1)-di(j))
	//									          + betai*suma(max(0,ci(j)-di(4))

	double fResult;
	double fAI = FNSpillover(fCi,fDi);		
	double fAlfaPenality = 0;
	double fBetaPenality = 0;

	for (int i=0; i<FN_PART; i++)
	{
		fAlfaPenality +=  max(0,fDi[0] - fCi[i]);
		fAlfaPenality +=  max(0,fCi[i] - fDi[3]);
	}

	fAlfaPenality = fAlfai * fAlfaPenality;
	fBetaPenality = fBetai * fBetaPenality;

	fResult = FNArea(fCi) - fAI + fAlfaPenality + fBetaPenality;

	return fResult;
}

double CFJSS::ObjectiveFunction9(CFN& fCi,CFN& fDi,double fAlfai,double fBetai,
								 double fLambda1i, double fLambda2i, double fLambda3i)
{
	double fResult;

	double fDist = (fLambda1i * fabs((fDi.Mean() - fCi.Mean()))) +
		(fLambda2i * FNLeft(fCi,fDi)) + (fLambda3i * FNRight(fCi,fDi));

	fResult = (fAlfai * FNSpillover(fCi,fDi)) + (fBetai * (1 - fDist));

	return (-1.0)*fResult;
}


int CFJSS::SolveAmongAbleCriterion(CPtrNodeArea& areapMachineNode)
{
	int nBackupCrit = m_nCriterion;

	if (m_nCREActIter < m_nCREIteration)
	{
		int nIndex = m_areafIteration.Add(CDoubleList());
		for (int i=0; i<m_nAbleCriterion; i++)
		{
			m_nCriterion = m_lstnAbleCriterion[i];
			m_areafIteration[nIndex].Add(GetVOOF(areapMachineNode));
		}

		m_nCriterion = nBackupCrit;
		m_areafIteration[nIndex].Add(m_nCriterion);
		m_areafIteration[nIndex].Add(GetVOOF(areapMachineNode));
	}
	
	m_nCREActIter++;
	return 0;
}

double CFJSS::SolveObjectiveFunction(CPtrNodeArea& areapMachineNode)
{
	// Rezimy :
	// 1. Prosty rezim -  pocita se pouze vybrana ucelova funkce
	// 2. Experimentalni rezim - pocitaji se vsechny ostatni mozna kriteria

	double fResult = 0;		// vysledek 
	double fValue = 0;		// poseldni hodnota ucelove fce
	double fMaxValue = 0;	// maximalni hodnota pro pripad sum
	CFN fCi;
	CFN fDi;
	double fAlfai;
	double fBetai;
	double fLambda1i;
	double fLambda2i;
	double fLambda3i;
	
	int nIndex;
	int nCIndex;

	BOOL bFirst = TRUE;		// priznak prvniho vypoctu ucelove fce, je max vzdycky

	// 0. CPT Makespan
	// 1. m(A)
	// 2. w1M + w2S
	// 3. w1m(a)+w2(sr(A)
	// 4. Suma(wi + a(i))
	// 5. Gi(Ci) = miDi(Ci)
	// 6. Gi(Ci) = area(Ci n Di)/area (Ci)
	// 7. Gi(Ci) = area(Ci n Di)/area (Ci u Di)
	// 8. kriterium
	// 9. Gi(ci) = wi1agr(Ci,Di) + wi2(1-distCi,Di)


for (int i=0; i<m_areaJobNode.GetSize(); i++)
{
	int nLastIndex = m_areaJobNode[i].GetSize()-1;
	if (nLastIndex > -1)
	{	
		// Nastavi parametry pro ucelovou funkci
		fCi = m_areaJobNode[i][nLastIndex].m_fC;
		fDi = m_lstJob[i].m_fDD;
		fAlfai = m_lstJob[i].m_fAlfa;
		fBetai = m_lstJob[i].m_fBeta;
		fLambda1i = m_lstJob[i].m_fLambda1;
		fLambda2i = m_lstJob[i].m_fLambda2;
		fLambda3i = m_lstJob[i].m_fLambda3;


		// vypocita konkretni ucelovou funkci (vsechny jsou prevedeny na min. problem)
		// simple solve or konvergence solve
		switch (m_nCriterion)
		{
			case 0: fValue = ObjectiveFunction0(fCi[0]); break;		
			case 1: fValue = ObjectiveFunction1(fCi); break;		
			case 2: fValue = ObjectiveFunction2(fCi); break;		
			case 3: fValue = ObjectiveFunction3(fCi); break;
			case 4: fValue = ObjectiveFunction4(fCi); break;		
			case 5: fValue = ObjectiveFunction5(fCi[0],fDi,fAlfai,fBetai); break;
			case 6: fValue = ObjectiveFunction6(fCi,fDi); break;
			case 7: fValue = ObjectiveFunction7(fCi,fDi); break;
			case 8: fValue = ObjectiveFunction8(fCi,fDi,fAlfai,fBetai); break;
			case 9: fValue = ObjectiveFunction9(fCi,fDi,fAlfai,fBetai,fLambda1i,fLambda2i,fLambda3i); break;
		}



		// MAX or SUM ???
		if (m_nCritMaxOrSum == 0)		// MAX CASE
		{
			if (bFirst)					// FIRST
			{
				fResult = fValue;
				m_pLastCritNode = &m_areaJobNode[i][nLastIndex];
				bFirst = FALSE;
			}
			else						// OTHER
			{
				if (fValue > fResult)	// byl nalzen horsi pripad
				{
					fResult = fValue;
					m_pLastCritNode = &m_areaJobNode[i][nLastIndex];
				}
			}

		}
		else							// SUMA CASE
		{
			if (bFirst)					// FIRST
			{
				fResult = fValue;
				fMaxValue = fValue;
				m_pLastCritNode = &m_areaJobNode[i][nLastIndex];
				bFirst = FALSE;
			}
			else						// OTHER
			{
				
				if (fValue > fMaxValue)	// Horsi pripad
				{
					fMaxValue = fValue;
					m_pLastCritNode = &m_areaJobNode[i][nLastIndex];
				}
				fResult += fValue;
			}

		}


	}
}

	areapMachineNode.m_fVOOF = fResult;

	return fResult;
}



int CFJSS::SetDefaultSAParam(int nSATime,int nSAMinTemp,double fSATemp,
		int nSACycle,double fSATempCoef,int nSACycleCoef)
{
	m_nSATime = nSATime;
	m_nSAMinTemp = nSAMinTemp;
	m_fSATemp = fSATemp;
	m_nSACycle = nSACycle;
	m_fSATempCoef = fSATempCoef;
	m_nSACycleCoef = nSACycleCoef;

	return 0;
	
}


int CFJSS::SetDefaultGAParam(int nGATime,int nGAPopulation,
		int nGACycle,int nGASelCoef,int nGAMinMSX,
		int nGA_MSMCycle,int nGA_LSCycle)
{
	m_nGATime = nGATime;
	m_nGAPopulation = nGAPopulation;
	m_nGACycle = nGACycle;
	m_nGASelCoef = nGASelCoef;	
	m_nGAMinMSX = nGAMinMSX;
	m_nGA_MSMCycle = nGA_MSMCycle;			
	m_nGA_LSCycle = nGA_LSCycle;		

	return 0;
}



//-----------------------------------------------
// Vytvareni sousedstvi
//-----------------------------------------------
int CFJSS::VicinityS1(CPtrNodeArea& areanSource,CPtrNodeArea& areanDestination)
{
		//--------------------------------
		// Tvorba sousedstvi:
		// S1: Zmena orientace disjunktivni hrany ij na ji na kritice ceste
		

		// S2 : kriticka hrana (i,j) , Tipred(i), Tisuc(i), omezme (i,j) na takove uzly,
		// u nich alespon jedna ze dvojice hran (ipred(i),i) nebo (j,isucc(j)) neni na 
		// kriticke ceste, tj i nebo j josu ulzy na konci kritickeho bloku
		// obratme orientaci hrany (i,j) a take orientaci (ipred(h),h) a (l,isucc(l),
		// jestlize existuji, kde Th je primy predchudce Ti v praci a Tl 
		// je primy naslednik Tj v praci

		// S3: Necht (i,j) je disjunktivni kriticka hrana. Uvazuje vsechy permutace
		// (ipred(i,i,j) a (i,j,isucc(j)

		// S3': S3, ale Ti nebo Tj museji byt uzly na konci kritickeho bloku

		// S4: Pro kazdy uzel Ti v kritickem bloku, posun Ti na uplny zacatek 
		// nebo konec tohoto bloku

		// Použiji: S1 a S4

	/*
	 Problem : Meni se orientace hrany na kriticke ceste;
	 Zmeni se nahodna hrana na prvni kriticke ceste a1
	 Pokud patrila do nektere dalsi kriticke cesty,
	 na teto se jiz hrany neotaceji.
	 Otoci se jen na ceste, ktere touto upravou nebyly 
	 postizeny, dodelat...
	 */

	// Prohodi zmeni smer kriticke hrany na vybrane ceste


	// 1. Vyseparuje disjunktivni hrany ze seznamu kritickych hran
	// 2. Nahodne vybereme jednu hranu a zmenime poradi operaci
	// 3. Zmena smeru nekdy vytvari cykly

	areanDestination = areanSource;

	CPtrEdgeList lstpDisEdge[FN_PART];

	for (int i=0; i<m_nPTPart; i++)
		for (int j=0; j<m_lstpCriticalPath[i].GetSize(); j++)
		{
            CEdge& Edge = *m_lstpCriticalPath[i][j];
			if (!Edge.m_bCon)
				lstpDisEdge[i].Add(&Edge);
		}

	// 0-seznam, tzn. kriticka cesta vede pouze pre conjunktivni hrany
	int nZeroDisBlock = 0;
	for (int i=0; i<m_nPTPart; i++)
	{
		if (!lstpDisEdge[i].GetSize()) nZeroDisBlock++;
		if (nZeroDisBlock == m_nPTPart) return 0;
	}

	// nahodne vybrana kriticka cesta
	int nRandPart; 
	do 
		nRandPart = RandI(m_nPTPart-1);	
	while (!lstpDisEdge[nRandPart].GetSize());

	// nahodny vyber disjuknktivni hrany
	int nRand = RandI((int)lstpDisEdge[nRandPart].GetSize()-1);
	
	
	int nMachine = lstpDisEdge[nRandPart][nRand]->m_nList;
	int nNode = lstpDisEdge[nRandPart][nRand]->m_nItem;

	// prohozeni operaci mezi sebou
	CNode* pFirst = areanDestination[nMachine][nNode];
	CNode* pSecond = areanDestination[nMachine][nNode+1];
	areanDestination[nMachine][nNode] = pSecond;
	areanDestination[nMachine][nNode+1] = pFirst;

	return 0;
}


int CFJSS::GetCriticalBlocks()
{
	// 1. Remove old data
	// 2. Separate critical blocks
	// 3. Remove Block < 3 

	// 1.
	for (int i=0; i<m_nPTPart;i++)
	{
		m_lstnBlockMachine[i].RemoveAll();
		m_lstnBlockBegin[i].RemoveAll();
		m_lstnBlockEnd[i].RemoveAll();
	}

	// 2.
	for (int j=0; j<m_nPTPart; j++)
	{
		int nLastMachine = -10;
		int nBlock = -1;
		for (int i=0; i<m_lstpCriticalPath[j].GetSize(); i++)
		{
			if (!m_lstpCriticalPath[j][i]->m_bCon)
			{
				CEdge& Edge= *m_lstpCriticalPath[j][i];
				if (nLastMachine == Edge.m_nList)
				{
					// Pokracovani bloku
					m_lstnBlockEnd[j][nBlock] = Edge.m_nItem+1;				
				}
				else
				{
					nLastMachine = Edge.m_nList;
					// Novy blok - index stroje, a index zacatku bloku v poradi dis hran
					m_lstnBlockMachine[j].Add(Edge.m_nList);
					m_lstnBlockBegin[j].Add(Edge.m_nItem);
					m_lstnBlockEnd[j].Add(Edge.m_nItem+1);
					nBlock++;
				}

			}
			// konjunktivni hrana, zavedeni noveho bloku
			else
				nLastMachine = -10;
		}
	}

	// 3. vyhozeni bloku s mene nez 3-mi uzly
	for (int j=0; j<m_nPTPart; j++)
	for (int i=(int)m_lstnBlockMachine[j].GetSize()-1; i>-1;i--)
	{
		int nNodeCount = m_lstnBlockEnd[j][i] - m_lstnBlockBegin[j][i] + 1;
		if (nNodeCount < 3)
		{
			m_lstnBlockMachine[j].RemoveAt(i);
			m_lstnBlockBegin[j].RemoveAt(i);
			m_lstnBlockEnd[j].RemoveAt(i);
		}
	}

	ASSERT((m_lstnBlockMachine[0].GetSize() == m_lstnBlockBegin[0].GetSize()) || 
		(m_lstnBlockBegin[0].GetSize() == m_lstnBlockEnd[0].GetSize()));

	return 0;
}


int CFJSS::VicinityS4(CPtrNodeArea& areapSource,CPtrNodeArea& areapDestination)
{
	// Posunuti uzlu v kritickem bloku bud na zacatek nebo na konec bloku
	areapDestination = areapSource;

	GetCriticalBlocks();

	// vyber casti kriticke cesty
	int nPart = RandI(m_nPTPart - 1);

	int nBlockCount = (int)m_lstnBlockMachine[nPart].GetSize();
	if (!nBlockCount) return -1;

	int nBlock = RandI(nBlockCount-1);
	int nMachine = m_lstnBlockMachine[nPart].GetAt(nBlock);

	int nNodeCount = m_lstnBlockEnd[nPart][nBlock] - m_lstnBlockBegin[nPart][nBlock] + 1;
	//int	nNode = Random(nNodeCount-1);		
	int	nNode = RandI(nNodeCount-3);		// prvni a posledni nebrat, vytvari cykly

	int nBeginNode = m_lstnBlockBegin[nPart][nBlock];
	int nActNode = nBeginNode + nNode + 1;	
	//int nActNode = nBeginNode + nNode;	
	int nEndNode = m_lstnBlockEnd[nPart][nBlock];
    
	int nMoveToBegin =  RandI(1);			// presun na Zacatek nebo na Konec
//	if (nActNode == nEndNode) nMoveToBegin = 1;
//	if (nActNode == nBeginNode) nMoveToBegin = 0;


	TRACE ("ToBegin = %d\n",nMoveToBegin);

	TRACE ("\n");

	if (nMoveToBegin)
	{
		CNode* pData = areapDestination[nMachine][nActNode];
		areapDestination[nMachine].RemoveAt(nActNode);
		areapDestination[nMachine].InsertAt(nBeginNode,pData);
	}

	if (!nMoveToBegin)
	{
		CNode* pData = areapDestination[nMachine][nActNode];
		areapDestination[nMachine].RemoveAt(nActNode);
		areapDestination[nMachine].InsertAt(nEndNode,pData);
	}

	TRACE ("\n");

	return 0;
}


int CFJSS::VicinityS4_2(CPtrNodeArea& areapSource,CPtrNodeArea& areapDestination)
{
	// Posunuti uzlu v kritickem bloku bud na zacatek nebo na konec bloku
	areapDestination = areapSource;

	GetCriticalBlocks();

	// vyber casti kriticke cesty
	int nPart = RandI(m_nPTPart - 1);

	int nBlockCount = (int)m_lstnBlockMachine[nPart].GetSize();
	if (!nBlockCount) return -1;

	int nBlock = RandI(nBlockCount-1);
	int nMachine = m_lstnBlockMachine[nPart].GetAt(nBlock);

	int nNodeCount = m_lstnBlockEnd[nPart][nBlock] - m_lstnBlockBegin[nPart][nBlock] + 1;
	//int	nNode = Random(nNodeCount-1);		
	int	nNode = RandI(nNodeCount-1);		// prvni a posledni nebrat, vytvari cykly

	int nBeginNode = m_lstnBlockBegin[nPart][nBlock];
	int nActNode = nBeginNode + nNode;	
	//int nActNode = nBeginNode + nNode;	
	int nEndNode = m_lstnBlockEnd[nPart][nBlock];
    
	int nMoveToBegin =  RandI(1);			// presun na Zacatek nebo na Konec
	if (nActNode == nEndNode) nMoveToBegin = 1;
	if (nActNode == nBeginNode) nMoveToBegin = 0;


	TRACE ("ToBegin = %d\n",nMoveToBegin);

	TRACE ("\n");

	if (nMoveToBegin)
	{
		CNode* pData = areapDestination[nMachine][nActNode];
		areapDestination[nMachine].RemoveAt(nActNode);
		areapDestination[nMachine].InsertAt(nBeginNode,pData);
	}

	if (!nMoveToBegin)
	{
		CNode* pData = areapDestination[nMachine][nActNode];
		areapDestination[nMachine].RemoveAt(nActNode);
		areapDestination[nMachine].InsertAt(nEndNode,pData);
	}

	TRACE ("\n");

	return 0;
}

// najde nejvzdalenejsi, nebo nejblizsi reseni dosazitelne jdou blokovou operci z X,
// pokud nenajde lepsi, X zustane, nMinDistType = 1 --> hleda minimum, 0 --> hleda maximum
int CFJSS::GetMinMaxCBN(CPtrNodeArea& areapParent,CPtrNodeArea& areapX,int nMinDistType)
{
	
	CPtrNodeArea areapNX;		// sousedni reseni
	CPtrNodeArea areapNXBest;	// nejlepsi dosazene reseni

	areapNX = areapX;			
	areapNXBest = areapX;

									// zatim min, resp. max. dosazena vzdalenost
	int nBestDist = GetDisGraphDistance(areapParent,areapNX);
	TRACE("Pocatecni vzdalenost = %d\n",nBestDist);

	int nDist = 0;				// vzdalenost mezi rodicem a novym sousednim resenim

	GetVOOF(areapX);			// zjisti kritickou cestu
	GetCriticalBlocks();		// separuje kriticke bloky

	// zjistit vsechny kombinace rozvrhu a vypocitat odpovidajici DisGraphDistance
	for (int i=0; i<m_nPTPart; i++)
		for (int j=0; j<m_lstnBlockMachine[i].GetSize();j++)
		{
			TRACE("i=%d,j=%d,Size=%d\n",i,j,m_lstnBlockMachine[i].GetSize());
			int nMachine = m_lstnBlockMachine[i][j];
			int nBeginNode = m_lstnBlockBegin[i][j];
			int nEndNode = m_lstnBlockEnd[i][j];

			int nInterNode = m_lstnBlockEnd[i][j] - m_lstnBlockBegin[i][j] - 1;
			TRACE("\nM=%d,B=%d,E=%d,I=%d\n",nMachine,nBeginNode,nEndNode,nInterNode);
			// pocet moznych kombinaci = pocet vnitrni uzlu * 2
			for (int k=0; k<nInterNode; k++)
			{
				int nActNode = nBeginNode + 1 + k;

				// presun na Zacatek

				for (int l=nBeginNode; l<nEndNode+1;l++)
					TRACE("%d",areapNX[nMachine][l]->m_nJob);
				TRACE("\n");

				CNode* pData = areapNX[nMachine][nActNode];
				areapNX[nMachine].RemoveAt(nActNode);
				areapNX[nMachine].InsertAt(nBeginNode,pData);

				for (int l=nBeginNode; l<nEndNode+1;l++)
					TRACE("%d",areapNX[nMachine][l]->m_nJob);
				TRACE("\n");
				
				nDist = GetDisGraphDistance(areapParent,areapNX);

				// Zjistemi MAXimalne vzdaleneho reseni pri presunu NAZACATEK
				if ((nBestDist < nDist) && (!nMinDistType))
				{
					nBestDist =	nDist;
					areapNXBest = areapNX;
				}
				// Zjistemi MINimalne vzdaleneho reseni
				if ((nBestDist > nDist) && (nMinDistType))
				{
					nBestDist =	nDist;
					areapNXBest = areapNX;
				}

                // Presun na konec, operaci presunutou na zacatek bloku da na konec bloku
				areapNX[nMachine].RemoveAt(nBeginNode);
				areapNX[nMachine].InsertAt(nEndNode,pData);

				for (int l=nBeginNode; l<nEndNode+1;l++)
					TRACE("%d",areapNX[nMachine][l]->m_nJob);
				TRACE("\n");


				// Zjistemi MAXimalne vzdaleneho reseni pri presunu NAKONEC bloku
				nDist = GetDisGraphDistance(areapParent,areapNX);

				if ((nBestDist < nDist) && (!nMinDistType))
				{
					nBestDist =	nDist;
					areapNXBest = areapNX;
				}
				// Zjistemi MINimalne vzdaleneho reseni
				if ((nBestDist > nDist) && (nMinDistType))
				{
					nBestDist =	nDist;
					areapNXBest = areapNX;
				}

				// Inverzni operace, presunuti z konce na puvodni pozici
				areapNX[nMachine].RemoveAt(nEndNode);
				areapNX[nMachine].InsertAt(nActNode,pData);

				for (int l=nBeginNode; l<nEndNode+1;l++)
					TRACE("%d",areapNX[nMachine][l]->m_nJob);
				TRACE("\n");
				TRACE("\n");


			}

		}

	if (nMinDistType) TRACE("Dosazena min. vzdalenost = %d\n",nBestDist);
	else TRACE("Dosazena maxn. vzdalenost = %d\n",nBestDist);

	areapX = areapNXBest;

	return nBestDist;
}



int CFJSS::UpdateDialogData(int nRunTime,double fX0,double fXBest,double fTemp,int nActCycle,int nMaxCycle,int nGAAction)
{
	// m_nAction
	// m_nSubAction
	// nGAAction = akce z GA

	switch (m_nAction)
	{
	case 0:			// SA

		m_pSADlg->m_fXBest = fXBest;
		m_pSADlg->m_strActTemp = GetStrNR(fTemp);
		m_pSADlg->m_strfXBest = GetStrNR(fXBest);
		m_pSADlg->m_strCycleProgress.Format(_T("%d z %d"),nActCycle,nMaxCycle);
		m_pSADlg->m_strTimeProgress.Format(_T("%d z %d s"),nRunTime,m_nSATime);
		m_pSADlg->m_prctrlCycle.SetRange(0,nMaxCycle);
		m_pSADlg->m_prctrlCycle.SetPos(nActCycle);
		m_pSADlg->m_prctrlTime.SetPos(nRunTime);
		m_pSADlg->UpdateData(FALSE);
		break;
	case 1:			// GA
		m_pGADlg->m_strCycleProgress.Format(_T("%d z %d"),nActCycle,nMaxCycle);
		m_pGADlg->m_strTimeProgress.Format(_T("%d z %d s"),nRunTime,m_nGATime);
		m_pGADlg->m_strX0.Format(_T("%3.2f"),fX0);
		m_pGADlg->m_strXBest.Format(_T("%3.2f"),fXBest);
		m_pGADlg->m_prctrlTime.SetPos(nRunTime);
		m_pGADlg->m_prctrlCycle.SetPos(nActCycle);
		m_pGADlg->UpdateOperation(nGAAction);

		m_pGADlg->UpdateData(FALSE);
		break;
	case 2:			// Convergence expreriment
		m_pConvExpDlg->m_prctrlPart.SetPos(nRunTime);
		m_pConvExpDlg->UpdateData(FALSE);
		break;
	case 3:			// Criteria experiment
		m_pCritExpDlg->m_prctrlPart.SetPos(m_nCREActIter);
		m_pCritExpDlg->UpdateData(FALSE);
		break;
	}

	return 0;
}

int CFJSS::SaveRunningData(double fExactTime,double fXStar, CPtrNodeArea& areapMachineNode)
{
	int nIndex;

	switch (m_nAction)
	{
	case 0:
	case 1: 
		nIndex = m_areaAxis.GetSize()-1;
		m_areaAxis[nIndex].Add(fExactTime,fXStar,m_fMinDelta);
		break;
	case 2: 
		break;
	case 3:
		SolveAmongAbleCriterion(areapMachineNode);
		break;
	}

	return 0;
}


//---------------------------------------------------------
//
//		Simulovane zihani
//
//---------------------------------------------------------
int CFJSS::SolveSA()
{
	m_nSubAction = 0;

 	PrepareSrand();
	DeleteAllSolving(0);
    	//------------------------------
		// Postup: (Prevzato z J. Burda)
		//
		// 1. Vyber pocatecni reseni x0;
		// 2. Vyber pocatecni hodnotu t;
		// 3. Vyber pocatecni hodnotu n;
		// x* = x0
		// CYKLUS {
		// CYKLUS (n) {
		//  4.  Generuj x1 z S(x0) , tzn. nahodne generovani pripustneho reseni 
		//      z mnoziny sousedu aktualniho reseni
		//  5. delta = f(x1) - f(x0);
		//  Pohyb do lepsiho reseni 
	    //	6. if delta < 0 then x0 = x1; if f(x0) < f(x*) then x* = x0;
		//  Pohyb do horsiho reseni je akceptovan s pravdepodobnosti exp(-delat/t)		
		//     else if exp(delta/t) > random (0,1) then x0 = x1;
		//  ENDCYKLUS }
		// 7. t = a(t); a(t) je funkce redukce teploty
		// 8. n=g(t); g(t) je funkce zmeny delky cyklu
		//  ENDCYKLUS(po splneni podminy ukonceni)
		//  x* = aproximace optimalniho reseni
		// 9. Ulozeni nejlepsiho reseni do Schedule

		// Pozn. pro genericka rozhodnuti:
		// funkce redukce teploty a(t) = at, kde 0 < a < 1, nejlepsi hodnoty 0,8 - 0,99
		// delka cyklu n - lze zvysovat geometricky pri kazde nove teplote g(n) = bn, b>1
		//				 -              aritmeticky g(n) = n + c ( c >0)

		// Problemove specificka rozhodnuti:
		// co nejoptimalneji vyresene struktury mnoziny sousedu

		// 1. pocatecni reseni x0 = nahodne generovane reseni

	CPtrEdgeList lstpEdge;
	CPtrNodeList lstpNode;
	CPtrNodeArea areapX0;
	CPtrNodeArea areapX1;
	CPtrNodeArea areapXStar;
	
	double	t = m_fSATemp;
	int		n = m_nSACycle;

	time_t	nBeginTime = time(NULL);

	double	nDelta = 0;
	double  nDeltaStar = 0;

	double fx0 = 0;
	double fx1 = 0;
	double fxStar = 0;

	int nActCycle = 0;

	if (m_nAction == 0 )
	{
		int nAxisIndex = m_areaAxis.GetSize();
		m_areaAxis.Add(CAxisList());
		m_areaAxis[nAxisIndex].m_strName = GetStrMethod(0);
	}
    
	GenerateBeginSchedule(areapX0);
	fx0 = GetVOOF(areapX0);
	fxStar = fx0;
	areapXStar = areapX0;

	do 
	{
		for (int i=0; i<n; i++)
		{

			VicinityS1(areapX0,areapX1);
			TRACE ("Vicinty\n");
			fx1 = GetVOOF(areapX1);
			for (int z=0; z<5000; z++) ;
			TRACE ("Makespan fx1\n");
			nDelta = fx1 - fx0;
			if (nDelta < 0)	// lepsi reseni
			{
				areapX0 = areapX1;
				fx0 = fx1;

				nDeltaStar = fx0 - fxStar;
				if (nDeltaStar < 0)
				{
					TRACE ("Nalezno nejlepsi reseni fxStar =  %d, \n", fxStar);
					areapXStar = areapX0;
					fxStar = fx0;
				}
			}
			else			// horsi reseni
			{
				double fExp = exp(-nDelta/t);
				double fRand = RandF();				
				TRACE ("Horsi reseni: fExp = %f, fRand = %f, n = %d \n", fExp,fRand,i);
				if ( fExp > fRand)
				{
					areapX0 = areapX1;
					fx0 = fx1;
				}
			}

			SaveRunningData(GetExactTime(nBeginTime),fxStar,areapXStar);
			UpdateDialogData(GetRuntime(nBeginTime),fx0,fxStar,t,i,n,0);			

			
			if (GetRuntime(nBeginTime) > m_nSATime) break;
		}

		t = t * m_fSATempCoef;
		n = n + m_nSACycleCoef;

		UpdateDialogData(GetRuntime(nBeginTime),fx0,fxStar,t,i,n,0);			
	
	} 

while (((GetRuntime(nBeginTime) < m_nSATime) && (t>m_nSAMinTemp) && (m_nAction < 3))
		|| ((m_nAction == 3) && (m_nCREActIter < m_nCREIteration)) );

	// Koncovych dat podle typu problemu
	SaveEndData(areapXStar,m_areaSASchedule,SCHEDULE_SA);
		
	m_nSubAction = -1;

	return 0;
}


//---------------------------------------------------------
//
//		Geneticky algoritmus
//
//---------------------------------------------------------

// setridi populaci od min Makespanu po nejhorsi 
int CFJSS::GenerateBeginPopulation(CPtrNodeAreaList& lareapPopulation)
{

		// 1.
		for (int i=0; i<m_nGAPopulation; i++)
		{
			lareapPopulation.Add(CPtrNodeArea());
			GenerateBeginSchedule(lareapPopulation[i]);
			lareapPopulation[i].m_fVOOF = GetVOOF(lareapPopulation[i]);
		}

		return 0;
}

int CFJSS::OrderPopulation(CPtrNodeAreaList& lareapPopulation)
{
		// 1.Setridi populaci

		CPtrNodeArea PtrNodeArea;
		// 1. Setrideni populace od min k max. , resp. od nejlepsiho reseni k nehorsimu
		BOOL bSort = TRUE;
		while (bSort)
		{
			for (int i=0; i<lareapPopulation.GetSize()-1; i++)
			{
				if (lareapPopulation[i].m_fVOOF > lareapPopulation[i+1].m_fVOOF)
				{
					PtrNodeArea = lareapPopulation[i];
					lareapPopulation[i] = lareapPopulation[i+1];
					lareapPopulation[i+1] = PtrNodeArea;
					bSort = TRUE;
					break;
				}
				bSort = FALSE;
			}
		}

		for (int i=0; i<lareapPopulation.GetSize(); i++)
			TRACE("%d  %f\n",i,lareapPopulation[i].m_fVOOF);

		TRACE("\n\n");


		return 0;

}

int CFJSS::SelectParents(CPtrNodeAreaList& lareapPopulation,
						 CPtrNodeArea& Parent1,CPtrNodeArea& Parent2)
{

		CUIntArray nRand;
		
		// pocet pokusu pro nalezeni nahodneho min.cisla
		// ulozena nahodna cisla, vybere se nejmensi
		// aby se vybraly lepsi rozvrhy
		
		// Rodic 1
		for (int i=0; i<m_nGASelCoef; i++) 
			nRand.Add(RandI((int)lareapPopulation.GetSize()-2));

		int nParent1 = nRand[0];
		for (int i=1; i<m_nGASelCoef; i++)  nParent1 = min((unsigned)nParent1,nRand[i]);

		// Rodic 2
		int nParent2 = nParent1+1;
		Parent1 = lareapPopulation[nParent1];
		Parent2 = lareapPopulation[nParent2];

		return 0;
}

// provede MSM mutaci, z prvhiho rodice vyslechti mutaci potomka
int CFJSS::MultiStepMutation(CPtrNodeArea& areapP1,CPtrNodeArea& areapChild)
{

	//1. Nastav x=p1; Seznam potomku ML = 0;
	//CYKLUS
	//2. y = N(x) takovy, ze y=arg max(d(z,p)); pridej y do ML; x = y;
	//KONEC_CYKLU(zvoleny_pocet_iteraci = nMSMCycle)
	//3. Ze seznamu je vybran nejvzdalenejsi (posleni);

	
	CPtrNodeAreaList	lareapML;			// Seznam potomku ML pro MSM

	// 1.
	CPtrNodeArea areapX = areapP1;
	lareapML.RemoveAll();

	for (int i=0; i<m_nGA_MSMCycle; i++)
	{					
		//2.
		GetMinMaxCBN(areapP1,areapX,0);
		int nIndex = (int)lareapML.Add(CPtrNodeArea());
		lareapML[nIndex] = areapX;
	}

	//3.      
	areapChild = lareapML[lareapML.GetSize()-1];

	return 0;
}

int CFJSS::MultiStepCrossover(CPtrNodeArea& areapP1,CPtrNodeArea& areapP2,CPtrNodeArea& areapChild)
{
	// 1.Nastav i=1; inicializuj seznam potomku OL = 0;
	// CYKLUS
	// 2. Zvol bod pi+2 = N(pi) takovy, ze y=arg min(d(z,pi+1))
	//    Jestlize d(pi+2,pi+1) --> break, jinak pridej pi+2 do OL
	//    tzn. ze p3 se vzdaluje od p2
	// 3. Nastav i = i +1
	// END_CYKLUS();
	// 4. Vyber potomka
	// min(V(z))				,je/li V(y)<min(V(p1),V(p2))
	// min(d(p1,z)-d(z,p2))		,jinak

	CPtrNodeAreaList	lareapOL;			// Seznam potomku OL vzniklych MSX

	//1.

	int i = 0;
	int nActDistance = 0;
	int nLastDistance = 0;
	CPtrNodeArea areapX;

	lareapOL.RemoveAll();

	// dam do seznamu oba puvodni rodice, kvuli automatizaci dalsiho algoritmu
	int nIndex = (int)lareapOL.Add(CPtrNodeArea());
	lareapOL[nIndex] = areapP1;
	nIndex = (int)lareapOL.Add(CPtrNodeArea());
	lareapOL[nIndex] = areapP2;

	nActDistance  = GetDisGraphDistance(areapP1,areapP2);

	do 
	{
		// 2.
		areapX = lareapOL[i];
		GetMinMaxCBN(lareapOL[i+1],areapX,1);	//hledam min. vzd. potomka rodice1 vuci rodici2,..
		nIndex = (int)lareapOL.Add(CPtrNodeArea());
		lareapOL[nIndex] = areapX;

		nLastDistance = nActDistance;
		nActDistance = GetDisGraphDistance(lareapOL[i+1],lareapOL[i+2]);

		// 3.
		i++;

	} while (nActDistance < nLastDistance);

	areapChild = lareapOL[lareapOL.GetSize()-1];	

	return 0;
	
}

double CFJSS::LocalSearch(CPtrNodeArea& areapChild)
{

	// 1. Zvol pocatecni bod x=x0=xBest
	// CYKLUS()
	//	2. if F(x) < F(xBest) --> xBest = x;
	//	3. Zvol y = N(x), x = y
	// END_CYKLUS(pocet_cyklu || jina podminka)

	// 1.


	CPtrNodeArea areapX = areapChild;
	CPtrNodeArea areapY;
	CPtrNodeArea areapBest = areapX;
			
	double fFx = GetVOOF(areapX);
	double fFy = 0;
	double fFBest = fFx;

	for (int i=0; i<m_nGA_LSCycle; i++)
	{
		// 2.
		VicinityS1(areapX,areapY);
		fFy = GetVOOF(areapY);		
		if (fFy < fFBest)
		{
			fFBest = fFy;
			areapBest = areapY;			
		}
		// 3.
		areapX = areapY;
	}
			
	TRACE ("Vypocitan potomek %f\n",fFBest);

    areapChild = areapBest;

	return fFBest;
}

int CFJSS::GetRuntime(time_t nBeginTime)
{
	time_t nActTime;
	nActTime = time(NULL);
	return (int)(nActTime - nBeginTime);
}

double CFJSS::GetExactTime(time_t nBeginTime)
{
	double fResult;
	time_t nActTime;
	struct __timeb64 tstruct;		

	nActTime = time(NULL);
	_ftime64( &tstruct );

	fResult = (double)(nActTime-nBeginTime) + (double)((double)tstruct.millitm/1000.0);

	return fResult;
}


int CFJSS::SolveGA()
{
	m_nSubAction = 1;

		// GA/MSX pro JSSP, CB sousedsvi, vzdalenosti DG
		// 1. Inicializace populace, aplikace lokalniho hledani na kazdy
		// CYKLUS()
    	// 2. Vyber 2 rozvrhy S a T nahodne z pouplace v zavislosti na delce
		//    trvani celeho rozvrhu
		// 3. Jestlize je vzdalenost DG mezi S a T kratsi nez nejaka mala a predem
		//    urcena hodnota, aplkijum MSM na S a vytvor tak novy rozvrh U a jdi
		//	  na krok 5.
		// 4. Aplikuj MSX na S a T pouzitim CB sousedstvi a DG vzdalenosti a 
		//    vytvor tak novy rozvrh U
		// 5. Aplikuj na U lokalni hledani a vytvor tak U'
		// 6. Jestlize  delka Rozvrhu U' je mensi nez nejhorsi rozvrh v 
		//    dosavadni populaci, pak vymen U' za nejhorsi rozvrh v populaci
		// 7. Opakujk kroky 2 - 6 dokud se nedosahne zlepseni po nekolik rucitych generaci
		// END_CYKLUS(pokud se nedosahne zlepseni po nekolik urcitych generaci);
		// 8. Ulozeni sekvenci nejlepsiho reseni

		// #inc: m_lareapPopulation[i] + GaParams
		
        CPtrNodeAreaList	lareapPopulation;	// pupulace				
		CPtrNodeArea		areapP1;			// prvni rodic
		CPtrNodeArea		areapP2;			// druhy rodic
		CPtrNodeArea		areapChild;			// potomek, vznikne bud MSM, nebo MSX
		double fChild = 0;

		int n = 0;								// citac hlavniho cyklu
		time_t nBeginTime = time(NULL);			// cas zahajeni vypoctu
		time_t nActTime = nBeginTime;			// aktualni cas
		
		if (m_nAction == 1 )
		{
			int nAxisIndex = m_areaAxis.GetSize();
			m_areaAxis.Add(CAxisList());
			m_areaAxis[nAxisIndex].m_strName = GetStrMethod(1);
		}

		PrepareSrand();
		DeleteAllSolving(1);
		
		// 1. VYTVORENI POPULACE (vytvoreni + setrideni)

		UpdateDialogData(GetRuntime(nBeginTime),fChild,0,0,n,m_nGACycle,0);
		GenerateBeginPopulation(lareapPopulation);
		OrderPopulation(lareapPopulation);

		do
		{	
			UpdateDialogData(GetRuntime(nBeginTime),fChild,lareapPopulation[0].m_fVOOF,0,n,m_nGACycle,1);
			// 2. SELEKCE
			SelectParents(lareapPopulation,areapP1,areapP2);
    		int nParentDistance = GetDisGraphDistance(areapP1,areapP2);
			if  (nParentDistance < m_nGAMinMSX)
			{
				// 3. MUTACE MSM, vzdalenost mezi rodici byla priliz mala
				UpdateDialogData(GetRuntime(nBeginTime),fChild,lareapPopulation[0].m_fVOOF,0,n,m_nGACycle,3);
				MultiStepMutation(areapP1,areapChild);
			}
			else
			{				
				// 4. MSX, Multi-step CroossOver, pokud je dostatecna vzdalenost
				UpdateDialogData(GetRuntime(nBeginTime),fChild,lareapPopulation[0].m_fVOOF,0,n,m_nGACycle,2);
				MultiStepCrossover(areapP1,areapP2,areapChild);
			}
			// 5. LS - Aplikace lokalniho hledani na Potomka (child) (prohledavani sousedstvi)
			UpdateDialogData(GetRuntime(nBeginTime),fChild,lareapPopulation[0].m_fVOOF,0,n,m_nGACycle,4);				LocalSearch(areapChild);
			
			// 6. Pokud potomek lepsi nez nejhorsi v populaci, vymenit nejhorsi v populaci za potomka
			UpdateDialogData(GetRuntime(nBeginTime),fChild,lareapPopulation[0].m_fVOOF,0,n,m_nGACycle,5);	
			double fRefuse = lareapPopulation[lareapPopulation.GetSize()-1].m_fVOOF;
			fChild = GetVOOF(areapChild);

            if (fRefuse > fChild)
				lareapPopulation[lareapPopulation.GetSize()-1] = areapChild;

			OrderPopulation(lareapPopulation);

			// Update ukoncovacich podminek
			n++;
			// Ulozeni hodnot prubehu
			SaveRunningData(GetExactTime(nBeginTime),lareapPopulation[0].m_fVOOF,lareapPopulation[0]);

		}	while (((n<m_nGACycle) && (GetRuntime(nBeginTime) < m_nGATime) && (m_nAction < 3))
			|| ((m_nAction == 3) && (m_nCREActIter < m_nCREIteration))) ;
		
	
	// 9. Ulozeni nejlepsiho reseni do Schedule
    SaveEndData(lareapPopulation[0],m_areaGASchedule,SCHEDULE_GA);

	m_nSubAction = -1;

	return 0;
}

// arepNode ==> areaSchedule
int CFJSS::SaveEndData(CPtrNodeArea& areapXBest,CScheduleArea& areaSchedule,int nSAorGA)
{
	int nIndex;
	int nSubIndex;

	switch (m_nAction)
	{

	case 0:										// Simple SA
		areaSchedule = areapXBest;
		IsAbleResult(1);
		break;

	case 1:										// Simple GA
		areaSchedule = areapXBest;
		IsAbleResult(2);
		break;

	case 2:				// Convergence Experiment
		nIndex = m_areafConvergence.GetSize()-1;
		m_areafConvergence[nIndex].Add(areapXBest.m_fVOOF);
		break;

	case 3:				// Criteria Experiment
						// propocitat vysledek dle ostatnich kriterii
		//SolveAmongAbleCriterion(areapXBest);
		break;

	}
	
	return 0;
}

// areaSchedule ==> areapNode;


int CFJSS::GetDisGraphDistance2(CPtrNodeArea& Parent1,CPtrNodeArea& Parent2)
{
	int nResult = 0;

	for (int i=0; i<Parent1.GetSize();i++)
		for (int j=0; i<Parent1.GetSize()-1;i++)
		{
			// Procese cely area
			CNode* Node11 = Parent1[i][j];
			CNode* Node12 = Parent2[i][j+1];
			nResult++;

			// u kazdeho prvku hleda stejny prvek u druheho rozvrhu
			for (int k=0; k<Parent2[i].GetSize()-1;k++)
			{
				CNode* Node21 = Parent2[i][k];
				// az ho najde porovna jeho naslednika
				if (Node21 == Node12)
				{
					CNode* Node22 = Parent2[i][k+1];
					// pokud jsou naslednici stejni, vzdalenost se nezvysi
					if (Node22 == Node11)
						nResult--;
				}
					
			}
		}
	

	return nResult;
}

int CFJSS::GetDisGraphDistance(CPtrNodeArea& Parent1,CPtrNodeArea& Parent2)
{
	int nResult = 0;

	for (int i=0; i<Parent1.GetSize();i++)
		for (int j=0; j<Parent1[i].GetSize();j++)
			if (Parent1[i][j] != Parent2[i][j]) 
				nResult++;

	return nResult;
}



/*

/////////////////////////////////////////////////
	Poznamky k metodam
/////////////////////////////////////////////////

/////////////////////////////////////////////////
	Geneticky algoritmus
/////////////////////////////////////////////////


		// Postup : (prevzato z J. Burda)
		//
		// P = pocatecni populace
		// CYKLUS
		//  R = selekce (P)
		//  Q = krizeni (R)
		//  Q = mutace (Q)
		//  P = nova_populace(P,Q)
		// END_CYKLUS(podminka_ukoonceni - obvykle ukonceni stanoveneho poctu iteraci)

		// Poznamky:
		// Populace (ani mala, ani velka), vytvorena z nahodnych pocatecnich reseni
		// Selekce vyber obou rodicu je nahodny v souladu rozdeneni pravdepodobnosti Ps
		//         pocitane  jako podil hodnoty fitnes chromozomu k celkove hodnote 
		//		   fitness populace (ruleta), existuji i dalsi monznosti,...
		// Krizeni - s jistotou nebo vysokou psti Pc
		//		   JEDNOBODOVE - zvoly se nahodny pulici bod a jedna polovina je se vezme
		//                       z prvniho rodice a druha z druheho rodice
		//         VICEBODOVE - matice nahodna 1 a 0, 1 bude dedit z 1. potomka,...
	    //         (u FJSS, 1 se zkopiruji a 2 se doplni v poradi jakem jsou u 2. rodice)
		// Mutace - s nizkou psti PM (<1%) na kazkou pozici chromozomu a zpusobuje modifikaci			
		//	hodno 0->1, (FJSS) obrati se poradi prvku na 2 nahodne vybranych pozicich,
		// nebo se na nahodne pozici vymeni poradi prvku
		// Vytvoreni nove pouplace - pomoci krizeni a mutace 
		//                         - prekryvani populace rodicu a potomku,
		//							 vygenerovany potomek nahrazuje nahodne vybraneho			
		//							prisluxnika aktualni populace

		// dalsi mozne pozadavky: aby se nejlepsi clen aktualni poupulac objevil i 
		// v populaci nove, tzn. nahradim jim nahodne vybraneho jedince z tech
		// kteri maji podprumernou hodnotu futnes0.

		// Prohledavani sousedstvi 
		// x = reseni, reprezenovano bodem v prostoru reseni
		// N(x) = mnozina reseni asociovana s x je sousedstvi dosazitelna z x pouze
		//        jednim prechodem, jednou permutaci x
		// V(x) = minimalizovana funkce, V(x) je ohodnoceni

		// 1. Zvol pocatecni bod x = x0 = xbest;
		// 2. If V(x) < V(xbest) then xbest = x;
		// 3. Zvol bod y patri do N(x) podle zvoleneho kriteria zalozeneho na hodnote V(y)
		//    pak x= y
		// 4. Opakuj kroky 2 a 3 az do splneni nejake ukoncovaci podminky

		// Vicekrokove krizeni multi/step crossover , MSX
		//
		// p1,p2 jsou resseni jako rodice a N(p) je soused p
		// Chceme vypocitat bod x patri do N(p1) ne pomoci hodnoty V(x), ale pomoci vzdalenosti
		// d(x,p2) mezi x a p2
		// Zaciname od p1, x je modifikovano krok za krokem a jedosmerne se priblizuje
		// k p2
		// V tomto procesu x postupne ztraci charakter p1 a postupne ziskava charakteristiky p2
		// Po urcitem poctu iteraci by  melo reseni obsahovat casti obou rodicu p1 a p2 v 
		// ruznych polomerech

		// Jedosmerne MSX:
		// 1. Nastav x = p1; inicializuj seznam potomku OL = 0;
		// 2. Zvol bod y patri do N(x) podle kriteria zalozenoho na d(y,p2).
		//    Prodej y do seznamu OL, nastav x = y
		// 3. pakuj bod 2 dokud neni splnena ukoncovaci podminka. 

		// Pozn. kriterium v bode 2 muze byt ruzne, zde se uvazuje tento jednoduchy pripad:
		// zvol bod y patri do N(x) takovy ze
		//
		//              y = arg                 min(d(z,p2))
		//                    z patri do N(x)
		// Pro dalsi generaci jsou vybrany pocomci ze seznamu OL,
		// Je vybran potomek,   ktery je nejlepsi
		//                 nebo jehoz vzdalenost od obou rodicu je skoro stejna
		// Zvol bod y patri do N(x) takovy ze
		//
		//				y = arg min (V(x))    , je/li V(y) < min(V(p1),V(p2))
		//				      z patri do N(x)
		//              y - arg min (|d(p1,z)-d(z,p2|)),  jinak
		//					  z patri do N(x)


		// Prevedena do obousmere MSX
		// 1. Nastav i = 1, inicializuj seznam potomku OL = 0;
		// 2. Zvol bod pi+2 patri do N(pi) takovy, ze 
		//               y = arg min (d(z,pi+1))
		//		                z patri do N(pi)
		// Jestlize d(pi+2,pi+1) > d(pi,pi+1) pak stop, jinak pridej pi+2 do OL
		// 3. Nastav i++ a jdi na krok 2

		// Pozn. agoritmu se zastavi, kdyz se dve reseni pi a pi+1 dostanou tak blizko, ze jiz			
		// zadny jiny  pohyb je nepriblizi, potomek pro dalsi generaci je vybran 
		// stejne jako u jednosmerneho MSX


		// MUTACE - vicekrokova mutace multi step mutation, MSM
		// p je jednotlivec na ktereho je mutacea aplikovana
		// vychazime z p a potomek x je opakovane modifikovan a vzdaluje se od p
		// nakonec je vybran potomek, jenz je nejlepsi nebo nejvzdalenejsi od p
		// 1. Naxtav x = p, inicializuj seznam potomku ML = 0;
		// 2. Zvol bod y patri do N(x) takovy, ze 
		// 
		//          y = arg max (d(z,p))
		//				  z patri do N(x)
		//  Pridej y do ML, nastav x = y;
		//  Opakuj krok 2 po zvoleny pocet iteraci

		// Vzdalenost v prostoru reseni pro JSSP
		// vzdalenost mezi dvemi rozvrhy S a T je brana jako pocet rozdilu v poradi 
		// na kazdem stroji. je to suma vsech disjunktivnich hran,
		// jenz maji rozdilne smery v S a T. Tuto vzdalenost nazivame vzdalenost 
		// disjunktivniho grafu (disjunctive graph distance, DG)

		// Sousedstvi kritickeho bloku
		//  velice silny permutacni operator
		//  Zamenuje poradi operaci v kritickem bloku presunutim urcite operace uvnitr 
		// bloku na zacatenebo konec bloku. V tomto sousedstvi vzdalenost mezi S a jakymkoli			
		// elementem v N(S) zavisla na pozici presunovane operace. 

		// GA/MSX pro JSSP, CB sousedsvi, vzdalenosti DG
		// 1. Inicializace populace, aplikace lokalniho hledani na kazdy
		// 2. Vyber 2 rozvrhy S a T nahodne z pouplace v zavislosti na delce
		//    trvani celeho rozvrhu
		// 3. Jestlize je vzdalenost DG mezi S a T kratsi nez nejaka mala a predem
		//    urcena hodnota, aplkijum MSM na S a vytvor tak novy rozvrh U a jdi
		//	  na krok 5.
		// 4. Aplikuj MSX na S a T pouzitim CB sousedstvi a DG vzdalenosti a 
		//    vytvor tak novy rozvrh U
		// 5. Aplikuj na U lokalni hledani a vytvor tak U'
		// 6. Jestlize  delka Rozvrhu U' je mensi nez nejhorsi rozvrh v 
		//    dosavadni populaci, pak vymen U' za nejhorsi rozvrh v populaci
		// 7. Opakujk kroky 2 - 6 dokud se nedosahne zlepseni po nekolik rucitych generaci

		// tento algoritmus max 200 uzlu


		// P = pocatecni populace
		// CYKLUS
		//  R = selekce (P)
		//  Q = krizeni (R)
		//  Q = mutace (Q)
		//  P = nova_populace(P,Q)
		// END_CYKLUS(podminka_ukoonceni - obvykle ukonceni stanoveneho poctu iteraci)


*/


