// FJSSImport.cpp : implementation file
//

#include "stdafx.h"
#include "fjss.h"
#include "FJSSImport.h"
#include "resource.h"


//-----------------------------------------------
// Import dat
//-----------------------------------------------
// Import dat ze souboru .csv
int CFJSS::Import(void)
{

	// POKUS O OTEVRENI SOUBORU
	CFile file;
	int nJob = 0;	
	int nMachine = 0;

	// 1. Nacteni projektovych udaju ze souboru
	// 2. Nazvy stroju
	// 3. Joby + operace

	try 
	{
		file.Open(m_strImportFile, CFile::modeRead);      

		// 1.
		// nazev projektu
		FReadToSemicolon(file);
		m_strName = FReadToSemicolon(file);
		FGoToNextLine(file);
		// datum vytvoreni projektu
		FReadToSemicolon(file);
		m_strDate = FReadToSemicolon(file);
		FGoToNextLine(file);
		// provedl
		FReadToSemicolon(file);
		m_strMaker = FReadToSemicolon(file);
		FGoToNextLine(file);
		
		// --- volny radek
		FGoToNextLine(file);	

		// typ projektu 0-crisp,1-fdd,2-fpt,3-all
		FReadToSemicolon(file);
		SetTypeProp(_tstol(FReadToSemicolon(file)));
		
		FGoToNextLine(file);
		// pocet jobu
		FReadToSemicolon(file);
		nJob = _tstol(FReadToSemicolon(file));
		FGoToNextLine(file);
		// pocet stroju
		FReadToSemicolon(file);
		nMachine = _tstol(FReadToSemicolon(file));
		FGoToNextLine(file);

		// --- volny radek
		FGoToNextLine(file);	

		//2.
		for (int i=0; i<nMachine; i++)			
		{
			CString strName;
			strName=FReadToSemicolon(file);
			AddMachine(strName);
			FGoToNextLine(file);
		}

		// --- volny radek
		FGoToNextLine(file);	

		//3.
		for (int i=0; i<nJob; i++)
		{
			CString strName;
			CFN fDD;

			strName = FReadToSemicolon(file);
			// C/F = OK
			
			if (!FDD()) fDD[0] = _tstol(FReadToSemicolon(file));
			if (FDD())  fDD = StrToFN(FReadToSemicolon(file));
			
			// Alfa a Beta
			double fAlfa = _tstof(FReadToSemicolon(file));
			double fBeta = _tstof(FReadToSemicolon(file));
			double fLambda1 = _tstof(FReadToSemicolon(file));
			double fLambda2 = _tstof(FReadToSemicolon(file));
			double fLambda3 = _tstof(FReadToSemicolon(file));

			AddJob(strName,fDD,fAlfa,fBeta,fLambda1,fLambda2,fLambda3);

			FGoToNextLine(file);
		}

		// --- volny radek
		FGoToNextLine(file);	

		// operace / predpoklada se pocet operaci = poctu stroju
		for (int i=0; i<nJob; i++)
		{
            for (int j=0; j<nMachine; j++)
			{
				int nMachineIndex = 0;
				CFN fPT;
						
				nMachineIndex=_tstol(FReadToSemicolon(file));
		
				// processing time
				// C/F = OK
				if (!FPT()) fPT[0]=_tstol(FReadToSemicolon(file));
				if ( FPT()) fPT=StrToFN(FReadToSemicolon(file));
				
				AddProcess(i,j,nMachineIndex,fPT);				
			}
		FGoToNextLine(file);
		}	

		// UZAVRENI SOUBORU
		file.Close();
	}
	catch (CFileException* e) 
	{
		// Something went wrong.
		e->ReportError (MB_ICONERROR,IDS_IMPORT_ERROR);
		e->Delete ();
	}
	return 0;
}


// nacte retezec od aktualni pozice az ke stredniku bez nej
CString CFJSS::FReadToSemicolon(CFile& file)
{
	CString str = _T("");

	CHAR buffer=' ';
	ULONGLONG dwBytesRemaining;

	// zbyvajici pocet bytu souboru
	dwBytesRemaining = file.GetLength () - file.GetPosition();

	// testovani stredniku (semicolon code = 59)
	while ((dwBytesRemaining) && (buffer != 59 ) && (buffer != 13 ))
	{
		UINT nBytesRead = file.Read (&buffer, sizeof (buffer)); 	    
		dwBytesRemaining -= sizeof(buffer);
		if ((buffer != 59 ) && (buffer != 13 )) str += buffer;
		
		// vraceni se o jeden byte zpet pokud najde konec radku 13
		if (buffer == 13) file.Seek(-1, CFile::current);
		dwBytesRemaining -= nBytesRead;
	}
	
	return str;
}

// skoci na dalsi radek v textu
int CFJSS::FGoToNextLine(CFile& file)
{
	CHAR buffer=13;
	ULONGLONG dwBytesRemaining;

	// zbyvajici pocet bytu souboru
	dwBytesRemaining = file.GetLength () - file.GetPosition();

	while (dwBytesRemaining)
	{
        UINT nBytesRead = file.Read (&buffer, sizeof (buffer)); 	    		
		dwBytesRemaining -= nBytesRead;
		// nasel konec radku
		if (buffer == 13)
		{
			// kod 10
			if (dwBytesRemaining) file.Read(&buffer,sizeof(buffer));
			break;
		}
	}
	return 0;
}

//-----------------------------------------------
// Export dat
//-----------------------------------------------

