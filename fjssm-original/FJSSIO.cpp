// FJSSIO.cpp : implementation file
//

#include "stdafx.h"
#include "FJSS.h"


//-----------------------------------------------
// OutString Function
//-----------------------------------------------
CString CFJSS::GetStrNR(CFN& FN,int nrType)
{
	CString str;
	// PT
	if (!nrType)
	{
		for (int i=0; i<m_nPTPart-1; i++)
			str.AppendFormat(_T("%3d,"),FN.m_a[i]);
		str.AppendFormat(_T("%3d"),FN.m_a[m_nPTPart-1]);
	}
	// DD
	if (nrType == 1)
	{
		for (int i=0; i<m_nDDPart-1; i++)
			str.AppendFormat(_T("%3d,"),FN.m_a[i]);
		str.AppendFormat(_T("%3d"),FN.m_a[m_nDDPart-1]);
	}
	
	// OTHER
	if (nrType == 2)
	{
		for (int i=0; i<FN_PART-1; i++)
			str.AppendFormat(_T("%3d,"),FN.m_a[i]);
		str.AppendFormat(_T("%3d"),FN.m_a[FN_PART-1]);
	}


	return str;
}

CString CFJSS::GetStrNR(int n)
{
	CString str;
	str.Format(_T("%d"),n);

	return str;
}
	

CString CFJSS::GetStrNR(double f,int nInt,int nDec)
{
	CString str;
	CString strIntDec;

	strIntDec.Format(_T("%d.%d"),nInt,nDec);


	str.AppendFormat(_T("%")+strIntDec+_T("f"),f);

	return str;
}

CString CFJSS::GetStrParam(CNode& Node, int nStyle)
{
	CString str = _T("Nezmany parametr");
	switch (nStyle)
	{
		case 0: return str = GetStrNR(Node.m_fr);break;
		case 1: return str = GetStrNR(Node.m_fPT);break;
		case 2: return str = GetStrNR(Node.m_fC);break;
		default: return str;break;
	}

	return str;
}

CString CFJSS::GetStrNode(CNode& Node,int nType)
{
	// 0. Simple
	// 1. Fat
	CString str;

	CString strPT = GetStrNR(Node.m_fPT);

	if (nType == 0)
	{
        str.Format(_T("%d,%d,%d (%s)"),
		Node.m_nJob, Node.m_nProcess, Node.m_nMachine,strPT);
	}

	if (nType == 1)
	{
		CString strr = GetStrNR(Node.m_fr);
		CString strC = GetStrNR(Node.m_fC);
        str.Format(_T("%d,%d,%d %s (%s - %s)"),
		Node.m_nJob, Node.m_nProcess, Node.m_nMachine, strPT,strr,strC);
	}

	return str;
}
CString CFJSS::GetStrEdge(CEdge& Edge)
{
	CString str;
	CString strType;

		if (Edge.m_bCon) strType = _T("c");
		else strType = _T("d");

		str.Format(_T("%3d,%3d,%3d -- %s --> %3d,%3d,%3d"),
			Edge.m_pNode1->m_nJob,
			Edge.m_pNode1->m_nProcess,
			Edge.m_pNode1->m_nMachine,
			strType,
			Edge.m_pNode2->m_nJob,
			Edge.m_pNode2->m_nProcess,
			Edge.m_pNode2->m_nMachine);

	return str;
}


CString CFJSS::GetStrType()
{
	CString str;
	switch (m_nType)
	{
		case 0: str.Format(_T("(%d x %d) - Deterministický pøípad"),m_nJob,m_nMachine); break;
		case 1: str.Format(_T("(%d x %d) - Fuzzy termíny dokonèení"),m_nJob,m_nMachine);break;
		case 2: str.Format(_T("(%d x %d) - Fuzzy èasy operací"),m_nJob,m_nMachine);break;
		case 3: str.Format(_T("(%d x %d) - Fuzzy termíny dokonèení")
					       _T(" i fuzzy èasy operací"),m_nJob,m_nMachine);break;
		default: str.Format(_T("(%d x %d) - Neznámý pøípad, Chyba !"),m_nJob,m_nMachine);break;
	}
	return str;
	
}

CString CFJSS::GetStrMethod(int nType)
{
	CString str;
	switch (nType)
	{
		case -1: str = _T("");break;
		case 0: str = _T("Simulované žíhání");break;
		case 1: str = _T("Genetický algoritmus");break;

	}

	return str;
}


