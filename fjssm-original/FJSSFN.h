#pragma once

int const FN_PART = 4;
// CFN command target


struct fnPart
{
	double x;
	double fx;
	int o;	
	double fx2;
	double fxMin;
	double fxo0;
	double fxo1;
	int oMin;
};

class CFN : public CObject
{
public:
	int m_a[FN_PART];

	CFN();
	CFN(CFN& FN);
	virtual ~CFN();

	CFN& Plus(CFN& op1,CFN& op2);
	CFN& Max(CFN& op);
	
	//OPERATOR
	CFN& operator=(CFN& FN);
	CFN& operator=(int n);
	int& operator[] (int nIndex);

	// porovnani fuzzy cisla pomoci stredni hodnoty a smerodatne odchylky
	int operator<(CFN& FN);
	int operator>(CFN& FN);
	CFN operator+(CFN& op);


	//Stredni hodnota 
	double Mean();
	//Smerodatna odchylka
	double Devi(); 
	// Prava smerodatna odchylka
	double RDevi();
	// stred cisla
	double MeanRightArea();
	double Center();
	double FY(double fX);
    
	void Check();	// zajisti, ze pokud se vykonvertovalo 
					// fuzzy cislo a1 = a2 = a3 =a4
					// tak udela   a4 = a3 + 1

	void Test();
	virtual void Serialize(CArchive& ar);
};

// Pomocne funkce
//-----------------------------------------------

	extern int PrepareSrand();			// pripravy srand podle aktualniho casu pro generovani rand()
	extern int RandI(int nMax);		// vrati nahodne cislo max nMax
	extern double RandF();				// vraci double nahodne cislo z int 0-1 vc.

	extern double FNSpillover(CFN& fn1,CFN& fn2);	// vypocita prunik dvou lich. fuzzy cisel
	extern double FNArea(CFN& FN);					// vypocita obsah lich. fuzzy cisla
	extern double FNFx(CFN& fn,double n);			// vratu mi(x) lich fuzzy cisla 
	// vrati mi(x) rozsireneho lich cisla
	extern double ExtendedFNFx(CFN& FN,double n,double fAlfa, double fBeta);	
	extern int GetNrRange(double fNr);
	extern double FNRight(CFN fCi,CFN fDi);
	extern double FNLeft(CFN fCi,CFN fDi);
	extern HBITMAP GetMaskedBitmap(CBitmap& bm,UINT nResID,COLORREF crMask);