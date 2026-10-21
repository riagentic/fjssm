// FJSSFN.cpp : implementation file
//

#include "stdafx.h"
#include "FJSSM.h"
#include "FJSSFN.h"
#include "math.h"



CFN::CFN()
{
	for (int i=0; i<FN_PART; i++) m_a[i] = 0;
}

CFN::~CFN()
{
}

CFN::CFN(CFN& FN)
{
	for (int i=0; i<FN_PART; i++) m_a[i] = FN.m_a[i];
}


// CFN member function
CFN& CFN::Plus(CFN& op1,CFN& op2)
{
	for (int i=0; i<FN_PART; i++) m_a[i] = op1.m_a[i] + op2.m_a[i];
	return *this;
}

CFN& CFN::Max(CFN& op)
{
	for (int i=0; i<FN_PART; i++) 
		if (op.m_a[i] > m_a[i]) m_a[i] = op.m_a[i];
	return *this;
}

CFN& CFN::operator =(int n)
{
	for (int i=0; i<FN_PART; i++) m_a[i] = n;
	return *this;
}

CFN& CFN::operator =(CFN& FN)
{
	for (int i=0; i<FN_PART; i++) m_a[i] = FN.m_a[i];
	return *this;
}

int& CFN::operator [] (int nIndex)
{
	return m_a[nIndex];
}

void CFN::Test()
{
	for (int i=0; i<FN_PART-1; i++)  
		if (m_a[i+1] < m_a[i]) 
			AfxMessageBox(_T("Kritická chyba u fuzzy èísla"),MB_OK);
	
	for (int i=0; i<FN_PART; i++)  
		if (m_a[i] < 0) 
			AfxMessageBox(_T("Fuzzy cislo s hodnotou < 0, kriticka chyba "),MB_OK);
}

double CFN::Mean()
// A4
{
	double a1 = m_a[0];
	double a2 = m_a[1];
	double a3 = m_a[2];
	double a4 = m_a[3];

	// pokud jsou vsechny slozky stejne => crisp, vraci tuto hodnotu
	if ((a1 == a2) && (a1 == a3) && (a1 == a4)) return a1;
	
	double fMean = (1.0/3.0) * (( pow(a4,2) + a4*a3 + pow(a3,2) - pow(a2,2)
		- a1*a2 - pow(a1,2) ) / (a4 + a3 - a2 - a1) );

	return fMean;
}

double CFN::Center()
// A4
{
	double a1 = m_a[0];
	double a2 = m_a[1];
	double a3 = m_a[2];
	double a4 = m_a[3];

	
	double fCenter = ( ((a1 + a4) /2) + ((a2 + a3)/2) ) / 2;

	return fCenter;
}

double CFN::Devi()
// A4
{
	double a1 = m_a[0];
	double a2 = m_a[1];
	double a3 = m_a[2];
	double a4 = m_a[3];

	double fMeanSq = pow(Mean(),2);
	double fDevi = sqrt(
					( 
						((1.0/6.0) * ( 	pow(a4,3) + pow(a4,2) * a3 + a4 * pow(a3,2) +
									pow(a3,3) - pow(a2,3) - pow(a2,2) * a1 -
									a2 * pow(a1,2) - pow(a1,3) 
								) / ( a4 + a3 - a2 - a1 ))
					 ) - fMeanSq )  ;
	
	return fDevi;
}

void CFN::Check()
{
	if ((m_a[0] == m_a[1]) && (m_a[0] == m_a[2]) && (m_a[0] == m_a[3])) 
	m_a[3] = m_a[2] + 1;
}

