#pragma once
#include "FJSSUnit.h"		// OK
#include "FJSSFN.h"			// OK
#include "FJSSImport.h"		// OK
#include "FJSSIO.h"			

int const EDGE_CON = 1;			// identifikace konjunktivni hrany
int const EDGE_DIS = 0;			// identifikace disjunktivni hrany

int const NR_PT		= 0;		// pro vypis = casy operaci a souvisejici
int const NR_DD		= 1;		// pro vypis = terminy dokoneni a souvisejici
int const NR_OTHER	= 2;		// dalsi cisla, vzdy vypsano jako fuzzy cislo

int const STYLE_R = 0;			// vypis parametru r
int const STYLE_PT = 1;			//		"		   PT
int const STYLE_C = 2;			//		"		   C

int const SCHEDULE_SA = 0;		// pro identifikaci metody
int const SCHEDULE_GA = 1;

int const CRIT_COUNT = 10;

class CSADlg;
class CGADlg;
class CConvExpDlg;
class CCritExpDlg;

// CFJSS command target
class CFJSS : public CObject
{
public:

	int m_nAction;			// hlavni provadena akce
	int m_nSubAction;		// index podakce

	CFJSS();
	virtual ~CFJSS();
	int UpdateDialogData(int nRunTime,double fX0,double fXBest,double fTemp,
								int nActCycle,int nMaxCycle,int nGAAction);

	int SaveRunningData(double fExactTime,double fXStar,CPtrNodeArea& areapMachine);
	
	int m_nActOperation;

//-----------------------------------------------
// Project
// in: fjss.h
//-----------------------------------------------

// BASE Data
//-----------------------------------------------
	CString m_strName;			// nazev projektu (max. 60 znaku)
	CString m_strDate;			// datum vytvoreni projektu
	CString m_strMaker;			// jmeno tvurce projektu rozvrhu
	CString m_strImportFile;	// cesta a nazev importovaneho souboru (full path)
	CString m_strExportFile;	// nazev exportovaneho souboru (full path);

	int m_nType;				// typ projektu 0-crisp, 1-fDD, 2-fPD, 3-All
	int m_nJob;					// pocet jobu
	int m_nMachine;				// pocet stroju

	CNodeArea		m_areaJobNode;		// seznam operaci
	CJobList		m_lstJob;			// seznam jobu
	CMachineList	m_lstMachine;		// seznam stroju


// Priznaky ovlivnujici beh progtramu
//-----------------------------------------------

	int m_nPTPart;				// pocet slozek pro casy operaci
	int m_nDDPart;				// pocet slozek cisla pro terminy dokoneni
	BOOL m_bFuzzyDD;			// priznak fuzzyDD
	BOOL m_bFuzzyPT;			// priznak fuzzyPT


// Funkce pro nastaveni priznaku
//-----------------------------------------------
// Typ projektu a behove flagy

	int SetTypeMask(int nCase);		// po konverzi nastavi masku m_bFPT,...
	int GetTypeFromMask();			// z masky zjisti a vrati typ
	int SetTypeProp(int nType);		// nastavi PTPart, DDPart, Provede CriteriaFlag

	// typ problemu FDD?, FPT?
	BOOL FDD(){if ((m_nType == 1) || (m_nType == 3)) return TRUE; else return FALSE;}
	BOOL FPT() {if ((m_nType == 2) || (m_nType == 3)) return TRUE; else return FALSE;}


// Countery
//-----------------------------------------------
	int MaxJobProcess();									// maximalni pocet v jednom jobu
	int MaxMachineProcess(CScheduleArea& areaSchedule);		// maximalni pocet operaci v jednom stroji
	int MaxCPEdges();				// maximalni pocet hran na nektere krticke ceste


// Konverze problemu a defaultni DD
	double m_fDefaultDDCoef;
	int SetDefaultDD();		// nastavi defaultni casy dokonceni

	double m_fDDCoef;		// koeficient pro vytvoreni defaultnich DD
	double m_fPTCoef;		// koeficient pro vytvoreni defaultnich PT
	double m_fJobCoef;		// koeficient vlivu poctu jobu na termin dokonceni
	double m_fMachineCoef;	// koeficient vlivu poctu stroju na termin dokonceni

	int ConvertToCrispDD();
	int ConvertToCrispPT();
	int ConvertToFuzzyDD();
	int ConvertToFuzzyPT();

// Adding and removing funkction
	int AddJob(LPCTSTR strName,CFN& fDD,double fAlfa,double fBeta,
										double fLambda1,double fLambda2,double fLambda3);	
	int AddMachine(LPCTSTR strName);			// pridani stroje (machinelist)
	int AddProcess(int nJobIndex,int nProcessIndex,int nMachineIndex,CFN& fPT);