CString CFJSS::GetStrCriterion(int nCriterion,int nType)
{
	CString str = _T("");
	
	
	if ((nType == 0) || (nType == 2))
	{
		if (nCriterion == 0)
			str.Format(_T("(CPT/*)"));
		else if (nCriterion < 5)
			str.Format(_T("(FPT/*)"));
		else if (nCriterion < 6)
			str.Format(_T("(FPT/CDD)"));
		else
			str.Format(_T("(FPT/FDD)"));
	}
	
	if (nType == 2)
		str.AppendFormat(_T("\n"));


	if ((nType == 1) || (nType == 2) || (nType == 3))
	{
		switch (nCriterion)
		{
			case 0: str.AppendFormat(_T("Crisp Makespan"));break;
			case 1: str.AppendFormat(_T("F(Ci) = m(Ci)"));break;
			case 2: str.AppendFormat(_T("F(Ci) = w1m(Ci) + w2s(Ci)"));break;			
			case 3: str.AppendFormat(_T("F(Ci) = w1m(Ci) + w2sR(Ci)"));break;
			case 4: str.AppendFormat(_T("F(Ci) = Suma(wi * a(i))"));break;
			case 5: str.AppendFormat(_T("F(Ci) = Ext_DD(Ci)"));break;
			case 6: str.AppendFormat(_T("F(Ci) = area(Ci n Di) / area(Ci)"));break;
			case 7: str.AppendFormat(_T("F(Ci) = area(Ci n Di) / area(Ci u Di)"));break;
			case 8: str.AppendFormat(_T("F(Ci) = area(CiE)+Area(CiT)-Area(CiD)+G(P)"));break;
			case 9: str.AppendFormat(_T("F(Ci) = nwi1 agr(Ci,Di)+wi2(1-dist(Ci,Di))"));break;

			default: str.AppendFormat(_T("Neznámé kriterium"));break;
		}
	}

	if  (nType == 2)
	if (m_nCritMaxOrSum == 0)
		str.AppendFormat(_T("\nVarianta Max"));
	else
		str.AppendFormat(_T("\nVarianta Suma"));

	return str;
	
}	


int CFJSS::GenerateProjectReport()
{
	// LEGENDA
	m_lstsLegend.Add("Parametr");
	m_lstsLegend.Add("Data parametru");

	// DATA
	for (int i=0; i<4; i++) m_areasData.Add(CStrList());

	m_areasData[0].Add("Název projektu:");
	m_areasData[1].Add("Datum:");
	m_areasData[2].Add("Vytvoøil:");
	m_areasData[3].Add("Typ problému");

	m_areasData[0].Add(m_strName);
	m_areasData[1].Add(m_strDate);
	m_areasData[2].Add(m_strMaker);
	m_areasData[3].Add(GetStrType());

	return 0;

}
int CFJSS::GenerateJobReport()
{
	CString str;

	int nIndex = m_areasData.Add(CStrList());
	m_areasData[nIndex].Add(_T("Index jobu"));
	m_areasData[nIndex].Add(_T("Název jobu"));
	m_areasData[nIndex].Add(_T("Termín dokonèení"));
	m_areasData[nIndex].Add(_T("Alfa"));
	m_areasData[nIndex].Add(_T("Beta"));
	m_areasData[nIndex].Add(_T("Lam1"));
	m_areasData[nIndex].Add(_T("Lam2"));
	m_areasData[nIndex].Add(_T("Lam3"));

	
	// DATA	

	for (int i=0; i<m_nJob; i++)
	{
		nIndex = m_areasData.Add(CStrList());
		str.Format(_T("%d"),i);
		m_areasData[nIndex].Add(str);
		m_areasData[nIndex].Add(m_lstJob[i].m_strName);
		m_areasData[nIndex].Add(GetStrNR(m_lstJob[i].m_fDD,NR_DD));
		m_areasData[nIndex].Add(GetStrNR(m_lstJob[i].m_fAlfa));
		m_areasData[nIndex].Add(GetStrNR(m_lstJob[i].m_fBeta));
		m_areasData[nIndex].Add(GetStrNR(m_lstJob[i].m_fLambda1));
		m_areasData[nIndex].Add(GetStrNR(m_lstJob[i].m_fLambda2));
		m_areasData[nIndex].Add(GetStrNR(m_lstJob[i].m_fLambda3));
	}

	return 0;
}

int CFJSS::GenerateMachineReport()
{
	CString str;

	int nIndex = m_areasData.Add(CStrList());
	m_areasData[nIndex].Add("Index stroje");
	m_areasData[nIndex].Add("Název stroje");	

	// DATA
	for (int i=0; i<m_nMachine; i++)
	{
		nIndex = m_areasData.Add(CStrList());
		str.Format(_T("%d"),i);
		m_areasData[nIndex].Add(str);
		m_areasData[nIndex].Add(m_lstMachine[i].m_strName);
	}

	return 0;
}