double CFN::RDevi()
{
	double a = m_a[0];
	double b = m_a[1]; 
	double c = m_a[2];
	double d = m_a[3];
	// Prava smerodatna odchylka
	// sR(A) = ( Int(x*miA(x)dx,x=m(A)..a(4)) - m(A) ) / AreaR(A)
	double fResult;
	double fRightArea;
	double fArea1 = 0;
	double fArea2 = 0;
	double fArea3 = 0;

	double fMean;

	fMean = Mean();

	// VYPOCET
	// oblast a-b
	if (fMean < b)
	{
		fArea1 =      (1.0/3.0) * (( pow(b,3) - pow(fMean,3) ) / (b-a)) 
			        - (1.0/2.0) * (pow(a,2) * ( pow(b,2) - pow(fMean,2) )) / (b -a);
	}
	// oblast b-c
	if (fMean <c)
	{
		fArea2 = (( 0.5 * pow(c,2)) - ( 0.5 * pow(max(b,fMean),2)));
	}
	// oblast c-d
		fArea3 = ( (-1.0/3.0) * ( ( pow(d,3) - pow(fMean,3) ) / (d - c) ) ) + 
				      ( (0.5)  * ( ( d * ( pow(d,2) - pow(fMean,2) ) ) / (d - c) ) );
	
	fResult = (((fArea1 + fArea2 + fArea3) - fMean ) / MeanRightArea()); 


	return fResult;
}

CFN CFN::operator +(CFN& op)
{
	CFN fResult;
	for (int i=0; i<FN_PART;i++) fResult[i] = m_a[i]  + op[i];
	return fResult;
}
int CFN::operator<(CFN& FN)
// A4
{
	if (Mean() < FN.Mean()) return 1;
	if (Mean() > FN.Mean()) return 0;
	if (Mean() == FN.Mean()) 
	{
		if (Devi() < FN.Devi()) return 1;
		else return 0;
	}
    
	return -1;
}
int CFN::operator >(CFN& FN)
{
	if (Mean() > FN.Mean()) return 1;
	if (Mean() < FN.Mean()) return 0;
	if (Mean() == FN.Mean()) 
	{
		if (Devi() > FN.Devi()) return 1;
		else return 0;
	}

	return -1;
}