	int RemoveMachine(int nMachine);
	int RemoveJob(int nJob);
	int RemoveProcess(int nJob,int nProcess);

	int MoveProcessUp(int nJob,int nProcess);
	int MoveProcessDown(int nJob,int nProcess);


// Criteria
	double m_fw1m;	// vahy pro agregacni kriteria
	double m_fw2s;
	double m_fw1;
	double m_fw2;
	double m_fw3;
	double m_fw4;

	BOOL m_bCrit[CRIT_COUNT];
	int	 m_nCriterion;				// aktualni cislo vybraneho kriteria
	int	 m_nAbleCriterion;		// pocet pouzitelnych kriterii pro soucasny pripad
	int  m_lstnAbleCriterion[CRIT_COUNT];
	int  m_nCritMaxOrSum;			// 0 = max, 1 = suma

	// nastavi spravne priznaky m_bCrit podle situace
	int SetCriterionFlag();	

// modifying base 

	BOOL m_bIsModified;			// je modifikovan problem
	BOOL SetModified(BOOL bModified=TRUE);

	
// Deleting
	int DeleteAll();						// dealokuje uplne vsechny zdroje v CFJSS
	int DeleteAllBase();					// dealokuje vsechny zdroje jobu, stroju a operaci
	int DeleteAllSolving(int nMethod);		// dealokuje vsechny zdroje tykajici se vypoctu 
	int SetDefaultSpecialNode();			// vynuluje pocateni a koncovy uzel


// SOLVING Data
//-----------------------------------------------
// Specialni uzly 0 a *
	CNode m_ndEnd;					// koncovy uzel *
	CNode m_ndStart;				// pocatecni uzel 0

// Uzly a hrany
	CPtrNodeList m_lstpNode;
	CPtrEdgeList m_lstpEdge;
	CEdgeArea m_areaConEdge;
	CEdgeArea m_areaDisEdge;

// Kriticka cesta
	CNode* m_pLastCritNode;						// posledni uzel na kriticke ceste
												// urceny primo kriteriem (last rev.)
	CTypedPtrArray<CObArray,CEdge*>		m_lstpCriticalPath[FN_PART];	// Kriticka cesta
	CUIntArray m_lstnBlockMachine[FN_PART];		// stroj kritickeho bloku
	CUIntArray m_lstnBlockBegin[FN_PART];		// index pocatecni operace bloku
	CUIntArray m_lstnBlockEnd[FN_PART];			// index koncove operace bloku

//-----------------------------------------------
// METHOD Part
// in: FJSSCompute.h
//-----------------------------------------------

// Neighboorhood
	int GetCriticalBlocks();		// vyseparuje jednotlive kriticke bloky
	int GetMinMaxCBN(CPtrNodeArea& areapParent,CPtrNodeArea& areapX,int nMinDistType);
	// S1: Nahodna zmena poradi dvou operaci na kriticke ceste (lepsi pro SA)
	int VicinityS1(CPtrNodeArea& areapSource,CPtrNodeArea& areapDestination);
	// S4: Posuniti uzlu Ti uvnitr kritickeho bloku na zacatek (konec) bloku (GA op)
	int VicinityS4(CPtrNodeArea& areapSource,CPtrNodeArea& areapDestination);
	// uvazuje S4 s pouzitim i krajnich bodu (nepouzito)
	int VicinityS4_2(CPtrNodeArea& areapSource,CPtrNodeArea& areapDestination);

// SetDefault Method Parameters
	int SetDefaultSAParam(int nSATime=10,int nSAMinTemp = 10,double fSATemp = 1000.0,
		int nSACycle = 100,double nSATempCoef = 0.88,int nSACycleCoef = 2);