int CFJSS::WriteLegend(CFile& file,LPCTSTR strLegend,CString strData)
{
	TCHAR tchReturn[2] = {13,10};
	TCHAR tchSemicol[1] = {';'};
	CString str = strLegend;
	LPCTSTR tstr = str;

	tstr = str;
	file.Write (tstr,(int)_tcslen(tstr)*sizeof(TCHAR));
	file.Write (tchSemicol,sizeof(TCHAR));
	str = strData;
	tstr = str;
	file.Write (tstr,(int)_tcslen(tstr)*sizeof(TCHAR));
	file.Write (tchReturn,2*sizeof(TCHAR));

	return 0;
}

int CFJSS::Export(void)
{
	CFile file;
	file.Open(m_strExportFile,CFile::modeCreate | CFile::modeReadWrite);

	TCHAR tchReturn[2] = {13,10};
	TCHAR tchSemicol[1] = {';'};
	CString str;
	LPCTSTR tstr = str;
	
	WriteLegend(file,_T("Název"),m_strName);
	WriteLegend(file,_T("Datum:"),m_strDate);
	WriteLegend(file,_T("Vytvoøil:"),m_strMaker);
	
	//--- Prazdny radek
	file.Write (tchReturn,2*sizeof(TCHAR));

	str.Format(_T("%d"),m_nType);
	WriteLegend(file,_T("Typ:"),str);
	str.Format(_T("%d"),m_nJob);
	WriteLegend(file,_T("Jobù:"),str);
	str.Format(_T("%d"),m_nMachine);
	WriteLegend(file,_T("Strojù:"),str);

	//--- Prazdny radek
	file.Write (tchReturn,2*sizeof(TCHAR));

	// STROJE
	for (int i=0; i<m_nMachine; i++)
	{
		str = m_lstMachine[i].m_strName;
		tstr = str;
		file.Write (tstr,(int)_tcslen(tstr)*sizeof(TCHAR));
		file.Write (tchReturn,2*sizeof(TCHAR));		
	}
	//--- Prazdny radek
	file.Write (tchReturn,2*sizeof(TCHAR));
	// JOBY
	for (int i=0; i<m_nJob;i++)
	{
		// JobName
		str = m_lstJob[i].m_strName;
		tstr = str;
		file.Write (tstr,(int)_tcslen(tstr)*sizeof(TCHAR));
		file.Write (tchSemicol,sizeof(TCHAR));
		// Job DD
		// C/F = OK
		str = GetStrNR(m_lstJob[i].m_fDD,NR_DD);
		tstr = str;
		file.Write (tstr,(int)_tcslen(tstr)*sizeof(TCHAR));
		file.Write (tchSemicol,sizeof(TCHAR));
		// Job Alfa
		str = GetStrNR(m_lstJob[i].m_fAlfa);
		tstr = str;
		file.Write (tstr,(int)_tcslen(tstr)*sizeof(TCHAR));
		file.Write (tchSemicol,sizeof(TCHAR));
		// Job Beta
		str = GetStrNR(m_lstJob[i].m_fBeta);
		tstr = str;
		file.Write (tstr,(int)_tcslen(tstr)*sizeof(TCHAR));
		file.Write (tchSemicol,sizeof(TCHAR));
		// Lambda 1 - 3
		str = GetStrNR(m_lstJob[i].m_fLambda1);
		tstr = str;
		file.Write (tstr,(int)_tcslen(tstr)*sizeof(TCHAR));
		file.Write (tchSemicol,sizeof(TCHAR));

		str = GetStrNR(m_lstJob[i].m_fLambda2);
		tstr = str;
		file.Write (tstr,(int)_tcslen(tstr)*sizeof(TCHAR));
		file.Write (tchSemicol,sizeof(TCHAR));

		str = GetStrNR(m_lstJob[i].m_fLambda3);
		tstr = str;
		file.Write (tstr,(int)_tcslen(tstr)*sizeof(TCHAR));
		file.Write (tchSemicol,sizeof(TCHAR));
		file.Write (tchReturn,2*sizeof(TCHAR));		
	}

	//--- Prazdny radek
	file.Write (tchReturn,2*sizeof(TCHAR));
	// OPERACE
	for (int i=0; i<m_nJob;i++)
	{
		for (int j=0; j<m_areaJobNode[i].GetSize();j++)
		{
			// Stroj procesu
			str.Format(_T("%d"),m_areaJobNode[i][j].m_nMachine);
			tstr = str;
			file.Write (tstr,(int)_tcslen(tstr)*sizeof(TCHAR));
			file.Write (tchSemicol,sizeof(TCHAR));

			// Process PT
			// C/F = OK
			str = GetStrNR(m_areaJobNode[i][j].m_fPT);
			
			tstr = str;
			file.Write (tstr,(int)_tcslen(tstr)*sizeof(TCHAR));
			
			if (j<m_areaJobNode[i].GetSize()-1) file.Write (tchSemicol,sizeof(TCHAR));			
		}
		file.Write (tchReturn,2*sizeof(TCHAR));

	}


	return 0;

}

CFN CFJSS::StrToFN(LPCTSTR strFN)
{
	// predpoklada se retezec ve tvaru a1,a2,a3,a4
	CString str = strFN;
	CString strPart;
	int nLength = 0;
	int nIndex1 = 0;
	int nIndex2 = 0;

	CFN fn;
	if (!str.GetLength()) 
		return fn;
	
	for (int i=0; i<FN_PART-1; i++)
	{		
		nLength = str.GetLength();
		nIndex2 = str.Find(_T(","),nIndex1);
		strPart = str.Mid(nIndex1, nIndex2 - nIndex1);
		fn.m_a[i] = _tstol(strPart);
		nIndex1 = nIndex2 + 1;
	}
		strPart = str.Mid(nIndex1,str.GetLength());
		fn.m_a[FN_PART-1] = _tstol(strPart);
	return fn;
}

//-----------------------------------------------
// Pro Import dat z fjssm.ini
//-----------------------------------------------

int CFJSS::ImportFromIni()
{
	return 0;
}