double FNSpillover(CFN& fn1, CFN& fn2)
{
	CFN fn[2];
	fnPart Array[8];
	double fResult = 0;

	fn[0] = fn1;
	fn[1] = fn2;
    
	// naplneni pole
	for (int i=0; i<FN_PART;i++)
	{
		Array[2*i].x = fn1[i];
		Array[2*i].o = 0;
		Array[2*i].fx = FNFx(fn1,fn1[i]);

		Array[2*i+1].x = fn2[i];
		Array[2*i+1].o = 1;
		Array[2*i+1].fx = FNFx(fn2,fn2[i]);
	}

	// setrideni cisel podle velikosti
	BOOL bSort = TRUE;
	while (bSort)
	{
		for (int i=0; i<2*FN_PART-1; i++)
		{
			if (Array[i].x > Array[i+1].x)
			{
				int o = Array[i].o;
				double x = Array[i].x;
				double fx = Array[i].fx;
			
				Array[i].o = Array[i+1].o;
				Array[i].x = Array[i+1].x;
				Array[i].fx = Array[i+1].fx;

				Array[i+1].o = o;
				Array[i+1].x = x;
				Array[i+1].fx = fx;
				bSort = TRUE;
				break;
			}
			bSort = FALSE;
		}		
	}

	for (int i=0; i<2*FN_PART; i++)
		TRACE("Slozka %d = %f, o = %d, f(x) = %f\n",i,Array[i].x,Array[i].o,Array[i].fx);

// ---- Hlavni vypocet
// cast 0 (0-1)
		
	
for (int i=0; i<2*FN_PART-1; i++)
{

	for (int j=0; j<2; j++)
	{
		// part i		
		// vypocitani nezname funkci hodnoty v bode 0
		Array[i+j].fx2 = FNFx(fn[!Array[i+j].o],Array[i+j].x);
		Array[i+j].fxMin = min(Array[i+j].fx,Array[i+j].fx2);
		if (Array[i+j].o == 0) 
		{	Array[i+j].fxo0 = Array[i+j].fx;
			Array[i+j].fxo1 = Array[i+j].fx2;
		}
		else 
		{	Array[i+j].fxo0 = Array[i+j].fx2;
			Array[i+j].fxo1 = Array[i+j].fx;
		}

		// zjisteni vlastnika minimalni funkcni hodnoty v 0
		if (Array[i+j].fx == Array[i+j].fx2) Array[i+j].oMin = 2;				// oba stejney
		if (Array[i+j].fx < Array[i+j].fx2) Array[i+j].oMin = Array[i+j].o;		// slozkovy je nizsi	
		if (Array[i+j].fx > Array[i+j].fx2) Array[i+j].oMin = !Array[i+j].o;	// druhy je nizsi
	}


	// vzdalenost na x-ove ose
	double fDistance = Array[i+1].x - Array[i].x;
	double fMaxHeight = max(Array[i].fxMin,Array[i+1].fxMin);
	double fMinHeight = min(Array[i].fxMin,Array[i+1].fxMin);

	// 2 Varianty:
	// a) minimalni hodnoty jsou jednou fuzzy cislo , snazsi vypocet plochy
	if ((Array[i].oMin == Array[i+1].oMin) || (Array[i].oMin == 2) || (Array[i+1].oMin == 2))
	{
		double fSquare = fMinHeight * fDistance;
		double fTriangle = (fMaxHeight - fMinHeight) * fDistance / 2;
		fResult += fSquare + fTriangle;
		TRACE ("%d.usek,min.jedno fuzzy cislo,tr. = %f,ct. = %f,sum = %f\n",i,fTriangle,fSquare,fResult);
	}
	// b) minimalni hodnoty jsou dve fuzzy cisla ==> je treba najit prusecik
	else
	{
		// Vypocet pozice pruseciku, je treba rozlisit vlastnika bodu

		double k1 = (Array[i+1].fxo0 - Array[i].fxo0) / fDistance;
		double k2 = (Array[i+1].fxo1 - Array[i].fxo1) / fDistance;
		double q1 = Array[i].fxo0;
		double q2 = Array[i].fxo1;

		double fISect = ((q2 - q1) / (k1 - k2)) + Array[i].x ;
		double fYISect = FNFx(fn[0],fISect);
		double teste = FNFx(fn[1],fISect);
		
		//if (fYISect != Fx(fn[1],fISect))
		//	AfxMessageBox("Intersection, bad value !!!",MB_ICONERROR);


			// Sklada se ze ctyr ukolu na kazdem useku
			// 1. ctverec1 (muze byt 0)
			// 2. ctverec2 (muze byt 0)
			// 3. trojuhelnik1
			// 4. trojuhelnik2

		// Vypocet ctverce
		double fSquare1 = 0;
		if (min(Array[i].fxMin,Array[i+1].fxMin) == 0)	
			fSquare1 = 0;		
		else
			fSquare1 = min(Array[i].fxMin,Array[i+1].fxMin) * fDistance;

		// Vypocet maleho ctverce
		double fSquare2 = 0;
		if (Array[i].fxMin == Array[i+1].fxMin) 
			fSquare2 = 0;				// pokud se rovnaji ctverec 0
		else
		{
			if (Array[i].fxMin > Array[i+1].fxMin)		// pred prusecikem
				fSquare2 = (fISect - Array[i].x) * (Array[i].fxMin - Array[i+1].fxMin);
			else										// za prusecikem
				fSquare2 = (Array[i+1].x - fISect) * (Array[i+1].fxMin - Array[i].fxMin);
		}

		// Vypocet leveho a praveho trojuhelniku
				
		double fLeftTriangle = (fISect - Array[i].x) * fabs(fYISect - Array[i].fxMin) / 2;
		double fRightTriangle = (Array[i+1].x - fISect) * fabs(fYISect - Array[i+1].fxMin) / 2;

		fResult += fSquare1 + fSquare2 + fLeftTriangle + fRightTriangle;
		TRACE ("%d.usek,min.ruzne,prx = %f,pry = %f, ct1 = %f,ct2 = %f,trl = %f, trp = %f,sum = %f\n",
			i,fISect,fYISect,fSquare1,fSquare2,fLeftTriangle,fRightTriangle,fResult);
	}
}

TRACE("Prunikova plocha by mela byt: %f\n",fResult);
	return fResult;
}

