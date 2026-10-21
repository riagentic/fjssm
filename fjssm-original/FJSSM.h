// FJSSM.h : main header file for the FJSSM application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols


// CFJSSMApp:
// See FJSSM.cpp for the implementation of this class
//

class CFJSSMApp : public CWinApp
{
public:
	CFJSSMApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()


};

extern CFJSSMApp theApp;