	int SetDefaultGAParam(int nGATime = 10,int nGAPopulation = 10,
		int nGACycle = 200,int nGASelCoef = 2,int nMinMSX = 3,
		int nGA_MSMCycle = 10,int nGA_LSCycle = 40);

// Begin Schedule
	int  GenerateBeginSchedule(CPtrNodeArea& areapMachineNode);
		int SolvePureEndTime();
		int ActiveScheduleAlgoritm(CPtrNodeArea& areapMachineNode); 

// Objective function
double GetVOOF(CPtrNodeArea& areapMachineNode);		//VOOF Value of Objective Function
	int GenerateNodePtrList(CPtrNodeList& lstpNode);	// vygeneruje seznam ukazatelu na uzly
	int GenerateEdgePtrList(CEdgeArea& areaConEdge,CEdgeArea& areaDisEdge,CPtrEdgeList& lstpEdge);
		int GenerateConEdges(CEdgeArea& areaConEdge);
		int GenerateDisEdges(CPtrNodeArea& areapMachineNode,CEdgeArea& areaDisEdge);
	int OrderAndCheckCycle(CPtrNodeList& r_lstpNode,CPtrEdgeList& r_lstpEdge);						
	int	SolveCPM(CPtrNodeList& lstpNode,CPtrEdgeList& lstpEdge,CPtrNodeArea& areapMachineNode);
	int GenerateCriticalPath();						// vyseparuje z disgrafu kritickou cestu
	double SolveObjectiveFunction(CPtrNodeArea& areapMachineNode);
		double ObjectiveFunction0(int nCi);	//CPT/*		Makespan
		double ObjectiveFunction1(CFN& fCi);	//FPT/*		m(A)
		double ObjectiveFunction2(CFN& fCi);	//FPT/*		w1m+w2s
		double ObjectiveFunction3(CFN& fCi);	//FPT/*		w1m+w2sR
		double ObjectiveFunction4(CFN& fCi);	//FPT/*		w1a(4)+..w4a(1)
		double ObjectiveFunction5(int nCi,CFN& fDi,double fAlfai,double fBetai);//CPT/FDD	Ext_Di(Ci)
		double ObjectiveFunction6(CFN& fCi,CFN& fDi);	//FPT/FDD	(Ci n Di)/(Ci)
		double ObjectiveFunction7(CFN& fCi,CFN& fDi);	//FPT/FDD	(Ci n Di)/(Ci u Di)
		double ObjectiveFunction8(CFN& fCi,CFN& fDi,double fAlfai,double fBetai);	//FPT/FDD
		double ObjectiveFunction9(CFN& fCi,CFN& fDi,double fAlfai,double fBetai,
								 double fLambda1i, double fLambda2i, double fLambda3i);	//FPT/FDD Gi=wi1agr(Ci,Di)+wi2
		int SolveAmongAbleCriterion(CPtrNodeArea& areapMachineNode);
	
// SA 
	int		m_nSATime;			//cas provadeni SA, podm.ukonceni
	int		m_nSAMinTemp;		//minimalni teplota, podm.ukonceni
	double	m_fSATemp;			//pocateni teplota
	int		m_nSACycle;			//pocatecni pocet cyklu SA
	double	m_fSATempCoef;		//koeficient redukce teploty
	int		m_nSACycleCoef;		//koeficient delky cyklu

	int SolveSA();

// GA
	int m_nGATime;			// cas provadeni GA
	int m_nGAPopulation;		// velikost pocatecni populace
	int m_nGACycle;				// pocet cyklu;
	int m_nGASelCoef;			// Zvyseni psti vybrani lepsiho rodice
	int m_nGAMinMSX;			// DGD Vybranych rodicu
	int m_nGA_MSMCycle;			// pocet cyklu pro MSM
	int m_nGA_LSCycle;			// pocet cyklu pro Local Search

	CSADlg* m_pSADlg;			// ukazatel na dialog pro simpl vypocet SA
	CGADlg* m_pGADlg;			// ukazatel na dialog pro simpl GA
	CConvExpDlg* m_pConvExpDlg;	// ukazatel na dialog zkoumani rychlosti konvergence
	CCritExpDlg* m_pCritExpDlg;	// ukazatel na dialog zkoumani kvality kriterii

	int SolveGA();
		int GetDisGraphDistance(CPtrNodeArea& Parent1,CPtrNodeArea& Parent2);
		int GetDisGraphDistance2(CPtrNodeArea& Parent1,CPtrNodeArea& Parent2);
		int GenerateBeginPopulation(CPtrNodeAreaList& lareapPopulation);	
		int OrderPopulation(CPtrNodeAreaList& lareapPopulation);			
		int SelectParents(CPtrNodeAreaList& Population, CPtrNodeArea& Parent1,CPtrNodeArea& Parent2);	
		int MultiStepMutation(CPtrNodeArea& areapParent1,CPtrNodeArea& areapChild);	
		int CFJSS::MultiStepCrossover(CPtrNodeArea& areapP1,CPtrNodeArea& areapP2,CPtrNodeArea& areapChild);			
		double LocalSearch(CPtrNodeArea& areapChild); //lokalni hledani 

// Solving flags
	BOOL IsAbleToSolve();				// zjisti, zda priklad obsahuje nezbytne minimum,
										// aby mohl byt resen

// Results
	double m_fMinDelta;					// nebude se ukladat vysledek dokud neuplyne fMinDelta sec.
	int	   m_nAbleResult;				// 0 - NONE, 1 - SA, 2-GA, 3-ALL, 10 - NONE ACTION
	int    m_nSelectedResult;			// vybrany typ vysledku 0-SA 1-GA
	int	   IsAbleResult(int nType = 10);		