double FNFx(CFN& FN,double n)
{
	double fResult = 0;
	double fn[FN_PART];

	for (int i=0; i<FN_PART; i++) fn[i] = (double)FN[i];

	if (n<=fn[0]) fResult = 0;

	if ((n>fn[0]) && (n<fn[1])) 
		fResult = (n-fn[0]) * 1/(fn[1]-fn[0]);

	if ((n>=fn[1]) && (n<=fn[2])) fResult = 1;

	if ((n>fn[2]) && (n<fn[3]))
		fResult = (fn[3] - n) *  1/(fn[3]-fn[2]);

	if (n>=fn[3]) fResult = 0;

	return fResult;
}

double CFN::FY(double fX)
{
	double fResult = 0;
	double fn[FN_PART];

	for (int i=0; i<FN_PART; i++) fn[i] = (double)m_a[i];

	if (fX<=fn[0]) fResult = 0;

	if ((fX>fn[0]) && (fX<fn[1])) 
		fResult = (fX-fn[0]) * 1/(fn[1]-fn[0]);

	if ((fX>=fn[1]) && (fX<=fn[2])) fResult = 1;

	if ((fX>fn[2]) && (fX<fn[3]))
		fResult = (fn[3] - fX) *  1/(fn[3]-fn[2]);

	if (fX>=fn[3]) fResult = 0;

	return fResult;
}

double FNLeft(CFN fCi,CFN fDi)
// vypocita int(max(0,LDi(alfa)-LCi(lafa))dalfa,alfa = 0..1)
// nebo		int(max(0,RCi(alfa)-RDi(lafa))dalfa,alfa = 0..1)
{
	double fCi1 = fCi[0];
	double fCi2 = fCi[1];
	double fDi1 = fDi[0];
	double fDi2 = fDi[1];
	double fResult;

	// ctyri moznosti

    if ((fCi1 >= fDi1) && (fCi2 >= fDi2))	return 0;
	
	if (((fCi1 <= fDi1) && (fCi2 < fDi2)) || ((fCi1 < fDi1) && (fCi2 <= fDi2)))
	{
		double fSmallArea = ((fCi2 - fCi1) / 2.0);
		double fLargeArea = ((fDi2 - fDi1) / 2.0) + (fDi1 - fCi1);
		fResult = fLargeArea - fSmallArea;
		return fResult;
	}

	if ((fCi1 < fDi1) && (fCi2 > fDi2))
	{
		// musi se spocitat prusecik  x = (qC-qD) / (kD - kC)
		double kCi = ((1.0) / (fCi2 - fCi1));
		double kDi = ((1.0) / (fDi2 - fDi1));
		double qCi = FNFx(fCi,fDi1);
		double qDi = 0;

		double fXIsec2 = ((qCi - qDi) / (kDi - kCi));
		double fXIsec = fXIsec2 + fDi1;
		double fYIsec = FNFx(fCi,fXIsec);

		double fYITest = FNFx(fDi,fXIsec);
		TRACE("Prusecik1 %f bsmr %f\n",fYIsec,fYITest);

        
		double fSmallArea = ((fXIsec - fCi1) * (fYIsec) / (2.0));
		double fLargeArea = ((fXIsec - fDi1) * (fYIsec) / (2.0)) + ((fDi1 - fCi1) * fYIsec);

		fResult = fLargeArea - fSmallArea;
		return fResult;

	}

	if ((fCi1 > fDi1) && (fCi2 < fDi2))
	{
		// musi se spocitat prusecik  x = (qC-qD) / (kD - kC)
		double kCi = ((1.0) / (fCi2 - fCi1));
		double kDi = ((1.0) / (fDi2 - fDi1));
		double qDi = FNFx(fDi,fCi1);
		double qCi = 0;

		double fXIsec2 = ((qCi - qDi) / (kDi - kCi));
		double fXIsec = fXIsec2 + fCi1;
		double fYIsec = FNFx(fCi,fXIsec);

		double fYITest = FNFx(fDi,fXIsec);
		TRACE("Prusecik2 %f bsmr %f\n",fYIsec,fYITest);

		double fSmallArea = (fCi2 - fXIsec) * (1-fYIsec) / (2.0);
		double fLargeArea = (fDi2 - fXIsec) * (1-fYIsec) / (2.0);

		fResult = fLargeArea - fSmallArea;
		return fResult;

	}
}