int CFJSS::GenerateProcessReport()
{
	
	CString str;
	
	int nIndex = m_areasData.Add(CStrList());
	m_areasData[nIndex].Add("Index");
	m_areasData[nIndex].Add("Název");

	int nMax = MaxJobProcess();
	for (int i=0; i<nMax;i++)
	{
		str.Format(_T("Operace %d"),i+1);
		m_areasData[nIndex].Add(str);
	}
	
	// DATA	JOB PROCESS
	for (int i=0; i<m_nJob; i++)
	{
		nIndex = m_areasData.Add(CStrList());
		str.Format(_T("%d"),i);
		m_areasData[nIndex].Add(str);
		m_areasData[nIndex].Add(m_lstJob[i].m_strName);

		for (int j=0; j<m_areaJobNode[i].GetSize(); j++)
			m_areasData[nIndex].Add(GetStrNode(m_areaJobNode[i][j]));
	}


	return 0;
}

int CFJSS::SolveScheduleForResult(CScheduleArea& areaSchedule)
{
	CPtrNodeArea areapSchedule;

	// 1.
	for (int i=0; i<areaSchedule.GetSize();i++)
	{
		areapSchedule.Add(CPtrNodeList());
		for (int j=0; j<areaSchedule[i].GetSize();j++)
		{
			int nJob = areaSchedule[i][j].m_nJob;
			int nProcess = areaSchedule[i][j].m_nProcess;

			areapSchedule[i].Add(&m_areaJobNode[nJob][nProcess]);
		}
	}

	//2
	GetVOOF(areapSchedule);

	return 0;
}
int CFJSS::GenerateResultReport(CScheduleArea& areaSchedule)
{
	SolveScheduleForResult(areaSchedule);

	int nIndex = 0;
	// LEGENDA
	nIndex = m_areasData.Add(CStrList());
	m_areasData[nIndex].Add("Název projektu: ");
	m_areasData[nIndex].Add(m_strName);
	nIndex = m_areasData.Add(CStrList());
	m_areasData[nIndex].Add("Použitá heuristická metoda: ");
	m_areasData[nIndex].Add(GetStrMethod(m_nSelectedResult));
	nIndex = m_areasData.Add(CStrList());

	int nMaxCol;
	if (m_nSelectedResult == 0) nMaxCol = max(MaxJobProcess(),MaxMachineProcess(m_areaSASchedule));
	if (m_nSelectedResult == 1) nMaxCol = max(MaxJobProcess(),MaxMachineProcess(m_areaGASchedule));
	nMaxCol += 2; // +Job, DD, alfa, beta,

	// DATA
	// head
	m_areasData.Add(CStrList());
	int nPos = (int)m_areasData.GetSize()-1;
	m_areasData[nPos].Add("Datum vytvoøení");
	m_areasData[nPos].Add(m_strDate);

	m_areasData.Add(CStrList());
	nPos = (int)m_areasData.GetSize()-1;
	m_areasData[nPos].Add("Typ projektu");
	m_areasData[nPos].Add(GetStrType());

	// -- void line
	m_areasData.Add(CStrList());
	nPos = (int)m_areasData.GetSize();

	// job proces
	for (int i=0; i<m_nJob; i++)
	{
		m_areasData.Add(CStrList());
		
		m_areasData[i+nPos].Add(GetStrNR(i));
		m_areasData[i+nPos].Add(m_lstJob[i].m_strName);

		for (int j=0; j<m_areaJobNode[i].GetSize(); j++)
			m_areasData[i+nPos].Add(GetStrNode(m_areaJobNode[i][j],1));
	}


	// -- void line
	m_areasData.Add(CStrList());
	nPos = (int)m_areasData.GetSize();

	// machinen schedule
	for (int i=0; i<m_nMachine; i++)
	{
		m_areasData.Add(CStrList());

		m_areasData[i+nPos].Add(GetStrNR(i));
		m_areasData[i+nPos].Add(m_lstMachine[i].m_strName);

		for (int j=0; j<areaSchedule[i].GetSize(); j++)
		{
			m_areasData[i+nPos].Add(GetStrNode(areaSchedule[i][j].Node(m_areaJobNode),1));
									
		}
	}

		// -- void line
	m_areasData.Add(CStrList());
	nPos = (int)m_areasData.GetSize();

	// VOOF a Makespan
	m_areasData.Add(CStrList());
	nPos = (int)m_areasData.GetSize()-1;
	m_areasData[nPos].Add("Hodnota uèelové funkce");
	m_areasData[nPos].Add(GetStrNR(areaSchedule.m_fVOOF));
	
	m_areasData.Add(CStrList());
	nPos = (int)m_areasData.GetSize()-1;
	m_areasData[nPos].Add("Makespan rozvrhu");
	m_areasData[nPos].Add(GetStrNR(areaSchedule.m_fMakespan));


	return 0;
}