	BOOL   m_bAbleConvResult;
	BOOL   m_bAbleCritResult;
	BOOL   IsAbleConvResult(int nType = 10);	// je vypocitan multivypocet konvergence
	BOOL   IsAbleCritResult(int nType = 10);	// je vypocitan multivypocet zjisteni kriterii

	CScheduleArea m_areaSASchedule;		// uschovany rozvrh z posledniho vypoctu SA
	CScheduleArea m_areaGASchedule;		// uschovany rozvrh z posledniho vypoctu GA

	CAxisArea m_areaAxis;				// prubeh poslednich vypoctu

	int SaveEndData(CPtrNodeArea& areapX,CScheduleArea& areaSchedule,int nSAorGA);

// Reports
	int SolveScheduleForResult(CScheduleArea& areaSchedule);

	int GenerateProjectReport();
	int GenerateJobReport();
	int GenerateMachineReport();
	int GenerateProcessReport();
	int GenerateResultReport(CScheduleArea& areaSchedule);

	int GenerateConvergenceReport();

	int GenerateIterationReport();
	int GenerateREReport();
	int GenerateAREReport();

double GetExactTime(time_t nBeginTime);
int GetRuntime(time_t nBeginTime);

// COnvegence Solving experiment
//-----------------------------------------------
	int m_nCOESolveCount;		// pocet opeakovani vypoctu pro zjisteni korvengence
	int m_nCOESolveTime;		// pocet casu pro konvergenci
	BOOL m_bCOESaveToFile;		// check box save to file
	BOOL m_bCOEAllCriterion;	// check box all useable criterion

	CDoubleArea m_areafConvergence;

// CRiterion Solving Experiment
// Porovnavani kriterii
	CDoubleArea m_areafIteration;		// iterace podle urciteho kriteria
	CDoubleArea m_areafRE;				// relativni chcyby 
	CDoubleArea m_areafAverageRE;			// stredni hodnota z relativnich chyb

	int m_nCREIteration;		// pocet volani ucelove fce
	int m_nCREActIter;			// aktualni iterace;
	BOOL m_bCRESaveToFile;
	int m_nCREMethod;			// 0 - SA, 1 - GA, 2 - Both
//-----------------------------------------------

//-----------------------------------------------
// Import a export dat, Load a Save
// in: FJSSImport.h
//-----------------------------------------------
	int Import(void);						// Import dat ze souboru .csv
		CString FReadToSemicolon(CFile& file);	// nacte retezec od aktualni pozice az ke stredniku bez nej
		int FGoToNextLine(CFile& file);			// skoci na dalsi radek v souboru
		CFN StrToFN(LPCTSTR strFN);				//pretransformuje string fuzzy cisla na fuzzy cislo
	
	int Export(void);						// export dat do souboru .csv
		int WriteLegend(CFile& file,LPCTSTR strLegend,CString strData);

	virtual void Serialize(CArchive& ar);

// TO DO , import parametru z ini souboru
	int ImportFromIni();					

//-----------------------------------------------
// OutString Function
// in: fjssIO.h
//-----------------------------------------------
	// Pole pro stringy do ListView
	CStrList  m_lstsLegend;						
	CStrArea  m_areasData;

	CString GetStrNR(CFN& FN, int nrType = NR_PT);	
	CString GetStrNR(int n);
	CString GetStrNR(double f,int nInt = 3,int nDec = 2);
	CString GetStrNode(CNode& Node,int nType = 0);
	CString GetStrEdge(CEdge& Edge);
	CString GetStrParam(CNode& Node,int nStyle = STYLE_C);	
	CString GetStrType();				// vrati informace o typu prikladu
	CString GetStrCriterion(int nCriterion,int nType = 2);	// vrati popis kriteria 0 = typ,1=naze,2-All
	CString GetStrMethod(int nType);	// vrati naze pouzite metody
	CString GetStrDate();				// vrati aktualni datum 
};