double FNRight(CFN fCi,CFN fDi)
// vypocita int(max(0,LDi(alfa)-LCi(lafa))dalfa,alfa = 0..1)
// nebo		int(max(0,RCi(alfa)-RDi(lafa))dalfa,alfa = 0..1)
{
	double fCi3 = fCi[2];
	double fCi4 = fCi[3];
	double fDi3 = fDi[2];
	double fDi4 = fDi[3];
	double fResult;

	// ctyri moznosti, 2 bez pruseciku, 2 s

    if ((fCi3 <= fDi3) && (fCi4 <= fDi4))	return 0;
	if (((fCi3 >= fDi3) && (fCi4 > fDi4)) || ((fCi3 > fDi3) && (fCi4 >= fDi4)))
	{
		double fSmallArea = ((fDi4 - fDi3) / 2.0);
		double fLargeArea = ((fCi4 - fCi3) / 2.0) + (fCi3 - fDi3);
		fResult = fLargeArea - fSmallArea;
		return fResult;
	}

	if ((fCi3 < fDi3) && (fCi4 > fDi4)) 
	{
		// musi se spocitat prusecik  x = (qC-qD) / (kD - kC)
		double kCi = ((-1.0) / (fCi4 - fCi3));
		double kDi = ((-1.0) / (fDi4 - fDi3));
		double qCi = -1.0 + FNFx(fCi,fDi3);
		double qDi = 0;

		double fXIsec2 = ((qCi - qDi) / (kDi - kCi));
		double fXIsec = fXIsec2 + fDi3;
		double fYIsec = FNFx(fCi,fXIsec);

		double fYITest = FNFx(fDi,fXIsec);
		TRACE("Prusecik3 %f bsmr %f\n",fYIsec,fYITest);

		double fSmallArea = ((fDi4 - fXIsec) * (fYIsec) / (2.0));
		double fLargeArea = ((fCi4 - fXIsec) * (fYIsec) / (2.0));

		fResult = fLargeArea - fSmallArea;
		return fResult;

	}

	if ((fCi3 > fDi3) && (fCi4 < fDi4))
	{
		// musi se spocitat prusecik  x = (qC-qD) / (kD - kC)
		double kCi = (-(1.0) / (fCi4 - fCi3));
		double kDi = (-(1.0) / (fDi4 - fDi3));
		double qDi = -1.0 + FNFx(fDi,fCi3);
		double qCi = 0;

		double fXIsec2 = ((qCi - qDi) / (kDi - kCi));
		double fXIsec = fXIsec2 + fCi3;
		double fYIsec = FNFx(fCi,fXIsec);

		double fYITest = FNFx(fDi,fXIsec);
		TRACE("Prusecik4 %f bsmr %f\n",fYIsec,fYITest);

		double fSmallArea = (fXIsec-fCi3) * (1-fYIsec) / (2.0);
		double fLargeArea = (fXIsec -fDi3) * (1-fYIsec) / (2.0);

		fResult = fLargeArea - fSmallArea;
		return fResult;

	}



}

double FNArea(CFN& FN)
// A4
{
	double fn[FN_PART];
	for (int i=0; i<FN_PART; i++) fn[i] = (double)FN[i];

	// obsah lich fuzzy cisla = Tr1 + Ctverec + Tr2
	
	double fTriangle1 = (fn[1] - fn[0]) / (2.0);
	double fSquare = fn[2] - fn[1];
	double fTriangle2 = (fn[3] - fn[2]) / (2.0);

	double fResult = fTriangle1 + fSquare + fTriangle2;

	return fResult;
}