int CFJSS::GenerateConvergenceReport()
{

	int nIndex;

	nIndex = m_areasData.Add(CStrList());
	m_areasData[nIndex].Add("");
	m_areasData[nIndex].Add("Max");
	m_areasData[nIndex].Add("");
	m_areasData[nIndex].Add("Suma");
	m_areasData[nIndex].Add("");

	nIndex = m_areasData.Add(CStrList());
	m_areasData[nIndex].Add("");
	m_areasData[nIndex].Add("SA");
	m_areasData[nIndex].Add("GA");
	m_areasData[nIndex].Add("SA");
	m_areasData[nIndex].Add("GA");

	for (int i=0; i<m_areafConvergence.GetSize(); i++)
	{
		nIndex = m_areasData.Add(CStrList());
		m_areasData[nIndex].Add(" Kriterium è. " + GetStrNR((int)m_areafConvergence[i][0]));

		for (int j=1; j<m_areafConvergence[i].GetSize(); j++)
			m_areasData[nIndex].Add(GetStrNR(m_areafConvergence[i][j]));
	}

	return 0;
}

int CFJSS::GenerateIterationReport()
{
	int nIndex;

	// Legenda
	nIndex = m_areasData.Add(CStrList());
	m_areasData[nIndex].Add("Iterace");	
	for (int i=0; i<m_nAbleCriterion; i++)
		m_areasData[nIndex].Add(_T("Krit. ") + GetStrNR(m_lstnAbleCriterion[i]));

	m_areasData[nIndex].Add("Podle krit.");	
	

	// Data
	for (int i=0; i<m_areafIteration.GetSize();i++)
	{
		nIndex = m_areasData.Add(CStrList());
		m_areasData[nIndex].Add(GetStrNR((i % m_nCREIteration) + 1 ));
		for (int j=0; j<m_areafIteration[i].GetSize()-2;j++)
		{
			m_areasData[nIndex].Add(GetStrNR(m_areafIteration[i][j]));
		}
		int nCritNr  = m_areafIteration[i][m_areafIteration[i].GetSize()-2];
		m_areasData[nIndex].Add(GetStrNR(nCritNr,3,0));

	}

	return 0;
}

int CFJSS::GenerateREReport()
{
	int nIndex;

	// Legenda
	nIndex = m_areasData.Add(CStrList());
	m_areasData[nIndex].Add("Iterace");	
	for (int i=0; i<m_nAbleCriterion; i++)
		m_areasData[nIndex].Add(_T("RE Krit. ") + GetStrNR(m_lstnAbleCriterion[i]));

	m_areasData[nIndex].Add("Podle krit.");	
	

	// Data
	for (int i=0; i<m_areafRE.GetSize();i++)
	{
		nIndex = m_areasData.Add(CStrList());
		m_areasData[nIndex].Add(GetStrNR((i % m_nCREIteration) + 1 ));
		for (int j=0; j<m_areafRE[i].GetSize()-2;j++)
		{
			m_areasData[nIndex].Add(GetStrNR(m_areafRE[i][j]));
		}
		int nCritNr  = m_areafIteration[i][m_areafIteration[i].GetSize()-2];
		m_areasData[nIndex].Add(GetStrNR(nCritNr,3,0));

	}

	return 0;
}

int CFJSS::GenerateAREReport()
{
	int nIndex;

	// Legenda
	nIndex = m_areasData.Add(CStrList());
	m_areasData[nIndex].Add("");	
	for (int i=0; i<m_nAbleCriterion; i++)
		m_areasData[nIndex].Add(_T("Kriterium è. ") + GetStrNR(m_lstnAbleCriterion[i]));
    m_areasData[nIndex].Add("Celkem");	
    
	// Data
	for (int i=0; i<m_areafAverageRE.GetSize();i++)
	{
		nIndex = m_areasData.Add(CStrList());
		m_areasData[nIndex].Add(_T("Optimalizace užitím kr. ") + GetStrNR(m_lstnAbleCriterion[i]));
		for (int j=0; j<m_areafAverageRE[i].GetSize();j++)
		{
			m_areasData[nIndex].Add(GetStrNR(m_areafAverageRE[i][j]));
		}
	}

	return 0;
}