double CFN::MeanRightArea()
{
	double a = m_a[0];
	double b = m_a[1];
	double c = m_a[2];
	double d = m_a[3];

	double fTri1 = 0;
	double fSq1 = 0;
	double fSq2 = 0;
	double fTri3 = 0;
	double fResult;
	// obsah plochy od stredni hodnoty do konce
	// 
	// Area = triangle1 + ctverec1 + ctverec2 + triangl3

	double fMean = Mean();

	// 1. interval a-b
	if (fMean < b)
	{
		fTri1 = (( (1.0 - FNFx(*this,fMean)) * (b - fMean) ) / 2.0 );
		fSq1 = FNFx(*this,fMean) *  (b - fMean);
	}
	// 2, interal b-c
	if (fMean <c)
	{
		fSq2 = ( c -  max(b,fMean));
	}
	// 3. interval c-d,
		fTri3 = ((d - max(c,fMean)) / 2.0);
   
	fResult = fTri1 + fSq1 + fSq2 + fTri3;

	return fResult;
}

double ExtendedFNFx(CFN& FN,double n,double fAlfa, double fBeta)
{
	double fResult;
	// interval 0-a(1)
	if (n<FN[0]) 
		fResult = fAlfa * (n - FN[0]);

	// interval a(1)-a(4)
	if ((n <= FN[0]) && (n >= FN[FN_PART-1])) 
		fResult = FNFx(FN,n);

	// interval a(4)-...
	if (n>FN[FN_PART-1])
		fResult = fBeta * (FN[FN_PART-1] - n);

	return fResult;
}


//-----------------------------------------------
// Pomocne funkce
//-----------------------------------------------
// nastavuje generovani nahodnych cisel dle aktualniho casu, pouzit pred cyklem generovani
int PrepareSrand()
{
	static time_t nLastTime = 0;
	time_t nTime = time(NULL);

	if (nLastTime != nTime)
	{
		srand((unsigned)nTime); 
		nLastTime = nTime;
	}
	return 0;
}

// generuje int nahodne cislo s hodnotou maximalne nMax, je dobre pred tim nastavit srand
int RandI(int nMax)
{
	float fRand = (float)rand();
	
	fRand = (fRand  / RAND_MAX) * nMax;
    	
	// zaokrouhleni
	int nRand = (int)fRand;
	fRand -= nRand;

	// zvyseni
	if (fRand >= 0.5) nRand++;

	return nRand;
}

double RandF()
{
	float fRand = (float)rand();
	fRand = fRand  / RAND_MAX;
    	
	return fRand;
}


void CFN::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{	// storing code
		// a1,a2,a3,an
		//for (int i=0; i<FN_PART; i++)
			ar << m_a[0];
			ar << m_a[1];
			ar << m_a[2];
			ar << m_a[3];

	}
	else
	{	// loading code
//		for (int i=0; i<FN_PART; i++)
			ar >> m_a[0];
			ar >> m_a[1];
			ar >> m_a[2];
			ar >> m_a[3];
	}
}


int GetNrRange(double fNr)
{
	// bude provadet v radu 10^-6 az 10^6
	int nResult = 0;
	double fPow;
	double fAbs = fabs(fNr);

	for (int i=-6; i<6; i++)
	{
		fPow = pow(10.0,i);
		if (fAbs < fPow)
		{
			nResult = i;
			break;
		}
	}
	return nResult;
}

HBITMAP GetMaskedBitmap(CBitmap& bm,UINT nResID,COLORREF crMask)
{
	COLORMAP cm[1];
	cm[0].from = crMask;
	cm[0].to = GetSysColor(COLOR_3DFACE);
	bm.LoadMappedBitmap(nResID,0,cm,1);

	return bm;
}

			

